#ifndef __lstring_h__
#define __lstring_h__
#if !defined(__UCLIBC__) && !defined(_STRLCPY_DEFINED) && !defined(WIN32)

static inline size_t 
strlcat(char *dst, register const char *src, size_t n)
{
	size_t len;
	char dummy[1];

	len = 0;

	while (1) {
		if (len >= n) {
			dst = dummy;
			break;
		}
		if (!*dst) {
			break;
		}
		++dst;
		++len;
	}

	while ((*dst = *src) != 0) {
		if (++len < n) {
			++dst;
		}
		++src;
	}

	return len;
}

static inline size_t 
strlcpy(char *__restrict dst, const char *__restrict src, size_t n)
{
	const char *src0 = src;
	char dummy[1];

	if (!n) {
		dst = dummy;
	} else {
		--n;
	}

	while ((*dst = *src) != 0) {
		if (n) {
			--n;
			++dst;
		}
		++src;
	}

	return src - src0;
}

#define _STRLCPY_DEFINED
#endif
#endif /* __lstring_h__ */
