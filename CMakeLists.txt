CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)

PROJECT(topology_sort)

SET(CMAKE_EXPORT_COMPILE_COMMANDS on)

INCLUDE_DIRECTORIES(

    )

LINK_DIRECTORIES(
    )

ADD_EXECUTABLE(topology_sort.exe
    topology_sort.c
    )

TARGET_LINK_LIBRARIES(topology_sort.exe
         )

INSTALL(TARGETS topology_sort.exe DESTINATION $ENV{project_source_dir})

