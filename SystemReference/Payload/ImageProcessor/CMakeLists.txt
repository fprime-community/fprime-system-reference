####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and auto coding files
# MOD_DEPS: (optional) module dependencies
#
# Note: using PROJECT_NAME as EXECUTABLE_NAME
####
set(SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/ImageProcessor.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/ImageProcessor.cpp"
        )
register_fprime_module()
if (DEFINED SYSTEM_REFERENCE_OPENCV_PATH AND NOT DEFINED FPRIME_PRESCAN)
    get_module_name("${CMAKE_CURRENT_LIST_DIR}")
    target_link_libraries(${MODULE_NAME} PUBLIC ${OpenCV_LIBS})
endif()
# Register the unit test build
set(UT_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/ImageProcessor.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
        )
set(UT_MOD_DEPS STest)
register_fprime_ut()
