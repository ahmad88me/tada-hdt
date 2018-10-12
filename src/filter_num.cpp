
// For Multi threading
#define NUMCLPROCS 1 // Number of processes handling class filteration for numerical properties
#define  NUMPRPROCS 1 // Number of processes per class filteration

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



void store_num_cols(string hdt_file_dir, string in_file_dir, string out_file_dir){
    HDT *hdt = HDTManager::mapHDT(hdt_file_dir.c_str());
    //HDT *hdt = HDTManager::mapHDT("/Users/aalobaid/workspaces/Cworkspace/TADA-HDT/dbpedia_all.hdt");
    std::list<string> *properties;
    std::list<string> *instances;
    string line, class_uri;
    ifstream in_file(in_file_dir);
    if(in_file.is_open()){
        while(getline(in_file, line)){
            store_single_class(hdt, line);
            break;
        }
    }
    else{
        cout << "Can't open the file: " << in_file_dir << endl;
    }
    cout << "my test";
    string property_uri = "http://dbpedia.org/ontology/Person/height";
    class_uri = "http://dbpedia.org/ontology/SoccerPlayer";
    //isNumeric(hdt, class_uri, property_uri);
    property_uri="http://dbpedia.org/ontology/team";
    delete hdt; // Remember to delete instance when no longer needed!
}

void store_single_class(HDT* hdt, string line){
    string property_uri, class_uri;
    std::list<string> *properties;
    std::list<string> *instances;
    class_uri = get_class_from_line(line);
    properties = get_properties_from_line(line);
    instances = get_instances(hdt, class_uri);
    for(auto it=properties->cbegin();it != properties->cend();it++){
        property_uri = *it;
        if(isNumeric(hdt, instances, property_uri)){
            cout<< class_uri << " - " << property_uri << " is Numeric\n";
        }
        else{
            cout << class_uri << " - " << property_uri << " not Not\n";
        }
    }
}




bool isNumeric(HDT *hdt, std::list<string> *instances, string property_uri){
    long num_of_num=0, num_of_lit=0;
    long long v=0;
    int i;
    string holder;
    IteratorTripleString *it;
    TripleString * triple;
    //for(i=0;i<instances->size();i++){
    for(auto ins_it=instances->cbegin();ins_it!=instances->cend();ins_it++){
        
        //holder = instances->pop_front();
        it = hdt->search((*ins_it).c_str(),property_uri.c_str(),"");
        //instances->push_back(holder);
        if(it->hasNext()){
            triple = it->next();
            triple->getObject();
            v = strtod(triple->getObject().c_str(),NULL);
            if(v==0 || v==HUGE_VAL || v==-HUGE_VAL){
                num_of_lit++;
            }
            else{
                num_of_num++;
            }
        }
    }
    printf("nums: %ld lit: %ld\n", num_of_num, num_of_lit);
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

std::list<string>* get_instances(HDT* hdt, string class_uri){
    //std::list<string> instances;
    std::list<string> *instances;
    instances = new std::list<string>;
    IteratorTripleString *it = hdt->search("", "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",class_uri.c_str());
    TripleString *triple;
    while(it->hasNext()){
        triple = it->next();
        instances->push_back(triple->getSubject());
        return instances;
    }
    delete it;
    return instances;
}

std::list<string> *get_properties_from_line(string line){
    std::list<string> *properties;
    int i,len;
    int start=0;
    cout << "initalizing the list\n";
    properties = new std::list<string>;
    cout << "init\n";
    for(i=0;i<line.length();i++){
        if(line[i]=='\t'){
            len = i-start;
            properties->push_back(line.substr(start,len));
            cout << "\nspliting: ("<<start << "," << i << ") " << line.substr(start,len)<< endl;
            start = i+1;
        }
        else if(i==line.length()-1){
            len = i-start;
            properties->push_back(line.substr(start,len));            
            cout<< "reached the last: " << line.substr(start,len) << endl ; 
        }
        else{
            cout << "Char <" <<line[i] << ">" ;//<<endl;
        }
    }
    cout << "properties\n=============\n";
    for(auto it=properties->cbegin(); it!=properties->cend() ;it++){
        cout << "property: " << *it << endl;
    }
    return properties;
}

