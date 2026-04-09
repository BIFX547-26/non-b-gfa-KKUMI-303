#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

extern SEXP c_nonbgfa_run(SEXP seq_file, SEXP out_prefix, SEXP chrom,
		SEXP options_list, SEXP flags_list);

static const R_CallMethodDef CallEntries[] = {
		{"c_nonbgfa_run", (DL_FUNC) &c_nonbgfa_run, 5},
		{NULL, NULL, 0}
};

void R_init_nonbGFA(DllInfo *dll) {
	R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
	R_useDynamicSymbols(dll, FALSE);
}
