option(ENABLE_ASAN "Enable Address sanitizer" false)
option(ENABLE_UBSAN "Enable Undefined Behavior sanitizer" false)

if(ENABLE_UBSAN)
	set(CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS} -fsanitize=undefined")
	set(CMAKE_EXE_LINKER_FLAGS
		"${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
endif()

if(ENABLE_ASAN)
	set(CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS} -fsanitize=address")
	set(CMAKE_EXE_LINKER_FLAGS
		"${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()
