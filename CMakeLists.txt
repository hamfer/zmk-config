if(SHIELD MATCHES "corne_.*")
    list(APPEND SHIELD_SOURCES ${CMAKE_CURRENT_LIST_DIR}/config/widgets/modifier_status.c)
    target_include_directories(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/config)
endif()