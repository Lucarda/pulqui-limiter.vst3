

project(PulquiLimiter_b
    VERSION 0.1.0
    DESCRIPTION "PulquiLimiter_b VST3 Plug-in"
)


# smtg_enable_vst3_sdk()

smtg_add_vst3plugin(PulquiLimiter_b
    plugcontroller.h
    plugids.h
    plugprocessor.h
    version.h
    plugfactory.cpp
    plugcontroller.cpp
    plugprocessor.cpp
    pulqui.cpp
    plug.uidesc
)

target_link_libraries(PulquiLimiter_b
    PRIVATE
        sdk
        vstgui_support
)

smtg_target_add_plugin_resources(PulquiLimiter_b
    RESOURCES
        plug.uidesc
)

smtg_target_add_plugin_snapshots (PulquiLimiter_b
    RESOURCES
)

smtg_target_configure_version_file(PulquiLimiter_b)

if(SMTG_MAC)
    smtg_target_set_bundle(PulquiLimiter_b
        BUNDLE_IDENTIFIER net.steinberg.hello-world
        COMPANY_NAME "Steinberg Media Technologies GmbH"
    )
    smtg_target_set_debug_executable(PulquiLimiter_b
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(PulquiLimiter_b PRIVATE 
        #resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PulquiLimiter_b)

        smtg_target_set_debug_executable(PulquiLimiter_b
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)


