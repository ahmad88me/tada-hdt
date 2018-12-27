#include <iostream>
#include <stdio.h>
#include <string>
#include <gtest/gtest.h>
#include <HDTEnums.hpp>
#include <fstream>
#include <algorithm>

#include "features.h"
#include "common.h"
#include "filter_num.h"

void delete_if_exists(string file_dir){
    if(access(file_dir.c_str(), F_OK ) != -1){
        remove(file_dir.c_str());
    }
}

void ttl_to_hdt(string ttl_dir){
    HDTSpecification spec("");
    string baseUri="<file://"+ttl_dir+">";
    string output_file;
    HDT *hdt = HDTManager::generateHDT(ttl_dir.c_str(), baseUri.c_str(), TURTLE, spec, NULL);
    output_file = ttl_dir.substr(0, ttl_dir.find_last_of(".")) +  ".hdt"; // same name but with .hdt instread of .ttl
    ofstream out;
    // Save HDT
    out.open(output_file.c_str(), ios::out | ios::binary | ios::trunc);
    if(!out.good()){
        throw std::runtime_error("Could not open output file.");
    }
    hdt->saveToHDT(out, NULL);
    out.close();
}

namespace {

    TEST(FeaturesTest, Teststrtodouble){
    string s = "\"204.0\"^^<http://dbpedia.org/datatype/centimetre>";
    double v;
    ASSERT_TRUE(str_to_double(s,v));
    ASSERT_EQ(v, 204.0);
    ASSERT_FALSE(str_to_double("'a2.0",v));
    ASSERT_FALSE(str_to_double("_a2.0",v));
    ASSERT_TRUE(str_to_double("204a.4",v));
    ASSERT_EQ(204,v);
    ASSERT_FALSE(str_to_double(".123aa",v));
    }

    TEST(FeaturesTest, ClsProPairFromLine) {
        string class_uri = "http://dbpedia.org/ontology/Person";
        string property_uri = "http://dbpedia.org/ontology/Person/height";
        ASSERT_EQ(1,1); // just to test the setup
        //ASSERT_EQ(1,2); // just to test the fail
        clspropair* pair1 =  get_clspropair_from_line(class_uri+"\t"+property_uri);
        clspropair* pair2 =  get_clspropair_from_line(class_uri+"\t"+property_uri);
        ASSERT_EQ(*pair1, *pair2);
        delete pair2, pair1;
        pair1 = get_clspropair_from_line(class_uri+"\t"+property_uri+"\t"+"123");
        pair2 = get_clspropair_from_line(class_uri+"\t"+property_uri+"\t"+"123");
        ASSERT_EQ(*pair1, *pair2);
        delete pair2, pair1;
        pair1 = get_clspropair_from_line(class_uri+"\t"+property_uri+"\n");
        pair2 = get_clspropair_from_line(class_uri+"\t"+property_uri+"\n");
        ASSERT_EQ(*pair1, *pair2);
        delete pair2, pair1;
        pair1 = get_clspropair_from_line(class_uri+"\t"+property_uri+"notthesame\t123");
        pair2 = get_clspropair_from_line(class_uri+"\t"+property_uri+"\t123");
        ASSERT_NE(*pair1,*pair2);
        delete pair1, pair2;
    }

    TEST(FeaturesTest, ClsProPairProc){
        string class_uri1 = "http://dbpedia.org/ontology/Person";
        string property_uri1_1 = "http://dbpedia.org/ontology/Person/height";
        string property_uri1_2 = "http://dbpedia.org/ontology/Person/weight";
        string class_uri2 = "http://dbpedia.org/ontology/test";
        string property_uri2_1 = "http://dbpedia.org/ontology/P1";
        string property_uri2_2 = "http://dbpedia.org/ontology/p2";
        string property_uri2_3 = "http://dbpedia.org/ontology/p3";
        string content="";
        content =
            "http://dbpedia.org/ontology/Person\thttp://dbpedia.org/ontology/Person/height\t1\t2\n"
            "http://dbpedia.org/ontology/Person\thttp://dbpedia.org/ontology/Person/weight\t1\t2\n"
            "http://dbpedia.org/ontology/test\thttp://dbpedia.org/ontology/Person/P1\t1\t2\n"
            "http://dbpedia.org/ontology/test\thttp://dbpedia.org/property/p2\t1\t2\n"
            "http://dbpedia.org/ontology/test\thttp://dbpedia.org/property/p1\t1\t2\n"
            ;
        string feat_test_file = "automated_test_features.tsv.tmp";
        ofstream out_file(feat_test_file);
        out_file << content;
        out_file.close();
        std::list<clspropair*>* processed_pairs;
        processed_pairs = get_processed_feat_clspairs(feat_test_file);
        ASSERT_EQ(processed_pairs->size(),5);
        ASSERT_EQ(processed_pairs->front()->class_uri,"http://dbpedia.org/ontology/Person");
        ASSERT_EQ(processed_pairs->front()->property_uri,"http://dbpedia.org/ontology/Person/height");
        ASSERT_EQ(processed_pairs->back()->class_uri,"http://dbpedia.org/ontology/test");
        ASSERT_EQ(processed_pairs->back()->property_uri,"http://dbpedia.org/property/p1");
        ASSERT_NE(processed_pairs->back()->class_uri,"http://dbpedia.org/ontology/wrong");
        ASSERT_NE(processed_pairs->back()->property_uri,"http://dbpedia.org/ontology/wrong");
        delete processed_pairs;
        remove(feat_test_file.c_str());
    }

