#include <iostream>
#include <string>
#include <gtest/gtest.h>
#include "features.h"

namespace {

  TEST(FeaturesTest, FeatFuncs) {
    string class_uri = "http://dbpedia.org/ontology/Person";
    string property_uri = "http://dbpedia.org/ontology/Person/height";
    EXPECT_EQ(1,1); // just to test the setup
    //EXPECT_EQ(1,2); // just to test the fail
    clspropair* pair1 =  get_clspropair_from_line(class_uri+"\t"+property_uri); 
    clspropair* pair2 =  get_clspropair_from_line(class_uri+"\t"+property_uri); 
    /*
    cout << "testing overloaded equality\n";
    if((*pair)==(*pair2)){
        cout << "pairs are equal, this is correct\n";
    }
    else{
        cout << "pairs are not equal, overloading is not successful\n";
    }
    */
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
      //nums = [10,100]
  }
}// namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
