#ifndef FEATURES_H
#define FEATURES_H
#include <easy_logger/easy_logger.h>
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


class Features{
public:
    Features(string, string);
    static double stdev(std::list<double>*, double); // standard deviation
    static double median(std::list<double>*); // the value in the middle
    static double mean(std::list<double>*); // avg

    static std::list<clspropair*>* get_processed_feat_clspairs(string features_file_name);
    static clspropair* get_clspropair_from_line(string line);

    void compute_store_features_for_pair(clspropair* pair, std::list<string>**);

    static std::list<clspropair*>* get_pairs_from_numfilter(string line);

    static std::list<string>* get_subset(std::list<string>*, int);
    string compute_features_line(string class_uri, string property_uri, std::list<string>**);

    void write_features(string num_property_dir);


    string features_fname;

private:
    HDT* hdt;
    EasyLogger* m_logger;
};

#endif
