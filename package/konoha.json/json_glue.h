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

#ifndef JSON_GLUE_H_
#define JSON_GLUE_H_

#include <stdbool.h>
#include <json/json.h>
#include <json/json_object_private.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef const struct _kJson kJson;
struct _kJson {
	kObjectHeader h;
	json_object *obj;
};

static void Json_init(CTX, kObject *o, void *conf)
{
	fprintf(stderr, "Json_init\n");
	struct _kJson *json = (struct _kJson *)o;
	json->obj = json_object_new_object();
}

static void Json_free(CTX, kObject *o)
{
	//struct _kCurl *c = (struct _kCurl*)o;
	//if(c->curl != NULL) {
	//	curl_easy_cleanup(c->curl);
	//	c->curl = NULL;
	//}
}
/* ------------------------------------------------------------------------ */
// [API METHODS]

//## Json Json.new();
static KMETHOD Json_new (CTX, ksfp_t *sfp _RIX)
{
	RETURN_(new_kObject(O_ct(sfp[K_RTNIDX].o), NULL));
}

//## @Static Json Json.parse(String str);
static KMETHOD Json_parse(CTX, ksfp_t *sfp _RIX)
{
	fprintf(stderr, "fson_parse start\n");
	struct _kJson *json = (struct _kJson*)sfp[0].o;
	const char *buf = S_text(sfp[1].s);
	json_object *obj = json_tokener_parse(buf);
	// [TODO]
	//if (!IS_Json(obj)) {
	//	DBG_P("[ERROR] Json parse failed to parse ==> new Json()");
	//	obj = json_object_new_object();
	//}
	json->obj = obj;
	RETURN_(json);
}

////## var Json.get(String key, Class _);
//KMETHOD Json_get(CTX ctx, ksfp_t *sfp _RIX)
//{
//	json_object *obj = RawPtr_to(json_object *, sfp[0]);
//	char *key = String_to(char *, sfp[1]);
//	kClass *c = sfp[2].c;
//	ksfp_t vsfp = {};
//	if (IS_Json(obj)) {
//		json_object *val = json_object_object_get(obj, key);
//		if (IS_Json(val)) {
//			convert_json_to_object(ctx, val, c->cTBL, &vsfp);
//		} else {
//			DBG_P("[WARNING] Json get no such value: this[\"%s\"] ==> 0", key);
//			RETURN_(KNH_NULVAL(knh_Class_cid(c)));
//		}
//	} else {
//		DBG_P("[ERROR] Json get this object is error ==> 0");
//		RETURN_(KNH_NULVAL(knh_Class_cid(c)));
//	}
//	sfp[_rix].ndata = vsfp.ndata;
//	RETURN_(vsfp.o);
//}
//
////## void Json.set(String key, dynamic value);
//KMETHOD Json_set(CTX ctx, ksfp_t *sfp _RIX)
//{
//	json_object *obj = RawPtr_to(json_object *, sfp[0]);
//	char *key = String_to(char *, sfp[1]);
//	if (IS_Json(obj)) {
//		json_object *val = json_object_knh_to_json(ctx, sfp[2].o, sfp[2].ndata);
//		if (IS_Json(val)) {
//			json_object_object_add(obj, key, val);
//		} else {
//			DBG_P("[WARNING] Json set cannnot set target object");
//		}
//	} else {
//		DBG_P("[ERROR] Json set this object is error");
//	}
//	RETURNvoid_();
//}
//
////## Json Map.toJson();
//KMETHOD Map_toJson(CTX ctx, ksfp_t *sfp _RIX)
//{
//	//kMap *map = sfp[0].m;
//	json_object *json = json_object_new_object();
//	kRawPtr *ptr = new_Json(ctx, json);
//	//map_traverse_init(json);
//	//map->dspi->ftrmap(ctx, map->map, map_traverse);
//	RETURN_(ptr);
//}
//
///* ------------------------------------------------------------------------ */
//static int _isFirstRead = 1;
//static json_object *_json;
//
//KMETHOD Map_toJsonString(CTX ctx, ksfp_t *sfp _RIX)
//{
//	//kMap *map = sfp[0].m;
//	if (unlikely(_isFirstRead)) {
//		_json = json_object_new_object();
//	}
//	//map_traverse_init(_json);
//	//map->dspi->ftrmap(ctx, map->map, map_traverse);
//	char *buf = (char*)json_object_to_json_string(_json);
//	RETURN_(new_String(ctx, buf));
//}

/* ------------------------------------------------------------------------ */

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _Im kMethod_Immutable
#define _F(F)   (intptr_t)(F)

#define CT_Json     cJson
#define TY_Json     cJson->cid
#define IS_Json(O)  ((O)->h.ct == CT_Json)

#define _KVi(T)  #T, TY_Int, T

static	kbool_t json_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	KDEFINE_CLASS defJson = {
		STRUCTNAME(Json),
		.cflag = kClass_Final,
		.init = Json_init,
		.free = Json_free,
	};
	kclass_t *cJson = Konoha_addClassDef(ks->packid, ks->packdom, NULL, &defJson, pline);

	intptr_t MethodData[] = {
		_Public|_Const|_Im, _F(Json_new), TY_Json, TY_Json, MN_("new"), 0,
		_Public|_Const|_Im, _F(Json_parse), TY_Json, TY_Json, MN_("parse"), 1, TY_String, FN_("data"),
		DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);
	return true;
}

static kbool_t json_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t json_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static kbool_t json_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}
/* ======================================================================== */

#endif /* JSON_GLUE_H_ */

#ifdef __cplusplus
}
#endif
