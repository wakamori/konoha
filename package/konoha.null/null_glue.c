#include <konoha2/konoha2.h>
#include <konoha2/sugar.h>
#include "null_glue.h"

KDEFINE_PACKAGE* null_init(void)
{
	static KDEFINE_PACKAGE d = {
		KPACKNAME("null", "1.0"),
		.initPackage = null_initPackage,
		.setupPackage = null_setupPackage,
		.initNameSpace = null_initNameSpace,
		.setupNameSpace = null_setupNameSpace,
	};
	return &d;
}
