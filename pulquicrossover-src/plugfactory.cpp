/*
 * Copyright (C) 2025 Lucas Cordiviola
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "public.sdk/source/main/pluginfactory.h"

#include "./plugcontroller.h"   // for createInstance
#include "./plugprocessor.h"    // for createInstance
#include "./plugids.h"          // for uids
#include "./version.h"          // for version and naming

#define stringSubCategory Vst::PlugType::kFxFilter // Subcategory for this plug-in (to be changed if needed, see PlugType in ivstaudioprocessor.h)

using namespace Steinberg::Vst;

BEGIN_FACTORY_DEF (stringCompanyName, stringCompanyWeb, stringCompanyEmail)

    DEF_CLASS2 (INLINE_UID_FROM_FUID(MyProcessorUID),
                PClassInfo::kManyInstances, // cardinality
                kVstAudioEffectClass,   // the component category (do not change this)
                stringPluginName,       // here the plug-in name (to be changed)
                Vst::kDistributable,    // means that component and controller could be distributed on different computers
                stringSubCategory,      // Subcategory for this plug-in (to be changed)
                FULL_VERSION_STR,       // Plug-in version (to be changed)
                kVstVersionString,      // the VST 3 SDK version (do not change this, always use this define)
                Steinberg::Vst::PlugProcessor::createInstance)  // function pointer called when this component should be instantiated

    DEF_CLASS2 (INLINE_UID_FROM_FUID(MyControllerUID),
                PClassInfo::kManyInstances,  // cardinality
                kVstComponentControllerClass,// the Controller category (do not change this)
                stringPluginName "Controller",  // controller name (can be the same as the component name)
                0,                      // not used here
                "",                     // not used here
                FULL_VERSION_STR,       // Plug-in version (to be changed)
                kVstVersionString,      // the VST 3 SDK version (do not change this, always use this define)
                Steinberg::Vst::PlugController::createInstance)// function pointer called when this component should be instantiated

END_FACTORY

