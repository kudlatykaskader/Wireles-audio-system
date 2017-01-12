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

#include "tcpinfoserver.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>
#include "Timer.h"
#include <cstring>
#include <string.h>
#include <vector>

#define RED "\033[1;31m"
#define END "\033[0m"
tcpInfoServer::tcpInfoServer()
{
    this->tcpServerPort = 9999;
    this->bindServerToPort(this->tcpServerPort);
    this->tcpSelector.add(this->tcpServerListener);
    this->done = 0;
    this->usingPacket = 0;
    this->tcpClients = new std::vector<sf::TcpSocket*>;
}

void tcpInfoServer::bindServerToPort(uint32_t port)
{
    while(this->tcpServerListener.listen(port) != sf::Socket::Done )
    {
        sleep(5);
    }
    std::cout << RED << "TCP_SERVER :: "<< END <<" Listener binded succesfully, accepting new clients" << std::endl;
}

void tcpInfoServer::tcpServerMainThread()
{
    while(!done)
    {
        if( isPendingAction() )
        {
            if( isNewClientConnectionRequest() )
            {
                acceptNewClientConnection();
            }
            else
            {
                for( int i = 0; i < tcpClients->size(); i++ )
                {
                    if( tcpSelector.isReady( *tcpClients->at(i) ) )
                    {
                        if(usingPacket)
                        {
                            sf::Packet inPacket, outPacket;
                            if( tcpClients->at(i)->receive(inPacket) == sf::Socket::Done )
                            {
                                std::stringstream inStream;
                                inStream << inPacket;
                                std::cout << "Message from client: " << tcpClients->at(i)->getRemoteAddress().toString() << std::endl;
                                std::cout << "Content: " << inStream.str() << std::endl;
                            }
                        }
                        else
                        {
                            char inBuffer[100];
                            size_t received;
                            if( tcpClients->at(i)->receive(inBuffer, sizeof(inBuffer), received) == sf::Socket::Done )
                            {
                                std::cout << "Message from client: " << tcpClients->at(i)->getRemoteAddress().toString() << std::endl;
                                std::cout << "Content: " << inBuffer << std::endl;
                                for (int i = 0; i < sizeof(inBuffer); i++)
                                    inBuffer[i] = 0;
                            }
                        }//TODO::Do something with informations from clients;
                    }
                }
            }
        }
    }
    std::cout <<  "TCP loop exit " <<  std::endl;
    
}

bool tcpInfoServer::isPendingAction()
{
    char testPacket[35] = {'t',  'e',  's',  't',  '_',  'p',  'a',  'c',  'k',  'e',  't'};
    for (int i = 0; i < tcpClients->size(); i++ )
        if ( tcpClients->at(i)->send(testPacket,  sizeof(testPacket) ) != sf::Socket::Done )
        {
            std::cout << "TCP_SERVER :: Client " << tcpClients->at(i)->getRemoteAddress() << " disconnected" <<  std::endl;
            tcpSelector.remove( *tcpClients->at(i) );
            delete tcpClients->at(i);
            tcpClients->erase(tcpClients->begin() + i);
            
            std::cout << "TCP_SERVER :: Clients list: "<< tcpClients->size() << std::endl;
        }
    return this->tcpSelector.wait( sf::seconds(5) );
}

bool tcpInfoServer::isNewClientConnectionRequest()
{
    return this->tcpSelector.isReady(this->tcpServerListener);
}

bool tcpInfoServer::acceptNewClientConnection()
{
    sf::TcpSocket *socket = new sf::TcpSocket;
    tcpServerListener.accept(*socket);
    std::cout << "TCP_SERVER :: New client connected. Ip = " << socket->getRemoteAddress().toString() << std::endl;
    if ( socket->send("[WIFI_DATA;NETIASPOT-436401;WIFI_DEMO_PASS]",  43 ) != sf::Socket::Done )
        {
            std::cout << "TCP_SERVER :: ERRPR" << std::endl;
        }
    //if( testBandwidth(socket) )
    //    return 0;
    tcpClients->push_back(socket);
    tcpSelector.add(*socket);
    //TODO:: Accepting initial message from new client, in order to identify him //
    return 0;
}

std::thread tcpInfoServer::tcpServerThread()
{
    return std::thread([=] { tcpServerMainThread(); });
}

