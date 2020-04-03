/* libcablintpq - embed Ruby and run cablint from a PostgreSQL function
 * Written by Rob Stradling
 * Copyright (C) 2016-2020 Sectigo Limited
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "postgres.h"
#include "funcapi.h"
#include "fmgr.h"

#include <ruby.h>
#include <stdlib.h>
#include <string.h>


#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif


typedef struct ruby_shared_data {
	VALUE obj;
	ID method_id;
	int nargs;
	VALUE args[4];
} ruby_shared_data;


/******************************************************************************
 * rescue_require()                                                           *
 ******************************************************************************/
VALUE rescue_require(
	VALUE data,
	VALUE err
)
{
	fprintf(stderr, "Error: [[[%s]]]\n",
		RSTRING(rb_obj_as_string(err))->as.heap.ptr);
	return Qnil;
}


/******************************************************************************
 * sure_require()                                                             *
 ******************************************************************************/
void sure_require(
	char* name
)
{
	rb_rescue2(
		&rb_require, (VALUE)name, &rescue_require, Qnil, rb_eLoadError,
		(VALUE)0
	);
}


/******************************************************************************
 * _PG_init()                                                                 *
 ******************************************************************************/
void _PG_init(void)
{
	ruby_init();
	ruby_script("runcablint");
	ruby_init_loadpath();
	ruby_incpush("/usr/local");
	ruby_incpush("/usr/local/certlint/lib");
	ruby_incpush("/usr/local/certlint/ext");
	sure_require("runcablint.rb");
}


/******************************************************************************
 * _PG_fini()                                                                 *
 ******************************************************************************/
void _PG_fini(void)
{
	(void)ruby_cleanup(0);
}


/******************************************************************************
 * ruby_callback()                                                            *
 ******************************************************************************/
static VALUE ruby_callback(
	VALUE ptr
)
{
	ruby_shared_data* data = (ruby_shared_data*)ptr;

	return rb_funcall2(data->obj, data->method_id, data->nargs, data->args);
}


typedef struct tCablintCtx_st{
	VALUE m_result;
} tCablintCtx;


/******************************************************************************
 * cablint_embedded()                                                         *
 ******************************************************************************/
PG_FUNCTION_INFO_V1(cablint_embedded);
Datum cablint_embedded(
	PG_FUNCTION_ARGS
)
{
	tCablintCtx* t_cablintCtx;
	FuncCallContext* t_funcCtx;

	if (SRF_IS_FIRSTCALL()) {
		MemoryContext t_oldMemoryCtx;

		/* Create a function context for cross-call persistence */
		t_funcCtx = SRF_FIRSTCALL_INIT();
		/* Switch to memory context appropriate for multiple function
		  calls */
		t_oldMemoryCtx = MemoryContextSwitchTo(
			t_funcCtx->multi_call_memory_ctx
		);

		/* Allocate memory for our user-defined structure and initialize
		  it */
		t_funcCtx->user_fctx = t_cablintCtx
						= palloc(sizeof(tCablintCtx));
		memset(t_cablintCtx, '\0', sizeof(tCablintCtx));

		/* One-time setup code */
		if (!PG_ARGISNULL(0)) {
			ruby_shared_data rbdata;
			rbdata.obj = 0;
			rbdata.method_id = rb_intern("runcablint");
			rbdata.nargs = 1;
			bytea* t_bytea = PG_GETARG_BYTEA_P(0);
			rbdata.args[0] = rb_str_new(
				(char*)VARDATA(t_bytea),
				VARSIZE(t_bytea) - VARHDRSZ
			);

			VALUE t_result = t_cablintCtx->m_result = rb_rescue(
				&ruby_callback, (VALUE)&rbdata, &rescue_require,
				Qnil
			);

			if (TYPE(t_result) == RUBY_T_ARRAY)
				t_funcCtx->max_calls = RARRAY_LEN(t_result);
		}

		MemoryContextSwitchTo(t_oldMemoryCtx);
	}

	/* Each-time setup code */
	t_funcCtx = SRF_PERCALL_SETUP();
	t_cablintCtx = t_funcCtx->user_fctx;

	if (t_funcCtx->call_cntr < t_funcCtx->max_calls) {
		char* t_issue = RSTRING_PTR(
			RARRAY_PTR(t_cablintCtx->m_result)[t_funcCtx->call_cntr]
		);
		text* t_text = palloc(strlen(t_issue) + VARHDRSZ);
		SET_VARSIZE(t_text, strlen(t_issue) + VARHDRSZ);
		memcpy((void*)VARDATA(t_text), t_issue, strlen(t_issue));
		SRF_RETURN_NEXT(t_funcCtx, PointerGetDatum(t_text));
	}
	else
		SRF_RETURN_DONE(t_funcCtx);
}
