#include "gfa.h"

void gfa_init_default_options(GFAOptions *options) {
	options->minGQrep = 3;
	options->maxGQspacer = 7;
	options->minMRrep = 10;
	options->maxMRspacer = 100;
	options->minIRrep = 6;
	options->maxIRspacer = 100;
	options->shortIRcut = 9;
	options->shortIRspacer = 4;
	options->maxDRspacer = 10;
	options->minDRrep = 10;
	options->maxDRrep = 300;
	options->minATracts = 3;
	options->minATractSep = 10;
	options->maxATractSep = 11;
	options->maxAPRlen = 9;
	options->minAPRlen = 3;
	options->minZlen = 10;
	options->minSTR = 1;
	options->maxSTR = 9;
	options->minSTRreps = 3;
	options->minSTRbp = 10;
	options->minCruciformRep = 10;
	options->maxCruciformSpacer = 4;
	options->minTriplexYRpercent = 10;
	options->maxTriplexSpacer = 8;
	options->maxSlippedSpacer = 0;
	options->minKVscore = 33;
}

void gfa_init_default_flags(GFAFlags *flags) {
	flags->DO_findSTR = TRUE;
	flags->DO_findDR = TRUE;
	flags->DO_findMR = TRUE;
	flags->DO_findGQ = TRUE;
	flags->DO_findIR = TRUE;
	flags->DO_findZ = TRUE;
	flags->DO_findAPR = TRUE;
	flags->DO_Cruciform = TRUE;
	flags->DO_Triplex = TRUE;
	flags->DO_Slipped = TRUE;
	flags->DO_KVzdna = TRUE;
	flags->DO_WGET = TRUE;
	flags->DO_CHMOD = FALSE;
	flags->KEEP_TIME = TRUE;
}
