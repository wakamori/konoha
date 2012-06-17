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
#include <jansson.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef const struct _kJson kJson;
struct _kJson {
	kObjectHeader h;
	json_t *obj;
};

static void Json_init(CTX, kObject *o, void *conf)
{
	struct _kJson *json = (struct _kJson *)o;
	json->obj = json_object();
	json_incref(json->obj);
}

static void Json_free(CTX, kObject *o)
{
	struct _kJson *json = (struct _kJson *)o;
	if(json->obj != NULL) {
		json_decref(json->obj);
		json->obj = NULL;
	}
}

static void Json_p(CTX, ksfp_t *sfp, int pos, kwb_t *wb, int level)
{
	struct _kJson *json = (struct _kJson *)sfp[pos].o;
	char* data = json_dumps(json->obj, JSON_ENSURE_ASCII);
	kwb_printf(wb, "%s", data);
	free(data);
}

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _Im kMethod_Immutable
#define _F(F)   (intptr_t)(F)

#define CT_Json     cJson
#define TY_Json     cJson->cid
#define IS_Json(O)  ((O)->h.ct == CT_Json)

#define _KVi(T)  #T, TY_Int, T

///* ------------------------------------------------------------------------ */
///* [API methods] */
//
//## Json Json.new();
static KMETHOD Json_new (CTX, ksfp_t *sfp _RIX)
{
	RETURN_(new_kObject(O_ct(sfp[K_RTNIDX].o), NULL));
}

//## @Static Json Json.parse(String str);
static KMETHOD Json_parse(CTX, ksfp_t *sfp _RIX)
{
	struct _kJson *json = (struct _kJson*)sfp[0].o;
	const char *buf = S_text(sfp[1].s);
	json_t* obj;
	json_error_t err;
	obj = json_loads(buf, 0, &err);
	if (!obj) {
		//fprintf(stderr, "ERROR: line %d: %s\n", err.line, err.text);
		DBG_P("ERROR: line %d: %s", err.line, err.text);
		RETURN_(K_NULL);
	}
	obj = json_incref(obj);
	json->obj = obj;
	RETURN_(json);
}

//## String Json.get(String key);
static KMETHOD Json_get(CTX, ksfp_t *sfp _RIX)
{
	json_t* obj = ((struct _kJson*)sfp[0].o)->obj;
	if (!json_is_object(obj)) {
		RETURN_(K_NULL);
	}
	const char *key = S_text(sfp[1].s);
	json_t* ret = json_object_get(obj, key);
	if (!json_is_object(ret)) {
		RETURN_(K_NULL);
	}
	ret = json_incref(ret);
	//fprintf(stderr, "key = '%s'\n", key);
	struct _kJson *json = (struct _kJson*)new_kObject(O_ct(sfp[K_RTNIDX].o), NULL);
	json->obj = ret;
	RETURN_(json);
}

//## String Json.getInt(String key);
static KMETHOD Json_getInt(CTX, ksfp_t *sfp _RIX)
{
	json_t* obj = ((struct _kJson*)sfp[0].o)->obj;
	if (!json_is_object(obj)) {
		RETURNi_(-1);
	}
	const char *key = S_text(sfp[1].s);
	json_t* ret = json_object_get(obj, key);
	if (!json_is_integer(ret)) {
		RETURN_(K_NULL);
	}
	json_int_t val = json_integer_value(ret);
	//fprintf(stderr, "key = '%s'\n", key);
	//fprintf(stderr, "ret='%lld'\n", val);
	RETURNi_((kint_t)val);
}

static KMETHOD Json_getArray(CTX, ksfp_t *sfp _RIX)
{
	json_t* obj = ((struct _kJson*)sfp[0].o)->obj;
	if (!json_is_object(obj)) {
		RETURN_(K_NULL);
	}
	const char *key = S_text(sfp[1].s);
	json_t* ja = json_object_get(obj, key);
	if (!json_is_array(ja)) {
		RETURN_(K_NULL);
	}
	ja = json_incref(ja);
	int i;
	struct _kArray* a = (struct _kArray*)new_kObject(CT_Array, NULL);
	size_t size = json_array_size(ja);
	//a->bytemax = size * sizeof(void*);
	//kArray_setsize((kArray*)a, size);
	//a->list = (kObject**)KCALLOC(a->bytemax, size);
	for (i = 0; i < size; i++) {
		struct _kJson* json = (struct _kJson*)new_kObject(O_ct(sfp[0].o), NULL);
		json->obj = json_array_get(ja, i);
		json_incref(json->obj);
		printf("json='%s'\n", json_dumps(json->obj, JSON_ENSURE_ASCII));
		kArray_add(a, json);
	}
	//fprintf(stderr, "key = '%s'\n", key);
	//fprintf(stderr, "ret='%s'\n", str);
	RETURN_(a);
}

