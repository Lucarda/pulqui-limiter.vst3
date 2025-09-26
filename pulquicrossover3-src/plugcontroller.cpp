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
class SplitParameter : public Vst::Parameter
{
public:
    SplitParameter (int32 flags, int32 id, char *title);

    void toString (Vst::ParamValue normValue, Vst::String128 string) const SMTG_OVERRIDE;
    bool fromString (const Vst::TChar* string, Vst::ParamValue& normValue) const SMTG_OVERRIDE;
};


//------------------------------------------------------------------------
// SplitParameter Implementation
//------------------------------------------------------------------------
SplitParameter::SplitParameter (int32 flags, int32 id, char *title)
{
    Steinberg::UString (info.title, USTRINGSIZE (info.title)).assign (USTRING (title));
    Steinberg::UString (info.units, USTRINGSIZE (info.units)).assign (USTRING ("hz"));

    info.flags = flags;
    info.id = id;
    info.stepCount = 0;
    info.defaultNormalizedValue = 0.5f;
    info.unitId = Vst::kRootUnitId;

    setNormalized (.5f);
}

//------------------------------------------------------------------------
void SplitParameter::toString (Vst::ParamValue normValue, Vst::String128 string) const
{
    char text[32];
    if (normValue > 0.001)
    {
        snprintf (text, 32, "%.0f", normValue * 20000.);
    }
    else
    {
        strcpy (text, "20");
    }

    Steinberg::UString (string, 128).fromAscii (text);
}

//------------------------------------------------------------------------
bool SplitParameter::fromString (const Vst::TChar* string, Vst::ParamValue& normValue) const
{
    String wrapper ((Steinberg::Vst::TChar*)string); // don't know buffer size here!
    double tmp = 0.0;
    if (wrapper.scanFloat (tmp))
    {
        // allow only values
        if (tmp < 20.)
        {
            tmp = 20.;
        }
        if (tmp > 20000.)
        {
            tmp = 20000.;
        }

        normValue = tmp / 20000.;
        return true;
    }
    return false;
}


//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugController::initialize (FUnknown* context)
{
    tresult result = EditController::initialize (context);
    if (result == kResultTrue)
    {
        //---Create Parameters------------
        parameters.addParameter (STR16 ("Bypass"), nullptr, 1, 0,
             Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsBypass,
             PulquiCrossoverParams::kBypassId);

        char text[32];

        snprintf(text, 32, "Split A-B");
        auto* splitParam1 = new SplitParameter (Vst::ParameterInfo::kCanAutomate,
            PulquiCrossoverParams::kParamSplit_1_Id, text);
        parameters.addParameter (splitParam1);

        snprintf(text, 32, "Split B-C");
        auto* splitParam2 = new SplitParameter (Vst::ParameterInfo::kCanAutomate,
            PulquiCrossoverParams::kParamSplit_2_Id, text);
        parameters.addParameter (splitParam2);


        parameters.addParameter (STR16 ("A band"), nullptr, 1, 0,
                                 Vst::ParameterInfo::kCanAutomate,
                                 PulquiCrossoverParams::kParam_A_Id);

        parameters.addParameter (STR16 ("B band"), nullptr, 1, 0,
                                 Vst::ParameterInfo::kCanAutomate,
                                 PulquiCrossoverParams::kParam_B_Id);

        parameters.addParameter (STR16 ("C band"), nullptr, 1, 0,
                                 Vst::ParameterInfo::kCanAutomate,
                                 PulquiCrossoverParams::kParam_C_Id);

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

    float Split_1 = 0.f;
    if (streamer.readFloat (Split_1) == false)
        return kResultFalse;
    setParamNormalized (PulquiCrossoverParams::kParamSplit_1_Id, Split_1);

    float Split_2 = 0.f;
    if (streamer.readFloat (Split_2) == false)
        return kResultFalse;
    setParamNormalized (PulquiCrossoverParams::kParamSplit_2_Id, Split_2);

    // read the bypass
    int32 BypassState;
    if (streamer.readInt32 (BypassState) == false)
        return kResultFalse;
    setParamNormalized (PulquiCrossoverParams::kBypassId, BypassState ? 1 : 0);

    int32 Param_A;
    if (streamer.readInt32 (Param_A) == false)
        return kResultFalse;
    setParamNormalized (PulquiCrossoverParams::kParam_A_Id, Param_A ? 1 : 0);

    int32 Param_B;
    if (streamer.readInt32 (Param_B) == false)
        return kResultFalse;
    setParamNormalized (PulquiCrossoverParams::kParam_B_Id, Param_B ? 1 : 0);

    int32 Param_C;
    if (streamer.readInt32 (Param_C) == false)
        return kResultFalse;
    setParamNormalized (PulquiCrossoverParams::kParam_C_Id, Param_C ? 1 : 0);

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
        paramID = PulquiCrossoverParams::kParam_A_Id;

    return (paramID != kNoParamId) ? kResultOk : kResultFalse;
}

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
