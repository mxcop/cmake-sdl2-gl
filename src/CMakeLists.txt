include(CMakePrintHelpers)

# Glob all source files
file(GLOB_RECURSE SOURCES
    "*.cpp"
)

cmake_print_variables(SOURCES)

# Add source files into target main
target_sources( main PRIVATE
    ${SOURCES}
)
