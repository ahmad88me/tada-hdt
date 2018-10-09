
#include <HDT.hpp>
//#include <HDTManager.hpp>
#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include "filter_num.h"

using namespace std;
using namespace hdt;

int main(){
    store_num_cols("dbpedia_all.hdt", "class_property_all.tsv", "somefile.out");
    cout << "\nDone\n";
    return 0;
}



