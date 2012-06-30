/* 
 * mod_konoha
 *
 * This is a konoha module for Apache HTTP Server.
 *
 * ## Settings
 * Add to /path/to/httpd.conf
 *
 * FIXME: Current version of mod_konoha are not able
 *        to set KonohaPackageDir.
 * If you want to specify package dir for konoha,
 * use 'KonohaPackageDir'.
 *   LoadModule konoha_module modules/mod_konoha.so
 *   AddHandler konoha-script .k
 *   KonohaPackageDir /path/to/konoha/package
 *
 * Then after restarting Apache via
 *   $ apachectl restart
 *
 * void Request.puts(String x);
 */

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"

#include "apr_strings.h"
#include "http_log.h"
#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>
#include "../../package/apache/apache_glue.h"

#ifndef K_PREFIX
#define K_PREFIX  "/usr/local"
#endif

typedef struct konoha_config {
	const char *package_dir;
} konoha_config_t;

module AP_MODULE_DECLARE_DATA konoha_module;
static const char *apache_package_path = NULL;

static const char* packname(const char *str)
{
	char *p = strrchr(str, '.');
	return (p == NULL) ? str : (const char*)p+1;
}

static const char* shell_packagepath(char *buf, size_t bufsiz, const char *fname)
{
	char *path = getenv("KONOHA_PACKAGEPATH"), *local = "";
	if(path == NULL) {
		path = getenv("KONOHA_HOME");
		local = "/package";
	}
	if(path == NULL) {
		path = getenv("HOME");
		local = "/.konoha2/package";
	}
	snprintf(buf, bufsiz, "%s%s/%s/%s_glue.k", path, local, fname, packname(fname));
#ifdef K_PREFIX
	FILE *fp = fopen(buf, "r");
	if(fp != NULL) {
		fclose(fp);
	}
	else {
		snprintf(buf, bufsiz, K_PREFIX "/konoha2/package" "/%s/%s_glue.k", fname, packname(fname));
	}
#endif
	return (const char*)buf;
}

static const char* apache_packagepath(char *buf, size_t bufsiz, const char *fname)
{
	if (apache_package_path) {
		snprintf(buf, bufsiz, "%s/%s/%s_glue.k", apache_package_path, fname, packname(fname));
		FILE *fp = fopen(buf, "r");
		if(fp != NULL) {
			fclose(fp);
			return buf;
		}
	}
	return shell_packagepath(buf, bufsiz, fname);
}

static const char* shell_exportpath(char *buf, size_t bufsiz, const char *pname)
{
	char *p = strrchr(buf, '/');
	snprintf(p, bufsiz - (p  - buf), "/%s_exports.k", packname(pname));
	FILE *fp = fopen(buf, "r");
	if(fp != NULL) {
		fclose(fp);
		return (const char*)buf;
	}
	return NULL;
}

static const char* begin(kinfotag_t t) { (void)t; return ""; }
static const char* end(kinfotag_t t) { (void)t; return ""; }

static void dbg_p(const char *file, const char *func, int L, const char *fmt, ...)
{
	va_list ap;
	va_start(ap , fmt);
	fflush(stdout);
	fprintf(stderr, "DEBUG(%s:%s:%d) ", file, func, L);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
}


static const kplatform_t apache_platform = {
	.name        = "apache",
	.stacksize   = K_PAGESIZE,
	.malloc_i    = malloc,
	.free_i      = free,
	.setjmp_i    = ksetjmp,
	.longjmp_i   = klongjmp,
	.realpath_i  = realpath,
	.fopen_i     = (FILE_i* (*)(const char*, const char*))fopen,
	.fgetc_i     = (int     (*)(FILE_i *))fgetc,
	.feof_i      = (int     (*)(FILE_i *))feof,
	.fclose_i    = (int     (*)(FILE_i *))fclose,
	.syslog_i    = syslog,
	.vsyslog_i   = vsyslog,
	.printf_i    = printf,
	.vprintf_i   = vprintf,
	.snprintf_i  = snprintf,
	.vsnprintf_i = vsnprintf,
	.qsort_i     = qsort,
	.exit_i      = exit,
	.packagepath = apache_packagepath,
	.exportpath  = shell_exportpath,
	.begin       = begin,
	.end         = end,
	.dbg_p       = dbg_p,
};

