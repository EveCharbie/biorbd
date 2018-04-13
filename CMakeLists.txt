project(S2M_rbdl)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)
# Prepare linking with RBDL
find_package(RBDL REQUIRED)

# Compile S2M_rbdl
aux_source_directory(src SRC_LIST)
INCLUDE_DIRECTORIES(include ${RBDL_INCLUDE_DIR})
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})

# Add linker to S2M_rbld
LINK_DIRECTORIES(${RBDL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} rbdl)

# Wrapper for S2M_rbdl - C
set(C_WRAPPER false CACHE BOOL "Compile the C wrapper")
if (${C_WRAPPER})
    include(${CMAKE_CURRENT_SOURCE_DIR}/wrapper/c/CMakeLists.txt)
endif (${C_WRAPPER})

# Wrapper for S2M_rbdl - Python
set(PYTHON3_WRAPPER false CACHE BOOL "Compile the Python3 wrapper")
if (${PYTHON3_WRAPPER})
    include(${CMAKE_CURRENT_SOURCE_DIR}/wrapper/python3/CMakeLists.txt)
endif (${PYTHON3_WRAPPER})

# Wrapper for S2M_rbdl - Matlab
set(MATLAB_WRAPPER false CACHE BOOL "Compile the Matlab wrapper")
if (${MATLAB_WRAPPER})
    include(${CMAKE_CURRENT_SOURCE_DIR}/wrapper/matlab/CMakeLists.txt)
endif (${MATLAB_WRAPPER})