    TEST(FeaturesTest, Feats){
        std::list<double> nums;
        nums.push_back(10);
        nums.push_back(100);
        double mean_value = mean(&nums);
        double median_value = median(&nums);
        double std_value = stdev(&nums, mean_value);
        ASSERT_EQ(mean_value, 55);
        ASSERT_EQ(median_value, mean_value);
        ASSERT_EQ(std_value, 45);
        nums.push_back(11);
        median_value = median(&nums);
        ASSERT_EQ(median_value, 11);
    }

    TEST(FeaturesTest, FeatsExt){
        string class_uri = "http://someclass/A";
        string prop1 = "http://someprop1";
        string prop2 = "http://anotherprop2";
        string line = class_uri+"\t"+prop1+"\t"+prop2;
        std::list<clspropair*>* pairs = get_pairs_from_numfilter(line);
        ASSERT_EQ(pairs->size(), 2);
        std::list<clspropair*>* mpairs = new std::list<clspropair*>;
        clspropair* pair = new clspropair;
        pair->class_uri = class_uri;
        pair->property_uri = prop1;
        mpairs->push_back(pair);
        pair = new clspropair;
        pair->class_uri = class_uri;
        pair->property_uri = prop2;
        mpairs->push_back(pair);
        ASSERT_EQ((*(pairs->cbegin()))->class_uri,(*(mpairs->cbegin()))->class_uri);
        ASSERT_EQ((*(pairs->cbegin()))->property_uri,(*(mpairs->cbegin()))->property_uri);
        ASSERT_EQ(((pairs->back()))->class_uri,((mpairs->back()))->class_uri);
        ASSERT_EQ(((pairs->back()))->property_uri,((mpairs->back()))->property_uri);
    }

