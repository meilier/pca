#include "common.hpp"
#include <fstream>
#include <iostream>
string WORKDIR;

bool readConfigFile(const char *cfgfilepath, const string &key, string &value)
{
    fstream cfgFile;
    cfgFile.open(cfgfilepath); //打开文件
    printf("path is %s\n",cfgfilepath);
    if (!cfgFile.is_open())
    {
        cout << "can not open cfg file!" << endl;
        return false;
    }
    char tmp[1000];
    while (!cfgFile.eof()) //read recursive
    {
        cfgFile.getline(tmp, 1000); //
        string line(tmp);
        printf("line is %s\n",line.c_str());
        size_t pos = line.find('='); //find =
        if (pos == string::npos)
            continue;
        string tmpKey = line.substr(0, pos); //get content in front of =
        printf("tmpKey is %s\n",tmpKey.c_str());
        if (key == tmpKey)
        {
            value = line.substr(pos + 1); //get content behind =
            return true;
        }
    }
    return false;
}
