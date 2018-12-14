#ifndef FEATURES_H
#define FEATURES_H
#include <HDTManager.hpp>
#include <list>
using namespace hdt;

//const string rdf_type = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type";


typedef struct clspropair{
    string class_uri;
    string property_uri;
    friend inline bool operator==(const struct clspropair& l, const struct clspropair& r){
      /* 
       if(l.class_uri == r.class_uri){
        cout<< "class uri are equal\n";
       }
       else{
        cout << "class uri are not\n";
       }
       if(l.property_uri == r.property_uri){
        cout<< "property uri are equal\n";
       }
       else{
        cout << "property uri are not\n";
       }
       */

        return l.class_uri == r.class_uri && r.property_uri == l.property_uri;
    }
    friend inline bool operator!=(const struct clspropair& l, const struct clspropair&r){
    return !(l==r);
    }
} clspropair;

std::list<clspropair*>* get_processed_feat_clspairs(string features_file_name);
clspropair* get_clspropair_from_line(string line);
void compute_store_features_for_pair(HDT* hdt, clspropair* pair, std::list<string>**);
void write_features(string hdt_file_dir, string num_property_dir);
clspropair* get_clspropair_from_line(string);
double stdev(std::list<double>*, double); // standard deviation
double median(std::list<double>*); // the value in the middle
double mean(std::list<double>*); // avg
//void compute_store_features_for_pair(clspropair*);
void test_enty_point();
std::list<clspropair*>* get_processed_feat_clspairs(string);
string compute_features_line(HDT* hdt, string class_uri, string property_uri, std::list<string>**);
std::list<clspropair*>* get_pairs_from_numfilter(string line);
std::list<string>* get_subset(std::list<string>*, int);
#endif
