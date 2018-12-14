
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
#include <unistd.h>

#include "filter_num.h"
#include "logger.h"
#include "common.h"


static int min_num_of_res = 20;


string numfile = "test-class_property_num.tsv";
string logfname = "test-filter_num.log";
//string numfile = "class_property_num.tsv";
//string logfname = "filter_num.log";
//static string rdf_type = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type";

Filternum::Filternum(string hdt_file){

}


std::list<string>* Filternum::get_processed_classes(){
    string line;
    std::list<string> *classes = new std::list<string>;
    ifstream in_file(numfile);
    if(in_file.is_open()){
        while(getline(in_file, line)){
            classes->push_back(get_class_from_line(line));
        }
    }
    else{
        cout << "unable to open the file: " << numfile << endl;
    }
    return classes;
}


//std::list<string> *get_leaf_classes(string hdt_file_dir){
//    HDT *hdt = HDTManager::mapIndexedHDT(hdt_file_dir.c_str());
//    IteratorTripleString *itt;
//    TripleString * triple;
//    set<string> classes;
//    std::list<string> *unique_classes = new std::list<string>;
//    itt = hdt->search("", rdf_type.c_str(), "");
//    long l=0;
//    while(itt->hasNext()){
//        //log(logfname, "counter: "+to_string(l)+"\tstored: "+to_string(classes.size()));
//        triple = itt->next();
//        classes.insert(triple->getObject());
//        //log(logfname, "type: "+triple->getObject());
//    }
//    delete itt;
//    // delete classes that has subclasses, we only want the leaves
//    for(auto it=classes.cbegin();it!=classes.end();it++){
//        itt = hdt->search("", rdfs_subclassof.c_str(), (*it).c_str());
//        if(itt->hasNext()){
//            //log(logfname, "discard: "+*it);
//        }
//        else{
//            unique_classes->push_back(*it);
//            //log(logfname, "include: "+*it);
//        }
//        delete itt;
//    }
//    log(logfname, "num of unique classes: "+to_string(unique_classes->size()));
//    log(logfname, "num of leaves: "+to_string(unique_classes->size()));
//    for(auto it=unique_classes->cbegin();it!=unique_classes->end();it++){
//        log(logfname, "class: "+*it);
//    }
//    return unique_classes;
//}


std::list<string> * Filternum::get_leaf_classes(string hdt_file_dir){
    HDT *hdt = HDTManager::mapIndexedHDT(hdt_file_dir.c_str());
    IteratorTripleString *itt;
    TripleString * triple;
    set<string> classes;
    std::list<string> *unique_classes = new std::list<string>;
    itt = hdt->search("", rdf_type.c_str(), "");
    long l=0;
    while(itt->hasNext()){
        //log(logfname, "counter: "+to_string(l)+"\tstored: "+to_string(classes.size()));
        triple = itt->next();
        classes.insert(triple->getObject());
        //log(logfname, "type: "+triple->getObject());
    }
    delete itt;
    // delete classes that has subclasses, we only want the leaves
    for(auto it=classes.cbegin();it!=classes.end();it++){
        itt = hdt->search("", rdfs_subclassof.c_str(), (*it).c_str());
        if(itt->hasNext()){
            //log(logfname, "discard: "+*it);
        }
        else{
            unique_classes->push_back(*it);
            //log(logfname, "include: "+*it);
        }
        delete itt;
    }
    log(logfname, "num of unique classes: "+to_string(unique_classes->size()));
    log(logfname, "num of leaves: "+to_string(unique_classes->size()));
    for(auto it=unique_classes->cbegin();it!=unique_classes->end();it++){
        log(logfname, "class: "+*it);
    }
    return unique_classes;
}


void Filternum::automic_write_classes(string hdt_file_dir, string out_file_dir){
    // if the file exists, then no need to proceed, because the data is already extracted
    if(access( out_file_dir.c_str(), F_OK ) != -1){
        log(logfname, "The classes already extracted to: "+out_file_dir);
        return;
    }
   std::list<string> *leaves = get_leaf_classes("dbpedia_all.hdt");
   log(logfname, "Number of leaves: "+to_string(leaves->size()));

//    HDT *hdt = HDTManager::mapIndexedHDT(hdt_file_dir.c_str());
//    std::list<string> * classes = get_leaf_classes(hdt_file_dir);
//    ofstream f;
//    f.open(out_file_dir, ios::app);

//    f<< class_uri;
//    for(auto it=properties->cbegin();it != properties->cend();it++){
//        property_uri = *it;
//        f << "\t" << property_uri;
//    }
//    f << endl;
//    f.close();

//    for(auto it=classes.cbegin();it!=classes.end();it++){
//        found = false;
//        for(auto it2=processed->cbegin();it2!=processed->cend();it2++){

//        }
//    }
//    delete processed;

}


