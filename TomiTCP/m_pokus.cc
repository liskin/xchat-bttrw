#include <iostream>
#include "irc.h"
using namespace std;

extern "C" {

void m_pokus_init()
{
    cout << "m_pokus init" << endl;
}

int m_pokus_config(string a, string b)
{
    return 1;
}

}
