//#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include "features.h"

namespace {

  TEST(FeaturesTest, FeatFuncs) {
    string class_uri = "http://dbpedia.org/ontology/Person";
    string property_uri = "http://dbpedia.org/ontology/Person/height";
    EXPECT_EQ(1,1);
  }

}// namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