bool tcpInfoServer::testBandwidth(sf::TcpSocket *client)
{
    std::cout << "TCP_SERVER :: Testing connection bandwidth.. " << std::endl;
    char inBuffer_1[] = "TCP_CONNECTION_BANDWIDTH_TEST_START";
    sf::Packet packet;
    packet << "TCP_CONNECTION_BANDWIDTH_TEST_START";
    if ( client->send(inBuffer_1, sizeof(inBuffer_1)) != sf::Socket::Done )
    {
        std::cout << "TCP_SERVER :: Bandwidth test initialization failed: Failed to send START command" << std::endl;
        return 1;
    }
    packet.clear();
    char inBuffer_2[100];
    size_t received;
    if ( client->receive(inBuffer_2, 38, received) != sf::Socket::Done )
    {
        std::cout << "TCP_SERVER :: Bandwidth test initialization failed: Failed to receive START_ACK command: Client propably disconnected" << std::endl;
        return 1;
    }
    if ( !strcmp(inBuffer_2, "START_ACK"))
    {
        std::cout << "TCP_SERVER :: Bandwidth test initialization failed: Failed to receive START_ACK command: Received wrong ACK: " << inBuffer_2 << std::endl;
        return 1;
    }
    packet.clear();

    std::cout << "TCP_SERVER :: Test init complete" << std::endl;

    for ( int i = 0 ; i < 100; i++ )
    {
        packet << ".";
    }

    Timer timer;
    timer.start();

    for ( int i = 0 ; i < 100; i++ )
    {
        if ( client->send(packet) != sf::Socket::Done )
        {
            std::cout << "TCP_SERVER :: Bandwidth test failed in mid-time: Propobly client disconnected" << std::endl;
            timer.stop("TCP_SERVER :: Test failed after");
            return 1;
        }
    }
    std::cout << "Bandwidth = " << 10/timer.getDelay() << " kb/s" << std::endl;
    timer.stop("TCP_SERVER :: Test passed");

    return 0;
}

bool tcpInfoServer::testBandwidth(std::vector<sf::TcpSocket *> *clients)
{
    for(int k = 0; k < clients->size(); k++ )
    {
        std::cout << "TCP_SERVER :: Testing connection bandwidth.. " << std::endl;
        char inBuffer_1[] = "TCP_CONNECTION_BANDWIDTH_TEST_START";
        sf::Packet packet;
        packet << "TCP_CONNECTION_BANDWIDTH_TEST_START";
        if ( clients->at(k)->send(inBuffer_1, sizeof(inBuffer_1)) != sf::Socket::Done )
        {
            std::cout << "TCP_SERVER :: Bandwidth test initialization failed: Failed to send START command" << std::endl;
            clients->erase(clients->begin() + k);
            continue;
        }
        packet.clear();
        char inBuffer_2[100];
        size_t received;
        if ( clients->at(k)->receive(inBuffer_2, 38, received) != sf::Socket::Done )
        {
            std::cout << "TCP_SERVER :: Bandwidth test initialization failed: Failed to receive START_ACK command: Client propably disconnected" << std::endl;
            clients->erase(clients->begin() +k);
            continue;
        }
        if ( !strcmp(inBuffer_2, "START_ACK"))
        {
            std::cout << "TCP_SERVER :: Bandwidth test initialization failed: Failed to receive START_ACK command: Received wrong ACK: " << inBuffer_2 << std::endl;
            clients->erase(clients->begin() +k);
            continue;
        }
        packet.clear();

        
        char testPacket[50000];
        for (int i = 0; i < sizeof(testPacket); i++)
            testPacket[i] = '$';
        Timer timer;
        timer.start();
        for ( int i = 0 ; i < 100; i++ )
        {
            if ( clients->at(k)->send(testPacket,  sizeof(testPacket) ) != sf::Socket::Done )
            {
                std::cout << "TCP_SERVER :: Bandwidth test failed in mid-time: Propobly client disconnected" << std::endl;
                timer.stop("TCP_SERVER :: Test failed after");
                clients->erase(clients->begin() + i);
                continue;
            }
        }
        std::cout << "Bandwidth = " << 10/timer.getDelay() << " kb/s" << std::endl;
        timer.stop("TCP_SERVER :: Test passed");

        return 0;

    }
    return 0;
}


void tcpInfoServer::stopTcpServer()
{
    tcpServerListener.close();
    done = true;
    std::cout << "Program EXIT" << std::endl;
}

void tcpInfoServer::testClients()
{
    for (int i = 0; i< tcpClients->size(); i++ )
    {
        testBandwidth( tcpClients->at(i) ); 
    }
}
void tcpInfoServer::printClients()
{
    for (int i = 0; i< tcpClients->size(); i++ )
    {
        std::cout <<  i << ": IP: " << tcpClients->at(i)->getRemoteAddress() << " " << tcpClients->at(i)->getRemotePort(); 
    }
}








