/*
 * libwebsockets-test-client - libwebsockets test implementation
 *
 * Copyright (C) 2011 Andy Green <andy@warmcat.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

#ifdef CMAKE_BUILD
#include "lws_config.h"
#endif

#include "../lib/libwebsockets.h"

static unsigned int opts;
static int was_closed;
static int deny_deflate;
static int deny_mux;
#if 1
static struct libwebsocket *wsi_zgj_test;
#else
static struct libwebsocket *wsi_mirror;
static int mirror_lifetime = 0;
#endif
static int force_exit = 0;
static int longlived = 0;

/*
 * This demo shows how to connect multiple websockets simultaneously to a
 * websocket server (there is no restriction on their having to be the same
 * server just it simplifies the demo).
 *
 *  dumb-increment-protocol:  we connect to the server and print the number
 *				we are given
 *
 *  lws-mirror-protocol: draws random circles, which are mirrored on to every
 *				client (see them being drawn in every browser
 *				session also using the test server)
 */

enum demo_protocols {

	PROTOCOL_DUMB_INCREMENT,
	PROTOCOL_LWS_MIRROR,
	PROTOCOL_ZGJ_TEST,

	/* always last */
	DEMO_PROTOCOL_COUNT
};


/* dumb_increment protocol */
#if 0
static int
callback_dumb_increment(struct libwebsocket_context *this,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
	switch (reason) {

	case LWS_CALLBACK_CLIENT_ESTABLISHED:
		fprintf(stderr, "callback_dumb_increment: LWS_CALLBACK_CLIENT_ESTABLISHED\n");
		break;

	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		fprintf(stderr, "LWS_CALLBACK_CLIENT_CONNECTION_ERROR\n");
		was_closed = 1;
		break;

	case LWS_CALLBACK_CLOSED:
		fprintf(stderr, "LWS_CALLBACK_CLOSED\n");
		was_closed = 1;
		break;

	case LWS_CALLBACK_CLIENT_RECEIVE:
		((char *)in)[len] = '\0';
		fprintf(stderr, "%s(%d) rx %d '%s'\n", __func__, __LINE__, (int)len, (char *)in);
		break;

	/* because we are protocols[0] ... */

	case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
		if ((strcmp(in, "deflate-stream") == 0) && deny_deflate) {
			fprintf(stderr, "denied deflate-stream extension\n");
			return 1;
		}
		if ((strcmp(in, "deflate-frame") == 0) && deny_deflate) {
			fprintf(stderr, "denied deflate-frame extension\n");
			return 1;
		}
		if ((strcmp(in, "x-google-mux") == 0) && deny_mux) {
			fprintf(stderr, "denied x-google-mux extension\n");
			return 1;
		}

		break;

	default:
		break;
	}

	return 0;
}

/* lws-mirror_protocol */


static int
callback_lws_mirror(struct libwebsocket_context *context,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
	unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 4096 +
						  LWS_SEND_BUFFER_POST_PADDING];
	int l = 0;
	int n;

	switch (reason) {

	case LWS_CALLBACK_CLIENT_ESTABLISHED:

		fprintf(stderr, "callback_lws_mirror: LWS_CALLBACK_CLIENT_ESTABLISHED\n");

		mirror_lifetime = 10 + (random() & 1023);
		/* useful to test single connection stability */
		if (longlived)
			mirror_lifetime += 50000;

		fprintf(stderr, "opened mirror connection with "
				     "%d lifetime\n", mirror_lifetime);

		/*
		 * mirror_lifetime is decremented each send, when it reaches
		 * zero the connection is closed in the send callback.
		 * When the close callback comes, wsi_mirror is set to NULL
		 * so a new connection will be opened
		 */

		/*
		 * start the ball rolling,
		 * LWS_CALLBACK_CLIENT_WRITEABLE will come next service
		 */

		libwebsocket_callback_on_writable(context, wsi);
		break;

	case LWS_CALLBACK_CLOSED:
		fprintf(stderr, "mirror: LWS_CALLBACK_CLOSED mirror_lifetime=%d\n", mirror_lifetime);
		wsi_mirror = NULL;
		break;

	case LWS_CALLBACK_CLIENT_RECEIVE:
		fprintf(stderr, "%s(%d) rx %d '%s'\n", __func__, __LINE__, (int)len, (char *)in); 
		break;

	case LWS_CALLBACK_CLIENT_WRITEABLE:

		for (n = 0; n < 1; n++)
			l += sprintf((char *)&buf[LWS_SEND_BUFFER_PRE_PADDING + l],
					"c #%06X %d %d %d;",
					(int)random() & 0xffffff,
					(int)random() % 500,
					(int)random() % 250,
					(int)random() % 24);

		n = libwebsocket_write(wsi,
		   &buf[LWS_SEND_BUFFER_PRE_PADDING], l, opts | LWS_WRITE_TEXT);

		if (n < 0)
			return -1;
		if (n < l) {
			lwsl_err("Partial write LWS_CALLBACK_CLIENT_WRITEABLE\n");
			return -1;
		}

		mirror_lifetime--;
		if (!mirror_lifetime) {
			fprintf(stderr, "closing mirror session\n");
			return -1;
		} else
			/* get notified as soon as we can write again */
			libwebsocket_callback_on_writable(context, wsi);
		break;

	default:
		break;
	}

	return 0;
}
#endif

