qt_add_executable(cloud-client
    src/action.cpp
    src/application.cpp
    src/central_widget.cpp
    src/cloud_actions_tree.cpp
    src/cloud_auth_token_manager_dialog.cpp
    src/cloud_auth_token_manager_widget.cpp
    src/cloud_group_add_dialog.cpp
    src/cloud_groups_tree.cpp
    src/cloud_processes_tree.cpp
    src/cloud_process_execute_dialog.cpp
    src/cloud_session_combo_box.cpp
    src/cloud_session_list_widget.cpp
    src/cloud_session_manager_dialog.cpp
    src/cloud_session_manager_widget.cpp
    src/cloud_statistics_dialog.cpp
    src/cloud_user_add_dialog.cpp
    src/cloud_user_groups_dialog.cpp
    src/cloud_user_manager_widget.cpp
    src/cloud_users_tree.cpp
    src/main.cpp
    src/main_window.cpp

    src/action.h
    src/application.h
    src/central_widget.h
    src/cloud_actions_tree.h
    src/cloud_auth_token_manager_dialog.h
    src/cloud_auth_token_manager_widget.h
    src/cloud_group_add_dialog.h
    src/cloud_groups_tree.h
    src/cloud_processes_tree.h
    src/cloud_process_execute_dialog.h
    src/cloud_session_combo_box.h
    src/cloud_session_list_widget.h
    src/cloud_session_manager_dialog.h
    src/cloud_session_manager_widget.h
    src/cloud_statistics_dialog.h
    src/cloud_user_add_dialog.h
    src/cloud_user_groups_dialog.h
    src/cloud_user_manager_widget.h
    src/cloud_users_tree.h
    src/main_window.h
)

add_dependencies(cloud-client range-base-lib range-cloud-lib range-gui-lib)

target_link_libraries(cloud-client
    PRIVATE
        range-gui-lib
        common_defines
)

set_target_properties(cloud-client
    PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE "${PROJECT_PACKAGE_NAME}"
)

if (WIN32)
    set(WIN32_ICON "${CMAKE_CURRENT_SOURCE_DIR}/pixmaps/${PROJECT_PACKAGE_NAME}.ico")
    set(ICON_RC "${CMAKE_CURRENT_BINARY_DIR}/cloud-client.rc")
    file(WRITE "${ICON_RC}"
        "IDI_ICON1 ICON \"${WIN32_ICON}\"\n"
    )
    set_source_files_properties("${ICON_RC}" PROPERTIES LANGUAGE RC)
    target_sources(cloud-client PRIVATE "${ICON_RC}")
elseif (APPLE)
    set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/pixmaps/${PROJECT_PACKAGE_NAME}.icns")
    set_source_files_properties("${APP_ICON}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(cloud-client PRIVATE "${APP_ICON}")

    add_custom_command(TARGET cloud-client POST_BUILD
        COMMAND /usr/libexec/PlistBuddy -c "Set :CFBundleName '${PROJECT_SHORT_NAME}'" "$<TARGET_BUNDLE_DIR_NAME:${PROJECT_TARGET_NAME}>/Contents/Info.plist"
        COMMAND /usr/libexec/PlistBuddy -c "Add :CFBundleDisplayName string '${PROJECT_SHORT_NAME}'" "$<TARGET_BUNDLE_DIR_NAME:${PROJECT_TARGET_NAME}>/Contents/Info.plist"
        COMMENT "Set CFBundleDisplayName"
    )
endif()

qt_add_resources(cloud-client icons-application
    PREFIX "/icons/application"
    FILES
        pixmaps/range-about.svg
        pixmaps/range-help.svg
        pixmaps/range-license.svg
        pixmaps/range-qt.svg
        pixmaps/range-quit.svg
        pixmaps/range-release_notes.svg
        pixmaps/range-settings.svg
        pixmaps/range-update.svg
)

qt_add_resources(cloud-client icons-action
    PREFIX "/icons/action"
    FILES
        pixmaps/range-add.svg
        pixmaps/range-cancel.svg
        pixmaps/range-clear.svg
        pixmaps/range-close.svg
        pixmaps/range-ok.svg
        pixmaps/range-remove.svg
        pixmaps/range-undo.svg
)

qt_add_resources(cloud-client icons-logos
    PREFIX "/icons/logos"
    FILES
        pixmaps/range-app.svg
        pixmaps/range-cloud-client.svg
)

qt_add_resources(cloud-client icons-file
    PREFIX "/icons/file"
    FILES
        pixmaps/range-new.svg
        pixmaps/range-open.svg
        pixmaps/range-save.svg
)

qt_add_resources(cloud-client icons-cloud
    PREFIX "/icons/cloud"
    FILES
        pixmaps/range-access_rights.svg
        pixmaps/range-add.svg
        pixmaps/range-download.svg
        pixmaps/range-edit.svg
        pixmaps/range-execute.svg
        pixmaps/range-refresh.svg
        pixmaps/range-remove.svg
        pixmaps/range-upload.svg
        pixmaps/range-upload_update.svg
)

qt_add_resources(cloud-client icons-session
    PREFIX "/icons/session"
    FILES
        pixmaps/range-session_manager.svg
        pixmaps/range-statistics.svg
)

qt_add_translations(cloud-client
    TS_FILES
        translations/en.ts
        translations/sk.ts
    RESOURCE_PREFIX /i18n/cloud-client
)
