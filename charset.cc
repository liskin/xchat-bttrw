#include "charset.h"

namespace xchat {
    extern "C" const char *program_name = "xchat-bttrw";

    /*
     * Init recode library.
     */
    recode_t recode_struct;
    void init_recode()
    {
	recode_struct.outer = recode_new_outer (false);
       	recode_struct.request = recode_new_request (recode_struct.outer);
	recode_scan_request (recode_struct.request, "html..u8");
	atexit(exit_recode);
    }

    /*
     * Free recode library.
     */
    void exit_recode()
    {
	recode_delete_request (recode_struct.request);
	recode_delete_outer (recode_struct.outer);
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
		    ret = recode_string(recode_struct.request, token.c_str());
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
