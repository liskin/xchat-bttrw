#include "charset.h"

namespace xchat {
    /*
     * Init recode library.
     */
    recode_t recode;
    void init_recode()
    {
	recode.outer = recode_new_outer (false);
       	recode.request = recode_new_request (recode.outer);
	recode_scan_request (recode.request, "html..u8");
	atexit(exit_recode);
    }

    /*
     * Free recode library.
     */
    void exit_recode()
    {
	recode_delete_request (recode.request);
	recode_delete_outer (recode.outer);
    }

    /*
     * Convert HTML entities to plain 7-bit ascii
     */
    void striphtmlent(string &m)
    {
	unsigned int a, b;
	unsigned int pos = 0;

	while (((a = m.find('&', pos)) != string::npos) &&
		((b = m.find(';', a + 1)) != string::npos)) {
	    bool fail = 0;
	    for (string::iterator i = m.begin() + a + 1; i != m.begin() + b; i++)
		fail |= isspace(*i);

	    if (!fail) {
		string token = string(m.begin() + a, m.begin() + b + 1).c_str();
		char *ret = 0;
		
		if (token == "&nbsp;")
		    ret = " ";
		if (!ret)
		    ret = recode_string(recode.request, token.c_str());
		if (ret) {
		    m.erase(m.begin() + a, m.begin() + b + 1);
		    m.insert(a, ret);

		    pos = a + strlen(ret);
		    free(ret);
		} else {
		    pos = a + 1;
		}
	    } else {
		pos = a + 1;
	    }
	}
    }
}
