set(SOURCES test.cpp)
set(TEST_TARGET unittest_${PROJECT_NAME})
add_executable(${TEST_TARGET} ${SOURCES})

target_link_libraries(${TEST_TARGET} ${PROJECT_NAME} ${EXTERNAL_LIBS})
