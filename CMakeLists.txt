cmake_minimum_required(VERSION 3.14)
project(HierarX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3")

include_directories($ENV{ANNOY_HOME}/src)
include_directories($ENV{ARGH_HOME})
add_library(fasttext SHARED IMPORTED)
add_executable(HierarX main.cpp src/PoincareVector.cpp src/PoincareVector.h src/EuclideanGeometry.cpp src/EuclideanGeometry.h src/NumericalDifferentiate.cpp src/NumericalDifferentiate.h src/RSGD.cpp src/RSGD.h src/HyperbolicEmbedding.cpp src/HyperbolicEmbedding.h src/BatchMaker.cpp src/BatchMaker.h src/VecBinder.cpp src/VecBinder.h src/Random.cpp src/Random.h src/MainProcessor.cpp src/MainProcessor.h src/UserInterface.cpp src/UserInterface.h src/Hierarchy.cpp src/Hierarchy.h src/HyperbolicVector.h src/HyperbolicVector.cpp src/PoincareStack.cpp src/PoincareStack.h src/Similarity.cpp src/Similarity.h src/LorentzVector.cpp src/LorentzVector.h src/DoubleVector.cpp src/DoubleVector.h src/Args.cpp src/Args.h src/Utils.h)


add_library(gtest SHARED IMPORTED)
set_target_properties(gtest PROPERTIES IMPORTED_LOCATION "$ENV{GTEST_HOME}/build/lib/libgtest.a")
add_library(gtest_main SHARED IMPORTED)
set_target_properties(gtest_main PROPERTIES IMPORTED_LOCATION "$ENV{GTEST_HOME}/build/lib/libgtest_main.a")
include_directories($ENV{GTEST_HOME}/include $ENV{GTEST_HOME})
add_executable(testsHierarX src/Similarity.cpp src/Similarity.h src/Args.cpp src/Args.h src/Utils.h src/PoincareVector.cpp src/PoincareVector.h src/EuclideanGeometry.cpp src/EuclideanGeometry.h tests/euclidean_geometry_test.cpp tests/poincare_vector_test.cpp src/NumericalDifferentiate.cpp src/NumericalDifferentiate.h tests/numerical_differentiate_test.cpp src/RSGD.cpp src/RSGD.h tests/sgd_test.cpp src/HyperbolicEmbedding.cpp src/HyperbolicEmbedding.h src/BatchMaker.cpp src/BatchMaker.h src/VecBinder.cpp src/VecBinder.h src/Random.cpp src/Random.h tests/fasttext_binder_test.cpp tests/batch_maker_test.cpp src/MainProcessor.cpp src/MainProcessor.h tests/main_processor_test.cpp src/UserInterface.cpp src/UserInterface.h tests/poincare_embedding_test.cpp src/HyperbolicVector.h src/HyperbolicVector.cpp src/PoincareStack.cpp src/PoincareStack.h src/LorentzVector.cpp src/LorentzVector.h src/DoubleVector.cpp src/DoubleVector.h)
target_link_libraries(testsHierarX gtest gtest_main)