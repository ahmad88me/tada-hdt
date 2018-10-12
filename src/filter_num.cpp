
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
#include "logger.h"

string numfile = "class_property_num.tsv";
string logfname = "filter_num.log";

std::list<string>* get_processed_classes(){
    string line;
    std::list<string> *classes = new std::list<string>;
    ifstream in_file(numfile);
    if(in_file.is_open()){
        while(getline(in_file, line)){
            classes->push_back(get_class_from_line(line));
        }
    }
    return classes;
}

void store_num_cols(string hdt_file_dir, string in_file_dir, string out_file_dir){
    HDT *hdt = HDTManager::mapHDT(hdt_file_dir.c_str());
    //HDT *hdt = HDTManager::mapHDT("/Users/aalobaid/workspaces/Cworkspace/TADA-HDT/dbpedia_all.hdt");
    std::list<string> *properties;
    std::list<string> *instances;
    std::list<string> *processed;
    string line, class_uri;
    ifstream in_file(in_file_dir);
    processed = get_processed_classes();
    bool found;
    if(in_file.is_open()){
        while(getline(in_file, line)){
            class_uri = get_class_from_line(line);
            //class_uri = "http://dbpedia.org/ontology/SoccerPlayer";
            found = 0;
            for(auto it=processed->cbegin();it!=processed->cend();it++){
                if((*it)==class_uri){
                    log(logfname, "The class "+class_uri+" is already processed");
                    found = 1; 
                    break;
                }    
            }
            if(!found){
                store_single_class(hdt, line);
            }
            break;
        }
    }
    else{
        cout << "Can't open the file: " << in_file_dir << endl;
    }
    delete hdt; // Remember to delete instance when no longer needed!
}

void store_single_class(HDT* hdt, string line){
    string property_uri, class_uri;
    std::list<string> *properties;
    std::list<string> *instances;
    class_uri = get_class_from_line(line);
    properties = get_properties_from_line(line);
    instances = get_instances(hdt, class_uri);
    std::list<string> * num_pros = new std::list<string>;
    for(auto it=properties->cbegin();it != properties->cend();it++){
        property_uri = *it;
        //cout << "processing: " << class_uri << "\t" << property_uri << endl;
        log(logfname, "processing: "+class_uri+"\t"+property_uri);
        if(isNumeric(hdt, instances, property_uri)){
            cout<< "\n\n" << class_uri << " - " << property_uri << " is Numeric\n";
            num_pros->push_back(property_uri);
        }
        else{
            cout << "\n\n" <<class_uri << " - " << property_uri << " not Not\n";
        }
    }
    write_single_class(class_uri, num_pros);
    delete properties;
    delete instances;
    delete num_pros;
}

void write_single_class(string class_uri, std::list<string>* properties){
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

bool isNumeric(HDT *hdt, std::list<string> *instances, string property_uri){
    long num_of_num=0, num_of_lit=0;
    double v=0;
    int i;
    string holder;
    IteratorTripleString *it;
    TripleString * triple;
    for(auto ins_it=instances->cbegin();ins_it!=instances->cend();ins_it++){
        it = hdt->search((*ins_it).c_str(),property_uri.c_str(),"");
        if(it->hasNext()){
            //cout << "has next: \n";
            triple = it->next();
            triple->getObject();
            if(str_to_double(triple->getObject(),v)){
            //v = strtod(triple->getObject().c_str(),NULL);
            //if(v==0 || v==HUGE_VAL || v==-HUGE_VAL){
                num_of_num++;
            }
            else{
                //cout << "not number: "<< triple->getObject() << endl;
                num_of_lit++;
            }
        }/*
        else{
            cout << "no next: \n";
            cout << "<" << (*ins_it).c_str() << "> <" << property_uri.c_str() << "> ";
        }*/
    }
    log(logfname, "nums: "+to_string(num_of_num)+"  literals: "+to_string(num_of_lit));
    //printf("nums: %ld lit: %ld\n", num_of_num, num_of_lit);
    delete it; // Remember to delete iterator to avoid memory leaks!
    return num_of_num > num_of_lit;
}

bool str_to_double(string s, double & val){
    int i;
    string num_str="";
    bool got_dec;
    got_dec=0;
    for(i=0;i<s.length();i++){
        if(s[i]>='0' && s[i]<='9'){
            num_str += s[i];
        }
        else if(s[i]=='.' && !got_dec){
            num_str += s[i];
        }
    }
    if(num_str=="" || num_str=="."){
        return 0;
    }
    else{

        //val=0;
        //int mul = 1;
        // do the pre dec
        /*
        for(i=0;i<num_str.length();i++){
            val *= mul;
            mul *= 10;
            val += atof(num_str[i]) ;
        }
        */
        // do the post dec
        val = strtod(num_str.c_str(),NULL);
        return 1;
    }
}

string get_class_from_line(string line){
    int i;
    for(i=0;i<line.length();i++){
        if(line[i]=='\t'){
            //cout << "get class: " << line.substr(0,i) << endl;
            //log(logfname, "get class: "+line.substr(0,i));
            return line.substr(0,i);
        }
    }
    return "";
}

std::list<string>* get_instances(HDT* hdt, string class_uri){
    std::list<string> *instances;
    instances = new std::list<string>;
    IteratorTripleString *it = hdt->search("", "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",class_uri.c_str());
    TripleString *triple;
    while(it->hasNext()){
        triple = it->next();
        instances->push_back(triple->getSubject());
        //return instances;
    }
    delete it;
    //cout << "num of instances: " << instances->size();
    log(logfname, "num of instances: "+to_string(instances->size()));
    return instances;
}

std::list<string> *get_properties_from_line(string line){
    std::list<string> *properties;
    int i,len;
    int start=0;
    properties = new std::list<string>;
    for(i=0;i<line.length();i++){
        if(line[i]=='\t'){
            len = i-start;
            properties->push_back(line.substr(start,len));
            //cout << "\nspliting: ("<<start << "," << i << ") " << line.substr(start,len)<< endl;
            start = i+1;
        }
        else if(i==line.length()-1){
            len = i-start;
            properties->push_back(line.substr(start,len+1));            
            //cout<< "reached the last: " << line.substr(start,len) << endl ; 
        }
        else{
            //cout << "Char <" <<line[i] << ">" ;//<<endl;
        }
    }
    if(properties->size()> 0){
        properties->pop_front();// Remove the class, which is typically the first element in each line
    }
    /*
    cout << "properties\n=============\n";
    for(auto it=properties->cbegin(); it!=properties->cend() ;it++){
        cout << "property: " << *it << endl;
    }
    */
    log(logfname, "num of properties: "+to_string(properties->size()));
    //cout << "num of properties: " << properties->size() << endl;
    return properties;
}

