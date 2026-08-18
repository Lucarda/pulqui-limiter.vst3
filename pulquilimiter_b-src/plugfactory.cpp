/*
 * Copyright (C) 2024-2026 Lucas Cordiviola
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "public.sdk/source/main/pluginfactory.h"

#include "./plugcontroller.h"   // for createInstance
#include "./plugprocessor.h"    // for createInstance
#include "./plugids.h"          // for uids
#include "./version.h"          // for version and naming

#define PLUGVERSION_STR "0.1.0"

#define stringSubCategory Vst::PlugType::kFxDynamics // Subcategory for this plug-in (to be changed if needed, see PlugType in ivstaudioprocessor.h)

using namespace Steinberg::Vst;

BEGIN_FACTORY_DEF (stringCompanyName, stringCompanyWeb, stringCompanyEmail)

    DEF_CLASS2 (INLINE_UID_FROM_FUID(MyProcessorUID),
                PClassInfo::kManyInstances, // cardinality  
                kVstAudioEffectClass,   // the component category (do not change this)
                stringPluginName,       // here the plug-in name (to be changed)
                Vst::kDistributable,    // means that component and controller could be distributed on different computers
                stringSubCategory,      // Subcategory for this plug-in (to be changed)
                PLUGVERSION_STR,       // Plug-in version (to be changed)
                kVstVersionString,      // the VST 3 SDK version (do not change this, always use this define)
                Steinberg::Vst::PlugProcessor::createInstance)  // function pointer called when this component should be instantiated

    DEF_CLASS2 (INLINE_UID_FROM_FUID(MyControllerUID),
                PClassInfo::kManyInstances,  // cardinality   
                kVstComponentControllerClass,// the Controller category (do not change this)
                stringPluginName "Controller",  // controller name (can be the same as the component name)
                0,                      // not used here
                "",                     // not used here
                PLUGVERSION_STR,       // Plug-in version (to be changed)
                kVstVersionString,      // the VST 3 SDK version (do not change this, always use this define)
                Steinberg::Vst::PlugController::createInstance)// function pointer called when this component should be instantiated

END_FACTORY