//## String Json.getString(String key);
static KMETHOD Json_getString(CTX, ksfp_t *sfp _RIX)
{
	json_t* obj = ((struct _kJson*)sfp[0].o)->obj;
	if (!json_is_object(obj)) {
		RETURN_(K_NULL);
	}
	const char *key = S_text(sfp[1].s);
	json_t* ret = json_object_get(obj, key);
	if (!json_is_string(ret)) {
		RETURN_(K_NULL);
	}
	ret = json_incref(ret);
	const char* str = json_string_value(ret);
	//fprintf(stderr, "key = '%s'\n", key);
	//fprintf(stderr, "ret='%s'\n", str);
	RETURN_(new_kString(str, strlen(str), 0));
}

//## String Json.getBool(String key);
static KMETHOD Json_getBool(CTX, ksfp_t *sfp _RIX)
{
	json_t* obj = ((struct _kJson*)sfp[0].o)->obj;
	if (!json_is_object(obj)) {
		RETURN_(K_NULL);
	}
	const char *key = S_text(sfp[1].s);
	json_t* ret = json_object_get(obj, key);
	ret = json_incref(ret);
	//fprintf(stderr, "key = '%s'\n", key);
	if (json_is_true(ret)) {
		//fprintf(stderr, "ret='true'\n");
		RETURNb_(K_TRUE);
	}
	else if (json_is_false(ret)) {
		//fprintf(stderr, "ret='false'\n");
		RETURNb_(K_FALSE);
	}
	else {
		RETURNb_(K_NULL);
	}
}

//## String Json.getFloat(String key);
static KMETHOD Json_getFloat(CTX, ksfp_t *sfp _RIX)
{
	json_t* obj = ((struct _kJson*)sfp[0].o)->obj;
	if (!json_is_object(obj)) {
		RETURNf_(-1);
	}
	const char *key = S_text(sfp[1].s);
	json_t* ret = json_object_get(obj, key);
	if (!json_is_real(ret)) {
		RETURN_(K_NULL);
	}
	ret = json_incref(ret);
	double val = json_real_value(ret);
	//fprintf(stderr, "key = '%s'\n", key);
	//fprintf(stderr, "ret='%lf'\n", val);
	RETURNf_(val);
}

//## void Json.set(String key, Json value);
static KMETHOD Json_set(CTX, ksfp_t *sfp _RIX)
{
	json_t* obj = ((struct _kJson*)sfp[0].o)->obj;
	if (!json_is_object(obj)) {
		DBG_P("ERROR: Object is not Json object.");
		RETURNvoid_();
	}
	const char *key = S_text(sfp[1].s);
	json_t* val = ((struct _kJson*)sfp[2].o)->obj;
	if (!json_is_object(val)) {
		DBG_P("ERROR: Value is not Json object.");
		RETURNvoid_();
	}
	int ret = json_object_set(obj, key, val);
	if (ret < 0) {
		DBG_P("[WARNING] Json set cannnot set target object");
	}
	RETURNvoid_();
}

//## void Json.setString(String key, String value);
static KMETHOD Json_setString(CTX, ksfp_t *sfp _RIX)
{
	json_t* obj = ((struct _kJson*)sfp[0].o)->obj;
	if (!json_is_object(obj)) {
		DBG_P("ERROR: Object is not Json object.");
		RETURNvoid_();
	}
	const char *key = S_text(sfp[1].s);
	const char *sval = S_text(sfp[2].s);
	//fprintf(stderr, "key='%s'\n", key);
	//fprintf(stderr, "val='%s'\n", sval);
	json_t* val = json_string(sval);
	if (!json_is_string(val)) {
		DBG_P("ERROR: Value is not Json object.");
		RETURNvoid_();
	}
	int ret = json_object_set(obj, key, val);
	//fprintf(stderr, "ret=%d\n", ret);
	if (ret < 0) {
		DBG_P("[WARNING] Json set cannnot set target object");
	}
	RETURNvoid_();
}

//## void Json.setInt(String key, int value);
static KMETHOD Json_setInt(CTX, ksfp_t *sfp _RIX)
{
	json_t* obj = ((struct _kJson*)sfp[0].o)->obj;
	if (!json_is_object(obj)) {
		DBG_P("ERROR: Object is not Json object.");
		RETURNvoid_();
	}
	const char *key = S_text(sfp[1].s);
	kint_t ival = sfp[2].ivalue;
	json_t* val = json_integer((json_int_t)ival);
	if (!json_is_integer(val)) {
		DBG_P("ERROR: Value is not Json object.");
		RETURNvoid_();
	}
	int ret = json_object_set(obj, key, val);
	if (ret < 0) {
		DBG_P("[WARNING] Json set cannnot set target object");
	}
	RETURNvoid_();
}

