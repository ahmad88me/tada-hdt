
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
#include <unordered_set>

#include "filter_num.h"
#include "logger.h"
#include "common.h"

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
        m_logger->log("The classes already extracted to: "+out_file_dir);
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

    std::unordered_set<string> processed_classes;
    std::list<string>* processed_classes_list = get_processed_classes(out_file);
    for(auto it=processed_classes_list->cbegin();it!=processed_classes_list->cend();it++){
        processed_classes.insert(*it);
    }
    delete processed_classes_list;

    for(auto it=classes->cbegin();it!=classes->cend();it++){
        if(processed_classes.find((*it))==processed_classes.cend()){ // not found (it means not processed)
            properties = get_properties_of_class(*it);
            write_single_class(out_file, *it, properties); // *it is the class_uri
            delete properties;
        }
    }
}

std::list<string>* Filternum::get_num_properties_from_line(string line){
    string class_uri = get_class_from_line(line);
    std::list<string>* properties = this->get_properties_from_line(line);
    std::list<string>* instances=nullptr;// = this->get_instances(class_uri);
    std::list<string>* num_properties = new std::list<string>;
    for(auto it=properties->cbegin();it!=properties->cend();it++){
        //if(this->isNumeric(instances, *it)){ // *it is the uri of a property
        if(this->fast_is_numeric(class_uri, *it,&instances)){
            num_properties->push_back(*it);
        }
    }

    if(instances!=nullptr){
        delete instances;
    }
    delete properties;
    return num_properties;
}


bool Filternum::fast_is_numeric(string class_uri, string property_uri, std::list<string>** instances_addr){
    string line;
    IteratorTripleString *itt;
    TripleString * triple;
    double v;
    unsigned long num_of_numeric=0, num_of_letrals=0;
    size_t estimated_of_class, estimated_of_property;
    itt = hdt->search("", property_uri.c_str(), "");
    estimated_of_property = itt->estimatedNumResults();
    delete itt;
    itt = hdt->search("", rdf_type.c_str(), class_uri.c_str());
    estimated_of_class = itt->estimatedNumResults();
    delete itt;
    std::list<string>* instances_of_class = new std::list<string>; // instances of a given class
    std::list<string>* instances_of_property = new std::list<string>; // instances that has a given property
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
                    //values->push_front(v);
                    num_of_numeric++;
                }
                else{
                    num_of_letrals++;
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
            //instances_addr = &instances_of_class;
            if((*instances_addr)==nullptr){
                m_logger->log("ERROR??");
            }
            else{
                m_logger->log("locally ok: "+to_string((*instances_addr)->size()));
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
                    //values->push_front(v);
                    num_of_numeric++;
                }
                else{
                    num_of_letrals++;
                }
            }
            delete itt;
        }
    }
    delete instances;
    delete instances_of_property;
    return num_of_numeric > num_of_letrals;
}




void Filternum::write_numeric_prop(string properties_file_dir, string numeric_prop_file_dir){
    std::list<string>* processed_classes_list = this->get_processed_classes(numeric_prop_file_dir);
    std::unordered_set<string> processed_classes;

    for(auto it=processed_classes_list->cbegin();it!=processed_classes_list->cend();it++){
        processed_classes.insert(*it);
    }

    ifstream prop_file(properties_file_dir);
    string line;
    string class_uri;
    std::list<string>* properties;
    while(getline(prop_file, line)){
        class_uri = get_class_from_line(line);
        if(processed_classes.find(class_uri)==processed_classes.cend()){ // not found
            cout<< "not found: "<<class_uri<<endl;
            properties = this->get_num_properties_from_line(line);
            this->write_single_class(numeric_prop_file_dir, class_uri, properties);
            delete properties;
        }
        else{
            cout<< "is found: "<<class_uri<<endl;
        }
    }
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


bool Filternum::isNumeric(std::list<string> *instances, string property_uri){
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
    m_logger->log("nums: "+to_string(num_of_num)+"  literals: "+to_string(num_of_lit));
    return num_of_num > num_of_lit;
}

std::list<string>* Filternum::get_instances(string class_uri){
    std::list<string> *instances;
    instances = new std::list<string>;
    IteratorTripleString *it = hdt->search("", "http://www.w3.org/1999/02/22-rdf-syntax-ns#type",class_uri.c_str());
    TripleString *triple;
    while(it->hasNext()){
        triple = it->next();
        instances->push_back(triple->getSubject());
    }
    delete it;
    m_logger->log("num of instances: "+to_string(instances->size()));
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
    m_logger->log("num of properties: "+to_string(properties->size()));
    return properties;
}

