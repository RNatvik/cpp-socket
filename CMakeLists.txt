cmake_minimum_required(VERSION 3.0.0)
project(Socket VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#include(CTest)
#enable_testing()

#----------------------------------
# External Libs
#----------------------------------
set(ExternalLib1_path lib/asio/asio)
add_library(asio 
    ${ExternalLib1_path}/include/asio.hpp
)
target_include_directories(asio PUBLIC ${ExternalLib1_path}/include)
set_target_properties(asio PROPERTIES LINKER_LANGUAGE CXX)


#----------------------------------
# Library
#----------------------------------
set(LibName1 Socket)
add_library(
    ${LibName1} 
    include/UdpSocket.hpp
    include/TSQueue.hpp
    src/UdpSocket.cpp
)
target_include_directories(
   Socket PUBLIC
   $<INSTALL_INTERFACE:include>
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
set_target_properties(${LibName1} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${LibName1} pthread asio) # ws2_32
IF (WIN32)
    target_link_libraries(${LibName1} ws2_32)
#ELSE()
# set stuff for other systems
ENDIF()


#----------------------------------
# Echo Server
#----------------------------------
set(ExeName1 EchoServer)
add_executable(${ExeName1}
    test/EchoServer.cpp
)
target_include_directories(${ExeName1} PUBLIC include)
target_link_libraries(${ExeName1} PUBLIC ${LibName1})


#----------------------------------
# Echo Client
#----------------------------------
set(ExeName2 EchoClient)
add_executable(${ExeName2}
    test/EchoClient.cpp
)
target_include_directories(${ExeName2} PUBLIC include)
target_link_libraries(${ExeName2} PUBLIC ${LibName1})


#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)
