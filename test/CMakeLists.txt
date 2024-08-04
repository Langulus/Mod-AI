file(GLOB_RECURSE
	LANGULUS_MOD_AI_TEST_SOURCES 
	LIST_DIRECTORIES FALSE CONFIGURE_DEPENDS
	*.cpp
)

add_executable(LangulusModAITest ${LANGULUS_MOD_AI_TEST_SOURCES})

target_link_libraries(LangulusModAITest
	PRIVATE		Langulus
				Catch2
)

add_dependencies(LangulusModAITest
	LangulusModAI
)

add_test(
	NAME		LangulusModAITest
	COMMAND		LangulusModAITest
	WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)