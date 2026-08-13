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

#include "./plugcontroller.h"
#include "./plugids.h"

#include "base/source/fstreamer.h"
#include "public.sdk/source/vst/utility/stringconvert.h"

#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/base/ustring.h"
#include "base/source/fstring.h"

#include "pluginterfaces/vst/vsttypes.h"

#include <string_view>

using namespace VSTGUI;

namespace Steinberg {
namespace Vst {

// example of custom parameter (overwriting to and fromString)
//------------------------------------------------------------------------
class TransferParameter : public Vst::Parameter
{
public:
    TransferParameter (int32 flags, int32 id, char *title);

    void toString (Vst::ParamValue normValue, Vst::String128 string) const SMTG_OVERRIDE;
    bool fromString (const Vst::TChar* string, Vst::ParamValue& normValue) const SMTG_OVERRIDE;
};


//------------------------------------------------------------------------
// TransferParameter Implementation
//------------------------------------------------------------------------
TransferParameter::TransferParameter (int32 flags, int32 id, char *title)
{
    Steinberg::UString (info.title, USTRINGSIZE (info.title)).assign (USTRING (title));
    Steinberg::UString (info.units, USTRINGSIZE (info.units)).assign (USTRING (" "));

    info.flags = flags;
    info.id = id;
    info.stepCount = 0;
    info.defaultNormalizedValue = 0.5f;
    info.unitId = Vst::kRootUnitId;

    setNormalized (.5f);
}

//------------------------------------------------------------------------
void TransferParameter::toString (Vst::ParamValue normValue, Vst::String128 string) const
{
    char text[32];
    if (normValue > 0.001)
    {
        snprintf (text, 32, "%.2f", normValue);
    }
    else
    {
        strcpy (text, "0");
    }

    Steinberg::UString (string, 128).fromAscii (text);
}

//------------------------------------------------------------------------
bool TransferParameter::fromString (const Vst::TChar* string, Vst::ParamValue& normValue) const
{
    String wrapper ((Steinberg::Vst::TChar*)string); // don't know buffer size here!
    double tmp = 0.0;
    if (wrapper.scanFloat (tmp))
    {
        // allow only values
        if (tmp < 0.001)
        {
            tmp = 0.;
        }
        if (tmp > 1.)
        {
            tmp = 1.;
        }

        normValue = tmp;
        return true;
    }
    return false;
}


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugController::initialize (FUnknown* context)
{
    tresult result = EditController::initialize (context);
    if (result == kResultTrue)
    {
        //---Create Parameters------------

        char text[32];

        parameters.addParameter (STR16 ("Bypass"), nullptr, 1, 0,
                                 Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsBypass,
                                 PulquiTransferParams::kBypassId);

        snprintf(text, 32, "Positive Knee");
        auto* transParam1 = new TransferParameter (Vst::ParameterInfo::kCanAutomate,
            PulquiTransferParams::kParamPositiveTreshId, text);
        parameters.addParameter (transParam1);

        snprintf(text, 32, "Positive Att");
        auto* transParam2 = new TransferParameter (Vst::ParameterInfo::kCanAutomate,
            PulquiTransferParams::kParamPositiveAttId, text);
        parameters.addParameter (transParam2);

        parameters.addParameter (STR16 ("Positive Makeup"), nullptr, 1, 0,
                                 Vst::ParameterInfo::kCanAutomate,
                                 PulquiTransferParams::kParamPositiveMakeUpId);
        snprintf(text, 32, "Negative Knee");
        auto* transParam3 = new TransferParameter (Vst::ParameterInfo::kCanAutomate,
            PulquiTransferParams::kParamNegativeTreshId, text);
        parameters.addParameter (transParam3);

        snprintf(text, 32, "Negative Att");
        auto* transParam4 = new TransferParameter (Vst::ParameterInfo::kCanAutomate,
            PulquiTransferParams::kParamNegativeAttId, text);
        parameters.addParameter (transParam4);

        parameters.addParameter (STR16 ("Negative Makeup"), nullptr, 1, 0,
                                 Vst::ParameterInfo::kCanAutomate,
                                 PulquiTransferParams::kParamNegativeMakeUpId);

        parameters.addParameter (STR16 ("Symetric"), nullptr, 1, 0,
                                 Vst::ParameterInfo::kCanAutomate,
                                 PulquiTransferParams::kParamSymetricId);

        parameters.addParameter (STR16 ("help"), nullptr, 1, 0,
                                 Vst::ParameterInfo::kCanAutomate,
                                 PulquiTransferParams::kGuiViewToggle);

    }


    return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API PlugController::createView (const char* _name)
{
    std::string_view name (_name);
    if (name == Vst::ViewType::kEditor)
    {
        auto* view = new VST3Editor (this, "view", "plug.uidesc");
        return view;
    }
    return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugController::setComponentState (IBStream* state)
{
    // we receive the current state of the component (processor part)
    // we read our parameters and bypass value...
    if (!state)
        return kResultFalse;

    IBStreamer streamer (state, kLittleEndian);



    int32 SmBypassId = 0;
    if (streamer.readInt32 (SmBypassId) == false)
        return kResultFalse;
        setParamNormalized (PulquiTransferParams::kBypassId, SmBypassId ? 1 : 0);
    float SmParamPositiveTreshId= 0.f;
    if (streamer.readFloat (SmParamPositiveTreshId) == false)
        return kResultFalse;
        setParamNormalized (PulquiTransferParams::kParamPositiveTreshId, SmParamPositiveTreshId);
    float SmParamPositiveAttId= 0.f;
    if (streamer.readFloat (SmParamPositiveAttId) == false)
        return kResultFalse;
        setParamNormalized (PulquiTransferParams::kParamPositiveAttId, SmParamPositiveAttId);
    int32 SmParamPositiveMakeUpId = 0;
    if (streamer.readInt32 (SmParamPositiveMakeUpId) == false)
        return kResultFalse;
        setParamNormalized (PulquiTransferParams::kParamPositiveMakeUpId, SmParamPositiveMakeUpId ? 1 : 0);
    float SmParamNegativeTreshId= 0.f;
    if (streamer.readFloat (SmParamNegativeTreshId) == false)
        return kResultFalse;
        setParamNormalized (PulquiTransferParams::kParamNegativeTreshId, SmParamNegativeTreshId);
    float SmParamNegativeAttId= 0.f;
    if (streamer.readFloat (SmParamNegativeAttId) == false)
        return kResultFalse;
        setParamNormalized (PulquiTransferParams::kParamNegativeAttId, SmParamNegativeAttId);
    int32 SmParamNegativeMakeUpId = 0;
    if (streamer.readInt32 (SmParamNegativeMakeUpId) == false)
        return kResultFalse;
        setParamNormalized (PulquiTransferParams::kParamNegativeMakeUpId, SmParamNegativeMakeUpId ? 1 : 0);
    int32 SmParamSymetricId = 0;
    if (streamer.readInt32 (SmParamSymetricId) == false)
        return kResultFalse;
        setParamNormalized (PulquiTransferParams::kParamSymetricId, SmParamSymetricId ? 1 : 0);
    int32 SmParamGUIAB = 0;
    if (streamer.readInt32 (SmParamGUIAB) == false)
        return kResultFalse;
        setParamNormalized (PulquiTransferParams::kGuiViewToggle, SmParamGUIAB ? 1 : 0);



    return kResultOk;
}

/* this function is unused */

//------------------------------------------------------------------------
tresult PLUGIN_API PlugController::getParameterIDFromFunctionName (Vst::UnitID unitID,
                                                                   FIDString functionName,
                                                                   Vst::ParamID& paramID)
{
    using namespace Vst;

    paramID = kNoParamId;

    if (unitID == kRootUnitId && FIDStringsEqual (functionName, FunctionNameType::kPanPosCenterX))
        paramID = PulquiTransferParams::kParamPositiveTreshId;

    return (paramID != kNoParamId) ? kResultOk : kResultFalse;
}

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
