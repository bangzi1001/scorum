file(GLOB HEADERS "include/scorum/account_statistics/*.hpp")

add_library( scorum_account_statistics
             account_statistics_plugin.cpp
             account_statistics_api.cpp
             metrics.cpp
           )

target_link_libraries( scorum_account_statistics
                       scorum_chain
                       scorum_protocol
                       scorum_app )
target_include_directories( scorum_account_statistics
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../common_statistics/include")

add_custom_target( scorum_account_statistics_manifest SOURCES plugin.json)

install( TARGETS
   scorum_account_statistics

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
