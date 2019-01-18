#ifndef _COMMON
#define _COMMON

using namespace std;
#include <string>
extern string WORKDIR;
bool readConfigFile(const char *cfgfilepath, const string &key, string &value);
vector<string> messageSplit(string message);
#endif