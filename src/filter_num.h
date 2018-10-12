
#ifndef FILTER_NUM_H
#define FILTER_NUM_H
#include <HDTManager.hpp>

//#include <HDT.hpp>

using namespace hdt;

bool isNumeric(HDT*, std::list<string>*, string);
void store_single_class(HDT* hdt, string line);
void store_num_cols(string, string, string);
string get_class_from_line(string);
std::list<string>* get_properties_from_line(string line);
std::list<string> *get_instances(HDT* hdt, string class_uri);

#endif
