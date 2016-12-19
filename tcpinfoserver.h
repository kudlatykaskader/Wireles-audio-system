/*
 * Copyright 2016 <copyright holder> <email>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */

#ifndef TCPINFOSERVER_H
#define TCPINFOSERVER_H

#include <SFML/Network.hpp>
#include <vector>
#include <thread>

class tcpInfoServer
{
public:
    tcpInfoServer();

    void bindServerToPort(uint32_t port);
    void stopTcpServer();
    void stopListening();
    void tcpServerMainThread();
    void testClients();
    void printClients();
    bool isNewClientConnectionRequest();
    bool isPendingAction();
    bool acceptNewClientConnection();
    bool testBandwidth(sf::TcpSocket *client);
    bool testBandwidth(std::vector<sf::TcpSocket*> *clients);
    
    
    
    std::string getClientMessage(sf::TcpSocket tcpSocket);
    std::thread tcpServerThread();
        
    std::vector<sf::TcpSocket*> *tcpClients;
    
    sf::TcpSocket getClientWithPendingMessage();
    
private:
    sf::TcpListener tcpServerListener;
    sf::SocketSelector tcpSelector;
    sf::TcpSocket tcpSocket;
    uint32_t tcpServerPort;

    bool done;
    bool usingPacket;
    
};

#endif // TCPINFOSERVER_H
