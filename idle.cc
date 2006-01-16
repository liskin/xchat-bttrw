#include "idle.h"

namespace xchat {
    using namespace std;

    /**
     * \brief Xor base for idle string generator.
     */
    static const char xor_base = 12;

    /**
     * Generates just a simple random string that can be recognized (and that
     * I hope noone normally writes).
     * \return The generated string.
     */
    string genidle()
    {
	char a[31] = { 0 };

	for (int i=0; i<15; i++) {
	    unsigned char x = rand() & 0xf;
	    a[i] = 0x41 + x;
	    a[i + 15] = 0x61 + (x ^ xor_base);
	}

	return a;
    }

    /**
     * Check, if the given string is the one we generated in #genidle.
     * \param s The input string.
     * \return True if it is.
     */
    bool checkidle(string s)
    {
	if (s.length() != 30)
	    return 0;

	for (int i=0; i<15; i++) {
	    if (s[i] < 0x41 || s[i] >= 0x51 || s[i + 15] < 0x61 || s[i + 15] >= 0x71)
		return 0;

	    if (((s[i] - 0x41) ^ (s[i + 15] - 0x61)) != xor_base)
		return 0;
	}

	return 1;
    }
}
