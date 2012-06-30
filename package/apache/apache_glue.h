#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>

#ifndef APACHE_GLUE_H
#define APACHE_GLUE_H
#define MOD_APACHE 23
#define kapacheshare ((kapacheshare_t*)_ctx->modshare[MOD_APACHE])
#define CT_Request   kapacheshare->cRequest

typedef struct kRequest {
	kObjectHeader h;
	request_rec *r;
} kRequest;

typedef struct {
	kmodshare_t h;
	kclass_t *cRequest;
} kapacheshare_t;

typedef struct {
	kmodlocal_t h;
	//kRequest *req;
} ctxapache_t;

#endif /* end of include guard */
