cmake_minimum_required(VERSION 3.14.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13 CACHE STRING "")

project(PulquiLimiter
    VERSION 1.0.0.0
    DESCRIPTION "PulquiLimiter VST 3 Plug-in"
)

add_subdirectory(external)

smtg_enable_vst3_sdk()

smtg_add_vst3plugin(PulquiLimiter
    include/plugcontroller.h
    include/plugids.h
    include/plugprocessor.h
    include/version.h
    source/plugfactory.cpp
    source/plugcontroller.cpp
    source/plugprocessor.cpp
    resource/plug.uidesc
)

target_link_libraries(PulquiLimiter
    PRIVATE
        sdk
        vstgui_support
)

smtg_target_add_plugin_resources(PulquiLimiter
    RESOURCES
        resource/plug.uidesc
        resource/background.png
        resource/animation_knob.png
        resource/background_2x.png
        resource/background_3x.png
        resource/animation_knob_3x.png
)

smtg_target_add_plugin_snapshots (PulquiLimiter
    RESOURCES
        resource/A2EAF7DB320640F48EDE380DDF89562C_snapshot.png
        resource/A2EAF7DB320640F48EDE380DDF89562C_snapshot_2.0x.png
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
        resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PulquiLimiter)

        smtg_target_set_debug_executable(PulquiLimiter
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
    endif()
endif(SMTG_MAC)
