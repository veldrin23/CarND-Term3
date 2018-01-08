#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include <vector>
#include "classifier.h"


using namespace std;
using Eigen::ArrayXd;

/**
 * Initializes GNB
 */
GNB::GNB() {
	left_means = ArrayXd(4);
	left_means << 0, 0, 0, 0;

	keep_means = ArrayXd(4);
	keep_means << 0, 0, 0, 0;

	right_means = ArrayXd(4);
	right_means << 0, 0, 0, 0;

	left_sds = ArrayXd(4);
	left_sds << 0, 0, 0, 0;

	keep_sds = ArrayXd(4);
	keep_sds << 0, 0, 0, 0;

	right_sds = ArrayXd(4);
	right_sds << 0, 0, 0, 0;

	left_prior = ArrayXd(4);
	left_prior << 0, 0, 0, 0;

	keep_prior = ArrayXd(4);
	keep_prior << 0, 0, 0, 0;
	
	right_prior = ArrayXd(4);
	right_prior << 0, 0, 0, 0;

	
}

GNB::~GNB() {}

void GNB::train(vector<vector<double>> data, vector<string> labels)
{

	/*
		Trains the classifier with N data points and labels.

		INPUTS
		data - array of N observations
		  - Each observation is a tuple with 4 values: s, d, 
		    s_dot and d_dot.
		  - Example : [
			  	[3.5, 0.1, 5.9, -0.02],
			  	[8.0, -0.3, 3.0, 2.2],
			  	...
		  	]

		labels - array of N labels
		  - Each label is one of "left", "keep", or "right".
	*/
	
	double left_size = 0.0;
	double keep_size = 0.0;
	double right_size = 0.0;

	// calculate means
	for(i = 0, i < labels.size(), i++)
	{
		if(labels[i] == "left") 
		{
			left_means += ArrayXd::Map(data[i], data[i].size());
			left_size += 1;
		}
		if(labels[i] == "keep") 
		{
			keep_means += ArrayXd::Map(data[i], data[i].size());
			keep_size += 1;
		}
		if(labels[i] == "right") 
		{
			right_means += ArrayXd::Map(data[i], data[i].size());
			right_size += 1;
		}

		left_means = left_means/left_size;
		keep_means = keep_means/keep_size;
		right_means = right_means/right_size;
	}


	// calculate sds
	ArrayXd data_point;
	for(i = 0, i < labels.size(), i++)
	{
		data_point = ArrayXd::Map(data[i].data(), data[i].size());
		if(labels[i] == "left") 
		{
			left_sds += pow((data_point - left_means), 2)
		}
		if(labels[i] == "keep") 
		{
			keep_sds += pow((data_point - keep_means), 2)
		}
		if(labels[i] == "right") 
		{
			right_sds += pow((data_point - right_means), 2)
		}

		left_sds = (left_sds/left_size).sqrt();
		keep_sds = (left_sds/left_size).sqrt();
		right_sds = (left_sds/left_size).sqrt();

		left_prior = left_size/labels.size();
		keep_prior = keep_size/labels.size();
		right_prior = keep_prior/labels.size()

	}


}

string GNB::predict(vector<double> sample)
{
	/*
		Once trained, this method is called and expected to return 
		a predicted behavior for the given observation.

		INPUTS

		observation - a 4 tuple with s, d, s_dot, d_dot.
		  - Example: [3.5, 0.1, 8.5, -0.2]

		OUTPUT

		A label representing the best guess of the classifier. Can
		be one of "left", "keep" or "right".
		"""
		# TODO - complete this
	*/
	
	double left_p = 0;

	for (int i = 0, i < 4, i ++) 
	{
		left_p += (1.0/ sqrt(2.0 * M_PI * pow(left_sds[i], 2))) * exp(-0.5 * pow(sample[i] - left_means[i], 2)/pow(left_sds[i],2));
		keep_p += (1.0/ sqrt(2.0 * M_PI * pow(keep_sds[i], 2))) * exp(-0.5 * pow(sample[i] - keep_means[i], 2)/pow(keep_sds[i],2));
		right_p += (1.0/ sqrt(2.0 * M_PI * pow(right_sds[i], 2))) * exp(-0.5 * pow(sample[i] - right_means[i], 2)/pow(right_sds[i],2));
	}	


	left_p *= left_prior;


	double probs[3] = {left_p, keep_p, right_p};
	double max = left_p;
	double max_index = 0;
	for(int i = 0, i < 3, i ++)
	{
		if(probs[i] > max)
		{
			max = probs[i];
			max_index = i;
		}
	}

	return this -> possible_labels[i];
}