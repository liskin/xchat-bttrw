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

    inline string inttostr(int i)
    {
	char a[12];
	sprintf(a,"%i",i);
	return string(a);
    }

    inline string llinttostr(long long int i)
    {
	char a[21];
	sprintf(a,"%lli",i);
	return string(a);
    }

    inline string uinttostr(unsigned int i)
    {
	char a[12];
	sprintf(a,"%u",i);
	return string(a);
    }

    inline string ullinttostr(unsigned long long int i)
    {
	char a[21];
	sprintf(a,"%llu",i);
	return string(a);
    }

    inline string uinttohstr(unsigned int i)
    {
	char a[9];
	sprintf(a,"%x",i);
	return string(a);
    }

    inline string ullinttohstr(unsigned long long int i)
    {
	char a[17];
	sprintf(a,"%llx",i);
	return string(a);
    }

    inline string doubletostr(double d, int precision = 6)
    {
	char a[precision + 7];
	sprintf(a,"%.*g",precision,d);
	return string(a);
    }

    inline string ldoubletostr(long double d, int precision = 10)
    {
	char a[precision + 7];
	sprintf(a,"%.*Lg",precision,d);
	return string(a);
    }

#define INDENT_DETECT "\033"
    int len(string s, int ini = 0, int tab = 8);
    int ntokens(string s);
    string justify(string in, int width, int maxspaces = 4, int tab = 8);
    string reformat(string ins, int width, string firstline = "", int tab = 8,
	    bool firstlinealone = 0, int maxspaces = 4,
	    string indent = INDENT_DETECT);

    string recode(const string& src, const string& from, const string& to);
}

#endif /* STR_H_INCLUDED */
