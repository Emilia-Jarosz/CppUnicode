add_subdirectory(lib/Catch2)

#################
# Configuration #
#################

set(CPP_UNICODE_TEST_TARGET_NAME ${PROJECT_NAME}Tests)

###################
# Test Executable #
###################

set(CPP_UNICODE_TEST_SOURCE_FILES
    detail/random_string_generator.cpp
    encoding/utf8.cpp
    encoding/utf16.cpp
    encoding/utf32.cpp
    iterator.cpp
    reverse_iterator.cpp
    string_view.cpp
)

list(TRANSFORM CPP_UNICODE_TEST_SOURCE_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/src/")

add_executable(${CPP_UNICODE_TEST_TARGET_NAME} ${CPP_UNICODE_TEST_SOURCE_FILES})

target_link_libraries(${CPP_UNICODE_TEST_TARGET_NAME} CppUnicode::CppUnicode)
target_link_libraries(${CPP_UNICODE_TEST_TARGET_NAME} Catch2::Catch2WithMain)

if(MSVC)
    target_compile_options(${CPP_UNICODE_TEST_TARGET_NAME} PRIVATE
        /Wall /WX /O2
    )
else()
    target_compile_options(${CPP_UNICODE_TEST_TARGET_NAME} PRIVATE
        -Wall -Wextra -Wpedantic -Werror -O2
        $<$<CXX_COMPILER_ID:Clang>:-Wno-error=unknown-attributes>
    )
endif()

###############################
# Automatic Test Registration #
###############################

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)
catch_discover_tests(${CPP_UNICODE_TEST_TARGET_NAME})
