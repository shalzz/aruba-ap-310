/*
 * Walled Garden
 */

#include "main.h"

#include "wg.h"
#include "heap.h"
#include "log.h"
#include "reqs.h"
#include "conf.h"
#include "util/util_instant.h"

#define FILTER_BUFFER_LEN (512)

static int err;

struct filter_list {
        struct filter_list *next;
        char *pat;
        regex_t *cpat;
	char *url_pat;
        regex_t *url_cpat;
};

static struct filter_list *wl = NULL;
static struct filter_list *bl = NULL;

static char *begin_shared_mem = NULL;
static char *cur_shared_mem = NULL;
static char *end_shared_mem = NULL;

static auto_wg_t *auto_wg = NULL;
extern ecp_cfg_t *ecp_cfg;
extern ecp_proxy_info_t *ecp_proxy_info;

static struct flock lock_wt, unlock_wt;
static int lock_wd = -1;

static void _shared_lock_init (void)
{
        char lock_file[] = "/tmp/tinyproxy.shared.lock.XXXXXX";

        /* Only allow u+rw bits. This may be required for some versions
         * of glibc so that mkstemp() doesn't make us vulnerable.
         */
        umask (0177);

        lock_wd = mkstemp (lock_file);
        unlink (lock_file);

        lock_wt.l_type = F_WRLCK;
        lock_wt.l_whence = SEEK_SET;
        lock_wt.l_start = 0;
        lock_wt.l_len = 0;

        unlock_wt.l_type = F_UNLCK;
        unlock_wt.l_whence = SEEK_SET;
        unlock_wt.l_start = 0;
        unlock_wt.l_len = 0;
}

void _shared_lock_wait (void)
{
        int rc;

        while ((rc = fcntl (lock_wd, F_SETLKW, &lock_wt)) < 0) {
                if (errno == EINTR)
                        continue;
                else
                        return;
        }
}

void _shared_lock_release (void)
{
        if (fcntl (lock_wd, F_SETLKW, &unlock_wt) < 0)
                return;
}


#define SHARE_MEMORY_LOCK()   _shared_lock_wait()
#define SHARE_MEMORY_UNLOCK() _shared_lock_release()


/*
 * Initializes a linked list of strings containing hosts/urls to be filtered
 */
void wg_init (void)
{
        struct filter_list *p, *q;

        for (p = q = wl; p; p = q) {
            if (p->cpat) {
                regfree (p->cpat);
                safefree (p->cpat);
            }
            safefree (p->pat);
            q = p->next;
            safefree (p);
        }
        wl = NULL;
        for (p = q = bl; p; p = q) {
            if (p->cpat) {
                regfree (p->cpat);
                safefree (p->cpat);
            }
            safefree (p->pat);
            q = p->next;
            safefree (p);
        }
        bl = NULL;
}


/*
 * Add name to black or white list
 */
static void wg_add_to_list (struct filter_list **list, char *buf)
{
    struct filter_list *p;
    int cflags;
    unsigned int l;

	if (!buf)
		return;

    l = strlen(buf);
    if (!l)
        return;

    cflags = REG_NEWLINE | REG_NOSUB | REG_EXTENDED | REG_ICASE;

    p = (struct filter_list *) safecalloc (1, sizeof (struct filter_list));
    p->pat = safemalloc(l + 3);
    if (!p->pat) {
        error_log("tinyproxy", "%s: %d: out of memory", __func__, __LINE__);
        free (p);
        return;
    }
    strcpy(p->pat, buf);
    p->cpat = (regex_t *) safemalloc (sizeof (regex_t));
    err = regcomp (p->cpat, p->pat, cflags);
    if (err != 0) {
        safefree(p->cpat);
        p->cpat = NULL;
        if (*buf != '*') {
            *p->pat = '*';
            strcpy(p->pat + 1, buf);
            l++;
        }
        if (p->pat[l-1] != '*') {
            p->pat[l] = '*';
            p->pat[l+1] = 0;
        }
        error_log("tinyproxy", "%s: %d: pattern '%s' is not regexp, using wild card pattern '%s'",
            __func__, __LINE__, buf, p->pat);
    }

	/* add to linked list */
	p->next = *list;
	*list = p;
}

