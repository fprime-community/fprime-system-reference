// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"
#include <STest/STest/Random/Random.hpp>

TEST(Nominal, getGyroscope) {
    Gnc::Tester tester;
    tester.testGetGyroTlm();
}

TEST(Nominal, getAccelerometer) {
  Gnc::Tester tester;
  tester.testGetAccelTlm();
}

TEST(Nominal, Error) {
  Gnc::Tester tester;
  tester.testError();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
