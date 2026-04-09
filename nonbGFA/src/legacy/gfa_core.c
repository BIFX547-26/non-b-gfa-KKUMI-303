#include <stdio.h>
#include <string.h>

#include "gfa.h"

static void set_error(char *error_message, size_t error_message_len,
		const char *message) {
	if (error_message == NULL || error_message_len == 0) {
		return;
	}
	snprintf(error_message, error_message_len, "%s", message);
}

int gfa_run_request(const GFARunRequest *request, char *error_message,
		size_t error_message_len) {
	GFAOptions defaults;
	GFAFlags default_flags;
	char *argv[128];
	char value_bufs[32][32];
	int argc = 0;
	int value_i = 0;
	int code;

	if (request == NULL) {
		set_error(error_message, error_message_len, "request must not be NULL");
		return 22;
	}
	if (request->seq_filename == NULL || request->seq_filename[0] == '\0') {
		set_error(error_message, error_message_len,
				"request->seq_filename must not be empty");
		return 22;
	}
	if (request->out_prefix == NULL || request->out_prefix[0] == '\0') {
		set_error(error_message, error_message_len,
				"request->out_prefix must not be empty");
		return 22;
	}

	gfa_init_default_options(&defaults);
	gfa_init_default_flags(&default_flags);

	/* These fields currently have no CLI switches and cannot be overridden. */
	if (request->options.minSTRreps != defaults.minSTRreps) {
		set_error(error_message, error_message_len,
				"minSTRreps override is not supported by legacy CLI");
		return 22;
	}
	if (request->options.minKVscore != defaults.minKVscore) {
		set_error(error_message, error_message_len,
				"minKVscore override is not supported by legacy CLI");
		return 22;
	}
	if (request->flags.KEEP_TIME != default_flags.KEEP_TIME) {
		set_error(error_message, error_message_len,
				"KEEP_TIME override is not supported by legacy CLI");
		return 22;
	}

	argv[argc++] = "gfa";
	argv[argc++] = "-seq";
	argv[argc++] = (char *) request->seq_filename;
	argv[argc++] = "-out";
	argv[argc++] = (char *) request->out_prefix;

	if (request->chrom != NULL && request->chrom[0] != '\0') {
		argv[argc++] = "-chrom";
		argv[argc++] = (char *) request->chrom;
	}

#define ADD_INT_ARG(switch_name, value) \
	do { \
		argv[argc++] = (switch_name); \
		snprintf(value_bufs[value_i], sizeof(value_bufs[value_i]), "%d", (value)); \
		argv[argc++] = value_bufs[value_i++]; \
	} while (0)

#define ADD_INT_ARG_IF_DIFF(switch_name, current_value, default_value) \
	do { \
		if ((current_value) != (default_value)) { \
			ADD_INT_ARG((switch_name), (current_value)); \
		} \
	} while (0)

	ADD_INT_ARG_IF_DIFF("-minGQrep", request->options.minGQrep, defaults.minGQrep);
	ADD_INT_ARG_IF_DIFF("-maxGQspacer", request->options.maxGQspacer, defaults.maxGQspacer);
	ADD_INT_ARG_IF_DIFF("-minMRrep", request->options.minMRrep, defaults.minMRrep);
	ADD_INT_ARG_IF_DIFF("-maxMRspacer", request->options.maxMRspacer, defaults.maxMRspacer);
	ADD_INT_ARG_IF_DIFF("-minIRrep", request->options.minIRrep, defaults.minIRrep);
	ADD_INT_ARG_IF_DIFF("-maxIRspacer", request->options.maxIRspacer, defaults.maxIRspacer);
	ADD_INT_ARG_IF_DIFF("-shortIRcut", request->options.shortIRcut, defaults.shortIRcut);
	ADD_INT_ARG_IF_DIFF("-shortIRspacer", request->options.shortIRspacer, defaults.shortIRspacer);
	ADD_INT_ARG_IF_DIFF("-minDRrep", request->options.minDRrep, defaults.minDRrep);
	ADD_INT_ARG_IF_DIFF("-maxDRrep", request->options.maxDRrep, defaults.maxDRrep);
	ADD_INT_ARG_IF_DIFF("-maxDRspacer", request->options.maxDRspacer, defaults.maxDRspacer);
	ADD_INT_ARG_IF_DIFF("-minATracts", request->options.minATracts, defaults.minATracts);
	ADD_INT_ARG_IF_DIFF("-minATractSep", request->options.minATractSep, defaults.minATractSep);
	ADD_INT_ARG_IF_DIFF("-maxATractSep", request->options.maxATractSep, defaults.maxATractSep);
	ADD_INT_ARG_IF_DIFF("-maxAPRlen", request->options.maxAPRlen, defaults.maxAPRlen);
	ADD_INT_ARG_IF_DIFF("-minAPRlen", request->options.minAPRlen, defaults.minAPRlen);
	ADD_INT_ARG_IF_DIFF("-minZlen", request->options.minZlen, defaults.minZlen);
	ADD_INT_ARG_IF_DIFF("-minSTR", request->options.minSTR, defaults.minSTR);
	ADD_INT_ARG_IF_DIFF("-maxSTR", request->options.maxSTR, defaults.maxSTR);
	ADD_INT_ARG_IF_DIFF("-minSTRbp", request->options.minSTRbp, defaults.minSTRbp);
	ADD_INT_ARG_IF_DIFF("-minCruciformRep", request->options.minCruciformRep, defaults.minCruciformRep);
	ADD_INT_ARG_IF_DIFF("-maxCruciformSpacer", request->options.maxCruciformSpacer, defaults.maxCruciformSpacer);
	ADD_INT_ARG_IF_DIFF("-minTriplexYRpercent", request->options.minTriplexYRpercent, defaults.minTriplexYRpercent);
	ADD_INT_ARG_IF_DIFF("-maxTriplexSpacer", request->options.maxTriplexSpacer, defaults.maxTriplexSpacer);
	ADD_INT_ARG_IF_DIFF("-maxSlippedSpacer", request->options.maxSlippedSpacer, defaults.maxSlippedSpacer);

#undef ADD_INT_ARG_IF_DIFF
#undef ADD_INT_ARG

	if (!request->flags.DO_findZ) argv[argc++] = "-skipZ";
	if (!request->flags.DO_findDR) argv[argc++] = "-skipDR";
	if (!request->flags.DO_findIR) argv[argc++] = "-skipIR";
	if (!request->flags.DO_findMR) argv[argc++] = "-skipMR";
	if (!request->flags.DO_findGQ) argv[argc++] = "-skipGQ";
	if (!request->flags.DO_findSTR) argv[argc++] = "-skipSTR";
	if (!request->flags.DO_findAPR) argv[argc++] = "-skipAPR";
	if (!request->flags.DO_Slipped) argv[argc++] = "-skipSlipped";
	if (!request->flags.DO_Triplex) argv[argc++] = "-skipTriplex";
	if (!request->flags.DO_Cruciform) argv[argc++] = "-skipCruciform";
	if (!request->flags.DO_KVzdna) argv[argc++] = "-skipKVzdna";
	if (!request->flags.DO_WGET) argv[argc++] = "-skipWGET";
	if (request->flags.DO_CHMOD) argv[argc++] = "-doCHMOD";

	code = gfa_run_cli(argc, argv);
	if (code == 0) {
		set_error(error_message, error_message_len, "");
	}
	return code;
}