static KMETHOD Request_puts(CTX, ksfp_t *sfp _RIX)
{
	kRequest *self = (kRequest *) sfp[0].o;
	kString *data = sfp[1].s;
	ap_rputs(S_text(data), self->r);
	RETURNvoid_();
}


konoha_t konoha_create(kclass_t **cRequest)
{
	konoha_t konoha = konoha_open(&apache_platform);
	CTX_t _ctx = konoha;
	kKonohaSpace *ks = KNULL(KonohaSpace);
	KREQUIRE_PACKAGE("apache", 0);
	*cRequest = CT_Request;
#define _P    kMethod_Public
#define _F(F) (intptr_t)(F)
#define TY_R  (CT_Request->cid)
	int FN_x = FN_("x");
	intptr_t MethodData[] = {
		_P, _F(Request_puts), TY_void, TY_R, MN_("puts"), 1, TY_String, FN_x,
		DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);
	return konoha;
}

static int konoha_handler(request_rec *r)
{
	//konoha_config_t *conf = ap_get_module_config(
	//		r->server->module_config, &konoha_module);
	if (strcmp(r->handler, "konoha-script")) {
		return DECLINED;
	}
	if (r->method_number != M_GET) {
		/* TODO */
		return HTTP_METHOD_NOT_ALLOWED;
	}
	kclass_t *cRequest;
	konoha_t konoha = konoha_create(&cRequest);
	//assert(cRequest != NULL);
	r->content_encoding = "utf-8";
	if (!konoha_load(konoha, r->filename)) {
		return DECLINED;
	}

	CTX_t _ctx = (CTX_t) konoha;
	kKonohaSpace *ks = KNULL(KonohaSpace);
	kMethod *mtd = kKonohaSpace_getMethodNULL(ks, TY_System, MN_("handler"));
	if (mtd == NULL) {
		ap_log_rerror(APLOG_MARK, APLOG_CRIT, 0, r, "System.handler() not found");
		return -1;
	}

	/* XXX: We assume Request Object may not be freed by GC */
	kObject *req_obj = new_kObject(cRequest, (void*)r);
	BEGIN_LOCAL(lsfp, K_CALLDELTA + 1);
	KSETv(lsfp[K_CALLDELTA+0].o, K_NULL);
	KSETv(lsfp[K_CALLDELTA+1].o, req_obj);
	KCALL(lsfp, 0, mtd, 1, knull(CT_Int));
	END_LOCAL();
	return lsfp[0].ivalue;
}

static int mod_konoha_init(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s)
{
	/* TODO: Create Global Instance to share constants objects */
	(void)p;(void)plog;(void)ptemp;(void)s;
	return 0;
}

static void mod_konoha_child_init(apr_pool_t *pool, server_rec *server)
{
	/* TODO: Create VM Instance per child process */
	(void)pool;(void)server;
	//konoha_config_t *conf = (konoha_config_t *) ap_get_module_config(
	//		server->module_config, &konoha_module);
	//konoha_t konoha = konoha_open(&apache_platform);
}

static const char *set_package_dir(cmd_parms *cmd, void *vp, const char *arg)
{
	(void)vp;
	const char *err = ap_check_cmd_context(cmd, NOT_IN_FILES | NOT_IN_LIMIT);
	konoha_config_t *conf = (konoha_config_t *)
		ap_get_module_config(cmd->server->module_config, &konoha_module);
	if (err != NULL) {
		return err;
	}
	if (arg) {
		conf->package_dir = apr_pstrdup(cmd->pool, arg);
	}
	return NULL;
}

/* konoha commands */
static const command_rec konoha_cmds[] = {
	AP_INIT_TAKE1("KonohaPackageDir",
			set_package_dir,
			NULL,
			OR_ALL,
			"set konoha package path"),
	{ NULL, {NULL}, NULL, 0, RAW_ARGS, NULL }
};

/* konoha register hooks */
static void konoha_register_hooks(apr_pool_t *p)
{
	(void)p;
	ap_hook_post_config(mod_konoha_init, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_child_init(mod_konoha_child_init, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_handler(konoha_handler, NULL, NULL, APR_HOOK_FIRST);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA konoha_module = {
	STANDARD20_MODULE_STUFF,
	NULL,       /* create per-dir    config structures */
	NULL,       /* merge  per-dir    config structures */
	NULL,       /* create per-server config structures */
	NULL,       /* merge  per-server config structures */
	konoha_cmds,/* table of config file commands       */
	konoha_register_hooks  /* register hooks           */
};

