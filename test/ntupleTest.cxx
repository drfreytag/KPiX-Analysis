#include "ntupleMaker.h"

using namespace std;

int main( int argc, char **argv){

  lycoris::ntupleMaker kk(argv[1]);
  //kk.loopKpix();

  kk.CreateTree();

  kk.makeTreeEx();
  return(1);

}
