
#ifndef FILTER_NUM_H
#define FILTER_NUM_H
#include <HDTManager.hpp>
#include <list>
//#include <HDT.hpp>

using namespace hdt;

typedef struct str_count{
    string str;
    string count;
    friend inline bool operator==(const struct str_count& l, const struct str_count& r){
        return l.str == r.str;
    }
    friend inline bool operator!=(const struct str_count& l, const struct str_count&r){
    return !(l==r);
    }
} str_count;


class Filternum{
public:
    Filternum(string);
//    bool str_to_double(string , double & );
    bool isNumeric(HDT*, std::list<string>*, string);
    void store_single_class(HDT* hdt, string line);
    void write_single_class(string class_uri, std::list<string>* properties);
    void store_num_cols(string, string);
//    string get_class_from_line(string);
    std::list<string>* get_properties_from_line(string line);
    std::list<string> *get_instances(HDT* hdt, string class_uri);
    std::list<string> *get_processed_classes();
    std::list<string> *get_leaf_classes(string);
    void automic_write_classes(string hdt_file_dir, string out_file_dir);
};
#endif
