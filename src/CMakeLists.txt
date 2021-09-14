cmake_minimum_required(VERSION 2.8)
project(socket.io-cpp-client-sample)

# socket.io
if (NOT ENABLE_SSL)
  list(APPEND extra_libs "sioclient")
else()
  list(APPEND extra_libs "sioclient_tls")
  find_package(OpenSSL)
  list(APPEND extra_libs ${OPENSSL_LIBRARIES})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

set (CMAKE_CXX_STANDARD 11)

add_executable(client client.cpp)
target_link_libraries(client ${extra_libs} pthread)
