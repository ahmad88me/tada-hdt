
#include <HDT.hpp>
//#include <HDTManager.hpp>
#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include "filter_num.h"
#include "features.h"

using namespace std;
using namespace hdt;


void try_filter_class_property(string hdt_fname, string class_uri, string property_uri){
    HDT *hdt = HDTManager::mapHDT(hdt_fname.c_str());
    std::list<string>* instances=nullptr;
    instances = get_instances(hdt, class_uri);
    string line = compute_features_line(hdt, class_uri, property_uri, instances);
    cout << "line: "<<line<<endl;
}

int main(){
    string HDT_FNAME = "dbpedia_all.hdt";
    string CLASS_PROP_FNAME = "class_property_all.tsv";
    string NUM_PROP_FNAME = "class_property_num.tsv";
    //comment this line to speedup, but should be uncommented if numerical properties are not extracted yet.
    //store_num_cols(HDT_FNAME, CLASS_PROP_FNAME);
    write_features(HDT_FNAME, NUM_PROP_FNAME);
    //try_filter_class_property(HDT_FNAME, "http://dbpedia.org/ontology/BasketballPlayer", "http://dbpedia.org/ontology/Person/height");
    cout << "\nDone\n";
    return 0;
}



