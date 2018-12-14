
#ifndef FILTER_NUM_H
#define FILTER_NUM_H
#include <HDTManager.hpp>
#include <list>
#include <easy_logger/easy_logger.h>

using namespace hdt;


class Filternum{
public:
    Filternum(string, string);
    std::list<string> *get_leaf_classes();
    void automic_write_classes(string out_file_dir); // To fetch classes
    std::list<string>* get_properties_of_class(string class_uri);
    void write_properties(string classes_file_dir, string out_file);


    //    bool str_to_double(string , double & );
    bool isNumeric(HDT*, std::list<string>*, string);
    void store_single_class(HDT* hdt, string line);
    void write_single_class(string file_dir, string class_uri, std::list<string>* properties);
    void store_num_cols(string, string);
//    string get_class_from_line(string);
    std::list<string>* get_properties_from_line(string line);
    std::list<string> *get_instances(string class_uri);
    std::list<string> *get_processed_classes(string);

    unsigned m_min_num_of_res = 20; // the minimum number of resources in a class to be included
    unsigned m_min_num_of_prop = 20; // the minimum number of properties in a class to be included


private:
    HDT* hdt;
    EasyLogger* m_logger;
};
#endif
