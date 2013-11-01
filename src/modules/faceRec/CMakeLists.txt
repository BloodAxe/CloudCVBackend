add_library(faceRec faceRec.cpp faceRec.hpp)
add_executable(faceRec_test test.cpp)
target_link_libraries(faceRec_test faceRec common ${OpenCV_LIBS})