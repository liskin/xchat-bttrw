#ifndef _IRC_H_INCLUDED
#define _IRC_H_INCLUDED

#include <string>
#include <vector>

namespace std {
    void parsein(string buf, string& prefix, vector<string>& cmd);
}

#endif /* _IRC_H_INCLUDED */
