cmake_minimum_required(VERSION 3.15)
project(sdl_app)

# Set C++ 17 standard
set( CMAKE_CXX_STANDARD 17 CACHE STRING "" FORCE )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE )

# Create executable target
add_executable( main )
set_target_properties( main PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME} )

# Post build command (copies the asset files)
add_custom_command( TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${PROJECT_SOURCE_DIR}/assets 
    $<CONFIG>/assets
)

# Post build command (runs the exe)
add_custom_command( TARGET main POST_BUILD
    COMMAND cd "${PROJECT_BINARY_DIR}/$<CONFIG>/" && ${CMAKE_PROJECT_NAME}
)

# Add header include directory
target_include_directories( main PRIVATE "src" "inc" )
# target_precompile_headers( main PRIVATE "inc/types.h" )

# Sub-directories
add_subdirectory( "src" )
add_subdirectory( "extern" )

if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set( CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" )
    set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME} )
    set_property( TARGET main PROPERTY WIN32_EXECUTABLE true )
endif()
