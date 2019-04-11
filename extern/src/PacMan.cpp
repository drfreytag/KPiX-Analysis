

  // ,##.                   ,==.
 //,#    #.                 \ o ',
//#        #     _     _     \    \
//#        #    (_)   (_)    /    ; 
 //`#    #'                 /   .'  
   //`##'                   "=="


#include "PacMan.h"

using namespace std;

PacMan::PacMan()
{
	Cluster.CoG = 0;
	Cluster.Charge = 0;
}

PacMan::~PacMan() { }




double PacMan::getClusterCoG()
{
	return Cluster.CoG;
}

double PacMan::getClusterCharge()
{
	return Cluster.Charge;
}

int PacMan::getElementssize()
{
	return Cluster.Elements.size();
}

clustr PacMan::getCluster()
{
	return Cluster;
}

void PacMan::Eater(clustr PACMAN, int element, int oldelement) //it works under the assumption that the clustr paired vector is sorted beforehand!
{
	
	if (PACMAN.Elements.count(element) != 0)
	{
	
		std::map<int,double>::iterator begin = PACMAN.Elements.begin();
		std::map<int,double>::iterator end = PACMAN.Elements.end();
		
		if (element == begin->first && PACMAN.Elements.size() != 1) // if we are at element 0 of the list but the size is larger than 1
		{
			if (oldelement - 1 != element) // if we did not come to this point by moving down
			{
				cout << "Up" << endl;
				PacMan::Eater(PACMAN, element+1, element); //start moving up
			}
		}
		
		if ( element != begin->first && element != end->first) // if we are neither at the first nor at the last element/strip of the list 
		{
			if (oldelement - 1 == element) // if we came here by moving down
			{
				cout << "Down" << endl;
				PacMan::Eater(PACMAN, element-1, element); // keep moving down
			}
			if (oldelement + 1 == element) // if we came here by moving down
			{
				cout << "Up" << endl;
				PacMan::Eater(PACMAN, element+1, element); // keep moving up
			}
			else if (oldelement == 9999)  // starting point for program
			{
				cout << "Start" << endl;
				PacMan::Eater(PACMAN, element-1, element); // move down
				PacMan::Eater(PACMAN, element+1, element); // move up
			}
		}
		if (element == end->first && PACMAN.Elements.size() != 1)
		{
			cout << "Down" << endl;
			PacMan::Eater(PACMAN, element-1, element); //start moving down
		}
		
		
		
	}
	if (oldelement == 9999 || PACMAN.Elements.count(element) != 0 ) // I always add the starting element and I only add the adjacent elements if their strip distance from the previous element is 1.
	{
		double Chargesum = PACMAN.Charge + PACMAN.Elements.at(element);
		Cluster.CoG = (Cluster.CoG*PACMAN.Charge + element*PACMAN.Elements.at(element))/Chargesum;
		Cluster.Charge = Chargesum;
		Cluster.Elements.insert(std::pair<int,double>(element,PACMAN.Elements.at(element)));
		////PACMAN.Elements.erase(PACMAN.Elements.begin()+element);
		////cout << "Leftoverelement size" << PACMAN.Elements.size() << endl;

		////cout << "Position is currently " << PACMAN.Elements.at(element).first << endl;
	}
	if (oldelement == 9999) 	cout << "End" << endl;
	
};
