\name{find_nonb_motifs}
\alias{find_nonb_motifs}
\title{Find non-B DNA motifs}
\usage{
find_nonb_motifs(
  sequence,
  chrom = NULL,
  out_prefix = "nonbgfa",
  workdir = tempdir(),
  options = list(),
  flags = list(),
  ...
)
}
\arguments{
\item{sequence}{DNA sequence as a single character string.}

\item{chrom}{Optional chromosome name used in output records.}

\item{out_prefix}{Output prefix for generated legacy files.}

\item{workdir}{Working directory for generated files.}

\item{options}{Named list of integer overrides for legacy numeric options.}

\item{flags}{Named list of boolean overrides for legacy motif/feature flags.}

\item{...}{Reserved for future options mapped from legacy GFA flags.}
}
\value{
A list with status, generated file paths, and loaded TSV tables.
}
\description{
Run legacy GFA motif detection from R via a native bridge.
}
