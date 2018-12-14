
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
#include <easy_logger/easy_logger.h>
#include <unistd.h>

#include "filter_num.h"
#include "logger.h"
#include "common.h"




string numfile = "test-class_property_num.tsv";
string logfname = "test-filter_num.log";
//string numfile = "class_property_num.tsv";
//string logfname = "filter_num.log";
//static string rdf_type = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type";

Filternum::Filternum(string hdt_file_dir, string log_file_dir){
   hdt = HDTManager::mapIndexedHDT(hdt_file_dir.c_str());
   m_logger = new EasyLogger(log_file_dir);
}

std::list<string>* Filternum::get_processed_classes(string file_dir){
    string line;
    std::list<string> *classes = new std::list<string>;
    ifstream in_file(file_dir);
    if(in_file.is_open()){
        while(getline(in_file, line)){
            classes->push_back(get_class_from_line(line));
        }
    }
    else{
        m_logger->log("unable to open the file: "+file_dir);
        cout << "unable to open the file: " << file_dir << endl;
    }
    return classes;
}


std::list<string> * Filternum::get_leaf_classes(){
    IteratorTripleString *itt;
    TripleString * triple;
    set<string> classes;
    std::map<string, unsigned>* classes_counts = new std::map<string, unsigned>;
    std::list<string> *unique_classes = new std::list<string>;
    itt = hdt->search("", rdf_type.c_str(), "");
    m_logger->log("getting the leaves");
    while(itt->hasNext()){
        triple = itt->next();
        if(classes_counts->find(triple->getObject())==classes_counts->end()){
            (*classes_counts)[triple->getObject()] = 1;
        }
        else{
            (*classes_counts)[triple->getObject()]++;
        }
    }
    delete itt;
    m_logger->log("this->m_min_num_of_res: "+to_string(this->m_min_num_of_res));
    // delete classes that has subclasses, we only want the leaves
    for(auto it=classes_counts->cbegin();it!=classes_counts->cend();it++){
        m_logger->log(it->first+" - "+to_string(it->second));
        itt = hdt->search("", rdfs_subclassof.c_str(), it->first.c_str());
        if(!itt->hasNext() && it->second >= this->m_min_num_of_res){
            m_logger->log("picked class: "+it->first+" "+to_string(it->second));
            unique_classes->push_back(it->first);
        }
        if(!(itt->hasNext())){
            m_logger->log("no next: "+it->first);
        }
        if(it->second >= this->m_min_num_of_res){
            m_logger->log("more that min: "+it->first);
        }
        delete itt;
    }
    m_logger->log("num of leaves: "+to_string(unique_classes->size()));
    return unique_classes;
}


void Filternum::automic_write_classes(string out_file_dir){
    // if the file exists, then no need to proceed, because the data is already extracted
    if(access( out_file_dir.c_str(), F_OK ) != -1){
        log(logfname, "The classes already extracted to: "+out_file_dir);
        return;
    }
    std::list<string> *leaves = get_leaf_classes();
    ofstream f;
    f.open(out_file_dir, ios::out);
    for(auto it=leaves->cbegin();it != leaves->cend();it++){
        f << *it <<endl;
    }
    f.close();
    delete leaves;
}

std::list<string>* Filternum::get_properties_of_class(string class_uri){
    IteratorTripleString *itt;
    TripleString * triple;
    std::list<string>* properties = new std::list<string>;
    std::list<string>* instances = get_instances(class_uri);
    std::map<string,unsigned> *prop_counts = new std::map<string,unsigned>;
    for(auto it_inst=instances->cbegin();it_inst!=instances->cend();it_inst++){
        itt = hdt->search(it_inst->c_str(),"", "");
        while(itt->hasNext()){
            triple = itt->next();
            if(prop_counts->find(triple->getPredicate()) != prop_counts->cend()){ // found
                (*prop_counts)[triple->getPredicate()]++;
            }
            else{
                (*prop_counts)[triple->getPredicate()] = 1;
            }
        }
        delete itt;
    }
    m_logger->log("properties counts: ");
    for(auto it=prop_counts->cbegin();it!=prop_counts->cend();it++){
        m_logger->log(it->first+" - "+to_string(it->second));
        if(it->second >= m_min_num_of_prop){
            properties->push_back(it->first);
        }
    }
    m_logger->log("included "+to_string(properties->size())+" out of "+to_string(prop_counts->size())+" properties");
    return properties;
}

void Filternum::write_properties(string classes_file_dir, string out_file){
    std::list<string>* classes = get_processed_classes(classes_file_dir);
    std::list<string>* properties;
    // just to clear the file
    ofstream f;
    f.open(classes_file_dir, ios::out);
    f.close();
    for(auto it=classes->cbegin();it!=classes->cend();it++){
        properties = get_properties_of_class(*it);
        write_single_class(out_file, *it, properties);
        delete properties;
    }
}

void Filternum::store_num_cols(string hdt_file_dir, string in_file_dir){
    HDT *hdt = HDTManager::mapHDT(hdt_file_dir.c_str());
    //HDT *hdt = HDTManager::mapHDT("/Users/aalobaid/workspaces/Cworkspace/TADA-HDT/dbpedia_all.hdt");
    //std::list<string> *properties;
    //std::list<string> *instances;
    std::list<string> *processed;
    string line, class_uri;
    ifstream in_file(in_file_dir);
    //processed = get_processed_classes();
    // get processed classes is changed, I need to fix this
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
    //instances = get_instances(hdt, class_uri);
    instances = get_instances(class_uri);
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
    // I have to fix it
    //write_single_class(class_uri, num_pros);
    delete properties;
    delete instances;
    delete num_pros;
}

void Filternum::write_single_class(string file_dir, string class_uri, std::list<string>* properties){
    ofstream f;
    string property_uri;
    f.open(file_dir, ios::app);
    f<< class_uri;
    for(auto it=properties->cbegin();it != properties->cend();it++){
        property_uri = *it;
        f << "\t" << property_uri;
    }
    f << endl;
    f.close();
}

//void Filternum::write_single_class(string class_uri, std::list<string>* properties){
//    ofstream f;
//    string property_uri;
//    f.open(numfile, ios::app);
//    f<< class_uri;
//    for(auto it=properties->cbegin();it != properties->cend();it++){
//        property_uri = *it;
//        f << "\t" << property_uri;
//    }
//    f << endl;
//    f.close();
//}

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

std::list<string>* Filternum::get_instances(string class_uri){
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

