

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


void PacMan::setEventList(vector<pair<int, double>> events)
{
	Cluster.EventList = events;
}

int PacMan::getEventListSize()
{
	return Cluster.EventList.size();
}

clustr PacMan::Eater(clustr PACMAN, int element, int oldelement)
{
	//cout << element << endl;
	if (element == 0 && PACMAN.EventList.size() != 1)
	{
		if (oldelement - 1 == element)
		{ 
			//cout << "A" << endl;
			double Chargesum = PACMAN.Charge + PACMAN.EventList.at(element).second;
			PACMAN.CoG = (PACMAN.CoG*PACMAN.Charge + PACMAN.EventList.at(element).first*PACMAN.EventList.at(element).second)/Chargesum;
			PACMAN.Charge = Chargesum;
			return PACMAN;
		}
		else
		{
			//cout << "B" << endl;
			PACMAN = PacMan::Eater(PACMAN, element+1, element);
			double Chargesum = PACMAN.Charge + PACMAN.EventList.at(element).second;
			PACMAN.CoG = (PACMAN.CoG*PACMAN.Charge + PACMAN.EventList.at(element).first*PACMAN.EventList.at(element).second)/Chargesum;
			PACMAN.Charge = Chargesum;
			return PACMAN;
		}
	}
	if (element == PACMAN.EventList.size() -1 && PACMAN.EventList.size() != 1 )
	{
		if (oldelement + 1 == element)
		{ 
			//cout << "C" << endl;
			double Chargesum = PACMAN.Charge + PACMAN.EventList.at(element).second;
			PACMAN.CoG = (PACMAN.CoG*PACMAN.Charge + PACMAN.EventList.at(element).first*PACMAN.EventList.at(element).second)/Chargesum;
			PACMAN.Charge = Chargesum;
			return PACMAN;
		}
		else
		{
			//cout << "D" << endl;
			//cout << "No upper strip numbers with charge above threshold" << endl;
			PACMAN = PacMan::Eater(PACMAN, element-1, element);
			double Chargesum = PACMAN.Charge + PACMAN.EventList.at(element).second;
			PACMAN.CoG = (PACMAN.CoG*PACMAN.Charge + PACMAN.EventList.at(element).first*PACMAN.EventList.at(element).second)/Chargesum;
			PACMAN.Charge = Chargesum;
			return PACMAN;
		}
	}
	if (PACMAN.EventList.size() == 1)
	{
		//cout << "O" << endl;
		double Chargesum = PACMAN.Charge + PACMAN.EventList.at(element).second;
		PACMAN.CoG = (PACMAN.CoG*PACMAN.Charge + PACMAN.EventList.at(element).first*PACMAN.EventList.at(element).second)/Chargesum;
		PACMAN.Charge = Chargesum;
		return PACMAN;
	}
	else if (element != 0 && element != PACMAN.EventList.size() -1)
	{
		//PACMAN = PacMan::Eater(PACMAN, element+1, element);		
		if (oldelement - 1 == element)
		{
			if (PACMAN.EventList.at(oldelement).first -1 == PACMAN.EventList.at(element).first)
			{
				//cout << "E" << endl;
				PACMAN = PacMan::Eater(PACMAN, element-1, element);
				double Chargesum = PACMAN.Charge + PACMAN.EventList.at(element).second;
				PACMAN.CoG = (PACMAN.CoG*PACMAN.Charge + PACMAN.EventList.at(element).first*PACMAN.EventList.at(element).second)/Chargesum;
				PACMAN.Charge = Chargesum;
				
			}
			//cout << "No lower strip numbers with charge above threshold" << endl;
			return PACMAN;
		}
		if (oldelement + 1 == element)
		{
			if (PACMAN.EventList.at(oldelement).first +1 == PACMAN.EventList.at(element).first)
			{
				//cout << "F" << endl;
				PACMAN = PacMan::Eater(PACMAN, element+1, element);
				double Chargesum = PACMAN.Charge + PACMAN.EventList.at(element).second;
				PACMAN.CoG = (PACMAN.CoG*PACMAN.Charge + PACMAN.EventList.at(element).first*PACMAN.EventList.at(element).second)/Chargesum;
				PACMAN.Charge = Chargesum;
				
			}
			//cout << "No upper strip numbers with charge above threshold" << endl;
			return PACMAN;
		}
		else if (oldelement == 9999)
		{
			
			
			//cout << "X" << endl;
			PACMAN = PacMan::Eater(PACMAN, element-1, element);
			PACMAN = PacMan::Eater(PACMAN, element+1, element);
			double Chargesum = PACMAN.Charge + PACMAN.EventList.at(element).second;
			PACMAN.CoG = (PACMAN.CoG*PACMAN.Charge + PACMAN.EventList.at(element).first*PACMAN.EventList.at(element).second)/Chargesum;
			PACMAN.Charge = Chargesum;
			return PACMAN;
		}
	}
};


