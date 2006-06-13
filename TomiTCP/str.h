#ifndef STR_H_INCLUDED
#define STR_H_INCLUDED

#include <string>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace std {
    /*
     * Whitespace stripping functions
     */
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

    /*
     * Case modifying functions
     */
    char char_toupper(char c);
    char char_tolower(char c);
    
    inline string& strtolower(string &str)
    {
	transform(str.begin(), str.end(), str.begin(), char_tolower);
	return str;
    }

    inline string& strtoupper(string &str)
    {
	transform(str.begin(), str.end(), str.begin(), char_toupper);
	return str;
    }

    /*
     * These are versions of above which don't take references and thus can be
     * used on temporary/constant variables.
     */
    inline string chomp_nr(string str) { return chomp(str); }
    inline string wstrip_nr(string str) { return wstrip(str); }
    inline string strtolower_nr(string str) { return strtolower(str); }
    inline string strtoupper_nr(string str) { return strtoupper(str); }

    /*
     * Something (especially integral types) to string converters
     */
    template <typename T> inline string tostr(T a)
    {
	stringstream s; s << a;
	return s.str();
    }

    template <typename T> inline string tostr_hex(T a, bool show_base = 0)
    {
	stringstream s;
	if (show_base)
#if __GNUC__ < 3
	    s.setf(ios::showbase);
#else
	    s.setf(ios_base::showbase);
#endif
	s << hex << a;
	return s.str();
    }

    template <typename T> inline string tostr_float(T a, int precision = -1)
    {
	stringstream s;
	if (precision != -1)
	    s << fixed << setprecision(precision);
	s << a;
	return s.str();
    }
    
    /*
     * And an opposite, a string to something converter.
     */
    template <typename T> inline T strto(const string &a)
    {
	stringstream s(a);
	T b;
       	s >> b;
	return b;
    }

    string recode(const string& src, const string& from, const string& to);
    unsigned int u8strlimit(const char *c, unsigned int limit);
    unsigned int u8strlen(const char *c);
}

#endif /* STR_H_INCLUDED */