/* Add name to white list */
void wg_add_wl (char *buf)
{
	wg_add_to_list(&wl, buf);
}

/* Add name to black list */
void wg_add_bl (char *buf)
{
	wg_add_to_list(&bl, buf);
}

static int wildcard_match(const char *pat, const char *str) {
    const char *cp = NULL, *mp = NULL;

    if (!pat || !str) {
        return 0;
    }

    while ((*str) && (*pat != '*')) {
        if ((*pat != *str) && (*pat != '?')) {
            return 0;
        }
        pat++;
        str++;
    }

    while (*str) {
        if (*pat == '*') {
            if (!*++pat) {
                return 1;
            }
            mp = pat;
            cp = str+1;
        } else if ((*pat == *str) || (*pat == '?')) {
            pat++;
            str++;
        } else {
            pat = mp;
            str = cp++;
        }
    }

    while (*pat == '*') {
        pat++;
    }
    return !*pat;
}

/* Return 0 if not found */
static int request_in_list (struct filter_list *list, const char *host, const char *url)
{
    struct filter_list *p;
    int result;
	char req[FILTER_BUFFER_LEN*2];

	
    if (!list)
		return 0;

	snprintf(req, sizeof(req), "%s%s", host, url);
	/* printf(" Got %s\n", req); */

    for (p = list; p; p = p->next) {
        if (p->cpat) {
            result = regexec (p->cpat, req, (size_t) 0, (regmatch_t *) 0, 0);
        } else {
            result = !wildcard_match (p->pat, req);
        }
		/* printf(" %d pat=%s\n", result, p->pat?p->pat:"n/a"); */
        if (!result) {
            return 1;
        }
    }
	return 0;
}

/* Return 0 if not found */
int request_in_wl (const char *host, const char *url)
{
	return request_in_list(wl, host, url);
}

/* Return 0 if not found */
int request_in_bl (const char *host, const char *url)
{
	return request_in_list(bl, host, url);
}

/***********************************************************
 *
 * Auto white list
 *
 ***********************************************************/

/* START OF LOCKING SECTION */

/*
 * These variables are required for the locking mechanism.  Also included
 * are the "private" functions for locking/unlocking.
 */
static struct flock lock_it, unlock_it;
static int lock_fd = -1;

static void _child_lock_init (void)
{
        char lock_file[] = "/tmp/tinyproxy.autowg.lock.XXXXXX";

        /* Only allow u+rw bits. This may be required for some versions
         * of glibc so that mkstemp() doesn't make us vulnerable.
         */
        umask (0177);

        lock_fd = mkstemp (lock_file);
        unlink (lock_file);

        lock_it.l_type = F_WRLCK;
        lock_it.l_whence = SEEK_SET;
        lock_it.l_start = 0;
        lock_it.l_len = 0;

        unlock_it.l_type = F_UNLCK;
        unlock_it.l_whence = SEEK_SET;
        unlock_it.l_start = 0;
        unlock_it.l_len = 0;
}

static void _child_lock_wait (void)
{
        int rc;

        while ((rc = fcntl (lock_fd, F_SETLKW, &lock_it)) < 0) {
                if (errno == EINTR)
                        continue;
                else
                        return;
        }
}

static void _child_lock_release (void)
{
        if (fcntl (lock_fd, F_SETLKW, &unlock_it) < 0)
                return;
}

/* END OF LOCKING SECTION */

