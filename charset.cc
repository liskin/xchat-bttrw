#include <map>
#include <stdexcept>
#include <iconv.h>
#include <errno.h>
#include "charset.h"
#include "TomiTCP/str.h"

namespace xchat {
    map<string, unsigned short> trans;

    /*
     * Fill in the translation hashtable
     */
    void init_recode()
    {
#define ENTRY(a,b) trans[b] = a
	ENTRY (33, "excl");
	ENTRY (34, "quot");
	ENTRY (35, "num");
	ENTRY (36, "dollar");
	ENTRY (37, "percnt");
	ENTRY (38, "amp");
	ENTRY (39, "apos");
	ENTRY (40, "lpar");
	ENTRY (41, "rpar");
	ENTRY (42, "ast");
	ENTRY (43, "plus");
	ENTRY (44, "comma");
	ENTRY (45, "horbar");
	ENTRY (46, "period");
	ENTRY (58, "colon");
	ENTRY (59, "semi");
	ENTRY (60, "lt");
	ENTRY (61, "equals");
	ENTRY (62, "gt");
	ENTRY (63, "quest");
	ENTRY (64, "commat");
	ENTRY (91, "lsqb");
	ENTRY (93, "rsqb");
	ENTRY (94, "uarr");
	ENTRY (95, "lowbar");
	ENTRY (96, "grave");
	ENTRY (123, "lcub");
	ENTRY (124, "verbar");
	ENTRY (125, "rcub");
	ENTRY (126, "tilde");
	ENTRY (160, "nbsp");
	ENTRY (161, "iexcl");
	ENTRY (162, "cent");
	ENTRY (163, "pound");
	ENTRY (164, "curren");
	ENTRY (165, "yen");
	ENTRY (166, "brkbar");
	ENTRY (166, "brvbar");
	ENTRY (167, "sect");
	ENTRY (168, "die");
	ENTRY (168, "uml");
	ENTRY (169, "copy");
	ENTRY (170, "ordf");
	ENTRY (171, "laquo");
	ENTRY (172, "not");
	ENTRY (173, "hyphen");
	ENTRY (173, "shy");
	ENTRY (174, "reg");
	ENTRY (175, "hibar");
	ENTRY (175, "macr");
	ENTRY (176, "deg");
	ENTRY (177, "plusmn");
	ENTRY (178, "sup2");
	ENTRY (179, "sup3");
	ENTRY (180, "acute");
	ENTRY (181, "micro");
	ENTRY (182, "para");
	ENTRY (183, "middot");
	ENTRY (184, "cedil");
	ENTRY (185, "sup1");
	ENTRY (186, "ordm");
	ENTRY (187, "raquo");
	ENTRY (188, "frac14");
	ENTRY (189, "half");
	ENTRY (189, "frac12");
	ENTRY (190, "frac34");
	ENTRY (191, "iquest");
	ENTRY (192, "Agrave");
	ENTRY (193, "Aacute");
	ENTRY (194, "Acircu");
	ENTRY (194, "Acirc");
	ENTRY (195, "Atilde");
	ENTRY (196, "Adiaer");
	ENTRY (196, "Auml");
	ENTRY (197, "Aring");
	ENTRY (198, "AE");
	ENTRY (198, "AElig");
	ENTRY (199, "Ccedil");
	ENTRY (200, "Egrave");
	ENTRY (201, "Eacute");
	ENTRY (202, "Ecircu");
	ENTRY (202, "Ecirc");
	ENTRY (203, "Ediaer");
	ENTRY (203, "Euml");
	ENTRY (204, "Igrave");
	ENTRY (205, "Iacute");
	ENTRY (206, "Icircu");
	ENTRY (206, "Icirc");
	ENTRY (207, "Idiaer");
	ENTRY (207, "Iuml");
	ENTRY (208, "ETH");
	ENTRY (209, "Ntilde");
	ENTRY (210, "Ograve");
	ENTRY (211, "Oacute");
	ENTRY (212, "Ocircu");
	ENTRY (212, "Ocirc");
	ENTRY (213, "Otilde");
	ENTRY (214, "Odiaer");
	ENTRY (214, "Ouml");
	ENTRY (215, "MULT");
	ENTRY (215, "times");
	ENTRY (216, "Ostroke");
	ENTRY (216, "Oslash");
	ENTRY (217, "Ugrave");
	ENTRY (218, "Uacute");
	ENTRY (219, "Ucircu");
	ENTRY (219, "Ucirc");
	ENTRY (220, "Udiaer");
	ENTRY (220, "Uuml");
	ENTRY (221, "Yacute");
	ENTRY (222, "THORN");
	ENTRY (223, "ssharp");
	ENTRY (223, "szlig");
	ENTRY (224, "agrave");
	ENTRY (225, "aacute");
	ENTRY (226, "acircu");
	ENTRY (226, "acirc");
	ENTRY (227, "atilde");
	ENTRY (228, "adiaer");
	ENTRY (228, "auml");
	ENTRY (229, "aring");
	ENTRY (230, "ae");
	ENTRY (230, "aelig");
	ENTRY (231, "ccedil");
	ENTRY (232, "egrave");
	ENTRY (233, "eacute");
	ENTRY (234, "ecircu");
	ENTRY (234, "ecirc");
	ENTRY (235, "ediaer");
	ENTRY (235, "euml");
	ENTRY (236, "igrave");
	ENTRY (237, "iacute");
	ENTRY (238, "icircu");
	ENTRY (238, "icirc");
	ENTRY (239, "idiaer");
	ENTRY (239, "iuml");
	ENTRY (240, "eth");
	ENTRY (241, "ntilde");
	ENTRY (242, "ograve");
	ENTRY (243, "oacute");
	ENTRY (244, "ocircu");
	ENTRY (244, "ocirc");
	ENTRY (245, "otilde");
	ENTRY (246, "odiaer");
	ENTRY (246, "ouml");
	ENTRY (247, "DIVIS");
	ENTRY (247, "divide");
	ENTRY (248, "ostroke");
	ENTRY (248, "oslash");
	ENTRY (249, "ugrave");
	ENTRY (250, "uacute");
	ENTRY (251, "ucircu");
	ENTRY (251, "ucirc");
	ENTRY (252, "udiaer");
	ENTRY (252, "uuml");
	ENTRY (253, "yacute");
	ENTRY (254, "thorn");
	ENTRY (255, "ydiaer");
	ENTRY (255, "yuml");
	ENTRY (338, "OElig");
	ENTRY (339, "oelig");
	ENTRY (352, "Scaron");
	ENTRY (353, "scaron");
	ENTRY (376, "Yuml");
	ENTRY (402, "fnof");
	ENTRY (710, "circ");
	ENTRY (732, "tilde");
	ENTRY (913, "Alpha");
	ENTRY (914, "Beta");
	ENTRY (915, "Gamma");
	ENTRY (916, "Delta");
	ENTRY (917, "Epsilon");
	ENTRY (918, "Zeta");
	ENTRY (919, "Eta");
	ENTRY (920, "Theta");
	ENTRY (921, "Iota");
	ENTRY (922, "Kappa");
	ENTRY (923, "Lambda");
	ENTRY (924, "Mu");
	ENTRY (925, "Nu");
	ENTRY (926, "Xi");
	ENTRY (927, "Omicron");
	ENTRY (928, "Pi");
	ENTRY (929, "Rho");
	ENTRY (931, "Sigma");
	ENTRY (932, "Tau");
	ENTRY (933, "Upsilon");
	ENTRY (934, "Phi");
	ENTRY (935, "Chi");
	ENTRY (936, "Psi");
	ENTRY (937, "Omega");
	ENTRY (945, "alpha");
	ENTRY (946, "beta");
	ENTRY (947, "gamma");
	ENTRY (948, "delta");
	ENTRY (949, "epsilon");
	ENTRY (950, "zeta");
	ENTRY (951, "eta");
	ENTRY (952, "theta");
	ENTRY (953, "iota");
	ENTRY (954, "kappa");
	ENTRY (955, "lambda");
	ENTRY (956, "mu");
	ENTRY (957, "nu");
	ENTRY (958, "xi");
	ENTRY (959, "omicron");
	ENTRY (960, "pi");
	ENTRY (961, "rho");
	ENTRY (962, "sigmaf");
	ENTRY (963, "sigma");
	ENTRY (964, "tau");
	ENTRY (965, "upsilon");
	ENTRY (966, "phi");
	ENTRY (967, "chi");
	ENTRY (968, "psi");
	ENTRY (969, "omega");
	ENTRY (977, "thetasym");
	ENTRY (978, "upsih");
	ENTRY (982, "piv");
	ENTRY (8194, "ensp");
	ENTRY (8195, "emsp");
	ENTRY (8201, "thinsp");
	ENTRY (8204, "zwnj");
	ENTRY (8205, "zwj");
	ENTRY (8206, "lrm");
	ENTRY (8207, "rlm");
	ENTRY (8211, "ndash");
	ENTRY (8212, "mdash");
	ENTRY (8216, "lsquo");
	ENTRY (8217, "rsquo");
	ENTRY (8218, "sbquo");
	ENTRY (8220, "ldquo");
	ENTRY (8221, "rdquo");
	ENTRY (8222, "bdquo");
	ENTRY (8224, "dagger");
	ENTRY (8225, "Dagger");
	ENTRY (8226, "bull");
	ENTRY (8230, "hellip");
	ENTRY (8240, "permil");
	ENTRY (8242, "prime");
	ENTRY (8243, "Prime");
	ENTRY (8249, "lsaquo");
	ENTRY (8250, "rsaquo");
	ENTRY (8254, "oline");
	ENTRY (8260, "frasl");
	ENTRY (8364, "euro");
	ENTRY (8465, "image");
	ENTRY (8472, "weierp");
	ENTRY (8476, "real");
	ENTRY (8482, "trade");
	ENTRY (8501, "alefsym");
	ENTRY (8592, "larr");
	ENTRY (8593, "uarr");
	ENTRY (8594, "rarr");
	ENTRY (8595, "darr");
	ENTRY (8596, "harr");
	ENTRY (8629, "crarr");
	ENTRY (8656, "lArr");
	ENTRY (8657, "uArr");
	ENTRY (8658, "rArr");
	ENTRY (8659, "dArr");
	ENTRY (8660, "hArr");
	ENTRY (8704, "forall");
	ENTRY (8706, "part");
	ENTRY (8707, "exist");
	ENTRY (8709, "empty");
	ENTRY (8711, "nabla");
	ENTRY (8712, "isin");
	ENTRY (8713, "notin");
	ENTRY (8715, "ni");
	ENTRY (8719, "prod");
	ENTRY (8721, "sum");
	ENTRY (8722, "minus");
	ENTRY (8727, "lowast");
	ENTRY (8730, "radic");
	ENTRY (8733, "prop");
	ENTRY (8734, "infin");
	ENTRY (8736, "ang");
	ENTRY (8743, "and");
	ENTRY (8744, "or");
	ENTRY (8745, "cap");
	ENTRY (8746, "cup");
	ENTRY (8747, "int");
	ENTRY (8756, "there4");
	ENTRY (8764, "sim");
	ENTRY (8773, "cong");
	ENTRY (8776, "asymp");
	ENTRY (8800, "ne");
	ENTRY (8801, "equiv");
	ENTRY (8804, "le");
	ENTRY (8805, "ge");
	ENTRY (8834, "sub");
	ENTRY (8835, "sup");
	ENTRY (8836, "nsub");
	ENTRY (8838, "sube");
	ENTRY (8839, "supe");
	ENTRY (8853, "oplus");
	ENTRY (8855, "otimes");
	ENTRY (8869, "perp");
	ENTRY (8901, "sdot");
	ENTRY (8968, "lceil");
	ENTRY (8969, "rceil");
	ENTRY (8970, "lfloor");
	ENTRY (8971, "rfloor");
	ENTRY (9001, "lang");
	ENTRY (9002, "rang");
	ENTRY (9674, "loz");
	ENTRY (9824, "spades");
	ENTRY (9827, "clubs");
	ENTRY (9829, "hearts");
	ENTRY (9830, "diams");
#undef ENTRY
    }

