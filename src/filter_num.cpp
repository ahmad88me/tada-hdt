#include <stdio.h>
#include <stdlib.h>

#include <HDTManager.hpp>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <list>

#include "filter_num.h"

using namespace std;
using namespace hdt;


void store_num_cols(string hdt_file_dir, string in_file_dir, string out_file_dir){
    HDT *hdt = HDTManager::mapHDT(hdt_file_dir.c_str());
    std::list<string> properties;
    string line, class_uri;
    ifstream in_file(in_file_dir);
    if(in_file.is_open()){
        while(getline(in_file, line)){
            class_uri = get_class_from_line(line);
            properties = get_properties_from_line(line);
            cout << "for the class: "<< class_uri << "we got "<< properties.size() << "properties" << endl;
            break;
        }
    }
    else{
        cout << "Can't open the file: " << in_file_dir << endl;
    }
    delete hdt; // Remember to delete instance when no longer needed!
}

bool isNumeric(HDT *hdt, string class_uri, string property_uri){
    long num_of_num=0, num_of_lit=0;
    long long v=0;
    IteratorTripleString *it = hdt->search(class_uri.c_str(), property_uri.c_str(),"");
    while(it->hasNext()){
        TripleString *triple = it->next();
        cout << triple->getSubject() <<
        ", " << triple->getPredicate() <<
        ", " << triple->getObject() << endl;
        v = strtod(triple->getObject().c_str(),NULL);
        if(v==0 || v==HUGE_VAL || v==-HUGE_VAL){
            num_of_lit++;
        }
        else{
            num_of_num++;
        }
    }
    printf("nums: %ld lit: %ld", num_of_num, num_of_lit);
    delete it; // Remember to delete iterator to avoid memory leaks!
    return num_of_num > num_of_lit;
}

string get_class_from_line(string line){
    int i;
    for(i=0;i<line.length();i++){
        if(line[i]=='\t')
            return line.substr(0,i);
    }
    return "";
}

std::list<string> get_properties_from_line(string line){
    std::list<string> properties;
    int i;
    int start=0;
    for(i=0;i<line.length();i++){
        if(line[i]=='\t'){
            properties.push_back(line.substr(start,i));
            start = i+1;
        }
        else if(i==line.length()-1){
            properties.push_back(line.substr(start,i+1));            
        }
    }
    return properties;
}

