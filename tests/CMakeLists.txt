add_subdirectory(lib/Catch2)

#################
# Configuration #
#################

set(CPP_UNICODE_TEST_TARGET_NAME ${PROJECT_NAME}Tests)

###################
# Test Executable #
###################

set(CPP_UNICODE_TEST_SOURCE_FILES
    # TODO
)

add_executable(${CPP_UNICODE_TEST_TARGET_NAME} ${CPP_UNICODE_TEST_SOURCE_FILES})

target_link_libraries(${CPP_UNICODE_TEST_TARGET_NAME} CppUnicode::CppUnicode)
target_link_libraries(${CPP_UNICODE_TEST_TARGET_NAME} Catch2::Catch2WithMain)

if(MSVC)
    target_compile_options(${CPP_UNICODE_TEST_TARGET_NAME} PRIVATE /Wall /WX)
else()
    target_compile_options(${CPP_UNICODE_TEST_TARGET_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror
        $<$<CXX_COMPILER_ID:Clang>:-Wno-error=unknown-attributes>
    )
endif()

###############################
# Automatic Test Registration #
###############################

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)
catch_discover_tests(${CPP_UNICODE_TEST_TARGET_NAME})