    TEST(FeaturesTest, WriteFeatures){
        string input_file = "sample_golf.ttl";
        string features_file = "sample-features.tsv";
        string numeric_prop_file = "sample-num-prop.tsv";
        string hdt_file = "sample_golf.hdt";
        string hdt_index = hdt_file+".index.v1-1";

        delete_if_exists(hdt_file);
        delete_if_exists(hdt_index);
        delete_if_exists(numeric_prop_file);
        delete_if_exists(features_file);
        ttl_to_hdt(input_file);

        string s;
        s = "http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/ontology/Person/height\thttp://dbpedia.org/ontology/Person/weight\t"
            "http://dbpedia.org/property/children\thttp://dbpedia.org/property/retired\n"
             "http://dbpedia.org/ontology/Company\thttp://dbpedia.org/property/employees";
        ofstream out_num_prop_file(numeric_prop_file);
        out_num_prop_file << s;
        out_num_prop_file.close();
        write_features(hdt_file, numeric_prop_file);

        string line;
        size_t loc1, loc2;

        std::list<std::list<double>*>* features_expected, *features_computed;// means, medians, stds;
        features_computed = new std::list<std::list<double>*>;
        features_expected = new std::list<std::list<double>*>;
        // to test fail
        //features_expected->push_back(new std::list<double>({8,178,179.07, 8}));
        // to have the corret one
        features_expected->push_back(new std::list<double>({8,178,179.07,  6.505632}));
        features_expected->push_back(new std::list<double>({8, 83.000000,83.000000,6.374363}));
        features_expected->push_back(new std::list<double>({8,2.000000,2.000000,2.121320}));
        features_expected->push_back(new std::list<double>({2,1964.000000,1964.000000,23.000000}));
        features_expected->push_back(new std::list<double>({3,77.000000,90.000000,41.448764}));
        string expected_pairs="", generated_pairs="";
        expected_pairs = "http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/ontology/Person/height"
                         "http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/ontology/Person/weight"
                         "http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/property/children"
                         "http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/property/retired"
                         "http://dbpedia.org/ontology/Company\thttp://dbpedia.org/property/employees";



//        std::list<string> expected, generated;
//        expected.push_back("http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/ontology/Person/height");
//        expected.push_back("http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/ontology/Person/weight");
//        expected.push_back("http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/property/children");
//        expected.push_back("http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/property/retired");
//        expected.push_back("http://dbpedia.org/ontology/Company\thttp://dbpedia.org/property/employees");

        size_t num_of_nums;
        double d;
        std::list<double> * features_per_pair;
        ifstream in_file(features_file);
        if(in_file.is_open()){
            while(getline(in_file, line)){
               loc1 = line.find_first_of("\t");
               s = line.substr(0,loc1+1);
               line.replace(loc1, 1, "\n");
               loc2 = line.find_first_of("\t");
               s+= line.substr(loc1+1, loc2-loc1-1);
               generated_pairs += s;
               // now collect the computed features
               line.replace(loc2, 1, "\n");
               num_of_nums = std::count(line.begin(), line.end(), '\t');
               loc1 = loc2;
               features_per_pair = new std::list<double>;
               for(size_t i=0;i<num_of_nums+1;i++){
                    loc2 = line.find_first_of("\t");
                    if(loc2==-1){
                        loc2 = line.size();
                    }
                    s = line.substr(loc1+1, loc2-loc1-1);
                    ASSERT_TRUE(str_to_double(s,d));
                    features_per_pair->push_back(d);
                    line.replace(loc2, 1, "\n");
                    loc1 = loc2;
               }
               features_computed->push_back(features_per_pair);
            }
        }
        auto ito1 = features_computed->cbegin();
        auto ito2 = features_expected->cbegin();
        auto iti1 = features_computed->front()->cbegin();
        auto iti2 = features_expected->front()->cbegin();
        for(;ito1 != features_computed->cend() && ito2!=features_expected->cend();ito1++, ito2++){
            for(iti1 = (*ito1)->cbegin(), iti2 = (*ito2)->cbegin(); iti1 !=  (*ito1)->cend() && iti2 !=  (*ito2)->cend(); iti1++, iti2++){
                ASSERT_FLOAT_EQ(*iti1, *iti2);
            }
        }
        ASSERT_EQ(expected_pairs, generated_pairs);
    }

