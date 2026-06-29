cmake_minimum_required(VERSION 3.16)

project(xml2c LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(xml2c
    main.cpp
    src/xml_parser.cpp
    src/xml_graph.cpp
    src/scheduler.cpp
    src/codegen.cpp
    src/xml2c.cpp
    third_party/tinyxml2/tinyxml2.cpp
)

target_include_directories(xml2c PRIVATE
    include
    third_party
)
