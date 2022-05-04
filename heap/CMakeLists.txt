add_library(heap src/heap.c include/heap.h)
target_include_directories(heap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../)