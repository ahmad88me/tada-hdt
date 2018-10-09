
#ifndef FILTER_NUM_H
#define FILTER_NUM_H

#include <HDT.hpp>

using namespace hdt;

bool isNumeric(HDT*, string, string);
void store_num_cols(string, string, string);
string get_class_from_line(string);
std::list<string> get_properties_from_line(string line);

#endif
