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

find_external_library(
  DEPENDENCY_NAME brite
  HEADER_NAME Brite.h
  LIBRARY_NAME brite
  SEARCH_PATHS ${NS3_WITH_BRITE}
)

if((NOT
    brite_FOUND)
   OR (NOT
       ${brite_FOUND})
)
  message(
    ${HIGHLIGHTED_STATUS}
    "BRITE NOT FOUND"
  )
  return()
endif()

include_directories(${brite_INCLUDE_DIRS})
set(NS3_BRITE
    "ON"
    CACHE INTERNAL
          "ON if Brite is found in NS3_WITH_BRITE"
)

set(source_files
    model/botnet.cc
    model/pulsingattackcc.cc
    model/pulsingattackbot.cc
    model/benignapplication.cc
    model/linkfloodingattack.cc
    model/linkfloodingattackcc.cc
    helper/botnet-helper.cc
    helper/pulsingattack-helper.cc)

set(header_files
    model/botnet.h
    model/pulsingattackcc.h
    model/pulsingattackbot.h
    model/benignapplication.h
    model/linkfloodingattack.h
    model/linkfloodingattackcc.h
    helper/botnet-helper.h
    helper/pulsingattack-helper.h)

build_lib(
    LIBNAME botnet
    SOURCE_FILES ${source_files}
    HEADER_FILES ${header_files}
    LIBRARIES_TO_LINK
        ${libcore}
        ${libbrite}
        ${libinternet}
        ${libpoint-to-point}
        ${libnix-vector-routing}
        ${libapplications}
        ${brite_LIBRARIES}
    TEST_SOURCES test/botnet-test-suite.cc
                 ${examples_as_tests_sources}
)

