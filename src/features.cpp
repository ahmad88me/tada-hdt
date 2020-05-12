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
#include "logger.h"
#include "common.h"


using namespace std;

Features::Features(string hdt_file, string log_file){
    hdt = hdt::HDTManager::mapIndexedHDT(hdt_file.c_str());
    //m_logger = new EasyLogger(log_file);
}

double Features::stdev(std::list<double>* values, double mean_value){
    int i;
    double numerator_sum = 0;
    double diff;
    double std_value;
    for(auto it=values->cbegin(); it!=values->cend();it++){
        diff = (*it) - mean_value;
        numerator_sum+=  diff*diff;
    }

    if(values->size()==0){
        std_value = 0;
    }
    else{
        std_value = numerator_sum/values->size();
        std_value = sqrt(std_value);
    }
    return std_value;
}


double Features::mean(std::list<double>* values){

    if(values->size()==0){
        return 0;
    }
    return std::accumulate(values->cbegin(), values->cend(), 0)/values->size();
}

double Features::median(std::list<double>* values){
    bool even;
    int i=0;
    even = values->size()%2==0;
    values->sort();

    if(values->size()==0){
        return 0;
    }

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

std::list<clspropair*>* Features::get_processed_feat_clspairs(string features_file_name){
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
        cout << "file not open: "<< features_file_name <<endl;
        return processed;
    }
}


