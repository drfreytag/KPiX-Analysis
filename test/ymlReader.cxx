//-----------------------------------------------------------------------------
// File          : ymlReader.cxx
// Author        : Mengqing Wu <mengqing.wu@desy.de>
// Created       : 05/10/2018
// Project       : Similar to the old XmlVariables, a new Yaml parser/handler.
//-----------------------------------------------------------------------------

#include <iostream>
//#include <iomanip>
#include <stdio.h>
//#include <stdarg.h>
#include <math.h>
#include <fstream>
#include <string.h>

#include "TFile.h"

#include <yaml.h>

using namespace std;

void myparser(string type, const char* str){};
int exParser();

int main(){

  // // File descriptor
  // int32_t fd_;
  
  // string file = "SimConfig.yml";
  
  // // Open a file
  // if ( (fd_ = ::open (file.c_str(),O_RDONLY | O_LARGEFILE)) < 0 ) {
  //   cout << "DataRead::open -> Failed to open file: " << file << endl;
  //   return 0;
  // }
  
  // // Read a file to a const char* with size of 
  // ::read(fd_, buff, 25);

  exParser();
  
  return 1;
}

// Process yml
/*void myparser(string type, const char* str){
}*/

int exParser()
{
  FILE *fh = fopen("cal.yml", "r");
  yaml_parser_t parser;
  yaml_token_t  token;   /* new variable */
  //  yaml_event_t  event;   /* New variable */
    
  /* Initialize parser */
  if(!yaml_parser_initialize(&parser))
    fputs("Failed to initialize parser!\n", stderr);
  if(fh == NULL)
    fputs("Failed to open file!\n", stderr);

  /* Set input file */
  yaml_parser_set_input_file(&parser, fh);

  /* CODE HERE */

  /* BEGIN new code */
  do {
    yaml_parser_scan(&parser, &token);
    switch(token.type)
    {
    /* Stream start/end */
    case YAML_STREAM_START_TOKEN: puts("STREAM START"); break;
    case YAML_STREAM_END_TOKEN:   puts("STREAM END");   break;
    /* Token types (read before actual token) */
    case YAML_KEY_TOKEN:   printf("(Key token)   "); break;
    case YAML_VALUE_TOKEN: printf("(Value token) "); break;
    /* Block delimeters */
    case YAML_BLOCK_SEQUENCE_START_TOKEN: puts("<b>Start Block (Sequence)</b>"); break;
    case YAML_BLOCK_ENTRY_TOKEN:          puts("<b>Start Block (Entry)</b>");    break;
    case YAML_BLOCK_END_TOKEN:            puts("<b>End block</b>");              break;
    /* Data */
    case YAML_BLOCK_MAPPING_START_TOKEN:  puts("[Block mapping]");            break;
    case YAML_SCALAR_TOKEN:  printf("scalar %s \n", token.data.scalar.value); break;
    /* Others */
    default:
      printf("Got token of type %d\n", token.type);
    }
    if(token.type != YAML_STREAM_END_TOKEN)
      yaml_token_delete(&token);
  } while(token.type != YAML_STREAM_END_TOKEN);
  yaml_token_delete(&token);
  /* END new code */
  

  
  /* Cleanup */
  yaml_parser_delete(&parser);
  fclose(fh);
  return 0;
}
