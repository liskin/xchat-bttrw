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
	recode_scan_request (recode.request, "html..flat");
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
	char *ret = recode_string(recode.request, m.c_str());
	if (ret) {
	    m = ret;
	    free(ret);
	}
    }
}
