cmake_minimum_required(VERSION 3.16)
project(xcrypt)

#set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_CXX_EXTENSIONS OFF)

option(${PROJECT_NAME}_BUILD_EXAMPLE "Build ${PROJECT_NAME} examples" ON)

#====== BOOST ======#
find_package(Boost 1.74.0 REQUIRED)
if (NOT Boost_FOUND)
    message(FATAL_ERROR "Package Boost not found! Set the variable BOOST_ROOT. Example: -DBOOST_ROOT=\"G:/boost\"")
endif()
#====== END ======#

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} INTERFACE include ${Boost_INCLUDE_DIR})

add_subdirectory(example)

##====== GMP ======#
#find_package(PkgConfig REQUIRED)
#pkg_check_modules(GMPXX REQUIRED IMPORTED_TARGET gmpxx)
##====== END ======#
#target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::GMPXX ${Boost_LIBRARIES})