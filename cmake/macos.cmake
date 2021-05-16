# -----------------------------------------------------------------------------
# CannonBall macOS Setup
# -----------------------------------------------------------------------------

# Need C++11 specified
set (CMAKE_CXX_STANDARD 11)

# Use OpenGL for rendering.
find_package(OpenGL REQUIRED)

# Fix broken boost include in CMakeLists.txt
set(BOOST_INCLUDEDIR ${boost_dir})
find_package(Boost REQUIRED)
message("BOOST Include : ${Boost_INCLUDE_DIRS}")
include_directories(
    ${Boost_INCLUDE_DIRS}
)

# Platform Specific Libraries
set(platform_link_libs
    ${OPENGL_LIBRARIES}
)
