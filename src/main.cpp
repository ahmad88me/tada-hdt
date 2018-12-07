
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


//void try_filter_class_property(string hdt_fname, string class_uri, string property_uri){
//    HDT *hdt = HDTManager::mapHDT(hdt_fname.c_str());
//    std::list<string>* instances=nullptr;
//    instances = get_instances(hdt, class_uri);
//    string line = compute_features_line(hdt, class_uri, property_uri, instances);
//    cout << "line: "<<line<<endl;
//}

// Commit this to focus on the optimization
//int main(){ahm
//    string HDT_FNAME = "dbpedia_all.hdt";
//    string CLASS_PROP_FNAME = "class_property_all.tsv";
//    string NUM_PROP_FNAME = "class_property_num.tsv";
//    //comment this line to speedup, but should be uncommented if numerical properties are not extracted yet.
//    //store_num_cols(HDT_FNAME, CLASS_PROP_FNAME);
//    write_features(HDT_FNAME, NUM_PROP_FNAME);
//    //try_filter_class_property(HDT_FNAME, "http://dbpedia.org/ontology/BasketballPlayer", "http://dbpedia.org/ontology/Person/height");
//    cout << "\nDone\n";
//    return 0;
//}

/*
 * This one is testing with the optimized approach, which is implemented in the features.cpp using the estimated number
 * of results

*/
int main(){
    string HDT_FNAME = "dbpedia_all.hdt";
    //string NUM_PROP_FNAME = "class_property_num-opti.tsv";
    string NUM_PROP_FNAME = "class_property_num-opti-swap.tsv";
    //string NUM_PROP_FNAME = "class_property_num-opti-static.tsv";
    //comment this line to speedup, but should be uncommented if numerical properties are not extracted yet.
    //store_num_cols(HDT_FNAME, CLASS_PROP_FNAME);
    write_features(HDT_FNAME, NUM_PROP_FNAME);
    //try_filter_class_property(HDT_FNAME, "http://dbpedia.org/ontology/BasketballPlayer", "http://dbpedia.org/ontology/Person/height");
    cout << "\nDone\n";
    return 0;
}


//int main(){
//    string property_uri = "x:x";
//    string class_uri = "http://www.w3.org/2003/01/geo/wgs84_pos#SpatialThing";
//    //string HDT_FNAME = "dbpedia_all.hdt";
//    string HDT_FNAME = "sample.hdt";
//    string rdf_type = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type";
//    HDT *hdt = HDTManager::mapHDT(HDT_FNAME.c_str());
//    cout << "\nnumber of entities for property <"<< property_uri << ">: "<< hdt->search("", property_uri.c_str(), "")->estimatedNumResults();
//    auto itt = hdt->search("", property_uri.c_str(), "");
//    long i;
//    while(itt->hasNext()){
//        i++;
//        itt->next();
//    }
//    cout << "\niterationts: "<<i <<endl;
//    cout << "\nnumber of entities for class: <"<< class_uri << ">: "<< hdt->search("",rdf_type.c_str() , class_uri.c_str())->estimatedNumResults();
//    cout << "\nDone\n";
//    return 0;
//}

// This is used to test the sample with estimatedNumResults
//int main(){
//    string property_uri = "x:x";
//    string HDT_FNAME = "sample.hdt";
//    //HDT *hdt = HDTManager::mapHDT(HDT_FNAME.c_str());
//    HDT *hdt = hdt::HDTManager::mapIndexedHDT(HDT_FNAME.c_str());
//    cout << "\nnumber of entities for property <"<< property_uri << ">: "<< hdt->search("", property_uri.c_str(), "")->estimatedNumResults();
//    auto itt = hdt->search("", property_uri.c_str(), "");
//    long i;
//    while(itt->hasNext()){
//        i++;
//        itt->next();
//    }
//    cout << "\niterationts: "<<i <<endl;
//    cout << "\nDone\n";
//    return 0;
//}
