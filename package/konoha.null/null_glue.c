#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>
#include "null_glue.h"

KDEFINE_PACKAGE* null_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("null", "1.0"),
		.initPackage = null_initPackage,
		.setupPackage = null_setupPackage,
		.initKonohaSpace = null_initKonohaSpace,
		.setupKonohaSpace = null_setupKonohaSpace,
	};
	return &d;
}
