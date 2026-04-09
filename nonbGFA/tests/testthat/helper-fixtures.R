fixture_golden_dir <- function() {
  testthat::test_path("..", "fixtures", "golden")
}

fixture_sequence <- function() {
  fasta <- file.path(fixture_golden_dir(), "gfa_test.fasta")
  lines <- readLines(fasta, warn = FALSE)
  paste(lines[!grepl("^>", lines)], collapse = "")
}

normalized_lines <- function(path) {
  readLines(path, warn = FALSE)
}
