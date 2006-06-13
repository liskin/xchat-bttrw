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
