set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GROWL_INCLUDE_DIRS
	${GROWL_PATH}/src/core/include
	${GROWL_PATH}/src/util/include
	)

set(GROWL_LIBS
	growl-core
	growl-util
	)

set(GROWL_INCLUDE_DIRS
	${GROWL_INCLUDE_DIRS}
	${GROWL_PATH}/src/platforms/desktop/include
	)
set(GROWL_LIBS
	${GROWL_LIBS}
	growl-platform-desktop
	)

include_directories(${GROWL_INCLUDE_DIRS})
