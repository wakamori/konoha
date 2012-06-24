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

#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>
#include <konoha2/float.h>
#define logpool_init g_logpool_init
#include <logpool.h>
#undef logpool_init
#include <protocol.h>

#define Int_to(T, a)               ((T)a.ivalue)
#define Float_to(T, a)             ((T)a.fvalue)

typedef struct kRawPtr {
	kObjectHeader h;
	void *rawptr;
} kRawPtr;

static void RawPtr_init(CTX, kObject *po, void *conf)
{
	kRawPtr *o = (kRawPtr*)(o);
	o->rawptr = conf;
}

static void Logpool_free(CTX, kObject *po)
{
	kRawPtr *o = (kRawPtr*)(o);
	logpool_close(o->rawptr);
	o->rawptr = NULL;
}
static void Log_free(CTX, kObject *po)
{
	kRawPtr *o = (kRawPtr*)(o);
	free(o->rawptr);
	o->rawptr = NULL;
}

//static kObject *new_RawPtr(CTX, const kclass_t *ct, void *ptr)
//{
//	kObject *ret = new_kObject(ct, ptr);
//	RawPtr_init(_ctx, ret, ptr);
//	return ret;
//}

//## LogPool LogPool.new(String host, int port)
static KMETHOD LogPool_new(CTX, ksfp_t *sfp _RIX)
{
	kRawPtr *ret = (kRawPtr *) sfp[0].o;
	char *host = (char *) S_text(sfp[1].s);
	int   port = sfp[2].ivalue;
	RawPtr_init(_ctx, sfp[0].o, logpool_open_client(NULL, host, port));
	RETURN_(ret);
}

//## Log LogPool.get()
static KMETHOD LogPool_get(CTX, ksfp_t *sfp _RIX)
{
	logpool_t *lp = (logpool_t *) ((kRawPtr *) sfp[0].o)->rawptr;
	char *buf = malloc(256);
	char *ret = logpool_client_get(lp, buf, 256);
	kObject *log = new_kObject(O_ct(sfp[K_RTNIDX].o), buf);
	assert (ret && "TODO");
	RETURN_(log);
}

//## String Log.get(String key)
static KMETHOD Log_get_(CTX, ksfp_t *sfp _RIX)
{
	kRawPtr *self = (kRawPtr *) sfp[0].o;
	struct Log *log = (struct Log *) self->rawptr;
	char *key  = (char *) S_text(sfp[1].s);
	int   klen = S_size(sfp[1].s);
	int   vlen;
	char *data = Log_get(log, key, klen, &vlen);
	RETURN_(new_kString(data, vlen, SPOL_ASCII|SPOL_POOL));
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _F(F)   (intptr_t)(F)
#define TY_Logpool  (ct0->cid)
#define TY_Log      (ct1->cid)

static	kbool_t logpool_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	KREQUIRE_PACKAGE("konoha.float", pline);
	static KDEFINE_CLASS Def0 = {
			.structname = "LogPool"/*structname*/,
			.cid = CLASS_newid/*cid*/,
			.init = RawPtr_init,
			.free = Logpool_free,
	};
	kclass_t *ct0 = Konoha_addClassDef(ks->packid, ks->packdom, NULL, &Def0, pline);

	static KDEFINE_CLASS Def1 = {
			.structname = "Log"/*structname*/,
			.cid = CLASS_newid/*cid*/,
			.init = RawPtr_init,
			.free = Log_free,
	};
	kclass_t *ct1 = Konoha_addClassDef(ks->packid, ks->packdom, NULL, &Def1, pline);

	int FN_x = FN_("x");
	int FN_y = FN_("y");
	intptr_t MethodData[] = {
		_Public|_Const, _F(LogPool_new), TY_Logpool, TY_Logpool, MN_("new"), 2, TY_String, FN_x, TY_Int, FN_y,
		_Public|_Const, _F(LogPool_get), TY_Log, TY_Logpool, MN_("get"), 0,
		_Public|_Const, _F(Log_get_), TY_String, TY_Log, MN_("get"), 2, TY_String, FN_x,
			DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);
	return true;
}

static kbool_t logpool_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t logpool_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t logpool_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}
KDEFINE_PACKAGE* logpool_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("logpool", "1.0"),
		.initPackage = logpool_initPackage,
		.setupPackage = logpool_setupPackage,
		.initKonohaSpace = logpool_initKonohaSpace,
		.setupKonohaSpace = logpool_setupKonohaSpace,
	};
	return &d;
}

