project(growl-plugin-dummy)

message(STATUS "[Growl] Enabled dummy plugin.")

set(SOURCES
	"src/dummy_plugin.cpp"
	)

add_library(growl-plugin-dummy ${SOURCES})
target_compile_definitions(growl-plugin-dummy INTERFACE "GROWL_DUMMY")
target_link_libraries(growl-plugin-dummy PRIVATE growl-core)
