#pragma implementation
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iconv.h>
#include <errno.h>
#include "str.h"

namespace std {
    /*
     * Calculate string length with given tab length
     */
    int len(string s, int ini, int tab)
    {
	if (!tab)
	    throw invalid_argument("tab cannot be 0, would cause division by zero");

	int ret = ini;

	for (string::iterator i = s.begin(); i != s.end(); i++) {
	    if (*i == '\t') {
		ret = (ret/tab + 1) * tab;
	    } else {
		ret++;
	    }
	}

	return ret;
    }

    /*
     * Calculate number of tokens (delimited by whitespace)
     */
    int ntokens(string s)
    {
	stringstream ss(s);
	int n = 0;

	while (ss >> s) {
	    n++;
	}

	return n;
    }

    /*
     * Justify line, maxspaces is the maximal number of spaces between toknes
     */
    string justify(string in, int width, int maxspaces, int tab)
    {
	int inw = len(in,0,tab);
	int need = width - inw;

	// nothing is needed to do
	if (!need)
	    return in;

	// more spaces between tokens is needed
	if (((ntokens(in) - 1) * (maxspaces - 1)) < need)
	    return in;

	string out;
	
	// go through string a add spaces while it is needed
	int phase = 0;
	while (need) {
	    int space = 0;
	    bool skip = 0;
	    out.clear();

	    for (string::reverse_iterator i = in.rbegin(); i != in.rend(); i++) {
		if (need) {
		    if (isspace(*i))
			space++;
		    else {
			// after last space between tokens
			if (space) {
			    // { phase mod 3 = 0 } add spaces after punctuation
			    if (phase/3 == (space - 1) && phase%3 == 0 && ispunct(*i)) {
				out = " " + out;
				need--;
			    }

			    // { phase mod 3 = 1 } add spaces between words
			    // sometimes
			    if (phase/3 == (space - 1) && phase%3 == 1) {
				if (skip) {
				    out = " " + out;
				    need--;
				}
				skip = !skip;
			    }

			    // { phase mod 3 = 2 } add spaces between all words
			    if (phase/3 == (space - 1) && phase%3 == 2) {
				out = " " + out;
				need--;
			    }
			}

			space = 0;
		    }
		}

		out = *i + out;
	    }

	    // prepare for next walk-through
	    in = out;
	    phase++;
	}

	return out;
    }

    /*
     * Reformat and justify paragraph with optional first line indentation,
     * firstlinealone specifies if not add indent to firstlineindent on first
     * line
     */
    string reformat(string ins, int width, string firstline, int tab,
	    bool firstlinealone, int maxspaces, string indent)
    {
	// get indentation
	int indentw, indentlen;
	if (indent == INDENT_DETECT) {
	    indentw = ins.find_first_not_of(" \f\n\r\t\v");
	    indent = string(ins,0,indentw);
	    ins.erase(0,indentw);
	}

	stringstream out, in(ins);

	// prepare first line
	string line;
	int linew;
	if (firstlinealone) {
	    line = firstline;
	    linew = len(firstline,0,tab);
	} else {
	    line = indent + firstline;
	    linew = len(firstline,indentw,tab);
	}
	indentw = len(line,0,tab);
	indentlen = line.length();
	bool nline = 1;

	// walk through tokens
	string s;
	while (in >> s) {
	    // calculate line lenght with this token
	    int neww = len(" " + s,linew,tab);

	    if (nline) {
		// first token on first line
		line += s;
		linew = len(line,0,tab);
		nline = 0;
	    } else if (neww > width) {
		// line would be too long, so output it
		out << string(line,0,indentlen) <<
		    justify(string(line,indentlen), width - indentw,
			    maxspaces, tab) << endl;
		line = indent + s;
		linew = len(line,0,tab);
		indentlen = indent.length();
		indentw = len(indent,0,tab);
	    } else {
		// add token
		linew = neww;
		line += " " + s;
	    }
	}

	// last line
	if (line.length()) {
	    out << line << endl;
	}

	return out.str();
    }

    string recode(const string& src, const string& from, const string& to)
    {
	iconv_t conv = iconv_open(to.c_str(), from.c_str());
	if (conv == (iconv_t) -1) {
	    return src;
	}

	size_t fromsize = src.length(), tosize = fromsize, ressize = tosize;
	const char *msgptr = src.c_str();
	char *result = new char[fromsize+1];
	char *resptr = result;

	while ((fromsize>0) && (tosize>0))
	{
	    if ((int)iconv(conv, (char **)&msgptr, &fromsize, &resptr, &tosize)==-1)
	    {
		// array is not big enough
		if (errno == E2BIG)
		{
		    // add fromsize + 4 more characters to array
		    result = (char*) realloc(result,ressize + fromsize + 4);
		    resptr = result + ressize;
		    ressize += fromsize + 4;
		    tosize += fromsize + 4;
		    continue;
		}

		delete []result;
		return src;
	    }
	}

	*resptr = 0;
	iconv_close(conv);
	string ret = result;
	delete []result;
	return ret;
    }
}
