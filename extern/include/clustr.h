#ifndef __CLUSTR_H__
#define __CLUSTR_H__
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <iterator>
#include <ctime>

using namespace std;

class clustr
{
	public:
	// Constructor
		clustr();	
	// Deconstructor
		~clustr();
	
	
		double CoG ;
		double Charge;
		double SigmaCoG;
		vector<pair<int, double> > Elements;

		void SetParameters();
		//void setEventList(vector<pair<double, double>>);
} ;



#endif
