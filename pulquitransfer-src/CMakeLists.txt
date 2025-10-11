
# execute_process(COMMAND ${CMAKE_COMMAND} -E remove ~/.vst3/PulquiTransfer.vst3)

project(PulquiTransfer
    VERSION 0.1.0
    DESCRIPTION "PulquiTransfer VST3 Plug-in"
)


# smtg_enable_vst3_sdk()

smtg_add_vst3plugin(PulquiTransfer
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

target_link_libraries(PulquiTransfer
    PRIVATE
        sdk
        vstgui_support
)

smtg_target_add_plugin_resources(PulquiTransfer
    RESOURCES
        plug.uidesc
)

smtg_target_add_plugin_snapshots (PulquiTransfer
    RESOURCES
)

smtg_target_configure_version_file(PulquiTransfer)

if(SMTG_MAC)
    smtg_target_set_bundle(PulquiTransfer
        BUNDLE_IDENTIFIER net.steinberg.hello-world
        COMPANY_NAME "Steinberg Media Technologies GmbH"
    )
    smtg_target_set_debug_executable(PulquiTransfer
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(PulquiTransfer PRIVATE 
        #resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PulquiTransfer)

        smtg_target_set_debug_executable(PulquiTransfer
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)


