#include<iostream>
#include<sys/socket.h>
int main(){
    char a[10] = "haha";
    std::cout << a << std::endl;
    if(strcmp(a,"haha") == 0){
        std::cout << "yes it is" << std::endl;
    }
    if(a == "haha" ){
        std::cout << "yes it is too" << std::endl;
    }
    std::cout << "hello world"<< std::endl;
    return 0;
}