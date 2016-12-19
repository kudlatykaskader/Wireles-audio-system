cmake_minimum_required(VERSION 2.6)
project(udp_sender_test)
set (CMAKE_CXX_FLAGS "-std=c++11 -lpulse -lpulse-simple -fpermissive -lmp3lame -lsfml-network -lsfml-window -lsfml-system -pthread")
add_executable(udp_sender_test main.cpp tcpinfoserver.cpp udpStreamSource.cpp Timer.cpp )

install(TARGETS udp_sender_test RUNTIME DESTINATION bin)
