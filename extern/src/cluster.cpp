#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <vector>

#include "cluster.h"
using namespace std;

void cluster::construct(vector<int> hit_position, vector<double> hit_charge)
{
	double weighted_position = 0;
	double charge_sum = 0;
	for (int i = 0; i < hit_position.size(); i++)
	{
		weighted_position += hit_charge.at(i)*hit_position.at(i);
		charge_sum += hit_charge.at(i);
	}
	position = weighted_position/charge_sum;
	charge = charge_sum;
}
