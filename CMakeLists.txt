cmake_minimum_required(VERSION 3.0.0)
project(Feature_Detection VERSION 0.1.0)

include(CTest)
enable_testing()
link_directories(/opt/homebrew/lib)

find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED)
pkg_check_modules(Tesseract5 REQUIRED IMPORTED_TARGET tesseract)
add_library(Tesseract::libtesseract ALIAS PkgConfig::Tesseract5)

pkg_check_modules(JSONCPP jsoncpp)

include_directories(${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR}/include ${JSONCPP_INCLUDE_DIRS})
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src SRC_DIR)

add_executable(${PROJECT_NAME} ${SRC_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS} Tesseract::libtesseract ${JSONCPP_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
