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
void write_features(string hdt_file_dir, string num_property_dir){
    HDT *hdt = HDTManager::mapHDT(hdt_file_dir.c_str());       
    ifstream in_file(num_property_dir);
    std::list<clspropair*>* processed=get_processed_feat_clspairs(FEAT_FNAME);
    std::list<string>* instances=NULL;
    bool found;
    string line,class_uri="";
    clspropair* pair;
    int num_processed=processed->size();
    if(in_file.is_open()){
        while(getline(in_file, line)){
            pair = get_clspropair_from_line(line);
            found = false;
            for(auto it=processed->cbegin();it!=processed->cend();it++){
                if((*it)->class_uri == pair->class_uri && (*it)->property_uri == pair->property_uri){
                    found = true;
                    break;
                }
            }
            if(!found){
                // if the instances are not for this class fetch them
                if(class_uri != pair->class_uri){
                    if(instances!=NULL){
                        delete instances;
                    }
                    instances = get_instances(hdt, class_uri);
                }
                compute_store_features_for_pair(hdt, instances, pair); 
                num_processed++;
                log(logfname, "processed classes: "+to_string(num_processed));
            }
        }
    }
    delete processed;
    delete hdt;
}

void compute_store_features_for_pair(HDT* hdt, std::list<string>* instances, clspropair* pair){
    string line;
    line = compute_features_line(hdt, pair->class_uri, pair->property_uri, instances);
    delete pair;
    ofstream outf;
    outf.open(FEAT_FNAME, ios::app);
    outf << line;
    outf.close();
}

string compute_features_line(HDT* hdt, string class_uri, string property_uri, std::list<string>* instances){
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
    line = class_uri + "\t" + property_uri +"\t"+ to_string(values->size())  + "\t" + to_string(mean_value) + "\t" + to_string(median(values)) + "\t" + to_string(stdev(values, mean_value));
    delete values;
    return line+"\n";
}

clspropair* get_clspropair_from_line(string line){
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

std::list<clspropair*>* get_processed_feat_clspairs(string features_file_name){
    string line;
    string class_uri;
    ifstream in_file(features_file_name);
    std::list<clspropair*>* processed = new std::list<clspropair*>;
    if(in_file.is_open()){
        while(getline(in_file, line)){
            processed->push_back(get_clspropair_from_line(line));
        }   
        return processed;
    }   
    else{
        cout << "unable to open: "<< features_file_name <<endl;
        return 0;
    }  
}

