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
//#include "filter_num.h"
#include "logger.h"
#include "common.h"




static string logfname = "sample-features.log";
static string FEAT_FNAME = "sample-features.tsv";

//static string logfname = "tests-features.log";
//static string FEAT_FNAME = "tests-features.tsv";
////static string FEAT_FNAME = "features-min-height-BasketballPlayer.tsv";
//static string rdf_type = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type";




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
    
    if(values->size()==0){
        std_value = 0;
    }
    else{
        std_value = numerator_sum/values->size();
        std_value = sqrt(std_value);
    }
    
    /*
    std_value = numerator_sum/values->size();
    std_value = sqrt(std_value);
    */
    return std_value;
}

double mean(std::list<double>* values){
    
    if(values->size()==0){
        return 0;
    }
    
    return std::accumulate(values->cbegin(), values->cend(), 0)/values->size();
}

double median(std::list<double>* values){
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


// This is the entry point
void write_features(string hdt_file_dir, string num_property_dir){
    //HDT *hdt = HDTManager::mapHDT(hdt_file_dir.c_str());
    HDT *hdt = hdt::HDTManager::mapIndexedHDT(hdt_file_dir.c_str());
    ifstream in_file(num_property_dir);
    std::list<clspropair*>* processed=get_processed_feat_clspairs(FEAT_FNAME);
    std::list<string>* instances=nullptr;
    std::list<string>* tem=nullptr;
    bool found, instances_fetched;
    string line,class_uri="";
    clspropair* pair;
    std::list<clspropair*>* pairs;
    int num_processed=processed->size();
    log(logfname, "number of already processed pairs: "+to_string(num_processed));
    if(in_file.is_open()){
        log(logfname, "open file: "+num_property_dir);
        while(getline(in_file, line)){
            pairs = get_pairs_from_numfilter(line);
            class_uri = get_class_from_line(line);
            instances_fetched = false;
            if(instances!=nullptr){
                delete instances;
            }
            instances=nullptr;
            log(logfname, "("+to_string(pairs->size())+") properties for "+" class: "+class_uri);
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
                    compute_store_features_for_pair(hdt, pair, &instances);
                    if(instances==nullptr){
                        log(logfname, "outside instances: null");
                    }
                    else{
                        log(logfname, "outside instances: "+to_string(instances->size()));
                    }
//                    if(instances!=nullptr){
//                        instances_fetched = true;
//                    }
                    num_processed++;
                    log(logfname, "processed class/property pairs: "+to_string(num_processed));
                }// if not found
            }// loop for fetch pairs
        }//while
    }
    else{
        log(logfname, "ERROR unable to open file: "+num_property_dir);
    }
    delete processed;
    delete hdt;
}

std::list<string>* get_subset(std::list<string> * l, int new_size){
    std::list<string> * subs = new std::list<string>;
    for (auto it = l->cbegin() ;  it != l->cend() && subs->size() < new_size; it++){
        subs->push_back(*(it));
    }
    return subs;
}


void compute_store_features_for_pair(HDT* hdt, clspropair* pair, std::list<string>** instances_addr){
    string line;
    line = compute_features_line(hdt, pair->class_uri, pair->property_uri, instances_addr);
    if((*instances_addr)==nullptr){
        log(logfname, "intermediate: null ");
    }
    else{
        log(logfname, "intermediate: "+to_string((*instances_addr)->size()));
    }
    delete pair;
    //log(logfname, "writing the results");
    ofstream outf;
    outf.open(FEAT_FNAME, ios::app);
    outf << line;
    outf.close();
}

string compute_features_line(HDT* hdt, string class_uri, string property_uri, std::list<string>** instances_addr){
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
    log(logfname, "num of instances of the class <"+class_uri+">: "+to_string(estimated_of_class));
    log(logfname, "num of instances that has the property <"+property_uri+">: "+to_string(estimated_of_property));
    //if(false){
    if(estimated_of_property < estimated_of_class){
        log(logfname, "SWAP!");
        log(logfname, "Getting the instance for the property: "+property_uri);
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
            //log(logfname, "internal hdt search");
            itt = hdt->search((*it).c_str(), property_uri.c_str(), "");
            //log(logfname, "has next? "+to_string(itt->hasNext()));
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
        log(logfname, "No swap!");
        // if instances of a given class is already fetched before
        if((*instances_addr)!=nullptr){
            log(logfname, "use cached instances");
            instances_of_class = (*instances_addr);
        }
        else{
            log(logfname, "fetching instances for a the class: "+class_uri);
            // get the entities for a given class
            itt = hdt->search("", rdf_type.c_str(), class_uri.c_str());
            while(itt->hasNext()){
                triple = itt->next();
                instances_of_class->push_back(triple->getSubject());
            }
            delete itt;
            (*instances_addr) = instances_of_class;
            //instances_addr = &instances_of_class;
            if((*instances_addr)==nullptr){
                log(logfname, "ERROR??");
            }
            else{
                log(logfname, "locally ok: "+to_string((*instances_addr)->size()));
            }
        }
        // get the objects for the class/property pairs
        for(auto it=instances_of_class->cbegin();it!=instances_of_class->cend();it++){
            //log(logfname, "internal hdt search");
            itt = hdt->search((*it).c_str(), property_uri.c_str(), "");
            //log(logfname, "has next? "+to_string(itt->hasNext()));
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
        cout << "file not open: "<< features_file_name <<endl;
        return processed;
    }  
}

std::list<clspropair*>* get_pairs_from_numfilter(string line){
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
