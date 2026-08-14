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

#pragma once

#include "vstgui/plugin-bindings/vst3editor.h"
#include "public.sdk/source/vst/vsteditcontroller.h"
#include "pluginterfaces/vst/ivstparameterfunctionname.h"

namespace Steinberg {
namespace Vst {

//-----------------------------------------------------------------------------
class PlugController : public Vst::EditController,
                       public VSTGUI::VST3EditorDelegate,
                       public Vst::IParameterFunctionName
{
public:
//------------------------------------------------------------------------
    // create function required for plug-in factory,
    // it will be called to create new instances of this controller
//------------------------------------------------------------------------
    static FUnknown* createInstance (void*)
    {
        return (Vst::IEditController*)new PlugController ();
    }

    //---from IPluginBase--------
    tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;

    //---from EditController-----
    IPlugView* PLUGIN_API createView (const char* name) SMTG_OVERRIDE;
    tresult PLUGIN_API setComponentState (IBStream* state) SMTG_OVERRIDE;

    //---from IParameterFunctionName----
    tresult PLUGIN_API getParameterIDFromFunctionName (Vst::UnitID unitID, FIDString functionName,
                                                       Vst::ParamID& paramID) override;


    OBJ_METHODS (PlugController, Vst::EditController)
    DEFINE_INTERFACES
        DEF_INTERFACE (Vst::IParameterFunctionName)
    END_DEFINE_INTERFACES (Vst::EditController)
    DELEGATE_REFCOUNT (Vst::EditController)


//-----------------------------------------------------------------------------
protected:
    double rsamplerate;

};

//------------------------------------------------------------------------
} // namespace PulquiLimiter
} // namespace Steinberg
