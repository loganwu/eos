#find_package(Soci)
#find_package(librdkafka REQUIRED)
find_package(Soci COMPONENTS mysql)
if(NOT SOCI_FOUND)
    message(STATUS "Database SQL plugin: disabled")
    return()
endif()

if (NOT SOCI_mysql_FOUND)
    message(STATUS "MySQL not found!!!")
    return()
endif()

message(STATUS "Database SQL plugin: enabled")

include_directories(${CMAKE_CURRENT_SOURCE_DIR} include be)

add_library(pubsub_plugin
    be/backend.cpp
    pubsub_plugin.cpp
    irreversible_block_storage.cpp
    applied_transaction_storage.cpp
    block_storage.cpp
    )

target_link_libraries(pubsub_plugin
    chain_plugin
    ${SOCI_LIBRARY}
    )

add_subdirectory(test)

