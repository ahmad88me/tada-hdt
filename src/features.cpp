
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <cmath>
#include <list>
#include <set>
#include <numeric>

#include "features.h"
#include "filter_num.h" 
#include "logger.h"

static string logfname = "features.log";
static string NUM_PROP_FNAME = "class_property_num.tsv";
static string FEAT_FNAME = "features.tsv";

using namespace std;

double stdev(std::list<double>* values, double mean_value){
    int i;
    double numerator_sum = 0;
    double diff;
    double std_value;
    for(auto it=values->cbegin(); it!=values->cend();it++){
        diff = (*it) - mean_value;
        numerator_sum+=  diff*diff;
    }
    std_value = numerator_sum/values->size();
    std_value = sqrt(std_value);
    return std_value;
}

double mean(std::list<double>* values){
    return std::accumulate(values->cbegin(), values->cend(), 0)/values->size();
}

double median(std::list<double>* values){
    bool even;
    int i=0;
    even = values->size()%2==0;
    values->sort();
    if(even){
        for(auto it=values->cbegin();it!=values->cend();it++, i++){
            if(i==values->size()/2-1){
                //return (*it) + *(it+1);
                return ((*it) + *(++it))/2;
            }
        }
    }
    else{
        for(auto it=values->cbegin();it!=values->cend();it++, i++){
            if(i==values->size()/2){
                return (*it);
            }
        }
    }
    return 0;// just for testing
}


// This is the entry point
void write_features(string hdt_file_dir){
    HDT *hdt = HDTManager::mapHDT(hdt_file_dir.c_str());       
    ifstream in_file(NUM_PROP_FNAME);
    //commented temprorarly
    std::set<string>* processed;// = get_processed_classes();
    bool found;
    string line,class_uri;
    int num_processed=processed->size();
    if(in_file.is_open()){
        while(getline(in_file, line)){
            class_uri = get_class_from_line(line);
            found = 0;
            for(auto it=processed->cbegin();it!=processed->cend();it++){
                if((*it)==class_uri){
                    found = 1;
                    break;
                }
            }
            if(!found){
                log(logfname, "processed classes: "+to_string(num_processed));
                //commented temp
                //write_class_features(class_uri); 
                num_processed++;
            }
        }
    }
    delete processed;
    delete hdt;
}

std::set<string>* get_processed_classes2(){
    std::set<string>* classes_set=new std::set<string>;
    string line;
    string class_uri;
    ifstream in_file(FEAT_FNAME);
    if(in_file.is_open()){
        while(getline(in_file, line)){
            class_uri = get_class_from_line(line);
            classes_set->insert(class_uri);
        }
        return classes_set;
    }
    else{
        cout << "unable to open: "<< FEAT_FNAME <<endl;
    }

}



void write_class_features(HDT* hdt, string class_uri, string line){
    std::list<string>* instances = get_instances(hdt, class_uri);
    std::list<string>* properties = get_properties_from_line(line);
    string lines=""; // lines to write
    for(auto it=properties->cbegin();it!=properties->cend();it++){
        //temproary comented this line
        //lines += get_class_prop_features_line(hdt, class_uri, (*it), instances) + "\n";
    }
    ofstream outf;
    outf.open(FEAT_FNAME, ios::app);
    outf << lines;
    outf.close();
    delete properties;
    delete instances;
}

string get_class_prop_features_line(HDT* hdt, string class_uri, string property_uri, std::list<string>* instances){
    string line;
    std::list<double> *values = new std::list<double>;
    IteratorTripleString *itt;
    TripleString * triple;
    double v;
    for(auto it=instances->cbegin();it!=instances->cend();it++){
        itt = hdt->search((*it).c_str(), property_uri.c_str(), "");
        if(itt->hasNext()){
            triple = itt->next();
            if(str_to_double(triple->getObject(),v)){
                values->push_front(v);
            }
        }
    }
    double mean_value;
    mean_value = mean(values);
    line = class_uri + "\t" + property_uri + "\t" + to_string(mean_value) + "\t" + to_string(median(values)) + "\t" + to_string(stdev(values, mean_value));
    delete values;
    return line;
}

clspropair*  get_clspropair_from_line(string line){
    int i; 
    string t;
    clspropair * pair = new clspropair;
    pair->class_uri = "";
    int start_idx = 0;
    int len;
    for(i=0;i<line.length();i++){
        if(line[i]=='\t' || line[i]=='\n' || i==line.length()-1){
            t = line.substr(start_idx,i-start_idx);
            if(pair->class_uri == ""){ //
                pair->class_uri = t;
                start_idx = i+1;
            }
            else{
                pair->property_uri = t;
                return pair;
            }
        }
    }
}

std::list<clspropair>* get_processed(){
    string line;
    string class_uri;
    ifstream in_file(FEAT_FNAME);
    if(in_file.is_open()){
        while(getline(in_file, line)){
            class_uri = get_class_from_line(line);
            //classes_set->insert(class_uri);
        }   
        //return classes_set;
        return NULL;
    }   
    else{
        cout << "unable to open: "<< FEAT_FNAME <<endl;
    }  
}

