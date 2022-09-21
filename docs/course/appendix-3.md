# IMU Exercise Appendix III: Common Errors

This section will capture some common errors as seen in the IMU exercise.

## Command Not Found (E.g.`fprime-util: command not found`)

This error is caused when a tool has not properly been installed. Make sure you've completed the following:

1. Set up your computer following the [prerequisites](./prerequisites.md)
2. Are running in the appropriate shell (i.e. Ubuntu shell on Windows)
3. Activated your virtual environment `. ~/class-venv/bin/activate`

> Activating your virtual environment must be done in each new terminal window and tab you open.

> Macintosh users should be aware that cross-compilation steps will not work on macOS, and they must team up with a
> Linux user or run the [docker container](./appendix-2.md).

## `fpp-...` Exited With Non-Zero Exit Code

This can occur for a variety of reasons.  Typically, this is caused by a malformed FPP model file. Please ensure that
your FPP model has the correct syntax.

> This error has been seen on initial setup of Windows computers. If this happens, make sure that you've run
> [step 2](./prerequisites.md#step-2:-cloning-the-f´-system-reference) from within an Ubuntu shell.


## CMake Error at CMakeLists.txt Line 32

The full error is presented below. This happens when a user has not correctly run
`git submodule update --init --recursive` during [step 2](./prerequisites.md#step-2:-cloning-the-f´-system-reference) of
the system setup. Rerunning that command should fix the problem.

**Full Error Message**
```text
CMake Error at CMakeLists.txt:32 (include):
-- Configuring incomplete, errors occurred!
include could not find requested file:
/Users/mstarch/code/fprime-infra/fprime-system-reference/fprime/cmake/FPrime.cmake


CMake Error at CMakeLists.txt:34 (include):
include could not find requested file:

    /Users/mstarch/code/fprime-infra/fprime-system-reference/fprime/cmake/FPrime-Code.cmake


CMake Error at CMakeLists.txt:43 (add_fprime_subdirectory):
Unknown CMake command "add_fprime_subdirectory".
```

