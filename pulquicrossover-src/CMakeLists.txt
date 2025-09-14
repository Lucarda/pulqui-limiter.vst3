project(PulquiCrossover
    VERSION 1.0.0.0
    DESCRIPTION "PulquiCrossover VST3 Plug-in"
)


smtg_enable_vst3_sdk()

smtg_add_vst3plugin(PulquiCrossover
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

target_link_libraries(PulquiCrossover
    PRIVATE
        sdk
        vstgui_support
)

smtg_target_add_plugin_resources(PulquiCrossover
    RESOURCES
        plug.uidesc
)

smtg_target_add_plugin_snapshots (PulquiCrossover
    RESOURCES
)

smtg_target_configure_version_file(PulquiCrossover)

if(SMTG_MAC)
    smtg_target_set_bundle(PulquiCrossover
        BUNDLE_IDENTIFIER net.steinberg.hello-world
        COMPANY_NAME "Steinberg Media Technologies GmbH"
    )
    smtg_target_set_debug_executable(PulquiCrossover
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(PulquiCrossover PRIVATE 
        #resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PulquiLimiter)

        smtg_target_set_debug_executable(PulquiCrossover
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)
