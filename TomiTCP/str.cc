#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iconv.h>
#include <errno.h>
#include <stdexcept>
#include <cstdarg>
#include "str.h"

namespace std {
    char char_toupper(char c) { return toupper(c); }
    char char_tolower(char c) { return tolower(c); }

    string recode(const string& src, const string& from, const string& to)
    {
	iconv_t conv = iconv_open(to.c_str(), from.c_str());
	if (conv == (iconv_t) -1) {
	    return src;
	}

	size_t fromsize = src.length(), tosize = fromsize, ressize = tosize, inc = fromsize + 4;
	const char *msgptr = src.c_str();
	char *result, *resptr;

	if ((resptr = result = (char*) malloc(ressize + 1)) == NULL)
	    throw bad_alloc();

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
		    if ((result = (char*) realloc(result, ressize)) == NULL)
			throw bad_alloc();
		    resptr = result + sz;
		    continue;
		}

		iconv_close(conv);
		free(result);

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
	free(result);
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
    
    string _strprintf(const string &fmt, int argstart, ...)
    {
	/* Guess we need no more than 200 bytes. */
	int n, size = 200;
	char *p;
	va_list ap;

	if ((p = (char*) malloc(size)) == NULL)
	    throw bad_alloc();

	while (1) {
	    /* Try to print in the allocated space. */
	    va_start(ap, argstart);
	    n = vsnprintf(p, size, fmt.c_str(), ap);
	    va_end(ap);

	    /* If that worked, return the string. */
	    if (n > -1 && n < size) {
		string ret(p);
		free(p);
		return ret;
	    }

	    /* Else try again with more space. */
	    if (n > -1)    /* glibc 2.1 */
		size = n + 1; /* precisely what is needed */
	    else           /* glibc 2.0, win32 */
		size *= 2;  /* twice the old size */
	    if ((p = (char*) realloc(p, size)) == NULL)
		throw bad_alloc();
	}
    }

    int _fstrprintf(FILE *stream, const string &fmt, int argstart, ...)
    {
	int n;
	va_list ap;

	va_start(ap, argstart);
	n = vfprintf (stream, fmt.c_str(), ap);
	va_end(ap);

	return n;
    }
}
