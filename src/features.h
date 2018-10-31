#ifndef FEATURES_H
#define FEATURES_H
#include <HDTManager.hpp>
#include <list>
using namespace hdt;


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

clspropair* get_clspropair_from_line(string);
double stdev(std::list<double>*, double); // standard deviation
double median(std::list<double>*); // the value in the middle
double mean(std::list<double>*); // avg
void write_features(HDT*);
void write_class_features(HDT* , string, string);
//void write_class_prop_features(string, string);
void test_enty_point();
std::list<clspropair*>* get_processed_feat_clspairs(string);
#endif
