#ifndef __CLUSTER_H__
#define __CLUSTER_H__
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class cluster
{
	private:
		//void cluster_finder(vector<int> hit_position, vector<double> hit_charge);
	public:
		double position;
		double charge;
		void construct(vector<int> hit_position, vector<double> hit_charge);
};




#endif