#if 1
static int
callback_zgj_test(struct libwebsocket_context *context,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
	unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 4096 +
						  LWS_SEND_BUFFER_POST_PADDING];
	int l = 0;
	int n;
    static int number = 0;

	switch (reason) {

	case LWS_CALLBACK_CLIENT_ESTABLISHED:

		fprintf(stderr, "%s: LWS_CALLBACK_CLIENT_ESTABLISHED\n", __func__);


		/*
		 * mirror_lifetime is decremented each send, when it reaches
		 * zero the connection is closed in the send callback.
		 * When the close callback comes, wsi_mirror is set to NULL
		 * so a new connection will be opened
		 */

		/*
		 * start the ball rolling,
		 * LWS_CALLBACK_CLIENT_WRITEABLE will come next service
		 */

		libwebsocket_callback_on_writable(context, wsi);
		break;

	case LWS_CALLBACK_CLOSED:
		fprintf(stderr, "zgj_test: LWS_CALLBACK_CLOSED\n");
		break;

	case LWS_CALLBACK_CLIENT_RECEIVE:
		fprintf(stderr, "%s(%d) rx %d '%s'\n", __func__, __LINE__, (int)len, (char *)in); 
		break;

	case LWS_CALLBACK_CLIENT_WRITEABLE:

		fprintf(stderr, "%s(%d) prepare to write to server\n", __func__, __LINE__); 
		l += sprintf((char *)&buf[LWS_SEND_BUFFER_PRE_PADDING + l],
					"zgj test send %d", number++);

		n = libwebsocket_write(wsi,
		   &buf[LWS_SEND_BUFFER_PRE_PADDING], l, opts | LWS_WRITE_TEXT);

		if (n < 0)
			return -1;
		if (n < l) {
			lwsl_err("Partial write LWS_CALLBACK_CLIENT_WRITEABLE\n");
			return -1;
		}

		libwebsocket_callback_on_writable(context, wsi);
		break;

	default:
		break;
	}

	return 0;
}
#endif


/* list of supported protocols and callbacks */

static struct libwebsocket_protocols protocols[] = {
#if 0
	{
		"dumb-increment-protocol",
		callback_dumb_increment,
		0,
		20,
	},
	{
		"lws-mirror-protocol",
		callback_lws_mirror,
		0,
		128,
	},
	{
		"central-protocol",
		callback_zgj_test,
		0,
		128,
	},
#else
	{
		"central-protocol",
		callback_zgj_test,
		0,
		128,
	},
#endif
	{ NULL, NULL, 0, 0 } /* end */
};

void sighandler(int sig)
{
	force_exit = 1;
}

static struct option options[] = {
	{ "help",	no_argument,		NULL, 'h' },
	{ "debug",      required_argument,      NULL, 'd' },
	{ "port",	required_argument,	NULL, 'p' },
	{ "ssl",	no_argument,		NULL, 's' },
	{ "version",	required_argument,	NULL, 'v' },
	{ "undeflated",	no_argument,		NULL, 'u' },
	{ "nomux",	no_argument,		NULL, 'n' },
	{ "longlived",	no_argument,		NULL, 'l' },
	{ NULL, 0, 0, 0 }
};


