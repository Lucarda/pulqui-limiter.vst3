




project(PulquiCrossover-3bands
    VERSION 0.1.0
    DESCRIPTION "PulquiCrossover-3bands VST3 Plug-in"
)


# smtg_enable_vst3_sdk()

smtg_add_vst3plugin(PulquiCrossover-3bands
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

target_link_libraries(PulquiCrossover-3bands
    PRIVATE
        sdk
        vstgui_support
)

smtg_target_add_plugin_resources(PulquiCrossover-3bands
    RESOURCES
        plug.uidesc
)

smtg_target_add_plugin_snapshots (PulquiCrossover-3bands
    RESOURCES
)

smtg_target_configure_version_file(PulquiCrossover-3bands)

if(SMTG_MAC)
    smtg_target_set_bundle(PulquiCrossover-3bands
        BUNDLE_IDENTIFIER net.steinberg.hello-world
        COMPANY_NAME "Steinberg Media Technologies GmbH"
    )
    smtg_target_set_debug_executable(PulquiCrossover-3bands
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(PulquiCrossover-3bands PRIVATE 
        #resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PulquiCrossover-3bands)

        smtg_target_set_debug_executable(PulquiCrossover-3bands
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)