clspropair* Features::get_clspropair_from_line(string line){
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

void Features::compute_store_features_for_pair(clspropair* pair, std::list<string>** instances_addr){
    string line;
    line = compute_features_line(pair->class_uri, pair->property_uri, instances_addr);
    if((*instances_addr)==nullptr){
        m_logger->log("intermediate: null ");
    }
    else{
        m_logger->log("intermediate: "+to_string((*instances_addr)->size()));
    }
    delete pair;
    ofstream outf;
    outf.open(features_fname, ios::app);
    outf << line;
    outf.close();
}


std::list<clspropair*>* Features::get_pairs_from_numfilter(string line){
    /* The line here is has the form
        class property1 property2 ....
    */
    std::list<clspropair*>* pairs = new std::list<clspropair*>;
    clspropair* pair;
    int i;
    string class_uri,t;
    int start_idx=0;
    for(i=0;i<line.length();i++){
        if(line[i]=='\t' || line[i]=='\n' || i==line.length()-1){
            // capture the whole string for the class or property
            if(i==line.length()-1 && line[i]!='\n' && line[i]!='\t'){
                t = line.substr(start_idx,i-start_idx+1);
            }
            else{
                t = line.substr(start_idx,i-start_idx);
            }
            //check if class
            if(start_idx==0){
                class_uri = t;
            }
            else{
                pair = new clspropair;
                pair->class_uri = class_uri;
                pair->property_uri = t;
                pairs->push_back(pair);
            }
            start_idx = i+1;
        }
    }
    return pairs;
}


std::list<string>* Features::get_subset(std::list<string> * l, int new_size){
    std::list<string> * subs = new std::list<string>;
    for (auto it = l->cbegin() ;  it != l->cend() && subs->size() < new_size; it++){
        subs->push_back(*(it));
    }
    return subs;
}


string Features::compute_features_line(string class_uri, string property_uri, std::list<string>** instances_addr){
    string line;
    std::list<double> *values = new std::list<double>;
    IteratorTripleString *itt;
    TripleString * triple;
    double v;
    size_t estimated_of_class, estimated_of_property;
    itt = hdt->search("", property_uri.c_str(), "");
    estimated_of_property = itt->estimatedNumResults();
    delete itt;
    itt = hdt->search("", rdf_type.c_str(), class_uri.c_str());
    estimated_of_class = itt->estimatedNumResults();
    delete itt;
    std::list<string>* instances_of_class = new std::list<string>;//get_instances(hdt, class_uri); // instances of a given class
    std::list<string>* instances_of_property = new std::list<string>;// instances that has a given property
    std::list<string>* instances = new std::list<string>; // a holder of the join of the two
    m_logger->log("num of instances of the class <"+class_uri+">: "+to_string(estimated_of_class));
    m_logger->log("num of instances that has the property <"+property_uri+">: "+to_string(estimated_of_property));
    if(estimated_of_property < estimated_of_class){
        m_logger->log("SWAP!");
        m_logger->log("Getting the instance for the property: "+property_uri);
        // get instances of a given property
        itt = hdt->search("", property_uri.c_str(), "");
        while(itt->hasNext()){
            triple = itt->next();
            instances_of_property->push_back(triple->getSubject());
        }
        delete itt;
        // get instances that are of the type class_uri
        for(auto it=instances_of_property->cbegin();it!=instances_of_property->cend();it++){
            itt = hdt->search((*it).c_str(), rdf_type.c_str(), "");
            if(itt->hasNext()){
                instances->push_back(*it);
            }
            delete itt;
        }
        // get the objects of the correct instances
        for(auto it=instances->cbegin();it!=instances->cend();it++){
            itt = hdt->search((*it).c_str(), property_uri.c_str(), "");
            if(itt->hasNext()){
                triple = itt->next();
                if(str_to_double(triple->getObject(),v)){
                    values->push_front(v);
                }
            }
            delete itt;
        }
    }
    else{
        m_logger->log("No swap!");
        // if instances of a given class is already fetched before
        if((*instances_addr)!=nullptr){
            m_logger->log("use cached instances");
            instances_of_class = (*instances_addr);
        }
        else{
            m_logger->log("fetching instances for a the class: "+class_uri);
            // get the entities for a given class
            itt = hdt->search("", rdf_type.c_str(), class_uri.c_str());
            while(itt->hasNext()){
                triple = itt->next();
                instances_of_class->push_back(triple->getSubject());
            }
            delete itt;
            (*instances_addr) = instances_of_class;
            if((*instances_addr)==nullptr){
                m_logger->log("ERROR??");
            }
            else{
                m_logger->log("locally ok: "+to_string((*instances_addr)->size()));
            }
        }
        // get the objects for the class/property pairs
        for(auto it=instances_of_class->cbegin();it!=instances_of_class->cend();it++){
            itt = hdt->search((*it).c_str(), property_uri.c_str(), "");
            if(itt->hasNext()){
                triple = itt->next();
                if(str_to_double(triple->getObject(),v)){
                    values->push_front(v);
                }
            }
            delete itt;
        }
    }

    double mean_value;
    mean_value = mean(values);
    line = class_uri + "\t" + property_uri +"\t"+ to_string(values->size())  + "\t" + to_string(mean_value) + "\t" + to_string(median(values)) + "\t" + to_string(stdev(values, mean_value));
    delete values;
    delete instances;
    //delete instances_of_class;
    delete instances_of_property;
    return line+"\n";
}


// This is the entry point
void Features::write_features(string num_property_dir){
    ifstream in_file(num_property_dir);
    std::list<clspropair*>* processed=get_processed_feat_clspairs(features_fname);
    std::list<string>* instances=nullptr;
    std::list<string>* tem=nullptr;
    bool found, instances_fetched;
    string line,class_uri="";
    clspropair* pair;
    std::list<clspropair*>* pairs;
    int num_processed=processed->size();
    m_logger->log("number of already processed pairs: "+to_string(num_processed));
    if(in_file.is_open()){
        m_logger->log("open file: "+num_property_dir);
        while(getline(in_file, line)){
            pairs = get_pairs_from_numfilter(line);
            class_uri = get_class_from_line(line);
            instances_fetched = false;
            if(instances!=nullptr){
                delete instances;
            }
            instances=nullptr;
            m_logger->log("("+to_string(pairs->size())+") properties for "+" class: "+class_uri);
            for(auto it_p=pairs->cbegin();it_p!=pairs->cend();it_p++){
                pair = (*it_p);
                found = false;
                //check if the pair is processed
                for(auto it=processed->cbegin();it!=processed->cend();it++){
                    if((*it)->class_uri == pair->class_uri && (*it)->property_uri == pair->property_uri){
                        found = true;
                        break;
                    }
                }
                if(!found){
                    compute_store_features_for_pair(pair, &instances);
                    if(instances==nullptr){
                        m_logger->log("outside instances: null");
                    }
                    else{
                        m_logger->log("outside instances: "+to_string(instances->size()));
                    }
                    num_processed++;
                    m_logger->log("processed class/property pairs: "+to_string(num_processed));
                }// if not found
            }// loop for fetch pairs
        }//while
    }
    else{
        m_logger->log("ERROR unable to open file: "+num_property_dir);
    }
    delete processed;
    delete hdt;
}















