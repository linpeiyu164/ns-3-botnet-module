build_lib_example(
    NAME botnet-example
    SOURCE_FILES botnet-example.cc
    LIBRARIES_TO_LINK
        ${libbrite}
        ${libinternet}
        ${libpoint-to-point}
        ${libapplications}
        ${libbotnet}
        ${libinternet-apps}
)

build_lib_example(
    NAME botnet-example-2
    SOURCE_FILES botnet-example-2.cc
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