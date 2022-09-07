# Component Implementation

Now it is time to write code! F´ implements components using C++ and as we saw in the design section, this
implementation typically involves filling-in template files allowing us to focus on the specific challenges at-hand.

## Setup

Before proceeding with implementation, we should add our `Imu.cpp` file created in the design step to the
`CMakeLists.txt` file in the `Imu` directory.  The new `CMakeList.txt` file should look like this:

```cmake
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/Imu.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/Imu.cpp"
        )
register_fprime_module()
```

## Component Implementation

F´ generates a series of handler functions for us to respond to each type of call. We need to fill in our specific logic
for each of these empty handlers. Additionally, F´ creates a series of helper functions that may be called to send
telemetry, emit events, and call output ports.

To build our implementation we run `fprime-util build` while in the `Imu` folder. Run this now to ensure that the cmake
system is configured correctly for the component. Feel free to re-run the `build` command to check for compiler errors
or other problems as you build.

Let's take a look at one specific handler `TODO`. 

**Note:** some helper functions for working with the specifics of the Imu are available in [Appendix I](./appendix-1.md)
and are useful for developers who are less familiar with C++ and working with hardware. Feel-free to use these helpers
in your code, or write your own!

A full implementation of the component is available [here](../../SystemReference/Gnc/Imu/Imu.cpp) and the associated
header is available [here](../../SystemReference/Gnc/Imu/Imu.hpp).

## Next Steps

- [Topology Integration and Testing](./topology-integration.md)



