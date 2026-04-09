#include <R.h>
#include <Rinternals.h>
#include <string.h>

#include "legacy/gfa.h"

static SEXP get_list_item(SEXP list, const char *name) {
	SEXP names;
	R_xlen_t i;
	if (TYPEOF(list) != VECSXP) {
		return R_NilValue;
	}
	names = getAttrib(list, R_NamesSymbol);
	if (TYPEOF(names) != STRSXP) {
		return R_NilValue;
	}
	for (i = 0; i < XLENGTH(list); i++) {
		const char *current = CHAR(STRING_ELT(names, i));
		if (strcmp(current, name) == 0) {
			return VECTOR_ELT(list, i);
		}
	}
	return R_NilValue;
}

static int scalar_to_int(SEXP value, int fallback) {
	if (value == R_NilValue || XLENGTH(value) == 0) {
		return fallback;
	}
	switch (TYPEOF(value)) {
	case INTSXP:
		if (INTEGER(value)[0] == NA_INTEGER) return fallback;
		return INTEGER(value)[0];
	case REALSXP:
		if (ISNA(REAL(value)[0])) return fallback;
		return (int) REAL(value)[0];
	case LGLSXP:
		if (LOGICAL(value)[0] == NA_LOGICAL) return fallback;
		return LOGICAL(value)[0] ? 1 : 0;
	default:
		return fallback;
	}
}

#define READ_OPTION_INT(name) do { \
	SEXP v = get_list_item(options_list, #name); \
	if (v != R_NilValue) request.options.name = scalar_to_int(v, request.options.name); \
} while (0)

#define READ_FLAG_BOOL(name) do { \
	SEXP v = get_list_item(flags_list, #name); \
	if (v != R_NilValue) request.flags.name = scalar_to_int(v, request.flags.name) ? TRUE : FALSE; \
} while (0)

SEXP c_nonbgfa_run(SEXP seq_file, SEXP out_prefix, SEXP chrom, SEXP options_list,
		SEXP flags_list) {
	GFARunRequest request;
	char error_message[256];
	int code;
	SEXP result;
	SEXP names;
	SEXP code_sexp;
	SEXP message_sexp;

	if (TYPEOF(seq_file) != STRSXP || XLENGTH(seq_file) != 1) {
		error("seq_file must be a single string");
	}
	if (TYPEOF(out_prefix) != STRSXP || XLENGTH(out_prefix) != 1) {
		error("out_prefix must be a single string");
	}
	if (chrom != R_NilValue && (TYPEOF(chrom) != STRSXP || XLENGTH(chrom) != 1)) {
		error("chrom must be NULL or a single string");
	}

	memset(&request, 0, sizeof(request));
	gfa_init_default_options(&request.options);
	gfa_init_default_flags(&request.flags);

	request.seq_filename = CHAR(STRING_ELT(seq_file, 0));
	request.out_prefix = CHAR(STRING_ELT(out_prefix, 0));
	request.chrom = (chrom == R_NilValue) ? NULL : CHAR(STRING_ELT(chrom, 0));
	request.flags.DO_WGET = FALSE;
	request.flags.DO_CHMOD = FALSE;

	READ_OPTION_INT(minGQrep);
	READ_OPTION_INT(maxGQspacer);
	READ_OPTION_INT(minMRrep);
	READ_OPTION_INT(maxMRspacer);
	READ_OPTION_INT(minIRrep);
	READ_OPTION_INT(maxIRspacer);
	READ_OPTION_INT(shortIRcut);
	READ_OPTION_INT(shortIRspacer);
	READ_OPTION_INT(maxDRspacer);
	READ_OPTION_INT(minDRrep);
	READ_OPTION_INT(maxDRrep);
	READ_OPTION_INT(minATracts);
	READ_OPTION_INT(minATractSep);
	READ_OPTION_INT(maxATractSep);
	READ_OPTION_INT(maxAPRlen);
	READ_OPTION_INT(minAPRlen);
	READ_OPTION_INT(minZlen);
	READ_OPTION_INT(minSTR);
	READ_OPTION_INT(maxSTR);
	READ_OPTION_INT(minSTRreps);
	READ_OPTION_INT(minSTRbp);
	READ_OPTION_INT(minCruciformRep);
	READ_OPTION_INT(maxCruciformSpacer);
	READ_OPTION_INT(minTriplexYRpercent);
	READ_OPTION_INT(maxTriplexSpacer);
	READ_OPTION_INT(maxSlippedSpacer);
	READ_OPTION_INT(minKVscore);

	READ_FLAG_BOOL(DO_findSTR);
	READ_FLAG_BOOL(DO_findDR);
	READ_FLAG_BOOL(DO_findMR);
	READ_FLAG_BOOL(DO_findGQ);
	READ_FLAG_BOOL(DO_findIR);
	READ_FLAG_BOOL(DO_findZ);
	READ_FLAG_BOOL(DO_findAPR);
	READ_FLAG_BOOL(DO_Cruciform);
	READ_FLAG_BOOL(DO_Triplex);
	READ_FLAG_BOOL(DO_Slipped);
	READ_FLAG_BOOL(DO_KVzdna);
	READ_FLAG_BOOL(DO_WGET);
	READ_FLAG_BOOL(DO_CHMOD);
	READ_FLAG_BOOL(KEEP_TIME);

	code = gfa_run_request(&request, error_message, sizeof(error_message));

	PROTECT(result = allocVector(VECSXP, 2));
	PROTECT(names = allocVector(STRSXP, 2));
	PROTECT(code_sexp = ScalarInteger(code));
	PROTECT(message_sexp = mkString(error_message));

	SET_VECTOR_ELT(result, 0, code_sexp);
	SET_VECTOR_ELT(result, 1, message_sexp);
	SET_STRING_ELT(names, 0, mkChar("code"));
	SET_STRING_ELT(names, 1, mkChar("message"));
	setAttrib(result, R_NamesSymbol, names);

	UNPROTECT(4);
	return result;
}
