#include <cstdio>
#include <cstdlib>
#include <stdbool.h>
#include <recodext.h>
#include "xchat.h"
#include "TomiTCP/str.h"

namespace xchat {
    string XChat::makeurl(const string& path)
    {
	return "http://x" + inttostr(1 + (rand() % servers)) + ".xchat.centrum.cz/" + path;
    }

    string XChat::makeurl2(const string& path)
    {
	return "http://x" + inttostr(1 + (rand() % servers)) + ".xchat.centrum.cz/~$" +
	    uid + "~" + sid + "/" + path;
    }

    void XChat::stripdate(string &m)
    {
	string n = m, d;

	n.erase(0, n.find_first_not_of(" "));
	d = string(n, 0, n.find_first_of(" "));
	n.erase(0, n.find_first_of(" "));
	n.erase(0, n.find_first_not_of(" "));

	d.erase(0, d.find_first_of(":"));
	d.erase(d.find_last_of(":") + 1);
	if (d.length() > 1 && d[0] == ':' && d[d.length()-1] == ':') {
	    m = n;
	}
    }

    void XChat::getnick(string &m, string &src, string &target)
    {
	string t = string(m, 0, m.find(": "));
	if (t.length() == m.length())
	    return;
	m.erase(0, t.length() + 2);
	if (m.length() && m[0] == ' ')
	    m.erase(m.begin());

	src = string(t, 0, t.find("->"));
	if (src.length() != t.length())
	    target = string(t, src.length() + 2);
    }

    void XChat::striphtmlent(string &m)
    {
	RECODE_OUTER outer = recode_new_outer (false);
	RECODE_REQUEST request = recode_new_request (outer);
	RECODE_TASK task;
	bool success;

	recode_scan_request (request, "html..flat");

	task = recode_new_task (request);
	task->input.buffer = m.c_str();
	task->input.cursor = m.c_str();
	task->input.limit = m.c_str() + m.length();
	task->output.buffer = 0;
	task->output.cursor = 0;
	task->output.limit = 0;
	success = recode_perform_task (task);

	if (task->output.buffer) {
	    *task->output.cursor = 0;
	    m = task->output.buffer;
	    free(task->output.buffer);
	    task->output.buffer = 0;
	}

	recode_delete_task (task);
	recode_delete_request (request);
	recode_delete_outer (outer);
    }
}
