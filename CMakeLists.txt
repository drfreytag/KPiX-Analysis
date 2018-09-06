# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
include(CMakeDependentOption)

# Project's name

project(Lycoris)

# cmake setup
set(CMAKE_BUILD_TYPE Debug )
set(CMAKE_PREFIX_PATH "/usr/local/lib/kpix")
SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "Install prefix" FORCE )

set(KPIX_INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib")

set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH ${KPIX_INSTALL_RPATH})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# C11 setup
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ADD_DEFINITIONS("-Wunused-value -Wall -pedantic -Wshadow -fdiagnostics-color=auto") 

# flags
find_package(LibXml2 REQUIRED)
find_package(ROOT REQUIRED)
#find_package(ROOT COMPONENTS RooStats)

# The following folder will be included
include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${ROOT_INCLUDE_DIR} )
include_directories( /usr/local/include/kpix )
include_directories( ${LIBXML2_INCLUDE_DIR} )

find_library(KPIX_LIB kpix)

# LinkDef.h
include(${ROOT_USE_FILE})
ROOT_GENERATE_DICTIONARY(G__Dict ${PROJECT_SOURCE_DIR}/include/ntupleMaker.h LINKDEF  ${PROJECT_SOURCE_DIR}/include/LinkDef.h)

add_library(Dict SHARED G__Dict.cxx)
target_link_libraries(Dict ${ROOT_LIBRARIES})

install( TARGETS Dict LIBRARY DESTINATION lib )
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/libDict_rdict.pcm ${CMAKE_CURRENT_BINARY_DIR}/libDict.rootmap DESTINATION lib )

# add exe:
add_executable(ana ${PROJECT_SOURCE_DIR}/src/analysis.cxx)
target_link_libraries(ana ${ROOT_LIBRARIES} ${KPIX_LIB} ${LIBXML2_LIBRARIES} Dict)
list(APPEND INSTALL_TARGETS ana)

add_executable(ntupleTest ${PROJECT_SOURCE_DIR}/src/ntupleTest.cxx)
target_link_libraries(ntupleTest ${ROOT_LIBRARIES} ${KPIX_LIB} ${LIBXML2_LIBRARIES} Dict)
list(APPEND INSTALL_TARGETS ntupleTest)

# install 
install(
  TARGETS ${INSTALL_TARGETS}
  RUNTIME DESTINATION bin 
  )
