add_library(analyze analyze.cpp analyze.hpp dominantColors.hpp dominantColors.cpp)
add_executable(analyze_test test.cpp)
target_link_libraries(analyze_test analyze common ${OpenCV_LIBS})