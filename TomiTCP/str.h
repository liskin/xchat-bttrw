#ifndef STR_H_INCLUDED
#define STR_H_INCLUDED

#pragma interface
#include <string>
#include <ctype.h>

namespace std {
    inline string& chomp(string &str)
    {
        str.erase(0,str.find_first_not_of("\r\n"));
	str.erase(str.find_last_not_of("\r\n")+1);
	return str;
    }

    inline string& wstrip(string &str)
    {
	str.erase(0,str.find_first_not_of(" \f\n\r\t\v"));
	str.erase(str.find_last_not_of(" \f\n\r\t\v")+1);
	return str;
    }
    
    inline string& strtolower(string &str)
    {
	for (string::iterator i = str.begin(); i != str.end(); i++)
	    *i = tolower(*i);
	return str;
    }

    inline string& strtoupper(string &str)
    {
	for (string::iterator i = str.begin(); i != str.end(); i++)
	    *i = toupper(*i);
	return str;
    }

    int len(string s, int ini = 0, int tab = 8);
    int ntokens(string s);
    string justify(string in, int width, int maxspaces = 3, int tab = 8);
    string reformat(string ins, int width, string firstline = "", int tab = 8);
}

#endif /* STR_H_INCLUDED */
