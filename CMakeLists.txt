cmake_minimum_required(VERSION 3.20)
project(Code-Visualizer-C LANGUAGES CXX)

find_package(LLVM CONFIG REQUIRED)
find_package(Clang REQUIRED)
find_package(CMakeRC CONFIG REQUIRED)

add_executable(Code-Visualizer-C)

target_sources(
  Code-Visualizer-C
  PRIVATE Include/Core.h
          Include/VisualizerAction.h
          Include/VisualizerASTConsumer.h
          Include/VisualizerFrontendActionFactory.h
          Include/VisualizerVisitor.h
          Source/Main.cpp
          Source/VisualizerAction.cpp
          Source/VisualizerASTConsumer.cpp
          Source/VisualizerFrontendActionFactory.cpp
          Source/VisualizerVisitor.cpp)

target_compile_features(Code-Visualizer-C PRIVATE cxx_std_20)
target_include_directories(Code-Visualizer-C PRIVATE ${LLVM_INCLUDE_DIRS}
                                                     Include)
target_link_directories(Code-Visualizer-C PRIVATE ${LLVM_LIBRARY_DIRS})

target_link_libraries(Code-Visualizer-C PRIVATE LLVMCore)

target_link_libraries(Code-Visualizer-C PRIVATE clangTooling)

cmrc_add_resource_library(
  Code-Visualizer-Resources
  NAMESPACE
  VisualizerResources
  Resources/Skeleton.html
  Resources/Styles.css
  Resources/Theme.css
  Resources/Script.js)

target_link_libraries(Code-Visualizer-C PRIVATE Code-Visualizer-Resources)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT
                                                            Code-Visualizer-C)
set_property(TARGET Code-Visualizer-C PROPERTY VS_DEBUGGER_WORKING_DIRECTORY
                                               ${CMAKE_SOURCE_DIR})
