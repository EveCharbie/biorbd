set(MASTER_PROJECT_NAME ${PROJECT_NAME})
project(${MASTER_PROJECT_NAME}_example)
add_executable(${PROJECT_NAME} "main.cpp")
add_dependencies(${PROJECT_NAME} ${MASTER_PROJECT_NAME})

# Headers
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${Boost_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR} ${RBDL_INCLUDE_DIR})

# Libraries linker
get_target_property(DEBUG_POSTFIX ${MASTER_PROJECT_NAME} DEBUG_POSTFIX)
IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(${MASTER_PROJECT_NAME}_LIB_NAME ${MASTER_PROJECT_NAME}${DEBUG_POSTFIX})
else()
    SET(${MASTER_PROJECT_NAME}_LIB_NAME ${MASTER_PROJECT_NAME})
endif()
if (UNIX)
    string(CONCAT ${MASTER_PROJECT_NAME}_LIB_NAME "lib" ${${MASTER_PROJECT_NAME}_LIB_NAME} ".so")
elseif(_WIN32)
    message("TO BE TESTED")
    string(CONCAT ${MASTER_PROJECT_NAME}_LIB_NAME ${${MASTER_PROJECT_NAME}_LIB_NAME} ".dll")
endif()
set(${MASTER_PROJECT_NAME}_LIBRARY ${CMAKE_BINARY_DIR}/${${MASTER_PROJECT_NAME}_LIB_NAME})
target_link_libraries(${PROJECT_NAME} ${${MASTER_PROJECT_NAME}_LIBRARY} ${RBDL_LIBRARY})

# Copy the c3d of the example
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pyomecaman.s2mMod
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})