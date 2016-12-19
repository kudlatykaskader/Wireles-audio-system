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

#ifndef UDPSTREAMSOURCE_H
#define UDPSTREAMSOURCE_H
#include <SFML/Network.hpp>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <lame/lame.h>
#include <thread>
#include <iostream>
#include <unistd.h>

class udpServer
{
public:
    udpServer();
    sf::UdpSocket socket;
    sf::Packet packet;
    sf::IpAddress receiver = "192.168.1.6";
    sf::UdpSocket test_socket;
    pa_simple *s = NULL;
    pa_buffer_attr buff_attr;
    
    pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };
    
    int ret = 1;
    int error;
    int read, write;
    int testModePacketDelay = 0;
    int testModePacketSize = 1400;
    uint32_t testModePacketCounter = 0;
    bool testMode = 0;
    bool done;
    
    bool streamActive = true;
    unsigned int port = 5002;
    
    static const int PCM_SIZE = 1*1024;
    static const int MP3_SIZE = 1*1024;
    
    short int pcm_buffer[PCM_SIZE*2];
    unsigned char mp3_bufer[MP3_SIZE];
    
    uint32_t k;
    uint16_t streamPacketCounter;
    
    lame_t lame;
    
    void runUdpServer(std::vector<sf::TcpSocket*> *clients);
    std::thread udpServerThread(std::vector<sf::TcpSocket*> *clients);
    void pauseAudioStream();
    void resumeAudioStream();
    bool sendtoAll(std::vector<sf::TcpSocket*> *clients, unsigned char* data);
    bool getEncodedSampleToBuffer();
    bool initSocket();
    bool getTestMode();
    void setTestMode(int var);
    void setTestModePacketDelay(uint32_t var);
    void setTestModeDataSize(uint32_t var);
    bool stopUdpServer();
    
    void runUdpServerTest(uint32_t port);
};

#endif // UDPSTREAMSOURCE_H
