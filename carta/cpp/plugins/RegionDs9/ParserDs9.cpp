#include <fstream>
#include <sstream>

#include <ParserDs9.h>
#include <ds9lex.h>



ParserDs9::ParserDs9( ) : trace_scanning(false), trace_parsing(false) {
}

bool ParserDs9::parse_stream(ContextDs9 &base, std::istream& in, const std::string& sname) {
    streamname = sname;

    ds9lex scanner(&in);
    scanner.set_debug(trace_scanning);
    lexer = &scanner;

    yy::ds9parse parser( *this, &base );

    parser.set_debug_level(trace_parsing);
    return (parser.parse() == 0);
}

bool ParserDs9::parse_file( ContextDs9 &base, const std::string &filename) {
    std::ifstream in(filename.c_str());
    if (!in.good()) return false;
    return parse_stream(base, in, filename);
}

bool ParserDs9::parse_string( ContextDs9 &base, const std::string &input, const std::string& sname) {
    std::istringstream iss(input);
    return parse_stream(base, iss, sname);
}

void ParserDs9::error(const class yy::location& l, const std::string& m) {
    std::cerr << l << ": " << m << std::endl;
}

void ParserDs9::error(const std::string& m) {
    std::cerr << m << std::endl;
}


