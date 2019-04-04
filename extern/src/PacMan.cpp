

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
	
	//DEBUG
	if (oldelement == 9999 || (PACMAN.Elements.at(oldelement).first + 1 == PACMAN.Elements.at(element).first) || (PACMAN.Elements.at(oldelement).first - 1 == PACMAN.Elements.at(element).first)) cout << "Strip position = " << PACMAN.Elements.at(element).first << endl; // I always add the starting element and I only add the adjacent elements if their strip distance from the previous element is 1.
	

	if (element == 0 && PACMAN.Elements.size() != 1)  // if we are at element 0 of the list but the size is larger than 1
	{
		if (oldelement - 1 != element) // if we did not come to this point by moving down
		{ 
			cout << "Up" << endl;
			PacMan::Eater(PACMAN, element+1, element); //start moving up
		}
	}
	if (element != 0 && element != PACMAN.Elements.size() - 1)  // if we are neither at the first nor at the last element of the list (list starts at 0, size does not)
	{
		if (oldelement - 1 == element) // if we came here by moving down
		{
			if (PACMAN.Elements.at(oldelement).first - 1 == PACMAN.Elements.at(element).first) // demanding that the next strip has to be within a distance of -2 from the previous strip
			{
				cout << "Down" << endl;
				PacMan::Eater(PACMAN, element-1, element); // keep moving down
			}
		}
		if (oldelement + 1 == element) // if we came here by moving down
		{
			if (PACMAN.Elements.at(oldelement).first + 1 == PACMAN.Elements.at(element).first) // demanding that the next strip has to be within a distance of +2 from the previous strip
			{
				//cout << "F" << endl;
				cout << "Up" << endl;
				PacMan::Eater(PACMAN, element+1, element); // keep moving up
			}
		}
		else if (oldelement == 9999)  // starting point for program
		{
			
			
			//cout << "X" << endl;
			cout << "Start" << endl;
			PacMan::Eater(PACMAN, element-1, element); // move down
			PacMan::Eater(PACMAN, element+1, element); // move up
			//return PACMAN;
		}
	}
	if (element == PACMAN.Elements.size() -1 && PACMAN.Elements.size() != 1 ) // if we are not at the last element of the list (list starts at 0, size does not) with the size being larger than 1.
	{
		if (oldelement + 1 != element) // if we did not come to this point by moving up
		{ 
			cout << "Down" << endl;
			PacMan::Eater(PACMAN, element-1, element); //start moving down
		}
	}
	if (oldelement == 9999 || (PACMAN.Elements.at(oldelement).first + 1 == PACMAN.Elements.at(element).first) || (PACMAN.Elements.at(oldelement).first - 1 == PACMAN.Elements.at(element).first)) // I always add the starting element and I only add the adjacent elements if their strip distance from the previous element is 1.
	{
		double Chargesum = PACMAN.Charge + PACMAN.Elements.at(element).second;
		Cluster.CoG = (Cluster.CoG*PACMAN.Charge + PACMAN.Elements.at(element).first*PACMAN.Elements.at(element).second)/Chargesum;
		Cluster.Charge = Chargesum;
		Cluster.Elements.push_back(PACMAN.Elements.at(element));
		
		
		
		//cout << "Position is currently " << PACMAN.Elements.at(element).first << endl;
	}
	if (oldelement == 9999) 	cout << "End" << endl;
	

};
