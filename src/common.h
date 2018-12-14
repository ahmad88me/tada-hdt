#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <iostream>

using namespace std;

const string rdf_type = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type";
const string rdfs_subclassof = "http://www.w3.org/2000/01/rdf-schema#subClassOf";

bool str_to_double(string s, double & val);
string get_class_from_line(string);


#endif // COMMON_H
