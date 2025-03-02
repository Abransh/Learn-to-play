cmake_minimum_required(VERSION 3.14)
project(GuitarLearningApp VERSION 0.1.0)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(PortAudio REQUIRED)
find_package(FFTW3 REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(SndFile REQUIRED)

# Choose GUI framework (uncomment one)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
# OR
# find_package(wxWidgets REQUIRED COMPONENTS core base)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${PORTAUDIO_INCLUDE_DIRS}
    ${FFTW3_INCLUDE_DIRS}
    ${SQLITE3_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIRS}
    ${SNDFILE_INCLUDE_DIRS}
)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Qt specific (if using Qt)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${PORTAUDIO_LIBRARIES}
    ${FFTW3_LIBRARIES}
    ${SQLITE3_LIBRARIES}
    ${SNDFILE_LIBRARIES}
    Qt5::Widgets  # If using Qt
    # OR
    # ${wxWidgets_LIBRARIES}  # If using wxWidgets
)

# Install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)