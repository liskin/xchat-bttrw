#ifndef _CHARSET_H_INCLUDED
#define _CHARSET_H_INCLUDED

#include <cstdio>
#include <recode.h>
#include <string>

namespace xchat {
    using namespace std;

    struct recode_t {
	RECODE_OUTER outer;
	RECODE_REQUEST request;
    };
    extern recode_t recode_struct;

    void init_recode();
    void exit_recode();
    void striphtmlent(string &m);
}

#endif /* _CHARSET_H_INCLUDED */
