/****************************************************************************
 * Copyright (c) 2012, the Konoha project authors. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <event.h>
#include <evhttp.h>

#define HTTPD_ADDR "0.0.0.0"
#define HTTPD_PORT 8080


#ifdef __cplusplus
extern "C" {
#endif

void req_handler (struct evhttp_request *r, void *arg)
{
	if (r->type != EVHTTP_REQ_GET) {
		evhttp_send_error(r, HTTP_BADREQUEST, "Available GET only");
	}
	else {
		//STUB:: parse body
	}
}

int main (int argc, char **av)
{
	struct event_base *ev_base;
	struct evhttp *httpd;

	ev_base = event_base_new();
	httpd = evhttp_new(ev_base);
	if (evhttp_bind_socket(httpd, HTTPD_ADDR, HTTPD_PORT) < 0) {
		perror("evhttp_bind_socket");
		exit(EXIT_FAILURE);
	}
	evhttp_set_gencb(httpd, req_handler, NULL);
	event_base_dispatch(ev_base);
	evhttp_free(httpd);
	event_base_free(ev_base);
}

#ifdef __cplusplus
}
#endif
