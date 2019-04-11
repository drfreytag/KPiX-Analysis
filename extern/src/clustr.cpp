//        _
//       | | _
//     _ | || |
//    | || || |
//    | || || |


#include "clustr.h"

using namespace std;

clustr::clustr()
{
	SigmaCoG = 0;
	CoG = 0;
	Charge = 0;
}

void clustr::SetParameters()
{
	double chargesum = 0;
	double position = 0;
	double sigmapos = 0;
	
	//cout << Elements.size() << endl;
	for (auto const& i : Elements)
	{
		chargesum += i.second;
		position += i.second*i.first;
	}
	Charge = chargesum;
	CoG = position/chargesum;
	
	for (auto const& i : Elements)
	{
		sigmapos +=  pow((CoG - i.first), 2)*i.second;
		//cout << "Position is " << i.first << endl;
		//cout << "Difference is " << CoG - i.first << endl;
	}
	SigmaCoG = sqrt(sigmapos/chargesum);;
}


int clustr::MaxCharge()
{
	double charge_max = 0;
	int strip_max;
	for (auto const& i : Elements)
	{
		//cout << "Current charge = " << i.second << " : " << i.first << endl;
		if (i.second > charge_max)
		{
			charge_max = i.second;
			strip_max = i.first;
			//cout << "+++++++++++" << endl;
			//cout << "Max Charge = " << charge_max << " : " << strip_max << endl;
			//cout << "+++++++++++" << endl;
		}
		
	
	}
	return strip_max;
}


clustr::~clustr() { }