    /*
     * Convert UCS-2 to UTF-8
     */
    string ucs2_to_utf8(const char* src, int sz)
    {
	iconv_t conv = iconv_open("UTF-8", "UCS-2");
	if (conv == (iconv_t) -1)
	    throw runtime_error("Could not init iconv UCS-2 => UTF-8");

	size_t fromsize = sz, tosize = fromsize, ressize = tosize;
	const char *msgptr = src;
	char *result = new char[fromsize+1];
	char *resptr = result;

	while ((fromsize>0) && (tosize>0)) {
#ifdef WIN32
	    if ((int)iconv(conv, &msgptr, &fromsize, &resptr, &tosize) == -1)
#else
	    if ((int)iconv(conv, (char **)&msgptr, &fromsize, &resptr, &tosize) == -1)
#endif
	    {
		int err = errno;
		
		// array is not big enough
		if (err == E2BIG) {
		    // add fromsize + 4 more characters to array
		    result = (char*) realloc(result,ressize + fromsize + 4);
		    resptr = result + ressize;
		    ressize += fromsize + 4;
		    tosize += fromsize + 4;
		    continue;
		}

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
     * Parse and convert _one_ HTML entity
     */
    char *striphtmlent_recode(const char *input)
    {
	if (*input != '&')
	    return 0;
	
	bool valid = true;
	unsigned value = 0;

	input++;
	if (*input == '#') {
	    input++;
	    if (*input == 'x' || *input == 'X') {
		/* Scan &#[xX][0-9a-fA-F]+; notation.  */

		input++;
		while (valid) {
		    if (*input >= '0' && *input <= '9')
			value = 16 * value + *input - '0';
		    else if (*input >= 'A' && *input <= 'F')
			value = 16 * value + *input - 'A' + 10;
		    else if (*input >= 'a' && *input <= 'f')
			value = 16 * value + *input - 'a' + 10;
		    else if (*input == ';')
			break;
		    else
			valid = false;

		    if (value >= 65535)
			valid = false;
		    else
			input++;
		}
	    } else {
		/* Scan &#[0-9]+; notation.  */

		while (valid)
		{
		    if (*input >= '0' && *input <= '9')
			value = 10 * value + *input - '0';
		    else if (*input == ';')
			break;
		    else
			valid = false;

		    if (value >= 65535)
			valid = false;
		    else
			input++;
		}
	    }
	} else if ((*input >= 'A' && *input <= 'Z') || (*input >= 'a' && *input <= 'z')) {
	    /* Scan &[A-Za-z][A-Za-z0-9]*; notation.  */

	    char *semicolon = strchr(input, ';');
	    if (!semicolon) {
		valid = false;
	    } else {
		map<string,unsigned short>::iterator i =
		    trans.find(string(input, semicolon - input));
		if (i == trans.end())
		    valid = false;
		else
		    value = i->second;
	    }
	} else
	    return 0;

	if (!valid)
	    return 0;
	
	static char buf[3] = {0, 0, 0};
	*(unsigned short*)buf = value;

	return strdup(ucs2_to_utf8(buf, 2).c_str());
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
		    ret = striphtmlent_recode(token.c_str());
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