//## void Json.setFloat(String key, String value);
static KMETHOD Json_setFloat(CTX, ksfp_t *sfp _RIX)
{
	json_t* obj = ((struct _kJson*)sfp[0].o)->obj;
	if (!json_is_object(obj)) {
		DBG_P("ERROR: Object is not Json object.");
		RETURNvoid_();
	}
	const char *key = S_text(sfp[1].s);
	float fval = sfp[2].fvalue;
	json_t* val = json_real(fval);
	if (!json_is_real(val)) {
		DBG_P("ERROR: Value is not Json object.");
		RETURNvoid_();
	}
	int ret = json_object_set(obj, key, val);
	if (ret < 0) {
		DBG_P("[WARNING] Json set cannnot set target object");
	}
	RETURNvoid_();
}

//## void Json.setBool(String key, String value);
static KMETHOD Json_setBool(CTX, ksfp_t *sfp _RIX)
{
	json_t* obj = ((struct _kJson*)sfp[0].o)->obj;
	if (!json_is_object(obj)) {
		DBG_P("ERROR: Object is not Json object.");
		RETURNvoid_();
	}
	const char *key = S_text(sfp[1].s);
	kbool_t bval = sfp[2].bvalue;
	json_t* val;
	if (bval) {
		val = json_true();
	}
	else {
		val = json_false();
	}
	int ret = json_object_set(obj, key, val);
	if (ret < 0) {
		DBG_P("[WARNING] Json set cannnot set target object");
	}
	RETURNvoid_();
}

//## String Json.dump();
static KMETHOD Json_dump(CTX, ksfp_t *sfp _RIX)
{
	json_t* obj = ((struct _kJson*)sfp[0].o)->obj;
	if (!json_is_object(obj)) {
		DBG_P("ERROR: Object is not Json object.");
		RETURNvoid_();
	}
	char* data = json_dumps(obj, JSON_ENSURE_ASCII);
	RETURN_(new_kString(data, strlen(data), 0));
}
/* ------------------------------------------------------------------------ */

static	kbool_t json_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	KREQUIRE_PACKAGE("konoha.float", pline);
	//KREQUIRE_PACKAGE("konoha.string", pline);
	KDEFINE_CLASS defJson = {
		STRUCTNAME(Json),
		.cflag = kClass_Final,
		.init = Json_init,
		.free = Json_free,
		.p    = Json_p,
	};
	kclass_t *cJson = Konoha_addClassDef(ks->packid, ks->packdom, NULL, &defJson, pline);
	struct _kclass *ct = (struct _kclass *)CT_Json;
	//ct->p0 = TY_String; // default

	kparam_t ps = {TY_Json, FN_("json")};
	kclass_t *CT_JsonArray2 = kClassTable_Generics(CT_Array, TY_Json, 1, &ps);
	kcid_t TY_JsonArray = CT_JsonArray2->cid;

	intptr_t MethodData[] = {
		_Public|_Const|_Im, _F(Json_new),       TY_Json,    TY_Json, MN_("new"),          0,
		_Public|_Const|_Im, _F(Json_parse),     TY_Json,    TY_Json, MN_("parse"),        1, TY_String, FN_("data"),
		_Public|_Const|_Im, _F(Json_get),       TY_Json,    TY_Json, MN_("get"),          1, TY_String, FN_("key"),
		_Public|_Const|_Im, _F(Json_getInt),    TY_Int,     TY_Json, MN_("getInt"),       1, TY_String, FN_("key"),
		_Public|_Const|_Im, _F(Json_getBool),   TY_Boolean, TY_Json, MN_("getBool"),      1, TY_String, FN_("key"),
		_Public|_Const|_Im, _F(Json_getFloat),  TY_Float,   TY_Json, MN_("getFloat"),     1, TY_String, FN_("key"),
		_Public|_Const|_Im, _F(Json_getString), TY_String,  TY_Json, MN_("getString"),    1, TY_String, FN_("key"),
		_Public|_Const|_Im, _F(Json_getArray),  TY_JsonArray,   TY_Json, MN_("getArray"), 1, TY_String, FN_("key"),
		_Public|_Const|_Im, _F(Json_set),       TY_void,    TY_Json, MN_("set"),          2, TY_String, FN_("key"), TY_Json, FN_("value"),
		_Public|_Const|_Im, _F(Json_setInt),    TY_void,    TY_Json, MN_("setInt"),       2, TY_String, FN_("key"), TY_Int, FN_("value"),
		_Public|_Const|_Im, _F(Json_setBool),   TY_void,    TY_Json, MN_("setBool"),      2, TY_String, FN_("key"), TY_Boolean, FN_("value"),
		_Public|_Const|_Im, _F(Json_setFloat),  TY_void,    TY_Json, MN_("setFloat"),     2, TY_String, FN_("key"), TY_Float, FN_("value"),
		_Public|_Const|_Im, _F(Json_setString), TY_void,    TY_Json, MN_("setString"),    2, TY_String, FN_("key"), TY_String, FN_("value"),
		_Public|_Const|_Im, _F(Json_dump),      TY_String,  TY_Json, MN_("dump"),         0,
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
	//KEXPORT_PACKAGE("konoha.string", ks,  pline);
	//KEXPORT_PACKAGE("konoha.float", ks, pline);
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
