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

#ifndef ARRAY_GLUE_H_
#define ARRAY_GLUE_H_

/* ------------------------------------------------------------------------ */

//## @Immutable method T0 Array.get(Int n);
static KMETHOD Array_get(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	size_t n = check_index(_ctx, sfp[1].ivalue, kArray_size(a), sfp[K_RTNIDX].uline);
	if(kArray_isUnboxData(a)) {
		RETURNd_(a->ndata[n]);
	}
	else {
		RETURN_(a->list[n]);
	}
}

//## method void Array.set(Int n, T0 v);
static KMETHOD Array_set(CTX, ksfp_t *sfp _RIX)
{
	kArray *a = sfp[0].a;
	size_t n = check_index(_ctx, sfp[1].ivalue, kArray_size(a), sfp[K_RTNIDX].uline);
	if(kArray_isUnboxData(a)) {
		a->ndata[n] = sfp[2].ndata;
	}
	else {
		KSETv(a->list[n], sfp[2].o);
	}
}

// --------------------------------------------------------------------------

#define _Public   kMethod_Public
#define _Const    kMethod_Const
#define _Coercion kMethod_Coercion
#define _Im       kMethod_Immutable
#define _F(F)     (intptr_t)(F)

static	kbool_t array_initPackage(CTX, kKonohaSpace *ks, int argc, const char**args, kline_t pline)
{
	int FN_x = FN_("x"), FN_n = FN_("n");
	intptr_t MethodData[] = {
		_Public|_Im,     _F(Array_get), TY_T0,   TY_Array, MN_("get"), 1, TY_Int, FN_n,
		_Public,         _F(Array_set), TY_void, TY_Array, MN_("set"), 2, TY_Int, FN_n,  TY_T0, FN_x,
		DEND,
	};
	kKonohaSpace_loadMethodData(ks, MethodData);
	return true;
}

static kbool_t array_setupPackage(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

static KMETHOD ParseExpr_BRANCET(CTX, ksfp_t *sfp _RIX)
{
	USING_SUGAR;
	VAR_ParseExpr(stmt, syn, tls, s, c, e);
	kToken *tk = tls->toks[c];
	if(s == c) {
		kExpr *expr = SUGAR Stmt_newExpr2(_ctx, stmt, tk->sub, 0, kArray_size(tk->sub));
		RETURN_(SUGAR Expr_rightJoin(_ctx, expr, stmt, tls, s+1, c+1, e));
	}
	else {
		kExpr *lexpr = SUGAR Stmt_newExpr2(_ctx, stmt, tls, s, c);
		if(lexpr == K_NULLEXPR) {
			RETURN_(lexpr);
		}
//		if(lexpr->syn->kw == KW_NEW) {  // new int[100]
//			lexpr = Stmt_addExprParams(_ctx, stmt, lexpr, tk->sub, 0, kArray_size(tk->sub));
//			RETURN_(kExpr_rightJoin(lexpr, stmt, tls, s+1, c+1, e));
//		}
//		else {   // X[1] => get X 1
			struct _kToken *tkN = new_W(Token, 0);
			tkN->tt = TK_MN; tkN->mn = MN_toGETTER(0);
			tkN->uline = tk->uline;
			syn = SYN_(kStmt_ks(stmt), KW_ExprMethodCall);
			lexpr  = SUGAR new_ConsExpr(_ctx, syn, 2, tkN, lexpr);
//		}
		lexpr = SUGAR Stmt_addExprParams(_ctx, stmt, lexpr, tk->sub, 0, kArray_size(tk->sub));
		RETURN_(SUGAR Expr_rightJoin(_ctx, lexpr, stmt, tls, s+1, c+1, e));
	}
}

static kbool_t array_initKonohaSpace(CTX,  kKonohaSpace *ks, kline_t pline)
{
	USING_SUGAR;
	KDEFINE_SYNTAX SYNTAX[] = {
		//{ TOKEN("()"), _EXPR, ParseExpr_(PARENTHESIS), .op2 = "*", .priority_op2 = 16, .right = 1, ExprTyCheck_(FuncStyleCall),}, //AST_PARENTHESIS
		{ TOKEN("[]"), ParseExpr_(BRANCET), /*_EXPR,*/ },  //AST_BRANCET
//		{ TOKEN("array"), .type = TY_Float, },
//		{ TOKEN("double"), .type = TY_Float, },
//		{ TOKEN("$FLOAT"), .kw = KW_TK(TK_FLOAT), .ExprTyCheck = ExprTyCheck_FLOAT, },
		{ .name = NULL, },
	};
	SUGAR KonohaSpace_defineSyntax(_ctx, ks, SYNTAX);
	return true;
}

static kbool_t array_setupKonohaSpace(CTX, kKonohaSpace *ks, kline_t pline)
{
	return true;
}

#endif /* ARRAY_GLUE_H_ */