#' Find non-B DNA motifs
#'
#' Placeholder public API for motif detection.
#'
#' @param sequence DNA sequence as a single character string.
#' @param chrom Optional chromosome name used in output records.
#' @param out_prefix Output prefix for generated legacy files.
#' @param workdir Working directory for generated files.
#' @param options Named list of integer overrides for legacy numeric options.
#' @param flags Named list of boolean overrides for legacy motif/feature flags.
#' @param ... Reserved for future options mapped from legacy GFA flags.
#'
#' @return A list with status, generated file paths, and loaded TSV tables.
#' @export
find_nonb_motifs <- function(sequence,
                             chrom = NULL,
                             out_prefix = "nonbgfa",
                             workdir = tempdir(),
                             options = list(),
                             flags = list(),
                             ...) {
  if (!is.character(sequence) || length(sequence) != 1L || is.na(sequence)) {
    stop("`sequence` must be a single non-missing character string.", call. = FALSE)
  }

  dir.create(workdir, recursive = TRUE, showWarnings = FALSE)
  fasta_path <- file.path(workdir, paste0(out_prefix, ".fasta"))
  file_prefix <- file.path(workdir, out_prefix)
  writeLines(c(">query", toupper(sequence)), fasta_path)

  bridge <- .Call(
    "c_nonbgfa_run",
    normalizePath(fasta_path, winslash = "\\", mustWork = TRUE),
    normalizePath(file_prefix, winslash = "\\", mustWork = FALSE),
    chrom,
    options,
    flags
  )

  if (!identical(bridge$code, 0L)) {
    stop(sprintf("GFA run failed with code %d: %s", bridge$code, bridge$message), call. = FALSE)
  }

  suffixes <- c("IR", "MR", "DR", "GQ", "Z", "STR", "APR")
  tsv_files <- setNames(file.path(workdir, paste0(out_prefix, "_", suffixes, ".tsv")), suffixes)
  gff_files <- setNames(file.path(workdir, paste0(out_prefix, "_", suffixes, ".gff")), suffixes)
  existing_tsv <- tsv_files[file.exists(tsv_files)]
  existing_gff <- gff_files[file.exists(gff_files)]

  tsv_tables <- lapply(existing_tsv, function(path) {
    if (file.size(path) == 0L) {
      return(data.frame())
    }
    utils::read.delim(path, sep = "\t", header = TRUE, check.names = FALSE, stringsAsFactors = FALSE)
  })

  list(
    status = bridge,
    fasta = fasta_path,
    tsv_files = unname(existing_tsv),
    gff_files = unname(existing_gff),
    tsv_tables = tsv_tables
  )
}
