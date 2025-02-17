cmake_minimum_required(VERSION 3.28)
project(LangulusModAI
    VERSION         1.0.0
    DESCRIPTION     "Langulus plug-in module for simulating minds and societies"
    HOMEPAGE_URL    https://langulus.com
)

file(GLOB_RECURSE
    LANGULUS_MOD_AI_SOURCES 
    LIST_DIRECTORIES FALSE CONFIGURE_DEPENDS
    source/*.cpp
)

# Build the module                                                              
add_langulus_mod(LangulusModAI ${LANGULUS_MOD_AI_SOURCES})

if(LANGULUS_TESTING)
	enable_testing()
    add_subdirectory(test)
endif()

add_subdirectory(demo)