void
auto_wg_init(int create)
{
    int r;
    int cfl = 0;
    unsigned size_needed = 0;
    unsigned int extra;

    size_needed = sizeof(auto_wg_t);

    if (size_needed > AUTO_WG_SHARED_MEM_SIZE) {
        log_message(LOG_ERR, "More memory needed for auto walled garden: %u/%u\n", 
			size_needed, AUTO_WG_SHARED_MEM_SIZE);
        abort();
    }

    size_needed = AUTO_WG_SHARED_MEM_SIZE + sizeof(ecp_cfg_t) + 4;

    if (size_needed > SWARM_ECP_SHARED_MEM_SIZE) {
        log_message(LOG_ERR, "More memory needed for ECP shared mem: %u/%u\n", 
			size_needed, SWARM_ECP_SHARED_MEM_SIZE);
        abort();
    }

    ecp_cfg = ecp_cfg_init(TRUE);
    if (!ecp_cfg) {
        perror("mmap");
        exit(1);
    }
    ecp_proxy_info = ecp_proxy_init(TRUE);
    if (!ecp_proxy_info) {
        perror("mmap");
        exit(1);
    }
    _shared_lock_init();
    auto_wg = get_auto_wg_from_ecp(ecp_cfg);

    begin_shared_mem = end_shared_mem = (char *)auto_wg;
    begin_shared_mem += sizeof(*auto_wg);
    extra = ((unsigned int)(begin_shared_mem)) % 4;
    if (extra) {
        /* align memory */
        begin_shared_mem += (4 - extra);
    }
    end_shared_mem += AUTO_WG_SHARED_MEM_SIZE;
    cur_shared_mem = begin_shared_mem;

    _child_lock_init();
}

void *malloc_wg_shared_memory(size_t size)
{
    size_t free_mem = end_shared_mem - cur_shared_mem;
    void * ret = cur_shared_mem;
    size_t aligned_size = size;
    unsigned int extra = aligned_size % 4;
    if (extra) {
        /* align memory */
        aligned_size += (4 - extra);
    }
    
    if (free_mem < aligned_size) {
        return NULL;
    }

    cur_shared_mem += aligned_size;
    return (ret);
}

/*
 * clear auto white list. This is needed when external captive
 * portal server is changed
 */
void
auto_wg_reset(void)
{
    if (is_child) {
        /* No need to reset shared memory in child as main process has done it */
        return;
    }
    _child_lock_wait();
    memset(auto_wg, 0, sizeof(*auto_wg));
    _child_lock_release();
}

/*
 * Reduce domain to last level. This could be
 * fed to regcomp eventually instead of direct
 * comparison
 * For example a.b.c.d.com => d.com
 */
static char *canonize_domain (char *domain)
{
    char *cp, *p1, *p2, *p3;
    struct in_addr inaddr;

    /* check if potentially IP address */
    if (inet_aton(domain, &inaddr))
        return domain;

    cp = domain;
    p1 = p2 = p3 = NULL;
    while (*(cp = strchrnul(cp, '.'))) {
        cp += 1;
        p1 = p2;
        p2 = p3;
        p3 = cp;
    }
    if (p2 && p3 && (cp - p3) >= 3 || (p3 - p2 -1) >= 4) {
        domain = p2;
    } else if (p1) {
        domain = p1;
    }
    return domain;
}

static int valid_domain (const char *domain)
{
    const char *s = domain;
    if (!s || !(*s)) {
        return 0;
    }
    if (!isalpha(*s) && !isdigit(*s)) {
        return 0;
    }
    s ++;
    if (*s == 0) {
        return 0;
    }
    for (; *(s+1); s++) {
        if (!isalpha(*s) && !isdigit(*s) && (*s != '-') && (*s != '.')) {
            return 0;
        }
    }
    if (*s == '-' || *s == '.') {
        return 0;
    }
    return 1;
}

/*
 * Add domain to auto walled garden 
 */
