#include <iostream>
#include <cstdlib>
using namespace std;

void Test1()
{
    try
    {
        char *p = new char[0x7fffffff]; //抛出异常
    }
    catch (exception e)
    {
        cout << e.what() << endl; //捕获异常，然后程序结束
    }
}
int main()
{
    Test1();
    system("pause");
    return 0;
}
