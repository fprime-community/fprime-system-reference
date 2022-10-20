# IMU Exercise: Unit Testing

This section of the course will cover unit testing. The goal of this exercise is to set up the unit test harness, write
unit tests, and try to achieve 100% code coverage. 

This exercise consists of several learning goals:

1. Generate unit test implementations using `fprime-util impl --ut`
2. Run unit tests using `fprime-util check`
3. Write unit tests to cover each of the [requirements](../../SystemReference/Gnc/Imu/docs/sdd.md#3-requirements) with
   verification of "Unit Test"
4. Generate coverage analysis for the unit tests developed

## Setup

Given the varied state of the project that various students have it may be beneficial to reset to the instructor model.
This will allow us to cover unit testing without accumulating issues from the implementation section. This can be
accomplished with the following commands. 

### Saving Your Current Work

Before resetting to the instruction setup, you may wish to save your work. This can be done quickly with the following
commands:

```bash
cd SystemReference
git checkout -b "my_class_branch"
git add Gnc
git commit -m "implementation and model of Imu component"
```

### Resetting Codebase

In order to reset the codebase, you must first ensure that all changes have been saved as above. If anything outside the
Imu directory was edited, you may need to repeat the `git add` and `git commit` commands above pointing to those files.

Once finished, we can switch to the latest instructor code with these commands:

```bash
git fetch origin
git checkout origin/main
```

### Preparing for Unit Testing

Like our modeling and implementation steps, we will need to move away the example unit tests in order to write
our own. This is done with the following commands:

```bash
cd SystemReference/Gnc/Imu
mv test test.bak
mkdir -p test/ut
```

We also need to pull the unittest `.cpp` source files out of our CMakeLists.txt. That file should now contain:

```cmake
####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Imu.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/Imu.cpp"
        )
register_fprime_module()

# Register the unit test build
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Imu.fpp"
#        "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
#        "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
        )
set(UT_MOD_DEPS STest)
register_fprime_ut()
```
> Note: in this case we just commented-out those .cpp files as we will need to re-add them once we have generated them.

## Unit Test Implementation Stubs

Like the implementation stubs you can implement the basic unittest harness with the `fprime-util impl` command. This is
done by adding the `--ut` flag to the end. We must remember to prepare a build-cache using `fprime-util generate --ut`.
This is done first.

```bash
fprime-util generate --ut
fprime-util impl --ut
```

The following three files have now been generated: `Tester.cpp`, `Tester.hpp`, and `TestMain.cpp`. These were placed in
the component's base directory and should be moved into the `test/ut` directory.

```bash
mv Test*.?pp test/ut
```

Now the `CMakeList.txt` may be restored to the reference and should contain:

```cmake
####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Imu.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/Imu.cpp"
        )
register_fprime_module()

# Register the unit test build
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Imu.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
        )
set(UT_MOD_DEPS STest)
register_fprime_ut()
```

> Learning goal #1 has now been accomplished!

To check that everything is done properly, we can run `fprime-util check` to execute the unittest. There are no test
cases defined, so it should pass.

```bash
fprime-util check
```
> Learning goal #2 has now been accomplished! Repeat this command to run the tests as we implement.

## Developing Unit Tests

Now it is time to write our first unit test. This is done in two steps:

1. Add a test case to `TestMain.cpp`
2. Add a test case implementation to `Tester.cpp` and `Tester.hpp`

These steps may be repeated for each test that we need to define.

### Adding Tests to `TestMain.cpp`

The first step is to open `test/ut/TestMain.cpp` and add a test case. This tells our unit test system (google test) to
run the supplied code as a test.  It takes a test suite name and a test case name. In the case below we are defining the
"Nominal" test suite and the "getGyroscope" test case.  Test suites are used to group tests and may be reused for future
tests, however; test case names should be unique.

> Note: the `testGetGyroTlm` member will be defined in the next step.

```c++
TEST(Nominal, getGyroscope) {
    Gnc::Tester tester;
    tester.testGetGyroTlm();
}
```

### Adding Tests to `Tester.cpp` and `Tester.hpp`

Next, we need to add test case implementations within the tester class. These are named with the function called in the
case in `TestMain.cpp` and run the unit test.

Below is an example test case implementation.  It should be added to `test/ut/Tester.cpp`.  Do not forget to add the
member declaration to `test/ut/Tester.hpp` shown just below our example.

**Test Case Implementation in `Tester.cpp`**
```c++
    Tester() :
      ImuGTestBase("Tester", MAX_HISTORY_SIZE),
      component("Imu"),
      gyroSerBuf(this->gyroBuf, sizeof this->gyroBuf) ...

void Tester ::testGetGyroTlm() {
      sendCmd_PowerSwitch(0, 0, PowerState::ON);
      for (U32 i = 0; i < 5; i++) {
          this->invoke_to_Run(0, 0);
          Gnc::Vector expectedVector;
          for (U32 j = 0; j < 3; ++j) {
              I16 intCoord = 0;
              const auto status = this->gyroSerBuf.deserialize(intCoord);
              EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
              const F32 f32Coord =
                  static_cast<F32>(intCoord) / Imu::gyroScaleFactor;
              expectedVector[j] = f32Coord;
          }
          ASSERT_TLM_gyroscope(i, expectedVector);
      }
}
...
Drv::I2cStatus Tester ::from_read_handler(const NATIVE_INT_TYPE portNum, U32 addr, Fw::Buffer& serBuffer) {
   this->pushFromPortEntry_read(addr, serBuffer);
   // Fill buffer with random data
   U8* const data = serBuffer.getData();
   const U32 size = serBuffer.getSize();
   const U32 imu_max_data_size = Gnc::Imu::IMU_MAX_DATA_SIZE_BYTES;
   EXPECT_LE(size, imu_max_data_size);
   for (U32 i = 0; i < size; ++i) {
       const U8 byte = i;
       data[i] = byte;
   }
   // Copy data into the gyro buffer
   this->gyroSerBuf.resetSer();
   const auto status = this->gyroSerBuf.pushBytes(&data[0], size);
   EXPECT_EQ(status, Fw::FW_SERIALIZE_OK);
   return Drv::I2cStatus::I2C_OK;
}
```
> Note: we also had to update the `from_read_handler` to respond as if we are an I2C read port.

**Test Case Definition in `Tester.hpp`**
```c++
#include "Fw/Types/SerialBuffer.hpp"
...
class Tester : public ImuGTestBase {
...
    //! Test to get gyroscope telemetry
    //!
    void testGetGyroTlm();

    //! Serial buffer wrapping gyroBuf
    Fw::SerialBuffer gyroSerBuf;

    //! Buffer for storing gyro data read by the component
    U8 gyroBuf[6];
...
};
```

We should now be able to run the unittest!

```c++
fprime-util check
```
> Learning goal 3 has been partially accomplished. Look at
> [classroom implementation](https://github.com/fprime-community/fprime-system-reference/tree/main/SystemReference/Gnc/Imu/test/ut)
> for the implementation of further unit tests.

## Test Coverage

Once we have implemented our unit tests, we can check our coverage by running the following command.

```c++
fprime-util check --coverage
```

This will generate a sub folder of the Imu directory called "coverage" containing an HTML file called "coverage.html".
Open that file to inspect the coverage report.

> Learning goal 4 has been accomplished!  Nice work!



