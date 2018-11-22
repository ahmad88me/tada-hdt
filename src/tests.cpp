#include <iostream>
#include <string>
#include <gtest/gtest.h>
#include "features.h"
#include "filter_num.h"

namespace {

    TEST(FeaturesTest, Teststrtodouble){
	string s = "\"204.0\"^^<http://dbpedia.org/datatype/centimetre>";
	double v;
	EXPECT_TRUE(str_to_double(s,v));
	EXPECT_EQ(v, 204.0);
	//cout<< "value: "<<v<<endl;
    }

    TEST(FeaturesTest, ClsProPairFromLine) {
        string class_uri = "http://dbpedia.org/ontology/Person";
        string property_uri = "http://dbpedia.org/ontology/Person/height";
        EXPECT_EQ(1,1); // just to test the setup
        //EXPECT_EQ(1,2); // just to test the fail
        clspropair* pair1 =  get_clspropair_from_line(class_uri+"\t"+property_uri); 
        clspropair* pair2 =  get_clspropair_from_line(class_uri+"\t"+property_uri); 
        EXPECT_EQ(*pair1, *pair2);
        delete pair2, pair1;
        pair1 = get_clspropair_from_line(class_uri+"\t"+property_uri+"\t"+"123");
        pair2 = get_clspropair_from_line(class_uri+"\t"+property_uri+"\t"+"123");
        EXPECT_EQ(*pair1, *pair2);
        delete pair2, pair1;
        pair1 = get_clspropair_from_line(class_uri+"\t"+property_uri+"\n");
        pair2 = get_clspropair_from_line(class_uri+"\t"+property_uri+"\n");
        EXPECT_EQ(*pair1, *pair2);
        delete pair2, pair1;
        pair1 = get_clspropair_from_line(class_uri+"\t"+property_uri+"notthesame\t123");
        pair2 = get_clspropair_from_line(class_uri+"\t"+property_uri+"\t123");
        EXPECT_NE(*pair1,*pair2);
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
        //cout << "content: "<<endl<<content<<endl;
        string feat_test_file = "automated_test_features.tsv.tmp";
        ofstream out_file(feat_test_file);
        out_file << content;
        out_file.close();
        std::list<clspropair*>* processed_pairs;
        processed_pairs = get_processed_feat_clspairs(feat_test_file); 
        EXPECT_EQ(processed_pairs->size(),5);
        EXPECT_EQ(processed_pairs->front()->class_uri,"http://dbpedia.org/ontology/Person");
        EXPECT_EQ(processed_pairs->front()->property_uri,"http://dbpedia.org/ontology/Person/height");
        EXPECT_EQ(processed_pairs->back()->class_uri,"http://dbpedia.org/ontology/test");
        EXPECT_EQ(processed_pairs->back()->property_uri,"http://dbpedia.org/property/p1");
        EXPECT_NE(processed_pairs->back()->class_uri,"http://dbpedia.org/ontology/wrong");
        EXPECT_NE(processed_pairs->back()->property_uri,"http://dbpedia.org/ontology/wrong");
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
        EXPECT_EQ(mean_value, 55);
        EXPECT_EQ(median_value, mean_value);
        EXPECT_EQ(std_value, 45);
        nums.push_back(11);
        median_value = median(&nums);
        EXPECT_EQ(median_value, 11);
    }

    TEST(FeaturesTest, FeatsExt){
        string class_uri = "http://someclass/A";
        string prop1 = "http://someprop1";
        string prop2 = "http://anotherprop2";
        string line = class_uri+"\t"+prop1+"\t"+prop2;
        std::list<clspropair*>* pairs = get_pairs_from_numfilter(line);
        EXPECT_EQ(pairs->size(), 2);     
        std::list<clspropair*>* mpairs = new std::list<clspropair*>;
        clspropair* pair = new clspropair;
        pair->class_uri = class_uri;
        pair->property_uri = prop1;
        mpairs->push_back(pair);
        pair = new clspropair;
        pair->class_uri = class_uri;
        pair->property_uri = prop2;
        mpairs->push_back(pair);
        EXPECT_EQ((*(pairs->cbegin()))->class_uri,(*(mpairs->cbegin()))->class_uri);
        EXPECT_EQ((*(pairs->cbegin()))->property_uri,(*(mpairs->cbegin()))->property_uri);
        EXPECT_EQ(((pairs->back()))->class_uri,((mpairs->back()))->class_uri);
        EXPECT_EQ(((pairs->back()))->property_uri,((mpairs->back()))->property_uri);
    }
}// namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
