
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <list>
#include <set>

#include "filter_num.h" 
#include "logger.h"

string logfname = "features.log"
string NUM_PROP_FNAME = "class_property_num.tsv"
string FEAT_FNAME = "features.tsv"

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
    return std_value
}

double mean(std::list<double>* values){
    return std::accumulate(values->cbegin(), values->cend(), 0)/values->size();
}

double median(std::list<double>* values){
    bool even;
    int i=0;
    if(values->size()%2==0){
        even=0;
    }
    else{
        even=1;
    }
    if(even){
        for(auto it=values->cbegin;it!=values->cend();it++, i++){
            if(i==values->size()/2-1){
                return (*it) + *(it+1);
            }
        }
    }
    else{
        for(auto it=values->cbegin;it!=values->end();it++, i++){
            if(i==values->size()/2){
                return (*it);
            }

        }
    }
}

void write_features(string hdt_file_dir){
    HDT *hdt = HDTManager::mapHDT(hdt_file_dir.c_str());       
    ifstream in_file(NUM_PROP_FNAME);
    std::set<string>* processed = get_processed_classes();
    bool found;
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
                write_class_features(class_uri); 
                num_processed++;
            }
        }
    }
    delete processed_classes;
    delete hdt;
}

std::set<string>* get_processed_classes(){
    std::set<string>* classes_set=new std::set<string>;
    string line;
    string class_uri;
    ifstream in_file(FEAT_FNAME);
    if(in_file.is_open()){
        while(getline(in_file, line)){
            class_uri = get_class_from_line(line);
            classes_set.insert(class_uri);
        }
        return classes_set
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
        lines += get_class_prop_features_line(hdt, class_uri, (*it), instances) + "\n";
    }
    ofstream outf;
    outf.open(FEAT_FNAME, ios::app);
    outf << lines;
    outf.close()
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
    line = class_uri + "\t" + property_uri + "\t" + to_string(mean_value) + "\t" + to_string(median(values)) + "\t" + to_string(stdev(values, mean_value))
    delete values;
    return line;
}