    TEST(FeaturesTest, FullNumFilter){
        string input_file = "sample_golf.ttl";
        string hdt_file = "sample_golf.hdt";
        string hdt_index = hdt_file+".index.v1-1";
        string class_property_file = "sample_golf_class_property_all.tsv";
        string property_num_file = "sample_golf_property_num.tsv";
        string classes_file = "test-classes.tsv";
        string properties_file = "test-properties.tsv";
        string numeric_prop_file = "test-numeric.tsv";
        string test_class = "http://dbpedia.org/ontology/GolfPlayer";
        string test_class2 = "http://dbpedia.org/ontology/Company";
        string log_file_dir = "test-filter.log";
        std::list<string> *leaves;
        std::list<string> *properties;

        delete_if_exists(hdt_file);
        delete_if_exists(hdt_index);
        delete_if_exists(classes_file);
        delete_if_exists(properties_file);
        delete_if_exists(numeric_prop_file);

        ttl_to_hdt(input_file);
        Filternum fn(hdt_file, log_file_dir);
        leaves = fn.get_leaf_classes();
        ASSERT_EQ(leaves->size(), 0);
        delete leaves;
        fn.m_min_num_of_res = 1;
        leaves = fn.get_leaf_classes();
        ASSERT_EQ(leaves->size(), 2);
        fn.automic_write_classes(classes_file);
        ifstream input_classes(classes_file);
        string s, t;        
        s = "";
        while (std::getline(input_classes, t)){
            s+=t;
            s+="\n";
        }
        t = test_class2+"\n"+test_class+"\n";
        ASSERT_EQ(s, t);
        properties = fn.get_properties_of_class(test_class);
        ASSERT_EQ(properties->size(),0);
        fn.m_min_num_of_prop = 1;
        delete properties;
        properties = fn.get_properties_of_class(test_class);
        ASSERT_EQ(properties->size(), 7);
        fn.m_min_num_of_prop = 2;
        delete properties;
        properties = fn.get_properties_of_class(test_class);
        ASSERT_EQ(properties->size(),6);
        delete properties;
        fn.write_properties(classes_file, properties_file);
        ifstream input_properties(properties_file);
        s = "";
        while (std::getline(input_properties, t)){
            s+=t;
        }
        string prop_res;

        prop_res = "http://dbpedia.org/ontology/Company\thttp://dbpedia.org/property/employees\thttp://dbpedia.org/property/name\t"
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type"
                "http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/ontology/Person/height\t"
                "http://dbpedia.org/ontology/Person/weight\thttp://dbpedia.org/property/children\t"
                "http://dbpedia.org/property/name\t"
                "http://dbpedia.org/property/retired\thttp://www.w3.org/1999/02/22-rdf-syntax-ns#type";
        ASSERT_EQ(prop_res, s);

        // To check that it doesn't append and do resume the processing for extracting properties for the list of classes
        fn.write_properties(classes_file, properties_file);
        input_properties.close();

        input_properties.open(properties_file);
        s = "";
        while (std::getline(input_properties, t)){
            s+=t;
        }
        prop_res = "http://dbpedia.org/ontology/Company\thttp://dbpedia.org/property/employees\thttp://dbpedia.org/property/name\t"
                "http://www.w3.org/1999/02/22-rdf-syntax-ns#type"
                "http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/ontology/Person/height\t"
                "http://dbpedia.org/ontology/Person/weight\thttp://dbpedia.org/property/children\t"
                "http://dbpedia.org/property/name\t"
                "http://dbpedia.org/property/retired\thttp://www.w3.org/1999/02/22-rdf-syntax-ns#type";
        ASSERT_EQ(prop_res, s);


        // Test the numeric properties filtration
        fn.write_numeric_prop(properties_file, numeric_prop_file);
        ifstream input_numeric_prop(numeric_prop_file);
        s = "";
        while (std::getline(input_numeric_prop, t)){
            s+=t;
        }
        input_numeric_prop.close();
        string num_res = "http://dbpedia.org/ontology/Company\thttp://dbpedia.org/property/employees"
                         "http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/ontology/Person/height\t"
                         "http://dbpedia.org/ontology/Person/weight\thttp://dbpedia.org/property/children\t"
                         "http://dbpedia.org/property/retired";
        ASSERT_EQ(num_res, s);


        // Test resume the filtration of numeric properties
        ofstream output_numeric_prop;
        output_numeric_prop.open(numeric_prop_file);
        output_numeric_prop << "http://dbpedia.org/ontology/Company\thttp://dbpedia.org/property/employees";
        output_numeric_prop.close();

        fn.write_numeric_prop(properties_file, numeric_prop_file);

        input_numeric_prop.open(numeric_prop_file);
        s = "";
        while (std::getline(input_numeric_prop, t)){
            s+=t;
        }
        input_numeric_prop.close();
        num_res = "http://dbpedia.org/ontology/Company\thttp://dbpedia.org/property/employees"
                  "http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/ontology/Person/height\t"
                  "http://dbpedia.org/ontology/Person/weight\thttp://dbpedia.org/property/children\t"
                  "http://dbpedia.org/property/retired";
        ASSERT_EQ(num_res, s);

        // Test resume the filtration of numeric properties with reversed order (to make is the file is not just overwritten)
        output_numeric_prop.open(numeric_prop_file);
        output_numeric_prop << "http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/ontology/Person/height\t";
        output_numeric_prop << "http://dbpedia.org/ontology/Person/weight\thttp://dbpedia.org/property/children\t";
        output_numeric_prop << "http://dbpedia.org/property/retired";

        output_numeric_prop.close();

//        input_numeric_prop.open(numeric_prop_file);
//        s = "";
//        while (std::getline(input_numeric_prop, t)){
//            s+=t;
//        }
//        input_numeric_prop.close();

        fn.write_numeric_prop(properties_file, numeric_prop_file);

        input_numeric_prop.open(numeric_prop_file);
        s = "";
        while (std::getline(input_numeric_prop, t)){
            s+=t;
        }
        input_numeric_prop.close();
        num_res = "http://dbpedia.org/ontology/GolfPlayer\thttp://dbpedia.org/ontology/Person/height\t"
                  "http://dbpedia.org/ontology/Person/weight\thttp://dbpedia.org/property/children\t"
                  "http://dbpedia.org/property/retired"
                  "http://dbpedia.org/ontology/Company\thttp://dbpedia.org/property/employees";
        ASSERT_EQ(num_res, s);

    }

    TEST(UtilTest, TTLTOHDT){
        string input_file = "sample_golf.ttl";
        string hdt_file = "sample_golf.hdt";
        ASSERT_NE(access( input_file.c_str(), F_OK ),-1);
        bool success_removal_of_hdt=true;
        // to remove the sample hdt file if it exists
        if(access( hdt_file.c_str(), F_OK ) != -1){
            success_removal_of_hdt =  remove(hdt_file.c_str()) == 0;
        }
        ASSERT_TRUE(success_removal_of_hdt);
        ttl_to_hdt(input_file);
        ASSERT_NE(access( hdt_file.c_str(), F_OK ),-1);
    }

}// namespace



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
