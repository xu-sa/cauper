#include "../src/public/server/include/server.h"
#include <iostream>
#include <sstream>
#include <string>

int main(){
    ScServer S;
    ScServer_init(&S);
    ScServer_run(&S);
    int a=1;
    std::string input1;
    while (a)
    {
        std::string value_0;
        std::string value_1;
        std::cout<<">>";
        getline(std::cin,input1);
        std::stringstream ss(input1);
        if(!(ss>>value_0))continue;
        int value_int=-1;
        std::stringstream(value_0)>>value_int;
        switch (value_int)
        {
        case -1:
            a=0;
            break;
        case 1://add
            {
                int i =sccap_stream_get(&S.P);
                sccap_stream_toggle(&S.P,i==1?0:1);
            }
            break;
        
        default:
            std::cout<<"no such option\n";
            break;
        }
    
    }
    ScServer_stop(&S);
    return 0;
}