####
# 'SystemReference' Deployment:
#
# This sets up the build for the 'SystemRef' Application, including the custom reference
# components. In addition, it imports FPrime.cmake, which includes the core F Prime
# components.
#
# This file has several sections.
#
# 1. Header Section: define basic properties of the build
# 2. F prime core: includes all F prime core components, and build-system properties
# 3. Local subdirectories: contains all deployment specific directory additions
####

##
# Section 1: Basic Project Setup
#
# This contains the basic project information. Specifically, a cmake version and
# project definition.
##
cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0048 NEW)
project(SystemReference VERSION 1.0.0 LANGUAGES C CXX)

##
# Section 2: F prime Core
#
# This includes all of the F prime core components, and imports the make-system. F prime core
# components will be placed in the F-Prime binary subdirectory to keep them from
# colliding with deployment specific items.
##
include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime.cmake")
# NOTE: register custom targets between these two lines
include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime-Code.cmake")
##
# Section 3: Components and Topology
#
# This section includes deployment specific directories. This allows use of non-
# core components in the topology, which is also added here.
##

# Add Topology subdirectory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../Drv/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Top/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Gnc/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Payload/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Com/")
set(SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Top/Main.cpp")
set(MOD_DEPS ${PROJECT_NAME}/Top)

register_fprime_deployment()
