#ifndef FEATURES_H
#define FEATURES_H
#include <HDTManager.hpp>
#include <list>
using namespace hdt;


typedef struct clspropair{
    string class_uri;
    string property_uri;
} clspropair;


double stdev(std::list<double>*, double); // standard deviation
double median(std::list<double>*); // the value in the middle
double mean(std::list<double>*); // avg
void write_features(HDT*);
void write_class_features(HDT* , string, string);
//void write_class_prop_features(string, string);
void test_enty_point();

#endif
