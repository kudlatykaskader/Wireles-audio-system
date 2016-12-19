#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <thread>
#include <unistd.h>
#include <string>
#include <sstream>
#include <cstdlib>

#include <unistd.h>
#include <sys/time.h>
#include "tcpinfoserver.h"
#include "udpStreamSource.h"

tcpInfoServer *tcpServer;
udpServer *udpStreamServer;

std::thread tcpServerThread;
std::thread udpServerThread;
std::thread *cliThread; 

bool systemExit;
void runUdpServer(unsigned short port);

void exitHandler()
{
    std::cout<< "Exit handler in main" << std::endl;
    
}

void prompt()
{
    std::cout << "#:";
}
void cliHandler()
{
    std::string userInputString;
    while(1)
    {
        prompt();
        std::cin >> userInputString;
        //prompt();
        
        if(userInputString == "pause_audio_stream")
        {
            udpStreamServer->pauseAudioStream();
        }
        else if(userInputString == "resume_audio_stream")
        {
            udpStreamServer->resumeAudioStream();
        }
        else if(userInputString == "test_clients")
        {
            tcpServer->testClients();
        }
        else if(userInputString == "run_bandwidth_test")
        {
            tcpServer->testBandwidth(tcpServer->tcpClients);
        }
        else if(userInputString == "print_clients")
        {
            tcpServer->printClients();
        }
        else if(userInputString == "set_audio_bitrate")
        {
            
        }
        else if( userInputString.find("set_test_mode") != std::string::npos )
        {
            udpStreamServer->setTestMode( std::stoi(userInputString.substr(userInputString.find(":") +1)));
        }
        else if(userInputString.find("test_mode_packet_size") != std::string::npos )
        {
            udpStreamServer->setTestModeDataSize( std::stoi(userInputString.substr(userInputString.find(":") +1)));
        }
        else if(userInputString.find("test_mode_delay_us") != std::string::npos )
        {
            udpStreamServer->setTestModePacketDelay(std::stoi(userInputString.substr(userInputString.find(":") +1)));
        }
        else if(userInputString == "exit")
        {
            tcpServer->stopTcpServer();
            udpStreamServer->stopUdpServer();
            exitHandler();
        }
        //else if(userInputString == "")
        else if(userInputString == "help")
        {
            std::cout << "Available Commands:"  << std::endl;
            std::cout << "1: pause_audio_strem"<< std::endl;
            std::cout << "2: resume_audio_stream"<< std::endl;
            std::cout << "3: test_clients"<< std::endl;
            std::cout << "4: run_bandwidth_test" << std::endl;
            std::cout << "5: print_clients" << std::endl;
            std::cout << "6: set_audio_bitrate" << std::endl;
            std::cout << "7: set_test_mode" << std::endl;
            std::cout << "8: test_mode_packet_size" << std::endl;
            std::cout << "9: test_mode_delay_us" << std::endl;
            std::cout << "10: exit" << std::endl;            
        }
        else
            std::cout <<"Unknown command, type 'help'" << std::endl;
        userInputString.clear();
        
            
    }
}

int main(int argc, char*argv[])
{
    tcpServer = new tcpInfoServer();
    std::atexit(exitHandler);
    udpStreamServer = new udpServer();
    
    
    tcpServerThread = tcpServer->tcpServerThread();
    udpServerThread = udpStreamServer->udpServerThread(tcpServer->tcpClients);
    cliThread = new std::thread( cliHandler ); 
    //udpStreamServer->setTestMode(true);
    //udpStreamServer->setTestModePacketDelay(1000000);
    tcpServerThread.join();
    udpServerThread.join();
    delete tcpServer;
    delete udpStreamServer;
    

}

