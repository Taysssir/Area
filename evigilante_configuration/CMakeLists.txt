cmake_minimum_required(VERSION 2.6)

project(evigilante_configuration)
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -o3 -std=c++11")
  find_package(TinyXML REQUIRED)
set(ADDITIONAL_LIBRARIES rt)

FIND_PACKAGE(Protobuf REQUIRED)
find_package(catkin REQUIRED)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES evigilante_configuration
)

SET(SOURCES
${CMAKE_CURRENT_SOURCE_DIR}/include/eos/configuration/models.pb.h
${CMAKE_CURRENT_SOURCE_DIR}/include/eos/configuration/configuration.hpp
${CMAKE_CURRENT_SOURCE_DIR}/source/models/models.pb.cc
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/loader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/loader.hpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/resource.hpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/resource_manager.cpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/resource_manager.hpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/ziparchive.hpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/ziparchive.cpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/loaders/map_loader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/loaders/map_loader.hpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/loaders/patrols_loader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/loaders/patrols_loader.hpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/loaders/sensors_loader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/loaders/sensors_loader.hpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/loaders/zones_loader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/loaders/zones_loader.hpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/loaders/settings_loader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/loaders/settings_loader.hpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/loaders/tasksscheduled_loader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/loaders/tasksscheduled_loader.hpp
${CMAKE_CURRENT_SOURCE_DIR}/source/resource_manager/file.hpp
${CMAKE_CURRENT_SOURCE_DIR}/source/configuration.cpp
${CMAKE_CURRENT_SOURCE_DIR}/source/pimpl_configuration.cpp
${CMAKE_CURRENT_SOURCE_DIR}/source/pimpl_configuration.cpp
)

SET(TEST_SOURCES
${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp
${CMAKE_CURRENT_SOURCE_DIR}/tests/catch.hpp
${CMAKE_CURRENT_SOURCE_DIR}/tests/test_patrols.cpp
${CMAKE_CURRENT_SOURCE_DIR}/tests/test_zones.cpp
${CMAKE_CURRENT_SOURCE_DIR}/tests/test_configuration.cpp
${CMAKE_CURRENT_SOURCE_DIR}/tests/test_zip.cpp
${CMAKE_CURRENT_SOURCE_DIR}/tests/test_sensors.cpp
${CMAKE_CURRENT_SOURCE_DIR}/tests/test_settings.cpp
${CMAKE_CURRENT_SOURCE_DIR}/tests/test_map.cpp
${CMAKE_CURRENT_SOURCE_DIR}/tests/test_tasksscheduled.cpp
)
include_directories(include)
include_directories(source)
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/eos/configuration/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ext/libzip/include/)
include_directories(include ${TinyXML_INCLUDE_DIRS})

add_library(evigilante_configuration SHARED ${SOURCES})

SET(LIB_ZIP "${CMAKE_CURRENT_SOURCE_DIR}/ext/libzip/lib/libzip.so")

target_link_libraries(evigilante_configuration ${CMAKE_THREAD_LIBS_INIT}
    ${ADDITIONAL_LIBRARIES} ${LIB_ZIP} ${TinyXML_LIBRARIES} ${PROTOBUF_LIBRARY})

add_executable(evigilante_configuration_test  ${TEST_SOURCES} )
target_link_libraries(evigilante_configuration_test ${CMAKE_THREAD_LIBS_INIT}
 ${ADDITIONAL_LIBRARIES} evigilante_configuration ${TinyXML_LIBRARIES} ${PROTOBUF_LIBRARY})

add_dependencies(evigilante_configuration ${catkin_EXPORTED_TARGETS})

install(DIRECTORY include/eos
      DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})

install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/libzip/lib/libzip.so
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/libzip/lib/libzip.so.4
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/libzip/lib/libzip.so.4.0.0
      DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION})

install(TARGETS evigilante_configuration
      ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
      LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
      RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})

  #install(TARGETS eosconfiguration DESTINATION /usr/local/lib/)
  #install(FILES include/eos/configuration/patrols.hpp DESTINATION /usr/local/include/eos/configuration/)
  #install(FILES include/eos/configuration/configuration.hpp DESTINATION /usr/local/include/eos/configuration/)
  #install(FILES include/eos/configuration/sensors.hpp DESTINATION /usr/local/include/eos/configuration/)
  #install(FILES include/eos/configuration/zones.hpp DESTINATION /usr/local/include/eos/configuration/)
  #install(FILES include/eos/configuration/settings.hpp DESTINATION /usr/local/include/eos/configuration/)

