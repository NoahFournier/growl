set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/fpng)

growl_thirdparty_lib(fpng
	SOURCES "${SOURCE_PREFIX}/fpng.cpp"
	INCLUDES PUBLIC "${SOURCE_PREFIX}"
	)
