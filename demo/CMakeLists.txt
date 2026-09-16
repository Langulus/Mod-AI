file(GLOB_RECURSE
   LANGULUS_MOD_AI_DEMO_SOURCES
   LIST_DIRECTORIES FALSE CONFIGURE_DEPENDS
   *.cpp
)

add_langulus_app(LangulusModAIDemo
   SOURCES        ${LANGULUS_MOD_AI_DEMO_SOURCES}
   LIBRARIES      Langulus
                  #$<$<NOT:$<BOOL:${WIN32}>>:pthread>
   DEPENDENCIES   LangulusModAI
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
                  LangulusModUMS
                  LangulusModGLFW
                  LangulusModVulkan
                  LangulusModImGui
)