build_lib_example(
    NAME botnet-example
    SOURCE_FILES botnet-example.cc
    LIBRARIES_TO_LINK
        ${libbotnet}
        ${libinternet}
        ${libbrite}
        ${libpoint-to-point}
        ${libapplications}
)

