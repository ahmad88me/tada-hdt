
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
    void automic_write_classes(string out_file_dir); // To fetch classes and write them to a tsv file

    std::list<string>* get_properties_of_class(string class_uri);
    void write_properties(string classes_file_dir, string out_file); // To use the classes tsv file to extract all properties with enough properties (see m_min_num_of_prop)

    std::list<string>* get_num_properties_from_line(string line);
    void write_numeric_prop(string properties_file_dir, string numeric_prop_file_dir);

    bool fast_is_numeric(string class_uri, string property_uri, std::list<string>** instances_addr);
    bool isNumeric(std::list<string>*, string);

    void write_single_class(string file_dir, string class_uri, std::list<string>* properties);

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
