cmake_minimum_required(VERSION 3.5)

project(FuserModDoorstop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(FuserModDoorstop SHARED 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dllmain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sha1.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/uasset.cpp
)

include(GenerateExportHeader)
generate_export_header(FuserModDoorstop)

target_include_directories(FuserModDoorstop PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_include_directories(FuserModDoorstop PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../shared")

set_target_properties(FuserModDoorstop PROPERTIES
    IMPORTED_IMPLIB "${CMAKE_CURRENT_BINARY_DIR}/FuserModDoorstop.lib"
    IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/FuserModDoorstop.dll"
)

target_link_libraries(FuserModDoorstop "${CMAKE_CURRENT_SOURCE_DIR}/lib/funchook.lib" "${CMAKE_CURRENT_SOURCE_DIR}/lib/funchook.lib")
set_target_properties(FuserModDoorstop PROPERTIES OUTPUT_NAME "winhttp")
target_compile_definitions(FuserModDoorstop 
PUBLIC FUSER_MOD_LOADER
PUBLIC UNICODE
)