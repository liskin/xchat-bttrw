#ifndef _CHARSET_H_INCLUDED
#define _CHARSET_H_INCLUDED

#include <cstdio>
#include <string>

namespace xchat {
    using namespace std;

    void init_recode();
    void striphtmlent(string &m);
}

#endif /* _CHARSET_H_INCLUDED */
