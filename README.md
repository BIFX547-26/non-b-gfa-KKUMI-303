# non-B-gfa / nonbGFA

`non-B-gfa` is the legacy C implementation for detecting sequences associated with non-B DNA motifs.  
This repository now also contains **`nonbGFA`**, an R package interface that runs the same core engine and preserves legacy behavior.

## Biological context

DNA can adopt conformations beyond right-handed B-DNA, including motifs associated with cruciform, triplex, slipped (hairpin), tetraplex (G-quadruplex), and Z-DNA structures.  
This toolkit identifies candidate sequence motifs for these non-B structures.

Website: <https://nonb-abcc.ncifcrf.gov/apps/site/default>

## Citation

Please cite:

Cer RZ, Donohue DE, Mudunuri US, Temiz NA, Loss MA, Starner NJ, Halusa GN, Volfovsky N, Yi M, Luke BT, Bacolla A, Collins JR, Stephens RM.  
**Non-B DB v2.0: a database of predicted non-B DNA-forming motifs and its associated tools.**  
*Nucleic Acids Research* (2013) 41(D1):D94-D100. doi:10.1093/nar/gks955

## Current layout

- Root C sources (`gfa.c`, `find*.c`, etc.): legacy engine and CLI-compatible build path.
- `nonbGFA/`: R package scaffold + native bridge to the C core.
- `phase1/`: canonical golden baseline artifacts used for regression parity.

## R package-first usage

The R package entrypoint is:

```r
result <- nonbGFA::find_nonb_motifs(
  sequence = "ACGTACGTACGT...",
  chrom = "chr1",                # optional
  out_prefix = "sample",
  workdir = tempdir(),
  options = list(),              # numeric overrides
  flags = list()                 # boolean overrides
)
```

Returned object includes:

- `status` (native run code/message)
- `fasta` (generated FASTA path)
- `gff_files` (existing GFF outputs)
- `tsv_files` (existing TSV outputs)
- `tsv_tables` (parsed TSV data.frames)

## Legacy CLI compatibility and migration

Legacy CLI usage remains:

```bash
./gfa -seq <input_fasta_filename> -out <output_file_prefix> [optional_switches]
```

Common mapping from legacy CLI to R:

| Legacy CLI | R (`find_nonb_motifs`) |
|---|---|
| `-seq <file>` | `sequence = "..."` (package writes temporary FASTA) |
| `-out <prefix>` | `out_prefix = "<prefix>"`, `workdir = "<dir>"` |
| `-chrom <name>` | `chrom = "<name>"` |
| `-minIRrep 12` | `options = list(minIRrep = 12)` |
| `-maxGQspacer 5` | `options = list(maxGQspacer = 5)` |
| `-skipGQ` | `flags = list(DO_findGQ = FALSE)` |
| `-skipAPR` | `flags = list(DO_findAPR = FALSE)` |
| `-skipTriplex` | `flags = list(DO_Triplex = FALSE)` |
| `-skipWGET` | `flags = list(DO_WGET = FALSE)` (default in R bridge) |

## Output schema (per motif)

The engine writes per-motif files:

- `*_IR.(gff|tsv)` Inverted Repeats
- `*_MR.(gff|tsv)` Mirror Repeats
- `*_DR.(gff|tsv)` Direct Repeats
- `*_GQ.(gff|tsv)` G-Quadruplex motifs
- `*_Z.(gff|tsv)` Z-DNA motifs
- `*_STR.(gff|tsv)` Short Tandem Repeats
- `*_APR.(gff|tsv)` A-Phased Repeats

## Parity guarantee status

Canonical fixture parity is enforced by tests in `nonbGFA/tests/testthat/` against golden outputs in `nonbGFA/tests/fixtures/golden/`.

- Canonical sequence regression compares generated GFF/TSV line-by-line to legacy artifacts.
- Flag-mapping tests validate skip behavior.
- Option-mapping tests validate threshold handling and empty-output behavior.

## Legacy CLI reference (original usage text)

```
************************  GFA2    ********************************************

usage:./gfa -seq <input_fasta_filename> -out <output_file_prefix> [optional_switches]
*****************************************************************************
 GFA2 takes in a DNA sequence in fasta format and returns Gene Feature Format
 (.gff) and Tab Separated Value (.tsv) files containing the location and details of potential non-B DNA forming motifs.
```
