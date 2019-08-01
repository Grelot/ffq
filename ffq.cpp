/* ceci est mon programme principal */
#include "ffq.h"

using namespace std;

//
// Global variables to hold command-line options.
//
FileT   in_file_type;
string  in_file;
string  out_path;
int     num_threads       = 1;
int     max_read_length   = 150;
int     min_read_length   = 3;
int     VERSION           = 1;

int main (int argc, char* argv[]) {

    parse_command_line(argc, argv);
    cerr << "ffq parameters selected:\n"
         << "  Min read length   : " << min_read_length << "\n"
         << "  Max read length   : " << max_read_length << "\n"
         << "  Number of threads : " << num_threads << "\n";
     #ifdef _OPENMP
     omp_set_num_threads(num_threads);
     #endif
     if (in_file_type == FileT::fastq) {
       filter_min_length_fastq(in_file, min_read_length);
     }
     else if(in_file_type == FileT::gzfastq) {
       //nada
     }
     else {
      cerr << "Unknow Type File : file cannot be read\n";
    }
    return 0;
}




int filter_min_length_fastq(string in_file, int min_read_length) {
  int i;
  string ligne ;
  ifstream monfichier ;
  monfichier.open(in_file, ifstream::in);
  //char c = monfichier.getline();
  i=0;
  string fheader ="";
  string fseq="";
  string fblank="";
  string fqual="";
  int lenfseq=0;
  while(getline(monfichier, ligne)) {
    switch (i) {
    case 0 :
     fheader = ligne;
     break;
    case 1 :
     fseq= ligne;
     break;
    case 2 :
     fblank= ligne;
     break;
    case 3 :
     i=-1;
     fqual= ligne;
     if(strchr(fseq.c_str(), 'N')) {
       lenfseq=-1;
     } else {
       lenfseq = fseq.length();
     }
     if ((int)lenfseq >= min_read_length) {
       printf("%s\n%s\n%s\n%s\n",fheader.c_str(),fseq.c_str(),fblank.c_str(),fqual.c_str());
     }
     string fheader ="";
     string fseq="";
     string fblank="";
     string fqual="";
     break;
    }
    i=i+1;
  }
  monfichier.close();
  return 0;
}




int
is_integer(const char *str)
{
    //
    // Adapted from the strtol manpage.
    //
    char *endptr;

    // To distinguish success/failure after call
    errno = 0;
    long val = strtol(str, &endptr, 10);

    //
    // Check for various possible errors
    //
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
        || (errno != 0 && val == 0)) {
        return -1;
    }

    if (endptr == str || *endptr != '\0')
        return -1;

    return (int) val;
}



const
map<string, FileT> known_extensions = {
    {".fa", FileT::fasta},
    {".fasta", FileT::fasta},
    {".fa.gz", FileT::gzfasta},
    {".fasta.gz", FileT::gzfasta},
    {".fq", FileT::fastq},
    {".fastq", FileT::fastq},
    {".fq.gz", FileT::gzfastq},
    {".fastq.gz", FileT::gzfastq},
    {".sam", FileT::sam},
    {".bam", FileT::bam}
};

string remove_suffix(FileT type, const string& orig) {
    string file (orig);

    int pos = file.find_last_of(".");

    if ((type == FileT::gzfastq || type == FileT::gzfasta) && file.substr(pos) == ".gz")
        file = file.substr(0, pos);

    pos = file.find_last_of(".");

    if (type == FileT::gzfastq || type == FileT::fastq) {

        if (file.substr(pos) == ".fastq" || file.substr(pos) == ".fq")
            file = file.substr(0, pos);

    } else if (type == FileT::gzfasta || type == FileT::fasta) {

        if (file.substr(pos) == ".fasta" || file.substr(pos) == ".fa")
            file = file.substr(0, pos);
    }

    return file;
}




regex init_file_ext_regex () {
    string s = "(";

    auto i = known_extensions.begin();
    assert(!known_extensions.empty());
    string ext = i->first;
    escape_char('.', ext);
    s += ext;
    ++i;
    while(i != known_extensions.end()) {
        ext = i->first;
        escape_char('.', ext);
        s += "|" + ext;
        ++i;
    }

    s += ")$";
    return regex(s);
}



