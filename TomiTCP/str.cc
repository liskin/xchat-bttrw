#pragma implementation
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iconv.h>
#include <errno.h>
#include <stdexcept>
#include "str.h"

namespace std {
    char char_toupper(char c) { return toupper(c); }
    char char_tolower(char c) { return tolower(c); }

    /*
     * Calculate string length with given tab length
     */
    int len(const string& s, int ini, int tab)
    {
	if (!tab)
	    throw invalid_argument("tab cannot be 0, would cause division by zero");

	int ret = ini;

	for (string::const_iterator i = s.begin(); i != s.end(); i++) {
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
    int ntokens(const string& s)
    {
	stringstream ss(s);
	string tmp;
	int n = 0;

	while (ss >> tmp) {
	    n++;
	}

	return n;
    }

    /*
     * Justify line, maxspaces is the maximal number of spaces between tokens
     */
    string justify(string in, int width, int maxspaces, int tab)
    {
	int inw = len(in,0,tab);
	int need = width - inw;

	// nothing is needed to do
	if (need <= 0)
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
	    out = "";

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
    string reformat(string ins, int width, const string& firstline,
	    int tab, bool firstlinealone, int maxspaces, string indent)
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

	size_t fromsize = src.length(), tosize = fromsize, ressize = tosize, inc = fromsize + 4;
	const char *msgptr = src.c_str();
	char *result = new char[ressize + 1];
	char *resptr = result;

	while ((fromsize>0) && (tosize>0)) {
	    if (((size_t (*)(iconv_t, const char **, size_t *, char **, size_t *))
		    iconv)(conv, &msgptr, &fromsize, &resptr, &tosize) == (size_t)-1)
	    {
		int err = errno;
		
		// array is not big enough
		if (err == E2BIG) {
		    // add more characters to array
		    size_t sz = resptr - result;
		    tosize += inc; ressize += inc;
		    result = (char*) realloc(result, ressize);
		    resptr = result + sz;
		    continue;
		}

		iconv_close(conv);
		delete []result;

		switch (err) {
		    case EILSEQ:
			throw runtime_error("An invalid multibyte sequence has been encountered in the input.");
		    case EINVAL:
		       	throw runtime_error("An incomplete multibyte sequence has been encountered in the input.");
		    default:
			throw runtime_error("Unknown error during iconv.");
		}
	    }
	}

	*resptr = 0;
	iconv_close(conv);
	string ret = result;
	delete []result;
	return ret;
    }

    /* 
     * Count number of real chars in UTF-8 string
     */
    unsigned int u8strlen(const char *c)
    {
	unsigned int len = 0;

	for (; *c; c++)
	    if (!(*c & 0x80 && ~*c & 0x40))
		len++;

	return len;
    }

    /*
     * Count number of 8-bit chars needed to represent 'limit' UTF-8 chars
     * from string c.
     */
    unsigned int u8strlimit(const char *c, unsigned int limit)
    {
	const char *tc = c;
	for (; *tc && (limit || (*tc & 0x80 && ~*tc & 0x40)); tc++)
	    if (!(*tc & 0x80 && ~*tc & 0x40))
		limit--;

	return tc - c;
    }
}
