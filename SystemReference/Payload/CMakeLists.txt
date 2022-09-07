if(DEFINED SYSTEM_REFERENCE_OPENCV_PATH AND NOT DEFINED FPRIME_PRESCAN)
    set(OpenCV_DIR "${SYSTEM_REFERENCE_OPENCV_PATH}/lib/cmake/opencv4/")
    find_package(OpenCV REQUIRED)
    set(OpenCV_STATIC ON)
    add_compile_definitions(-DUSES_OPENCV)
endif()


#Ports
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ImagePort")
# Components
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Camera/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ImageProcessor/")
