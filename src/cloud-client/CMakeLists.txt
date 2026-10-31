qt_add_executable(cloud-client
    src/action.cpp
    src/application.cpp
    src/central_widget.cpp
    src/cloud_actions_tree.cpp
    src/cloud_auth_token_manager_dialog.cpp
    src/cloud_auth_token_manager_widget.cpp
    src/cloud_file_quota_dialog.cpp
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
    src/cloud_file_quota_dialog.h
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
        MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_SHORT_NAME}"
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

    # Info.plist is generated from a template rather than patched after the
    # build, because the App Store requires keys CMake does not write on its own
    # (bundle identifier, category, copyright, encryption declaration).
    set(MACOSX_BUNDLE_EXECUTABLE_NAME "${PROJECT_TARGET_NAME}${DEBUG_EXT}")
    set(MACOSX_BUNDLE_ICON_FILE "${PROJECT_PACKAGE_NAME}")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "${MACOS_BUNDLE_IDENTIFIER}")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_SHORT_REL_NAME}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
    # The App Store requires CFBundleVersion to grow with every upload, across
    # releases too, so with a build number it is the build number alone, which
    # grows with every store run independently of the release version.
    if(RANGE_BUILD_NUMBER GREATER 0)
        set(MACOSX_BUNDLE_BUNDLE_VERSION "${RANGE_BUILD_NUMBER}")
    else()
        set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}")
    endif()
    set(MACOSX_BUNDLE_CATEGORY "${MACOS_APP_CATEGORY}")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright (C) ${PROJECT_YEAR} ${PROJECT_AUTHOR}")
    configure_file(
        ${CMAKE_SOURCE_DIR}/range-build-tools/packaging/macos/Info.plist.in
        ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
        @ONLY
    )

    set_target_properties(cloud-client
        PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/Info.plist"
    )
endif()

# Adds icon resources under given prefix together with their dark color scheme
# variants from pixmaps/dark (loaded at runtime through RIcon::fromFile).
function(cloud_client_add_icon_resources name prefix)
    set(files ${ARGN})
    foreach(file IN LISTS ARGN)
        get_filename_component(fileName ${file} NAME)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pixmaps/dark/${fileName}")
            list(APPEND files pixmaps/dark/${fileName})
        endif()
    endforeach()
    qt_add_resources(cloud-client ${name}
        PREFIX ${prefix}
        FILES ${files}
    )
endfunction()

cloud_client_add_icon_resources(icons-application "/icons/application"
        pixmaps/range-about.svg
        pixmaps/range-help.svg
        pixmaps/range-license.svg
        pixmaps/range-qt.svg
        pixmaps/range-quit.svg
        pixmaps/range-release_notes.svg
        pixmaps/range-settings.svg
        pixmaps/range-update.svg
)

cloud_client_add_icon_resources(icons-action "/icons/action"
        pixmaps/range-add.svg
        pixmaps/range-cancel.svg
        pixmaps/range-clear.svg
        pixmaps/range-close.svg
        pixmaps/range-ok.svg
        pixmaps/range-remove.svg
        pixmaps/range-undo.svg
)

cloud_client_add_icon_resources(icons-logos "/icons/logos"
        pixmaps/range-app.svg
        pixmaps/range-cloud-client.svg
        pixmaps/range-cloud-client-windows.svg
)

cloud_client_add_icon_resources(icons-file "/icons/file"
        pixmaps/range-new.svg
        pixmaps/range-open.svg
        pixmaps/range-save.svg
)

cloud_client_add_icon_resources(icons-cloud "/icons/cloud"
        pixmaps/range-access_rights.svg
        pixmaps/range-add.svg
        pixmaps/range-download.svg
        pixmaps/range-edit.svg
        pixmaps/range-execute.svg
        pixmaps/range-refresh.svg
        pixmaps/range-remove.svg
        pixmaps/range-upload.svg
        pixmaps/range-upload_replace.svg
        pixmaps/range-upload_update.svg
)

cloud_client_add_icon_resources(icons-session "/icons/session"
        pixmaps/range-session_manager.svg
        pixmaps/range-statistics.svg
)

qt_add_translations(cloud-client
    TS_FILES
        translations/en.ts
        translations/sk.ts
    RESOURCE_PREFIX /i18n/cloud-client
)
