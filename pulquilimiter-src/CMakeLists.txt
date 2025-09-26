
# execute_process(COMMAND ${CMAKE_COMMAND} -E remove ~/.vst3/PulquiLimiter.vst3)

project(PulquiLimiter
    VERSION 0.1.2
    DESCRIPTION "PulquiLimiter VST3 Plug-in"
)


# smtg_enable_vst3_sdk()

smtg_add_vst3plugin(PulquiLimiter
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

target_link_libraries(PulquiLimiter
    PRIVATE
        sdk
        vstgui_support
)

smtg_target_add_plugin_resources(PulquiLimiter
    RESOURCES
        plug.uidesc
)

smtg_target_add_plugin_snapshots (PulquiLimiter
    RESOURCES
)

smtg_target_configure_version_file(PulquiLimiter)

if(SMTG_MAC)
    smtg_target_set_bundle(PulquiLimiter
        BUNDLE_IDENTIFIER net.steinberg.hello-world
        COMPANY_NAME "Steinberg Media Technologies GmbH"
    )
    smtg_target_set_debug_executable(PulquiLimiter
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(PulquiLimiter PRIVATE 
        #resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PulquiLimiter)

        smtg_target_set_debug_executable(PulquiLimiter
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)


