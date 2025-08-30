cmake_minimum_required(VERSION 3.10)

project(HL-GST_Project CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

find_package(Boost REQUIRED)
find_package(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} include)

# ---- Main Experiment Programs ----
# For run_label_generation_exp.sh
add_executable(
    label_generator
    src/label_generation/exp_HL4GST.cpp
)

# For run_nonHOP_maintain_exp.sh and its generateLPPR script
add_executable(
    label_maintainer
    src/nonHOP_maintain/exp_HL4GST_nonHOP_maintain.cpp
)

# For run_HOP_maintain_exp.sh and its generateLPPR script
add_executable(
    hop_maintain_exp
    src/HOP_maintain/exp_HL4GST_HOP_maintain.cpp
)


# ---- Test Programs ----
# For run_label_generation_test.sh
add_executable(
    label_generation_test
    src/label_generation/test_HL4GST.cpp
)

# For run_nonHOP_maintain_test.sh
add_executable(
    nonhop_maintain_test
    src/nonHOP_maintain/nonHOP_maintain_test.cpp
)

# For run_HOP_maintain_test.sh
add_executable(
    hop_maintain_test
    src/HOP_maintain/HOP_maintain_test.cpp
)

target_link_libraries(label_generator PRIVATE Threads::Threads)
target_link_libraries(label_maintainer PRIVATE Threads::Threads)
target_link_libraries(hop_maintain_exp PRIVATE Threads::Threads)
target_link_libraries(label_generation_test PRIVATE Threads::Threads)
target_link_libraries(nonhop_maintain_test PRIVATE Threads::Threads)
target_link_libraries(hop_maintain_test PRIVATE Threads::Threads)

message(STATUS "Project configured successfully! Defined 6 executables.")
message(STATUS "Found Boost at: ${Boost_INCLUDE_DIRS}")