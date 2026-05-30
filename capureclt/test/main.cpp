#include "../src/client/include/client.h"
#include <iostream>
#include <string>
#include <sstream>
int main(int argc, char const *argv[]) {
    
    ScClient Clt;
    Clt.state=0;
    if(argc==2){
        Clt.Us.port = 9992;
        Clt.Ur.port = 9993;
    }
    else{
        Clt.Us.port = 9990;
        Clt.Ur.port = 9991;
    }
    scclient_init(&Clt,NULL);
    int server_port=9971;
    std::string server_ip="127.0.0.1";
    std::string cmd;
    std::string option;
    std::string value;
    while (true) {
        std::cout << "Commands: ip , port , connect , toggle, stop, exit\n>";
        getline(std::cin, cmd);
        std::stringstream ss(cmd);
        ss>>option>>value;    
        if (option=="ip")server_ip=value;
        else if(option=="port")server_port=std::stoi(value);
        else if (option == "connect"){
            if(Clt.state==CLIENT_STATE_OPERATING)scclient_stop(&Clt);
            if(udp_connect(&Clt.Us, NULL, server_port,server_ip.c_str()))
            scclient_start(&Clt);
        }
        else if (option == "toggle")udp_send(&Clt.Us, UDPDATA_TOGGLE_STREAM, 0, NULL);
        else if (option == "stop")scclient_stop(&Clt); 
        else if (option == "exit"){
            if(sclient_deinit(&Clt)){
                std::cout << "Exited\n";
                break;
            }
            
        }   
    }
    return 0;
}
