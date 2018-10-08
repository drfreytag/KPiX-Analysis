//-----------------------------------------------------------------------------
// File          : YmlVariables.cpp
// Author        : Mengqing Wu <mengqing.wu@desy.de>
// Created       : 2018/10/05
// Project       : Lycoris based on Rogue/SLAC
//-----------------------------------------------------------------------------
// Description :
// Yml string parser and reader, based on Ryan's XmlVariables class.
//-----------------------------------------------------------------------------
// Modification history :
// 2018/10/05: created
//-----------------------------------------------------------------------------

#include <YmlVariables.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>


using namespace std;

// Constructor
YmlVariables::YmlVariables ( ) {
  // xmlInitParser();
  // vars_.clear();  
}

// Deconstructor
YmlVariables::~YmlVariables ( ) { 
  // xmlCleanupParser();
  // xmlMemoryDump();
}
