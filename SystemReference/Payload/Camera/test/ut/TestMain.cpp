// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, cameraSave) {
  Payload::Tester tester;
  tester.testCameraActionSave();
}

TEST(Nominal, cameraProcess) {
  Payload::Tester tester;
  tester.testCameraActionProcess();
}

TEST(Nominal, blankFrame) {
  Payload::Tester tester;
  tester.testBlankFrame();
}

TEST(Nominal, badBuffer) {
  Payload::Tester tester;
  tester.testBadBufferRawImg();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