int main(int argc, char **argv)
{
	int n = 0;
	int ret = 0;
	int port = 7681;
	int use_ssl = 0;
	struct libwebsocket_context *context;
	const char *address;
#if 0
	struct libwebsocket *wsi_dumb;
#endif
	int ietf_version = -1; /* latest */
	struct lws_context_creation_info info;

	memset(&info, 0, sizeof info);

	fprintf(stderr, "libwebsockets test client\n"
			"(C) Copyright 2010-2013 Andy Green <andy@warmcat.com> "
						    "licensed under LGPL2.1\n");

	if (argc < 2)
		goto usage;

	while (n >= 0) {
		n = getopt_long(argc, argv, "nuv:hsp:d:l", options, NULL);
		if (n < 0)
			continue;
		switch (n) {
		case 'd':
			lws_set_log_level(atoi(optarg), NULL);
			break;
		case 's':
			use_ssl = 2; /* 2 = allow selfsigned */
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'l':
			longlived = 1;
			break;
		case 'v':
			ietf_version = atoi(optarg);
			break;
		case 'u':
			deny_deflate = 1;
			break;
		case 'n':
			deny_mux = 1;
			break;
		case 'h':
			goto usage;
		}
	}

	if (optind >= argc)
		goto usage;

	signal(SIGINT, sighandler);

	address = argv[optind];

	/*
	 * create the websockets context.  This tracks open connections and
	 * knows how to route any traffic and which protocol version to use,
	 * and if each connection is client or server side.
	 *
	 * For this client-only demo, we tell it to not listen on any port.
	 */

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
#ifndef LWS_NO_EXTENSIONS
	info.extensions = libwebsocket_get_internal_extensions();
#endif
	info.gid = -1;
	info.uid = -1;

	context = libwebsocket_create_context(&info);
	if (context == NULL) {
		fprintf(stderr, "Creating libwebsocket context failed\n");
		return 1;
	}

	/* create a client websocket using dumb increment protocol */

#if 0
	wsi_dumb = libwebsocket_client_connect(context, address, port, use_ssl,
			"/", argv[optind], argv[optind],
			 protocols[PROTOCOL_DUMB_INCREMENT].name, ietf_version);
		    
	if (wsi_dumb == NULL) {
		fprintf(stderr, "libwebsocket connect failed\n");
		ret = 1;
		goto bail;
	}

#else
#if 1
		    /* create a client websocket using mirror protocol */

		    wsi_zgj_test = libwebsocket_client_connect(context,
		    	address, port, use_ssl,  "/",
		    	argv[optind], argv[optind],
		    	protocols[PROTOCOL_ZGJ_TEST].name, ietf_version);

		    if (wsi_zgj_test == NULL) {
		    	fprintf(stderr, "libwebsocket "
		    			      "zgj test connect failed\n");
		    	ret = 1;
		    	goto bail;
		    }
#else
		    /* create a client websocket using mirror protocol */

		    wsi_mirror = libwebsocket_client_connect(context,
		    	address, port, use_ssl,  "/",
		    	argv[optind], argv[optind],
		    	protocols[PROTOCOL_LWS_MIRROR].name, ietf_version);

		    if (wsi_mirror == NULL) {
		    	fprintf(stderr, "libwebsocket "
		    			      "mirror connect failed\n");
		    	ret = 1;
		    	goto bail;
		    }
#endif
#endif

	fprintf(stderr, "Waiting for connect...\n");

	/*
	 * sit there servicing the websocket context to handle incoming
	 * packets, and drawing random circles on the mirror protocol websocket
	 * nothing happens until the client websocket connection is
	 * asynchronously established
	 */

	n = 0;
	while (n >= 0 && !was_closed && !force_exit) {
		n = libwebsocket_service(context, 10);

		if (n < 0)
			continue;
#if 0
		if (wsi_mirror)
			continue;

		/* create a client websocket using mirror protocol */

		wsi_mirror = libwebsocket_client_connect(context,
			address, port, use_ssl,  "/",
			argv[optind], argv[optind],
			protocols[PROTOCOL_LWS_MIRROR].name, ietf_version);

		if (wsi_mirror == NULL) {
			fprintf(stderr, "libwebsocket "
					      "mirror connect failed\n");
			ret = 1;
			goto bail;
		}
#else
#if 0
		if (!wsi_mirror)
        {
		    /* create a client websocket using mirror protocol */

		    wsi_mirror = libwebsocket_client_connect(context,
		    	address, port, use_ssl,  "/",
		    	argv[optind], argv[optind],
		    	protocols[PROTOCOL_LWS_MIRROR].name, ietf_version);

		    if (wsi_mirror == NULL) {
		    	fprintf(stderr, "libwebsocket "
		    			      "mirror connect failed\n");
		    	ret = 1;
		    	goto bail;
		    }
        }
		
        if (!wsi_zgj_test)
        {
		    /* create a client websocket using mirror protocol */

		    wsi_zgj_test = libwebsocket_client_connect(context,
		    	address, port, use_ssl,  "/",
		    	argv[optind], argv[optind],
		    	protocols[PROTOCOL_ZGJ_TEST].name, ietf_version);

		    if (wsi_zgj_test == NULL) {
		    	fprintf(stderr, "libwebsocket "
		    			      "zgj test connect failed\n");
		    	ret = 1;
		    	goto bail;
		    }
        }
#endif
#endif
	}

bail:
	fprintf(stderr, "Exiting\n");

	libwebsocket_context_destroy(context);

	return ret;

usage:
	fprintf(stderr, "Usage: libwebsockets-test-client "
				"<server address> [--port=<p>] "
				"[--ssl] [-k] [-v <ver>] "
				"[-d <log bitfield>] [-l]\n");
	return 1;
}
