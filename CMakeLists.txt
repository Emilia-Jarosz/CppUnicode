cmake_minimum_required(VERSION 3.12)

project(CppUnicode LANGUAGES CXX VERSION 0.1.0)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CPPUNICODE_NOT_SUBDIRECTORY ON)
endif()

###########
# Options #
###########

option(CPPUNICODE_BUILD_TESTS "Generate rule to build tests when used as a submodule." OFF)
option(CPPUNICODE_INSTALL "Generate install rule." ON)

#################
# Configuration #
#################

set(CPPUNICODE_TARGET_NAME "${PROJECT_NAME}")

######################
# Library Definition #
######################

set(CPPUNICODE_HEADER_FILES
    unicode/detail/byte_swap.hpp
    unicode/detail/error_code.hpp
    unicode/detail/exceptions.hpp
    unicode/encoding/utf8.hpp
    unicode/encoding/utf16.hpp
    unicode/encoding/utf32.hpp
    unicode/encoding.hpp
    unicode/iterator.hpp
    basic_string_view.hpp
)

list(TRANSFORM CPPUNICODE_HEADER_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/include/bigj/")

add_library(${CPPUNICODE_TARGET_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${CPPUNICODE_TARGET_NAME} ALIAS ${CPPUNICODE_TARGET_NAME})

target_compile_features(${CPPUNICODE_TARGET_NAME} INTERFACE cxx_std_20)

target_include_directories(${CPPUNICODE_TARGET_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(${CPPUNICODE_TARGET_NAME} SYSTEM INTERFACE $<INSTALL_INTERFACE:include/>)

target_sources(${CPPUNICODE_TARGET_NAME} INTERFACE "$<BUILD_INTERFACE:${CPPUNICODE_HEADER_FILES}>")

#########
# Tests #
#########

include(CTest)

if(CPPUNICODE_BUILD_TESTS OR CPPUNICODE_NOT_SUBDIRECTORY)
    enable_testing()
    add_subdirectory(tests)
endif()

################
# Installation #
################

if(CPPUNICODE_INSTALL)
    install(
        DIRECTORY include/bigj
        DESTINATION include
    )

    install(
        TARGETS ${CPPUNICODE_TARGET_NAME}
        EXPORT ${PROJECT_NAME}
    )

    install(
        EXPORT ${PROJECT_NAME}
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/${PROJECT_NAME}
    )
endif()
