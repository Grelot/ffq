/* ce fichier est une part de FFQ */

#ifndef __FFQ_H__
#define __FFQ_H__



#ifdef _OPENMP
#include <omp.h>    // OpenMP library
#endif

#include <getopt.h> // Process command-line options
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <regex>
#include <cassert>
#include <limits.h>


#include <zlib.h>

#include <string>
using std::string;

#include <iostream>
#include <fstream>
#include <sstream>
using std::ofstream;
using std::ifstream;
using std::stringstream;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;






enum class FileT {unknown,
    sql,     gzsql,
    fasta,   gzfasta,
    fastq,   gzfastq,
    bowtie,  sam, bam, tsv,
    bustard, phase, fastphase, beagle};

std::string remove_suffix(FileT, const string&);

class FQREAD {
  string header;
  string seq;
  string blank;
  string qual;
};



int filter_min_length_fastq(string , int );
int   is_integer(const char *);
FileT guess_file_type(const string&);
void  escape_char(char c, string& s);
int   parse_command_line(int, char**);


void help( void );

#endif
