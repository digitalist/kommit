cmake_minimum_required(VERSION 3.5)
project(kommit)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
ADD_DEFINITIONS(-std=c++14
        -DBOOST_LOG_DYN_LINK
        -Wall
        #-Wno-unused-function
        #-Wno-unused-variable
        #-Wunused-but-set-variable
        #old boost
        #-Wno-deprecated-declarations
        # Other flags
        )

set(SOURCE_FILES main.cpp git.h)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

find_package(Boost COMPONENTS program_options REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_LIBRARIES})
endif()


target_link_libraries(${CMAKE_PROJECT_NAME}
        git2
)