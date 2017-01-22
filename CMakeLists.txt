cmake_minimum_required(VERSION 2.6)
project(udp_sender_test)
set (CMAKE_CXX_FLAGS "-std=c++11 -lpulse -lpulse-simple -fpermissive -lmp3lame -lsfml-network -lsfml-window -lsfml-system -pthread")
add_executable(udp_sender_test main.cpp tcpinfoserver.cpp udpStreamSource.cpp Timer.cpp )

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system window graphics network audio)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries( udp_sender_test ${SFML_LIBRARIES})
endif()
install(TARGETS udp_sender_test RUNTIME DESTINATION bin)
