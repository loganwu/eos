
include_directories(${CMAKE_CURRENT_SOURCE_DIR} include be)

add_library(pubsub_plugin
    be/backend.cpp
    pubsub_plugin.cpp
    irreversible_block_storage.cpp
    applied_action.cpp
    applied_transaction_storage.cpp
    block_storage.cpp
    )

target_link_libraries(pubsub_plugin
    chain_plugin
    )

add_subdirectory(test)

