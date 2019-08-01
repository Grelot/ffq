# FFQ
[![Project Status: Suspended – Initial development has started, but there has not yet been a stable, usable release; work has been stopped for the time being but the author(s) intend on resuming work.](https://www.repostatus.org/badges/latest/suspended.svg)](https://www.repostatus.org/#suspended)

A tool designed to provide preprocessing for FASTQ files. This tool is developed in C++


# usage


for single-end non-compressed fastq :
```
ffq -f example/bad_test.fastq -L 150 -l 4 -t fastq > filtered_test.fastq
```
it keeps only sequences with a size between 4 and 150 base pairs.

# install FFQ

Simply type
```
make
```
it generates a binary `ffq`. 
Check if the program is working:
```
./ffq -h
```
should display
```
fqq 1
ffq -f fastq_path -o path [-L max_length] [-l min_length] [-p num_threads]
  f: input file path.
  o: output path to write results.
  L: Maximum read's length (in nucleotides) to keep (default 150).
  l: Minimum read's length  to keep (default 3).
  p: enable parallel execution with num_threads threads.
  t: input file type. Supported types: fasta, fastq, gzfasta, or gzfastq (default: guess).

  h: display this help messsage.
```

