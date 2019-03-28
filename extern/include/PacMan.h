#ifndef __PACMAN_H__
#define __PACMAN_H__
#include "clustr.h"

using namespace std;


class PacMan
{
	public:
	// Constructor
		PacMan();	
	// Deconstructor
		~PacMan();
		
		double getClusterCoG();
		double getClusterCharge();
		double getClusterSize();
		
		void setEventList(vector<pair<int, double>>);
		
		int getEventListSize();
		
		clustr Eater(clustr, int, int);
		
		
		
	private:
		clustr Cluster;

};

#endif