void Filternum::store_num_cols(string hdt_file_dir, string in_file_dir){
    HDT *hdt = HDTManager::mapHDT(hdt_file_dir.c_str());
    //HDT *hdt = HDTManager::mapHDT("/Users/aalobaid/workspaces/Cworkspace/TADA-HDT/dbpedia_all.hdt");
    //std::list<string> *properties;
    //std::list<string> *instances;
    std::list<string> *processed;
    string line, class_uri;
    ifstream in_file(in_file_dir);
    processed = get_processed_classes();
    bool found;
    int num_of_processed=0; // number of processed classes
    // testing block
    //cout<< "testing one specific" << endl;
    //store_single_class(hdt, "http://dbpedia.org/ontology/Village\thttp://dbpedia.org/property/areaImperial");
    //cout<< "done"<<endl;
    //return;
    // testing block
    log(logfname, "the number of processed classes: "+to_string(processed->size()));
    if(in_file.is_open()){
        while(getline(in_file, line)){
            class_uri = get_class_from_line(line);
            found = 0;
            for(auto it=processed->cbegin();it!=processed->cend();it++){
                if((*it)==class_uri){
                    log(logfname, "The class "+class_uri+" is already processed");
                    found = 1; 
                    num_of_processed++;
                    break;
                }    
            }
            if(!found){
                store_single_class(hdt, line);
                num_of_processed++;
            }
            log(logfname, "Processed classes: "+to_string(num_of_processed));
            //break;
        }
    }
    else{
        cout << "Can't open the file: " << in_file_dir << endl;
    }
    delete hdt; // Remember to delete instance when no longer needed!
}

void Filternum::store_single_class(HDT* hdt, string line){
    int i=0;
    string property_uri, class_uri;
    std::list<string> *properties;
    std::list<string> *instances;
    std::list<string> * num_pros = new std::list<string>;
    class_uri = get_class_from_line(line);
    log(logfname, "getting properties for: "+class_uri);
    properties = get_properties_from_line(line);
    log(logfname, "getting instances for: "+class_uri);
    instances = get_instances(hdt, class_uri);
    for(auto it=properties->cbegin();it != properties->cend();it++, i++){
        property_uri = *it;
        log(logfname, "processing: "+class_uri+"\t"+property_uri);
        log(logfname, "property progress: ("+to_string(i*100/properties->size())+" %)");
        if(isNumeric(hdt, instances, property_uri)){
            num_pros->push_back(property_uri);
        }
        else{
        }
    }
    write_single_class(class_uri, num_pros);
    delete properties;
    delete instances;
    delete num_pros;
}

void Filternum::write_single_class(string class_uri, std::list<string>* properties){
    ofstream f;
    string property_uri;
    f.open(numfile, ios::app);
    f<< class_uri;
    for(auto it=properties->cbegin();it != properties->cend();it++){
        property_uri = *it;
        f << "\t" << property_uri;
    }
    f << endl;
    f.close();
}

bool Filternum::isNumeric(HDT *hdt, std::list<string> *instances, string property_uri){
    long num_of_num=0, num_of_lit=0;
    double v=0;
    int i;
    IteratorTripleString *it;
    TripleString * triple;
    for(auto ins_it=instances->cbegin();ins_it!=instances->cend();ins_it++){
        it = hdt->search((*ins_it).c_str(),property_uri.c_str(),"");
        if(it->hasNext()){
            triple = it->next();
            if(str_to_double(triple->getObject(),v)){
                num_of_num++;
            }
            else{
                num_of_lit++;
            }
        }
        delete it;
    }
    log(logfname, "nums: "+to_string(num_of_num)+"  literals: "+to_string(num_of_lit));
    return num_of_num > num_of_lit;
}

//bool Filternum::str_to_double(string s, double & val){
//    int i;
//    string num_str="";
//    bool got_dec;
//    got_dec=0;
//    for(i=0;i<s.length();i++){
//        if(s[i]>='0' && s[i]<='9'){
//            num_str += s[i];
//        }
//        else if(s[i]=='.' && !got_dec){
//            num_str += s[i];
//        }
//    }
//    if(num_str=="" || num_str=="."){
//        return 0;
//    }
//    else{

//        val = strtod(num_str.c_str(),NULL);
//        return 1;
//    }
//}

//string Filternum::get_class_from_line(string line){
//    int i;
//    for(i=0;i<line.length();i++){
//        if(line[i]=='\t' || line[i]=='\n'){
//            return line.substr(0,i);
//        }
//    }
//    return line;
//    //return "";
//}

std::list<string>* Filternum::get_instances(HDT* hdt, string class_uri){
    std::list<string> *instances;
    instances = new std::list<string>;
    //log(logfname, "getting instances for: "+class_uri);
    IteratorTripleString *it = hdt->search("", "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",class_uri.c_str());
    TripleString *triple;
    while(it->hasNext()){
        triple = it->next();
        instances->push_back(triple->getSubject());
        //return instances;
    }
    delete it;
    //delete triple;
    log(logfname, "num of instances: "+to_string(instances->size()));
    return instances;
}

std::list<string> * Filternum::get_properties_from_line(string line){
    std::list<string> *properties;
    int i,len;
    int start=0;
    properties = new std::list<string>;
    for(i=0;i<line.length();i++){
        if(line[i]=='\t'){
            len = i-start;
            properties->push_back(line.substr(start,len));
            start = i+1;
        }
        else if(i==line.length()-1){
            len = i-start;
            properties->push_back(line.substr(start,len+1));            
        }
        else{
        }
    }
    if(properties->size()> 0){
        properties->pop_front();// Remove the class, which is typically the first element in each line
    }
    log(logfname, "num of properties: "+to_string(properties->size()));
    return properties;
}

