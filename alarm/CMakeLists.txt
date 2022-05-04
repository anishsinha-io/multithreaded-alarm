add_library(alarm src/alarm.c include/alarm.h)
target_include_directories(alarm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../)