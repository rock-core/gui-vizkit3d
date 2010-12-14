# Generate and install the pkg-config file
configure_file(${PROJECT_SOURCE_DIR}/configuration/${PROJECT_NAME}.pc.in
		${PROJECT_BINARY_DIR}/configuration/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/configuration/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)

