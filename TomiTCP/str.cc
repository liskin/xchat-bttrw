#pragma implementation
#include <iostream>
#include <string>
#include <sstream>
#include "str.h"

namespace std {
    /*
     * Calculate string length with given tab length
     */
    int len(string s, int ini = 0, int tab = 8)
    {
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
    string justify(string in, int width, int maxspaces = 3, int tab = 8)
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
	string indent;
	
	// get indentation
	{
	    int indentw = in.find_first_not_of(" \f\n\r\t\v");
	    indent = string(in,0,indentw);
	    in.erase(0,indentw);
	}

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

	return indent + out;
    }

    /*
     * Reformat and justify paragraph with optional first line indentation
     */
    string reformat(string ins, int width, string firstline = "", int tab = 8)
    {
	// get indentation
	int indentw = ins.find_first_not_of(" \f\n\r\t\v");
	string indent = string(ins,0,indentw);
	ins.erase(0,indentw);
	indentw = len(indent,0,tab);

	stringstream out, in(ins);

	// prepare first line
	string line = indent + firstline;
	int linew = len(firstline,indentw,tab);
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
		// line would be too long
		out << justify(line,width,tab) << endl;
		line = indent + s;
		linew = len(line,0,tab);
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

}
