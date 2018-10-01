# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
include(CMakeDependentOption)

# Project's name

project(Lycoris)

# should we use our own math functions
option(USE_KPIX 
  "Use extern KPiX libraries to decode data!" ON)
  
##-- cmake setup
set(CMAKE_BUILD_TYPE Debug )
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
message("\tDebug: ${PROJECT_BINARY_DIR}, while cmake_source_dir = ${CMAKE_SOURCE_DIR}")

set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH ${KPIX_INSTALL_RPATH})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(LYCORIS_EXT_LIBRARY extern) 

##-- C11 setup
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ADD_DEFINITIONS("-Wunused-value -Wall -pedantic -Wshadow -fdiagnostics-color=auto") 


##-- Flags
find_package(LibXml2 REQUIRED)
find_package(ROOT REQUIRED)
#find_package(ROOT COMPONENTS RooStats)

##-- The following folder will be included
include_directories( ${PROJECT_SOURCE_DIR}/include )

include_directories( ${ROOT_INCLUDE_DIR} )
include_directories( ${LIBXML2_INCLUDE_DIR} )
message("\t")

##-- local lib test:
# use KPiX library?
if (USE_KPIX)
   include_directories( ${PROJECT_SOURCE_DIR}/extern/include)
   
   aux_source_directory( ${PROJECT_SOURCE_DIR}/extern/src LIB_SRC)
   add_library(${LYCORIS_EXT_LIBRARY} SHARED ${LIB_SRC})

   install(TARGETS ${LYCORIS_EXT_LIBRARY} LIBRARY DESTINATION ${LIBRARY_OUTPUT_PATH})
   
endif (USE_KPIX)

#install(FILES Data.h DESTINATION include)


##-- LinkDef.h
# include(${ROOT_USE_FILE})
# ROOT_GENERATE_DICTIONARY(G__Dict ${PROJECT_SOURCE_DIR}/include/ntupleMaker.h LINKDEF  ${PROJECT_SOURCE_DIR}/include/LinkDef.h)

# add_library(Dict SHARED G__Dict.cxx)
# target_link_libraries(Dict ${ROOT_LIBRARIES})

# install( TARGETS Dict LIBRARY DESTINATION lib )
# install( FILES ${CMAKE_CURRENT_BINARY_DIR}/libDict_rdict.pcm ${CMAKE_CURRENT_BINARY_DIR}/libDict.rootmap DESTINATION lib )

# add exe:

#add_executable(printDat ${PROJECT_SOURCE_DIR}/src/printDat.cxx)
#target_link_libraries(printDat ${ROOT_LIBRARIES} ${KPIX_LIB} ${LIBXML2_LIBRARIES} Dict)
#target_link_libraries(printDat ${ROOT_LIBRARIES} ${LIBXML2_LIBRARIES} Dict)
#list(APPEND INSTALL_TARGETS printDat)

# install 
#install(
#  TARGETS ${INSTALL_TARGETS}
#  RUNTIME DESTINATION bin 
#  )
