//-----------------------------------------------------------------------------
// File          : useYmlVariables.cxx
// Author        : Mengqing Wu <mengqing.wu@desy.de>
// Created       : 25/10/2018
// Project       : Example to use YmlVariables class.
//-----------------------------------------------------------------------------

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <string.h>
#include <map>
#include <vector>
#include <sstream> //isstringstream
#include <string>
#include "YmlVariables.h"


using namespace std;


/// --- application main func --- ///

int main(int argc, const char* argv[])
{
 if (argc<2){
    cout << "Usage: ymlHandler config.yml"<<endl;
    return 1;
  }
 
 YmlVariables yhand;
 auto data   = yhand.fakeDataReader( argv[1] );

 // cout << "main :: debug - data" << endl;
 // cout << data << endl;
 
 yhand.buffParser( yml_level[RunControl], data);
 yhand.print(); 
 return (0);
  
}