FileT guess_file_type (const string& path) {

    static const regex reg = init_file_ext_regex();

    smatch m;
    regex_search(path, m, reg);

    if (m.empty())
        return FileT::unknown;
    else
        return known_extensions.at(m.str());
}

void escape_char(char c, string& s) {
    vector<size_t> dots;
    size_t i = -1;
    while ((i = s.find(c, i+1)) != string::npos)
        dots.push_back(i);

    for(auto j=dots.rbegin(); j!=dots.rend(); ++j)
        s.insert(*j, 1, '\\');
}



int parse_command_line(int argc, char* argv[]) {
    int c;

    while (1) {
        static struct option long_options[] = {
            {"help",             no_argument,       NULL, 'h'},
            {"infile_type",      required_argument, NULL, 't'},
            {"file",             required_argument, NULL, 'f'},
            {"outpath",          required_argument, NULL, 'o'},
            {"max_readlength",         required_argument, NULL, 'L'},
            {"min_readlength",     required_argument, NULL, 'l'},
            {"num_threads",      required_argument, NULL, 'p'},
            {0, 0, 0, 0}
        };
        // getopt_long stores the option index here.
        int option_index = 0;

        c = getopt_long(argc, argv, "h:L:l:f:o:p:t:", long_options, &option_index);

        // Detect the end of the options.
        if (c == -1)
            break;

        switch (c) {
        case 'h':
            help();
            break;
        case 't':
            if (strcmp(optarg, "tsv") == 0)
                in_file_type = FileT::tsv;
            else if (strcmp(optarg, "fasta") == 0)
                in_file_type = FileT::fasta;
            else if (strcmp(optarg, "fastq") == 0)
                in_file_type = FileT::fastq;
            else if (strcasecmp(optarg, "gzfasta") == 0)
                in_file_type = FileT::gzfasta;
            else if (strcasecmp(optarg, "gzfastq") == 0)
                in_file_type = FileT::gzfastq;
            else
                in_file_type = FileT::unknown;
            break;
        case 'f':
            in_file = optarg;
            break;
        case 'o':
            out_path = optarg;
            break;
        case 'l':
            min_read_length = is_integer(optarg);
            break;
        case 'L':
            max_read_length = is_integer(optarg);
            break;
        case 'p':
            num_threads = is_integer(optarg);
            break;
        default:
            cerr << "Unknown command line option '" << (char) c << "'\n";
            help();
            abort();
        }
    }
    if (optind < argc) {
        cerr << "Error: Failed to parse command line: '" << argv[optind] << "' is seen as a positional argument. Expected no positional arguments.\n";
        help();
    }
    if (min_read_length < 2 || min_read_length > max_read_length) {
        cerr << "Minimum/Maximum read length must be at least 2 bp.\nMaximum read length must be greater than minimum read length.\n";
        help();
    }
    if (in_file.empty()) {
        cerr << "You must specify an input file.\n";
        help();
    }
    if (in_file_type == FileT::unknown) {
        in_file_type = guess_file_type(in_file);
        if (in_file_type == FileT::unknown) {
            cerr << "Unable to recongnize the extention of file '" << in_file << "'.\n";
            help();
        }
    }
    if (out_path.length() == 0)
        out_path = ".";
    if (out_path.at(out_path.length() - 1) != '/')
        out_path += "/";
    return 0;
}



void help() {
    cerr << "fqq " << VERSION << "\n"
              << "ffq -f fastq_path -o path [-L max_length] [-l min_length] [-p num_threads]" << "\n"
              << "  f: input file path.\n"
              << "  o: output path to write results.\n"
              << "  L: Maximum read's length (in nucleotides) to keep (default 150).\n"
              << "  l: Minimum read's length  to keep (default 3).\n"
              << "  p: enable parallel execution with num_threads threads.\n"
              << "  t: input file type. Supported types: fasta, fastq, gzfasta, or gzfastq (default: guess).\n"
              << "\n"
              << "  h: display this help messsage.\n";

    exit(0);
}
