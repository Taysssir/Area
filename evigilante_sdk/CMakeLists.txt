cmake_minimum_required(VERSION 2.8.3)
PROJECT(evigilante_sdk)

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(Protobuf REQUIRED)
find_package(catkin REQUIRED evigilante_configuration)

SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS} -D_USE_MATH_DEFINES")

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES evigilante_sdk
  CATKIN_DEPENDS evigilante_configuration
)

INCLUDE_DIRECTORIES(
  ${THREADS_INCLUDE_DIR}
  ${PROTOBUF_INCLUDE_DIRS}
  ${GFLAGS_INCLUDE_DIRS}
  ${GLOG_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/engine
  ${CMAKE_CURRENT_SOURCE_DIR}/src/
  ${CMAKE_CURRENT_SOURCE_DIR}/include/eos/sdk/
  ${CMAKE_CURRENT_SOURCE_DIR}/include/eos/
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
  ${catkin_INCLUDE_DIRS}
  )

include_directories(/usr/local/lib/libzip/include)
#include_directories(${ZIP_INCLUDE_DIR})
FOREACH(MODULE src/ include/eos/sdk/ src/engine src/tcp include/eos)
  FILE(GLOB MODULES_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/*.h
    )
  SET(SOURCES ${MODULES_SOURCES} ${SOURCES})
ENDFOREACH(MODULE)

ADD_LIBRARY(evigilante_sdk SHARED ${SOURCES})
TARGET_LINK_LIBRARIES(evigilante_sdk ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARY} ${catkin_LIBRARIES})
SET(SOURCES_SERVER tests/test_server.cpp)
ADD_EXECUTABLE(server ${SOURCES_SERVER})
TARGET_LINK_LIBRARIES(server ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARY} evigilante_sdk ${catkin_LIBRARIES})

SET(SOURCES_CLIENT tests/test_client.cpp)
ADD_EXECUTABLE(client ${SOURCES_CLIENT})
TARGET_LINK_LIBRARIES(client ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARY} evigilante_sdk ${catkin_LIBRARIES})

SET(SOURCES_TEST tests/test.cpp tests/catch.hpp)
ADD_EXECUTABLE(evigilante_sdk_test ${SOURCES_TEST})
TARGET_LINK_LIBRARIES(evigilante_sdk_test ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARY} evigilante_sdk ${ZIP_LIBRARY} ${catkin_LIBRARIES})

add_dependencies(evigilante_sdk ${catkin_EXPORTED_TARGETS})

#install(TARGETS evigilante_sdk DESTINATION /usr/local/lib/)
#
#install(FILES include/eos/evigilante/client.hpp DESTINATION /usr/local/include/eos/evigilante/)
#install(FILES include/eos/evigilante/msgs.pb.h DESTINATION /usr/local/include/eos/evigilante/)
#install(FILES include/eos/evigilante/server.hpp DESTINATION /usr/local/include/eos/evigilante/)

install(DIRECTORY include/eos
      DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})

install(TARGETS evigilante_sdk
      ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
      LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
      RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})

