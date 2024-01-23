build_lib_example(
    NAME pulsing-example
    SOURCE_FILES pulsing-example.cc
    LIBRARIES_TO_LINK
        ${libbrite}
        ${libinternet}
        ${libpoint-to-point}
        ${libapplications}
        ${libbotnet}
        ${libinternet-apps}
)

build_lib_example(
    NAME pulsing-example-2
    SOURCE_FILES pulsing-example-2.cc
    LIBRARIES_TO_LINK
        ${libbrite}
        ${libinternet}
        ${libpoint-to-point}
        ${libapplications}
        ${libbotnet}
        ${libinternet-apps}
)

build_lib_example(
    NAME benign-example
    SOURCE_FILES benign-example.cc
    LIBRARIES_TO_LINK
        ${libcore}
        ${libbrite}
        ${libinternet}
        ${libpoint-to-point}
        ${libapplications}
        ${libbotnet}
        ${libinternet-apps}
)

build_lib_example(
    NAME link-flooding-example
    SOURCE_FILES link-flooding-example.cc
    LIBRARIES_TO_LINK
        ${libcore}
        ${libbrite}
        ${libinternet}
        ${libpoint-to-point}
        ${libapplications}
        ${libbotnet}
        ${libinternet-apps}
)