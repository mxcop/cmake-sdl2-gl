# Glob all source files
file(GLOB SOURCES
    "./*.cpp"
    "./**/*.cpp"
)

# Add source files into target main
target_sources( main PRIVATE
    ${SOURCES}
)
