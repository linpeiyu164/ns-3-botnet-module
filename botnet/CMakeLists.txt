check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/botnet-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME botnet
    SOURCE_FILES model/botnet.cc
                 helper/botnet-helper.cc
    HEADER_FILES model/botnet.h
                 helper/botnet-helper.h
    LIBRARIES_TO_LINK ${libcore}
    TEST_SOURCES test/botnet-test-suite.cc
                 ${examples_as_tests_sources}
)

