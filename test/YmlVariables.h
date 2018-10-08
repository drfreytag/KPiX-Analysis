//-----------------------------------------------------------------------------
// File          : YmlVariables.h
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
#ifndef __YML_VARIABLES_H__
#define __YML_VARIABLES_H__

#include <string>
#include <map>
#include <yaml.h>

using namespace std;

//! Class to contain generic Yml data registered by Rogue-DAQ
class YmlVariables {

public:

//! Constructor
YmlVariables();

//! Deconstructor
~YmlVariables();

};

#endif