void auto_wg_add_wl (char *domain)
{
        struct auto_filter_list *f;
        int i, result, cflags;
        char *host = domain;
        unsigned int candidate = -1;
        time_t lru;

        if (!valid_domain(domain)) {
            error_log("tinyproxy", "%s: %d: rejected invalid domain '%s'", __func__, __LINE__, domain);
            return;
        }
	    if (!domain || !(domain = canonize_domain(domain)))
		    return;
	    if (!auto_wg)
		    return;

        cflags = REG_NEWLINE | REG_NOSUB | REG_EXTENDED | REG_ICASE;

	    _child_lock_wait();
        time(&lru);

        for (i=0; i<auto_wg->count; i++) {
            /* result = regexec (&f->cpat, domain, (size_t) 0, (regmatch_t *) 0, 0); */
            result = strcasecmp(auto_wg->filter[i].pat, domain);
		    if (!result) {  /* found */
                _child_lock_release();
		        return;
            }
            if (auto_wg->filter[i].atime < lru) {
                lru = auto_wg->filter[i].atime;
                candidate = i;
            }
        }

        /* replace least used entry if table is full */
        if (auto_wg->count == AUTO_WG_MAX_NAMES) {
            i = candidate;
            log_message (LOG_CRIT, "Auto WL full: replace %s by %s at index %d",
                auto_wg->filter[i].pat, host, i);
        } else
            auto_wg->count++;

        f = &(auto_wg->filter[i]);
        strncpy(f->pat, domain, SIZE_WG_NAME);
        f->nl = '\n';
        time(&f->atime);

        /*
       	err = regcomp (&f->cpat, f->pat, cflags);
       	if (err != 0) {
               	fprintf (stderr, "Bad regex in %s\n", f->pat);
                return;
       	}
        */

        /* fprintf(stderr, "Add %s (%s) to slot %d\n", domain, host, auto_wg->count); */

	    _child_lock_release();
}

/* Return 0 if domain not found in auto walled garden */
int request_in_auto_wl (char *host, char *url)
{
    char *domain;
    int i, result;
    struct auto_filter_list *f;

    if (!auto_wg || !auto_wg->count)
		return 0;
    if (!(domain = canonize_domain(host)))
        return 0;

    for (i=0, f=&(auto_wg->filter[0]); i<auto_wg->count; i++,f++) {
        result = strcasecmp(f->pat, domain);
        /* result = regexec (&f->cpat, host, (size_t) 0, (regmatch_t *) 0, 0); */
		if (!result) {   /* match */
            time(&f->atime);
		    return 1;
        }
    }
	return 0;
}

void
ecp_proxy_add(char *cmac, char *cip, int cport, int sport, pid_t pid)
{
    int i = 0;
    ecp_proxy_t *client;

    SHARE_MEMORY_LOCK();
    for (i = 0; i < SWARM_ECP_PROXY_NUMBER; i ++){
        client = &(ecp_proxy_info->client_info[i]);
        if (client->sport == 0 && client->cport == 0){
            strcpy(client->cmac, cmac);
            strcpy(client->cip, cip);
            client->cport = cport;
            client->sport = sport;
            client->pid = pid;
            log_message(LOG_INFO, "%s, %d: Add client mac-[%s], ip-[%s], port-[%d], sport-[%d], pid-[%d]", __func__, __LINE__, client->cmac, client->cip, client->cport, client->sport, (int)client->pid);
            break;
        }
    }
    SHARE_MEMORY_UNLOCK();

    if (i == SWARM_ECP_PROXY_NUMBER)
        log_message(LOG_WARNING, "%s, %d: No space to add client mac-[%s], ip-[%s], port-[%d], sport-[%d], pid-[%d]", __func__, __LINE__, cmac, cip, cport, sport, (int)pid);    
    return;
}

void
ecp_proxy_remove(int sport, pid_t pid)
{
    int i = 0;
    ecp_proxy_t *client;

    SHARE_MEMORY_LOCK();
    for (i = 0; i < SWARM_ECP_PROXY_NUMBER; i ++){
        client = &(ecp_proxy_info->client_info[i]);
        if (client->sport == sport || client->pid == pid){
            log_message(LOG_INFO, "%s, %d: Remove client mac-[%s], sport-[%d]", __func__, __LINE__, client->cmac, client->sport);
            memset(client, 0, sizeof(ecp_proxy_t));
            SHARE_MEMORY_UNLOCK();
            return;
        }
    }
    SHARE_MEMORY_UNLOCK();
    return;
}

int
ecp_proxy_number()
{
    int i = 0, number = 0;
    ecp_proxy_t *client;

    for (i = 0; i < SWARM_ECP_PROXY_NUMBER; i ++){
        client = &(ecp_proxy_info->client_info[i]);
        if (client->sport != 0 || client->pid != 0){
            number ++;
        }
    }
    return number;
}

