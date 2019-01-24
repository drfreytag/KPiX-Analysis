// WORKS IN PRACTICE NEEDS TO BE REWRITTEN AS EXTERNAL FUNCTIOn



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

struct cluster
{
	double CoG;
	double charge;
} ;

cluster PacMan(vector<pair<double, double> > charge_events, cluster PACMAN, double element, double oldelement)
{
	cout << element << endl;
	if (element == 0)
	{
		if (oldelement - 1 == element)
		{ 
			cout << "A" << endl;
			double chargesum = PACMAN.charge + charge_events.at(element).second;
			PACMAN.CoG = (PACMAN.CoG*PACMAN.charge + charge_events.at(element).first*charge_events.at(element).second)/chargesum;
			PACMAN.charge = chargesum;
			
			return PACMAN;
		}
		else
		{
			cout << "B" << endl;
			PACMAN = PacMan(charge_events, PACMAN, element+1, element);
			double chargesum = PACMAN.charge + charge_events.at(element).second;
			PACMAN.CoG = (PACMAN.CoG*PACMAN.charge + charge_events.at(element).first*charge_events.at(element).second)/chargesum;
			PACMAN.charge = chargesum;
			
			return PACMAN;
		}
	}
	if (element == charge_events.size() -1 )
	{
		if (oldelement + 1 == element)
		{ 
			cout << "C" << endl;
			double chargesum = PACMAN.charge + charge_events.at(element).second;
			PACMAN.CoG = (PACMAN.CoG*PACMAN.charge + charge_events.at(element).first*charge_events.at(element).second)/chargesum;
			PACMAN.charge = chargesum;
			
			return PACMAN;
		}
		else
		{
			cout << "D" << endl;
			PACMAN = PacMan(charge_events, PACMAN, element-1, element);
			double chargesum = PACMAN.charge + charge_events.at(element).second;
			PACMAN.CoG = (PACMAN.CoG*PACMAN.charge + charge_events.at(element).first*charge_events.at(element).second)/chargesum;
			PACMAN.charge = chargesum;
			
			return PACMAN;
		}
	}
	else if (element != 0 && element != charge_events.size() -1)
	{
		//PACMAN = PacMan(charge_events, PACMAN, element+1, element);		
		if (oldelement - 1 == element)
		{
			if (charge_events.at(oldelement).first -1 == charge_events.at(element).first)
			{
				cout << "E" << endl;
				PACMAN = PacMan(charge_events, PACMAN, element-1, element);
				double chargesum = PACMAN.charge + charge_events.at(element).second;
				PACMAN.CoG = (PACMAN.CoG*PACMAN.charge + charge_events.at(element).first*charge_events.at(element).second)/chargesum;
				PACMAN.charge = chargesum;
				
				
			}
			return PACMAN;
		}
		if (oldelement + 1 == element)
		{
			if (charge_events.at(oldelement).first +1 == charge_events.at(element).first)
			{
				cout << "F" << endl;
				PACMAN = PacMan(charge_events, PACMAN, element+1, element);
				double chargesum = PACMAN.charge + charge_events.at(element).second;
				PACMAN.CoG = (PACMAN.CoG*PACMAN.charge + charge_events.at(element).first*charge_events.at(element).second)/chargesum;
				PACMAN.charge = chargesum;
				
				
			}
			return PACMAN;
		}
		else if (oldelement == 9999)
		{
			
			
			cout << "X" << endl;
			PACMAN = PacMan(charge_events, PACMAN, element-1, element);
			PACMAN = PacMan(charge_events, PACMAN, element+1, element);
			double chargesum = PACMAN.charge + charge_events.at(element).second;
			PACMAN.CoG = (PACMAN.CoG*PACMAN.charge + charge_events.at(element).first*charge_events.at(element).second)/chargesum;
			PACMAN.charge = chargesum;
			
			return PACMAN;
		}
	}
	
	
	//else
	//{
		////if (oldelement = element + 1) PACMAN = PacMan(charge_events, PACMAN, element-1, element);
		////else if (oldelement = element -1) PACMAN = PacMan(charge_events, PACMAN, element+1, element);
		
		//if (oldelement = element - 1) PACMAN = PacMan(charge_events, PACMAN, element+1, element);
		//else if (oldelement = element + 1) PACMAN = PacMan(charge_events, PACMAN, element-1, element);
		
		//if (charge_events.at(oldelement).first + 1 != charge_events.at(element).first) return PACMAN;
		//else
		//{
			//cout << "=========1=========" << endl;
			//cout << "Element " << element << endl;
			//cout << "Strip " << charge_events.at(element).first << endl;
			//cout << "Old CG " << PACMAN.CoG << endl;
			//cout << "Old Charge Sum" <<PACMAN.charge << endl;
			
			//double chargesum = PACMAN.charge + charge_events.at(element).second;
			//PACMAN.CoG = (PACMAN.CoG*PACMAN.charge + charge_events.at(element).first*charge_events.at(element).second)/chargesum;
			//PACMAN.charge = chargesum;
			
			
			//cout << "New CoG " << PACMAN.CoG << endl;
			//cout << "New Charge Sum " << PACMAN.charge << endl;
			//cout << "=========2=========" << endl;
			
			
			
			//return PACMAN;
		//}
	//}
};

//cluster PacMan(vector<pair<double, double> > charge_events, cluster PACMAN, double element, double oldelement)
//{
	//if (element+1 == charge_events.size()) return PACMAN;
	//else
	//{
		//cout << "AAA" << endl;
		//bool test = (charge_events.at(element+1).first != charge_events.at(element).first+1);
		//cout << test << endl;
	//}
	//return PACMAN;
//}

int main()
{
	
	vector<pair<double, double> > ab;
	double max = 0;
	double startelement;
	
	
	ab.push_back(make_pair(1, 4));
	ab.push_back(make_pair(2, 5));
	ab.push_back(make_pair(3, 1));
	ab.push_back(make_pair(4, 5));
	ab.push_back(make_pair(5, 8));
	
	ab.push_back(make_pair(7, 3));
	ab.push_back(make_pair(8, 4));
	ab.push_back(make_pair(10, 3));
	ab.push_back(make_pair(11, 4));
	ab.push_back(make_pair(15, 12));
	
	
	sort(ab.begin(), ab.end());

	for (int i = 0; i < ab.size(); ++i)
	{
		cout << i << endl;
		if (ab.at(i).second > max)
		{
			max = ab.at(i).second;
			startelement = i;
		}
	}
	
	cout << "starting element = " << startelement << endl;
	
	cluster pman;
	pman.CoG = 0;
	pman.charge = 0;
	
	pman = PacMan(ab, pman, startelement, 9999);
	
	cout << pman.CoG << endl;
	cout << pman.charge << endl;

	return 0;
}
