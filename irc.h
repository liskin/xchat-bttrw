#ifndef _IRC_H_INCLUDED
#define _IRC_H_INCLUDED

#include <string>
#include <vector>

namespace std {
    void parsein(const string &buf, string& prefix, vector<string>& cmd);
    void parsemode(const vector<string>& cmd, vector<pair<string,string> >& mode);
}

#endif /* _IRC_H_INCLUDED */
