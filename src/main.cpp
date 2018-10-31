
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

int main(){
    string HDT_FNAME = "dbpedia_all.hdt";
    string CLASS_PROP_FNAME = "class_property_all.tsv"; 
    string NUM_PROP_FNAME = "class_property_num.tsv";
    store_num_cols(HDT_FNAME, CLASS_PROP_FNAME);
    write_features(HDT_FNAME, NUM_PROP_FNAME);
    cout << "\nDone\n";
    return 0;
}



