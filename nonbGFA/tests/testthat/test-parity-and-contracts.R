test_that("canonical fixture matches legacy golden outputs", {
  golden <- fixture_golden_dir()
  seq <- fixture_sequence()
  wd <- file.path(tempdir(), "nonbgfa-parity")
  if (dir.exists(wd)) unlink(wd, recursive = TRUE, force = TRUE)
  dir.create(wd, recursive = TRUE)

  result <- find_nonb_motifs(seq, chrom = "Test", out_prefix = "gfa_test", workdir = wd)
  expect_equal(result$status$code, 0L)

  suffixes <- c("IR", "MR", "DR", "GQ", "Z", "STR", "APR")
  for (suffix in suffixes) {
    expected_gff <- file.path(golden, sprintf("gfa_test_%s.gff", suffix))
    expected_tsv <- file.path(golden, sprintf("gfa_test_%s.tsv", suffix))
    actual_gff <- file.path(wd, sprintf("gfa_test_%s.gff", suffix))
    actual_tsv <- file.path(wd, sprintf("gfa_test_%s.tsv", suffix))

    expect_true(file.exists(actual_gff))
    expect_true(file.exists(actual_tsv))
    expect_equal(normalized_lines(actual_gff), normalized_lines(expected_gff))
    expect_equal(normalized_lines(actual_tsv), normalized_lines(expected_tsv))
  }
})

test_that("flag mapping skips selected motif families", {
  seq <- fixture_sequence()
  wd <- file.path(tempdir(), "nonbgfa-flags")
  if (dir.exists(wd)) unlink(wd, recursive = TRUE, force = TRUE)
  dir.create(wd, recursive = TRUE)

  result <- find_nonb_motifs(
    seq,
    out_prefix = "flags",
    workdir = wd,
    flags = list(DO_findGQ = FALSE, DO_findAPR = FALSE, DO_findZ = FALSE)
  )

  expect_equal(result$status$code, 0L)
  expect_false(file.exists(file.path(wd, "flags_GQ.gff")))
  expect_false(file.exists(file.path(wd, "flags_GQ.tsv")))
  expect_false(file.exists(file.path(wd, "flags_APR.gff")))
  expect_false(file.exists(file.path(wd, "flags_APR.tsv")))
  expect_false(file.exists(file.path(wd, "flags_Z.gff")))
  expect_false(file.exists(file.path(wd, "flags_Z.tsv")))
  expect_true(file.exists(file.path(wd, "flags_IR.gff")))
  expect_true(file.exists(file.path(wd, "flags_MR.gff")))
})

test_that("option mapping can suppress motif outputs with strict thresholds", {
  seq <- fixture_sequence()
  wd <- file.path(tempdir(), "nonbgfa-options")
  if (dir.exists(wd)) unlink(wd, recursive = TRUE, force = TRUE)
  dir.create(wd, recursive = TRUE)

  result <- find_nonb_motifs(
    seq,
    out_prefix = "strict",
    workdir = wd,
    flags = list(DO_findSTR = FALSE),
    options = list(
      minIRrep = 200,
      minMRrep = 200,
      minDRrep = 200,
      minGQrep = 10,
      minZlen = 200,
      minATracts = 200
    )
  )

  expect_equal(result$status$code, 0L)
  expect_true(all(file.info(result$gff_files)$size == 0))
  expect_true(all(vapply(result$tsv_tables, nrow, integer(1)) == 0L))
})

test_that("input validation rejects invalid sequence values", {
  expect_error(find_nonb_motifs(c("AC", "GT")), "single non-missing")
  expect_error(find_nonb_motifs(NA_character_), "single non-missing")
  expect_error(find_nonb_motifs(1), "single non-missing")
})
