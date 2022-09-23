// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, getGyroscope) {
    Gnc::Tester tester;
    tester.testGetGyroTlm();
}

TEST(Nominal, getAccelerometer) {
    Gnc::Tester tester;
    tester.testGetAccelTlm();
}

TEST(Nominal, TelemetryError) {
    Gnc::Tester tester;
    tester.testTlmError();
}

TEST(Nominal, PowerError) {
    Gnc::Tester tester;
    tester.testPowerError();
}

TEST(Nominal, SetupError) {
    Gnc::Tester tester;
    tester.testSetupError();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
