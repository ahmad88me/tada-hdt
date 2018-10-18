#ifndef FEATURES_H
#define FEATURES_H
#include <HDTManager.hpp>

using namespace hdt;

double stdev(std::list<double>*); // standard deviation
double median(std::list<double>*); // the value in the middle
double mean(std::list<double>*); // avg
void write_features(HDT*);
void write_class_prop_features(string, string);

#endif
