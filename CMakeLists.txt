
#------------------
#- Author: Mengqing Wu <mengqing.wu@desy.de>
#- Generated: September, 2018
#- Modified: October 1st, 2018
#- for LYCORIS project at DESY.
#------------------


##-- Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
include(CMakeDependentOption)

##-- Project's name

project(Lycoris)

##-- Options for extern packages
option(USE_KPIX 
  "Use extern KPiX libraries to decode data!" ON)

option(MODE_DEBUG
  "Use DEBUG mode, ONLY single module to compile!" OFF)

option(USE_YAML
  "Turn libyaml-dependency ON/OFF." OFF)

option(ADD_EXE_SIMPLE
  "Add simiple executables: cout/printDat etc." OFF)

##-- cmake setup
set(CMAKE_BUILD_TYPE Debug )

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "Install prefix" FORCE )

set(LIBRARY_INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH ${LIBRARY_INSTALL_RPATH})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

##-- C11 setup
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ADD_DEFINITIONS("-Wunused-value -Wall -pedantic -Wshadow -fdiagnostics-color=auto") 


##-- Flags
find_package(LibXml2 REQUIRED)
if (LibXml2_FOUND)
   message(" [debug] libxml2 found!")
endif (LibXml2_FOUND)
find_package(ROOT REQUIRED)
if (ROOT_FOUND)
   message(" [debug] ROOT found!")
endif(ROOT_FOUND)
#find_package(ROOT COMPONENTS RooStats)

##-- everything needed for YAML: 'libyaml-dev' used
if (USE_YAML)
  # w/o Anaconda, use this other than the 2 below lines!
  #FIND_PATH(LIBYAML_INCLUDE_DIR NAMES yaml.h) 
  #FIND_LIBRARY(LIBYAML_LIBRARIES NAMES yaml libyaml)
  set(LIBYAML_INCLUDE_DIR /usr/include)
  set(LIBYAML_LIBRARIES /usr/lib/x86_64-linux-gnu/libyaml.so)
  
  INCLUDE(FindPackageHandleStandardArgs)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(Yaml DEFAULT_MSG LIBYAML_LIBRARIES LIBYAML_INCLUDE_DIR)
  MARK_AS_ADVANCED(LIBYAML_INCLUDE_DIR LIBYAML_LIBRARIES)
  #message("[debug] LIBYAML_INCLUDE_DIR = ${LIBYAML_INCLUDE_DIR}")
  #message("[debug] LIBYAML_LIBRARIES = ${LIBYAML_LIBRARIES}")

endif(USE_YAML)

##-- Library folder to be included
include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${ROOT_INCLUDE_DIR} )
include_directories( ${LIBXML2_INCLUDE_DIR} )
message(" [debug] LIBXML2_INC_DIR = ${LIBXML2_INCLUDE_DIR}")
message(" [debug] LIBXML2_LIBs = ${LIBXML2_LIBRARIES}")

##-- Extern Packages to install prior
if (USE_KPIX)
  message(" [INFO] local KPiX lite package used.")
  set(KPIX_LOCAL_LIB extern) 
  include_directories( ${PROJECT_SOURCE_DIR}/extern/include)
  
  aux_source_directory( ${PROJECT_SOURCE_DIR}/extern/src LIB_SRC)
  add_library(${KPIX_LOCAL_LIB} SHARED ${LIB_SRC})
  target_link_libraries(${KPIX_LOCAL_LIB} ${LIBXML2_LIBRARIES}) # this is necessary for MAC!

  install(TARGETS ${KPIX_LOCAL_LIB} LIBRARY DESTINATION lib)
  # file(GLOB INC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/eudaq/*.hh")
  
