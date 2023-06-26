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
                 model/pulsingattackcc.cc
                 model/pulsingattackbot.cc
                 helper/botnet-helper.cc
                 helper/pulsingattack-helper.cc
    HEADER_FILES model/botnet.h
                 model/pulsingattackcc.h
                 model/pulsingattackbot.h
                 helper/botnet-helper.h
                 helper/pulsingattack-helper.h
    LIBRARIES_TO_LINK ${libcore} ${libbrite}
    TEST_SOURCES test/botnet-test-suite.cc
                 ${examples_as_tests_sources}
)

