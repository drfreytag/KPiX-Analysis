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

clustr::~clustr() { }
