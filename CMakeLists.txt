if(CONFIG_ZMK_DISPLAY)
    target_sources_ifdef(CONFIG_ZMK_DISPLAY app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/config/widgets/modifier_status.c)
    target_include_directories_ifdef(CONFIG_ZMK_DISPLAY app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/config)
endif()