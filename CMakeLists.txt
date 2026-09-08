cmake_minimum_required(VERSION 3.10)
project(g1_joint_state_reader CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(UNITREE_SDK "$ENV{HOME}/unitree_sdk2" CACHE PATH "Path to unitree_sdk2")

if(NOT EXISTS "${UNITREE_SDK}/include/unitree/idl/hg")
  message(FATAL_ERROR "No hg headers in ${UNITREE_SDK}. Pass -DUNITREE_SDK=...")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
  set(SDK_ARCH "aarch64")
else()
  set(SDK_ARCH "x86_64")
endif()

add_executable(g1_state_reader g1_state_reader.cpp)

target_include_directories(g1_state_reader PRIVATE
  ${UNITREE_SDK}/include
  ${UNITREE_SDK}/thirdparty/include
  ${UNITREE_SDK}/thirdparty/include/ddscxx
  ${UNITREE_SDK}/thirdparty/include/iceoryx/v2.0.2
)

target_link_directories(g1_state_reader PRIVATE
  ${UNITREE_SDK}/lib/${SDK_ARCH}
)

target_link_libraries(g1_state_reader
  unitree_sdk2
  ddsc
  ddscxx
  pthread
)
