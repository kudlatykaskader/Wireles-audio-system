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

#include "udpStreamSource.h"


udpServer::udpServer()
{

    lame = lame_init();
    lame_set_in_samplerate(lame, 44000);
    lame_set_VBR(lame, vbr_off);
    lame_init_params(lame);
    lame_set_decode_on_the_fly(lame, 1);
    lame_set_quality(lame, vbr_off);
    if(lame != NULL)
        std::cout << "UDP_SERVER_CONSTRUCTOR :: Lame OK " << std::endl;
    buff_attr.maxlength = 1024*8;
    //buff_attr.fragsize = 32;
    /* Create the recording stream */
    if (! (s = pa_simple_new(NULL, NULL, PA_STREAM_RECORD, NULL, "record", &ss, NULL, &buff_attr, &error)))
    {
        std::cout << "Error creating stream" << std::endl;
    }
    else
    {
        std::cout << "UDP_SERVER_CONSTRUCTOR :: Pulse Stream OK " << std::endl;
    }
}
void udpServer::runUdpServer(std::vector<sf::TcpSocket*> *clients)
{
    //initSocket();
    while(!done)
    {
        if( clients->size() == 0 )
        {
            std::cout << "UDP_SERVER :: No stream clients detected, going sleep.. " << std::endl;
            while( clients->size() == 0 )
            {
                sleep(5);
            }
        }
        if(!testMode && streamActive)
        {
            getEncodedSampleToBuffer();
            if(streamPacketCounter%1024 == 0 )
                std::cout << "Packet Counter: " <<(uint16_t)streamPacketCounter<< ", bytes to read: "<< (uint16_t)( mp3_bufer[2] << 8 ) + (uint8_t)(mp3_bufer[3]) << std::endl;
            sendtoAll(clients, mp3_bufer);
            //usleep(10000);
        }
        else if (testMode)
        {
            unsigned char t_data[testModePacketSize];
            t_data[0] = (testModePacketCounter >> 24 ) & 255;
            t_data[1] = (testModePacketCounter >> 16 ) & 255;
            t_data[2] = (testModePacketCounter >> 8  ) & 255;
            t_data[3] = testModePacketCounter & 255;
            t_data[sizeof(t_data) -1] = 1;
            if( !(testModePacketDelay == 0) )
                usleep(testModePacketDelay);
            std::cout << "UDP_SOURCE :: Packet:     "<<(uint32_t)t_data[0]<<"-"<<(uint32_t)t_data[1]<<"-"<<(uint32_t)t_data[2]<<"-"<< (uint32_t)t_data[3]<<"    flyied away.. " << std::endl;
            sendtoAll(clients, t_data);
            std::cout << "test loop " << std::endl;
            testModePacketCounter++;
        }

        else
        {
            std::cout.flush();
            std::cout << "*";
            sleep(10);
        }

    }
}

std::thread udpServer::udpServerThread(std::vector<sf::TcpSocket*> *clients)
{
    return std::thread([=]
    {
        runUdpServer(clients);
    });
}

bool udpServer::sendtoAll(std::vector<sf::TcpSocket*> *clients, unsigned char* data)
{
    for (int i = 0; i < clients->size(); i++)
    {
        if (socket.send(data, write,  clients->at(i)->getRemoteAddress(), 10000) != sf::Socket::Done)
            break;
        if (write == 0)
        {
            std::cout << "Empty packet detected at no " << streamPacketCounter << std::endl; 
        }
        //std::cout << "UDP_SERVER :: Packet send to client " << clients->at(i)->getRemoteAddress() << std::endl;
    }
    return true;
}

bool udpServer::getEncodedSampleToBuffer()
{
    k = k + 1;

    if (pa_simple_read(s, pcm_buffer, sizeof(pcm_buffer), &error) < 0)
        std::cout << "Error while reading samples" << std::endl;
    write = lame_encode_buffer_interleaved(lame, pcm_buffer, PCM_SIZE, mp3_bufer, MP3_SIZE);

    for(int i = MP3_SIZE; i >=4 ; i-- )
    {
        mp3_bufer[i] = mp3_bufer[i-4];
    }
    mp3_bufer[2] = ( write >> 8 ) & 255;
    mp3_bufer[3] = write & 255;
    mp3_bufer[0] = ( streamPacketCounter >> 8 ) & 255;
    mp3_bufer[1] = streamPacketCounter & 255;

    streamPacketCounter++;
    return true;
}

void udpServer::pauseAudioStream()
{
    this->streamActive = false;
}

void udpServer::resumeAudioStream()
{
    this->streamActive = true;
}



bool udpServer::initSocket()
{

    while (socket.bind(5002) != sf::Socket::Done)
    {
        std::cout << "Unable to bind UDP " << port << " port, waiting for repair " << std::endl;
        sleep(10);
    }
    std::cout << "UDP_SERVER :: Port binding OK " << std::endl;
    return true;
}

bool udpServer::getTestMode()
{
    return this->testMode;
}

void udpServer::setTestMode(int var)
{
    this->testMode = var;
    if(var)
        std::cout << "UDP_SERVER :: TEST MODE ENABLE!" << std::endl;
    else
        std::cout << "UDP_SERVER :: TEST MODE DISABLE!" << std::endl;
}

void udpServer::setTestModePacketDelay(uint32_t var)
{
    this->testModePacketDelay = var;
    std::cout << "UDP_SERVER :: Test mode packet size udpated to " << (int)var << std::endl;

}

void udpServer::setTestModeDataSize(uint32_t var)
{
    this->testModePacketSize = var;
}

bool udpServer::stopUdpServer()
{
    this->done = true;
    return true;
}

void udpServer::runUdpServerTest(uint32_t port)
{
    static int ready = 0;
    while(1)
    {
        if(!ready)
        {
            // Listen to messages on the specified port
            if (test_socket.bind(port) != sf::Socket::Done)
                return;
            std::cout << "Server is listening to port " << port << ", waiting for a message... " << std::endl;
        }
        // Wait for a message
        char in[128];
        std::size_t received;
        sf::IpAddress sender;
        unsigned short senderPort;
        if (test_socket.receive(in, sizeof(in), received, sender, senderPort) != sf::Socket::Done)
            return;
        std::cout << "Message received from client " << sender << ": \"" << in << "\"" << std::endl;

        // Send an answer to the client
        const char out[] = "Hi, I'm the server";
        if (test_socket.send(out, sizeof(out), sender, senderPort) != sf::Socket::Done)
            return;
        std::cout << "Message sent to the client: \"" << out << "\"" << std::endl;

    }
}





