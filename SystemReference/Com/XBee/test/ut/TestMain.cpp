// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, Initial) {
    Com::Tester tester;
    tester.test_initial();
}

TEST(Nominal, BasicIo) {
    Com::Tester tester;
    tester.test_basic_io();
}


TEST(Nominal, Fail) {
    Com::Tester tester;
    tester.test_fail();
}

TEST(OffNominal, Retry) {
    Com::Tester tester;
    tester.test_retry();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