else () # not tested, definately not working...
  message(" [INFO] external KPiX packages used.")

  set(CMAKE_PREFIX_PATH "/usr/local/lib/kpix")
  set(KPIX_INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib")
  set(CMAKE_INSTALL_RPATH ${KPIX_INSTALL_RPATH})
  include_directories( /usr/local/include/kpix )
  find_library(KPIX_EXT_LIB kpix)
  
endif (USE_KPIX)

##-- libraries to link against:
if (DEFINED KPIX_LOCAL_LIB)
  list(APPEND LYCORIS_LIBRARIES ${KPIX_LOCAL_LIB})
endif()

if (DEFINED KPIX_EXT_LIB)
  list(APPEND LYCORIS_LIBRARIES ${KPIX_EXT_LIB})
endif()

##-- LinkDef.h
include(${ROOT_USE_FILE})
message("\tDebug: ${ROOT_USE_FILE}")
#ROOT_GENERATE_DICTIONARY(G__Dict ${PROJECT_SOURCE_DIR}/include/ntupleMaker.h LINKDEF  ${PROJECT_SOURCE_DIR}/include/LinkDef.h)

#add_library(Dict SHARED G__Dict.cxx)
#target_link_libraries(Dict ${ROOT_LIBRARIES})

#install( TARGETS Dict LIBRARY DESTINATION lib )
#install( FILES ${CMAKE_CURRENT_BINARY_DIR}/libDict_rdict.pcm ${CMAKE_CURRENT_BINARY_DIR}/libDict.rootmap DESTINATION lib )

##-- add exe: # maybe cmake can be scripted with a loop using aux_source_directory func?

add_executable(useYmlVariables ${PROJECT_SOURCE_DIR}/src/useYmlVariables.cxx)
target_link_libraries(useYmlVariables ${ROOT_LIBRARIES} ${LYCORIS_LIBRARIES} ${LIBXML2_LIBRARIES} )
list(APPEND INSTALL_EXE_TARGETS useYmlVariables)

# simple evt loop code:

if (ADD_EXE_SIMPLE)
  add_executable(printDat ${PROJECT_SOURCE_DIR}/src/printDat.cxx)
  target_link_libraries(printDat ${ROOT_LIBRARIES}  ${LYCORIS_LIBRARIES} ${LIBXML2_LIBRARIES})
  list(APPEND INSTALL_EXE_TARGETS printDat)
  
  add_executable(count ${PROJECT_SOURCE_DIR}/src/count.cxx)
  target_link_libraries(count ${ROOT_LIBRARIES} ${LYCORIS_LIBRARIES} ${LIBXML2_LIBRARIES})
  list(APPEND INSTALL_EXE_TARGETS count)
endif(ADD_EXE_SIMPLE)

# calibration ana:

add_executable(new_CalibrationFitter ${PROJECT_SOURCE_DIR}/src/new_CalibrationFitter.cxx)
target_link_libraries(new_CalibrationFitter ${ROOT_LIBRARIES} ${LYCORIS_LIBRARIES} ${LIBXML2_LIBRARIES} )
list(APPEND INSTALL_EXE_TARGETS new_CalibrationFitter)

add_executable(DuoKpix_CalibFitter ${PROJECT_SOURCE_DIR}/src/DuoKpix_CalibFitter.cxx)
target_link_libraries(DuoKpix_CalibFitter ${ROOT_LIBRARIES} ${LYCORIS_LIBRARIES} ${LIBXML2_LIBRARIES} )
list(APPEND INSTALL_EXE_TARGETS DuoKpix_CalibFitter)

##-- new for analyzing yml data:
add_executable(CalibAnaYml ${PROJECT_SOURCE_DIR}/src/CalibAnaYml.cxx)
target_link_libraries(CalibAnaYml ${ROOT_LIBRARIES} ${LYCORIS_LIBRARIES} ${LIBXML2_LIBRARIES} )
list(APPEND INSTALL_EXE_TARGETS CalibAnaYml)

# full ana:

add_executable(analysisLite ${PROJECT_SOURCE_DIR}/src/analysisLite.cxx)
target_link_libraries(analysisLite ${ROOT_LIBRARIES} ${LYCORIS_LIBRARIES} ${LIBXML2_LIBRARIES} )
list(APPEND INSTALL_EXE_TARGETS analysisLite)

add_executable(analysis ${PROJECT_SOURCE_DIR}/src/analysis.cxx)
target_link_libraries(analysis ${ROOT_LIBRARIES} ${LYCORIS_LIBRARIES} ${LIBXML2_LIBRARIES} )
list(APPEND INSTALL_EXE_TARGETS analysis)

add_executable(cluster_analysis ${PROJECT_SOURCE_DIR}/src/cluster_analysis.cxx)
target_link_libraries(cluster_analysis ${ROOT_LIBRARIES} ${LYCORIS_LIBRARIES} ${LIBXML2_LIBRARIES} )
list(APPEND INSTALL_EXE_TARGETS cluster_analysis)

# tree builders:

#add_executable(TreeMaker ${PROJECT_SOURCE_DIR}/src/TreeMaker.cxx)
#target_link_libraries(TreeMaker ${ROOT_LIBRARIES} ${LYCORIS_LIBRARIES} ${LIBXML2_LIBRARIES} Dict)
#list(APPEND INSTALL_EXE_TARGETS TreeMaker)




# install 
install(
 TARGETS ${INSTALL_EXE_TARGETS}
 RUNTIME DESTINATION bin 
 )
