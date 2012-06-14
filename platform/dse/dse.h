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

/* ************************************************************************ */
#ifndef DSE_H_
#define DSE_H_
#include "dse_debug.h"

struct dse_request_t {
	int method;
	int context;
	int taskid;
	char logpoolip[16];
	char *script;
};
#define JSON_INITGET(O, K) \
		json_t *K = json_object_get(O, #K)

static struct dse_request_t *dse_parseJson(const char *input)
{
	json_t *root;
	json_error_t error;
	root = json_loads(input, 0, &error);
	if (!root) {
		D_("error occurs");
		return 1;
	}
	D_("now, parse");
	int i;
///	json_t *taskid, *type, *context,
//		*method, *logpool, *script;
//	taskid= json_object_get(root, "taskid");
	JSON_INITGET(root, taskid);
	JSON_INITGET(root, type);
	JSON_INITGET(root, context);
	JSON_INITGET(root, method);
	JSON_INITGET(root, logpool);
	JSON_INITGET(root, script);

	D_("taskid:%s, type:%s, context:%d",
			json_string_value(taskid),
			json_string_value(type),
			json_string_value(context)
			);
	D_("method:%s, logpool:%s",
			json_string_value(method),
			json_string_value(logpool)
			);
	D_("script:%s", json_string_value(script));
	json_decref(root);
	return 0;
}

void dse_dispatch(int method)
{

}

#endif /* DSE_H_ */
