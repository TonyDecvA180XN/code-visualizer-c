cmake_minimum_required(VERSION 3.20)
project(Code-Visualizer-C LANGUAGES CXX)

find_package(LLVM CONFIG REQUIRED)
find_package(Clang REQUIRED)
find_package(CMakeRC CONFIG REQUIRED)

add_executable(Code-Visualizer-C Main.cpp)

target_compile_features(Code-Visualizer-C PRIVATE cxx_std_20)
target_include_directories(Code-Visualizer-C PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_directories(Code-Visualizer-C PRIVATE ${LLVM_LIBRARY_DIRS})

target_link_libraries(Code-Visualizer-C PRIVATE LLVMCore)

target_link_libraries(Code-Visualizer-C PRIVATE clangTooling)

cmrc_add_resource_library(Code-Visualizer-Resources NAMESPACE VisualizerResources Templates/ResponsePage.html)
target_link_libraries(Code-Visualizer-C PRIVATE Code-Visualizer-Resources)
