#include "xchat.h"
#include "TomiTCP/http.h"
using namespace net;

namespace xchat {
    /**
     * Helper function to add hostname if server exists and to add server if
     * not.
     */
    static void addserver(vector<server> &servers, const sockaddr_uni &host,
            string hostname, server_type type)
    {
        for (vector<server>::iterator i = servers.begin(); i != servers.end(); i++) {
            if (i->host == host) {
                i->types[type] = hostname;
                return;
            }
        }

        server s;
        s.host = host;
        s.types[type] = hostname;
        servers.push_back(s);
    }

    /**
     * Get the list of available servers from DNS.
     * If not possible (in proxy environments for example), add
     * "xchat.centrum.cz" (clustering and failure tracking won't work).
     */
    void XChat::makeservers()
    {
	int i;
	string hostname;

	for (char *c = "sp"; *c; c++) {
	    for (i = 1; i <= 100 /* safety limit */; i++) {
		vector<sockaddr_uni> sockaddrs;
		hostname = *c + string("x") + tostr<int>(i) + ".xchat.centrum.cz";
	        try {
		    resolve(hostname, "", sockaddrs);
		} catch (...) { }

		if (!sockaddrs.size())
		    break;

                for (vector<sockaddr_uni>::iterator i = sockaddrs.begin();
                        i != sockaddrs.end(); i++)
                    addserver(servers, *i, hostname, SERVER_MODCHAT);
	    }
	}

        {
            vector<sockaddr_uni> sockaddrs;
            hostname = "scripts.xchat.centrum.cz";
            try {
                resolve(hostname, "", sockaddrs);
            } catch (...) { }

            for (vector<sockaddr_uni>::iterator i = sockaddrs.begin();
                    i != sockaddrs.end(); i++)
                addserver(servers, *i, hostname, SERVER_SCRIPTS);
        }

        if (servers.empty()) {
            server s;
            s.host.sa.sa_family = 0;
            s.types[SERVER_MODCHAT] = "xchat.centrum.cz";
            s.types[SERVER_SCRIPTS] = "scripts.xchat.centrum.cz";
            servers.push_back(s);
        }
    }

    /**
     * Last server broke, count it.
     * \return A string representation of the server to let the user know.
     */
    string XChat::lastsrv_broke()
    {
	if (lastsrv >= 0 && servers.size() > 0) {
	    servers[lastsrv].last_break = time(0);
	    servers[lastsrv].total_break_count++;
	    servers[lastsrv].break_count++;

	    if (servers[lastsrv].break_count >= tries_to_rest) {
		auto_ptr<EvError> e(new EvError);
		e->s = "Server " + tomi_ntop(servers[lastsrv].host) + " is having a rest.";
		recvq_push((auto_ptr<Event>) e);
	    }
            
            return tomi_ntop(servers[lastsrv].host);
	}

        return "";
    }
	    
    /**
     * Get a random working server for a given server type. Unrest servers
     * when the time comes or when no server is available.
     * \param type Server type.
     * \return Index in the #servers array.
     */
    int XChat::makesrv(server_type type)
    {
	vector<int> good;

	for (vector<server>::iterator i = servers.begin(); i != servers.end(); i++) {
            if (!i->supports(type))
                continue;

	    if (i->break_count >= tries_to_rest &&
		    (i->last_break + rest_duration) < time(0)) {
		auto_ptr<EvError> e(new EvError);
		e->s = "Server " + tomi_ntop(i->host)
		    + " is no longer having a rest.";
		recvq_push((auto_ptr<Event>) e);

		i->break_count = 0;
		good.push_back(i - servers.begin());
	    } else if (i->break_count < tries_to_rest &&
		    (i->last_break + nextchance_interval) < time(0)) {
		good.push_back(i - servers.begin());
	    }
	}

	if (good.size())
	    return (lastsrv = good[rand() % good.size()]);
	else {
	    auto_ptr<EvNeedRelogin> e(new EvNeedRelogin);
	    e->s = "All servers considered bad, cleaning.";
	    recvq_push((auto_ptr<Event>) e);

	    for (vector<server>::iterator i = servers.begin(); i != servers.end(); i++) {
                if (i->supports(type)) {
                    i->break_count = 0;
                    i->last_break = 0;
                    good.push_back(i - servers.begin());
                }
	    }

	    return (lastsrv = good[rand() % good.size()]);
	}
    }

    /**
     * Construct a path using a given path and type.
     * \param path The path.
     * \param type Type, see path_type.
     */
    string XChat::makepath(const string& path, path_type type)
    {
        if (type == PATH_PLAIN)
            return "/" + path;
        else if (type == PATH_AUTH)
            return "/~$" + uid + "~" + sid + "/" + path;
        else if (type == PATH_STATIC)
            return "http://" + fallback_server + "/~$" + uid + "~" + sid + "/" + path;
        else
            throw invalid_argument("Unknown path_type");
    }

    /**
     * Do a GET request...
     */
    int XChat::request_GET(TomiHTTP &s, server_type st, const string& path,
            path_type pt)
    {
        int si = makesrv(st);
        server &ss = servers[si];

        s.doconnect(ss.host.sa.sa_family ? tomi_ntop(ss.host) : ss.types[st], 80);
        s.doGET(ss.types[st], 80, makepath(path, pt), 0);
        return s.parseresponse(0);
    }
    
    /**
     * Do a POST request...
     */
    int XChat::request_POST(TomiHTTP &s, server_type st, const string& path,
            path_type pt, const string &data)
    {
        int si = makesrv(st);
        server &ss = servers[si];

        s.doconnect(ss.host.sa.sa_family ? tomi_ntop(ss.host) : ss.types[st], 80);
        s.doPOST(ss.types[st], 80, makepath(path, pt), data, 0);
        return s.parseresponse(0);
    }
}
