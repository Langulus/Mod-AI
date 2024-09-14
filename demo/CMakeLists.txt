file(GLOB_RECURSE
    LANGULUS_MOD_AI_DEMO_SOURCES
    LIST_DIRECTORIES FALSE CONFIGURE_DEPENDS
    *.cpp
)

add_executable(LangulusModAIDemo ${LANGULUS_MOD_AI_DEMO_SOURCES})

target_link_libraries(LangulusModAIDemo
    PRIVATE		Langulus
                $<$<NOT:$<BOOL:${WIN32}>>:pthread>
)

add_dependencies(LangulusModAIDemo
    LangulusModAI
    LangulusModASCII
    LangulusModFTXUI
    LangulusModFileSystem
    LangulusModAssetsGeometry
    LangulusModAssetsImages
    LangulusModAssetsMaterials
    LangulusModPhysics
    LangulusModInputSDL
    LangulusModLife
    LangulusModNetwork
    LangulusModStoryteller
)

if (NOT ${LANGULUS_TOASTER_MODE})
add_dependencies(LangulusModAIDemo
	LangulusModGLFW
	LangulusModVulkan
)
endif()


#[[ Make the data dir                                                     
add_custom_command(
    TARGET LangulusModAIDemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:LangulusModAIDemo>/data/assets/meshes/maxwell"
    COMMAND ${CMAKE_COMMAND} -E copy
		"$<TARGET_PROPERTY:LangulusModAssetsGeometry,SOURCE_DIR>/test/data/maxwell.obj"
		"$<TARGET_FILE_DIR:LangulusModAIDemo>/data/assets/meshes/maxwell/maxwell.obj"
    COMMAND ${CMAKE_COMMAND} -E copy
		"$<TARGET_PROPERTY:LangulusModAssetsGeometry,SOURCE_DIR>/test/data/maxwell.mtl"
		"$<TARGET_FILE_DIR:LangulusModAIDemo>/data/assets/meshes/maxwell/maxwell.mtl"
    COMMAND ${CMAKE_COMMAND} -E copy
		"$<TARGET_PROPERTY:LangulusModAssetsGeometry,SOURCE_DIR>/test/data/dingus_nowhiskers.png"
		"$<TARGET_FILE_DIR:LangulusModAIDemo>/data/assets/meshes/maxwell/dingus_nowhiskers.png"
    COMMAND ${CMAKE_COMMAND} -E copy
		"$<TARGET_PROPERTY:LangulusModAssetsGeometry,SOURCE_DIR>/test/data/dingus_whiskers.png"
		"$<TARGET_FILE_DIR:LangulusModAIDemo>/data/assets/meshes/maxwell/dingus_whiskers.png"
    COMMAND ${CMAKE_COMMAND} -E copy
		"$<TARGET_PROPERTY:LangulusModAIDemo,SOURCE_DIR>/data/castle.obj"
		"$<TARGET_FILE_DIR:LangulusModAIDemo>/data/assets/meshes/castle.obj"
)#]]
