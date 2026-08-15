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
class VolParameter : public Vst::Parameter
{
public:
    VolParameter (int32 flags, int32 id, Steinberg::UString label);

    void toString (Vst::ParamValue normValue, Vst::String128 string) const SMTG_OVERRIDE;
    bool fromString (const Vst::TChar* string, Vst::ParamValue& normValue) const SMTG_OVERRIDE;
};


//------------------------------------------------------------------------
// VolParameter Implementation
//------------------------------------------------------------------------
VolParameter::VolParameter (int32 flags, int32 id, Steinberg::UString label)
{
    Steinberg::UString (info.title, USTRINGSIZE (info.title)).assign (USTRING (label));
    Steinberg::UString (info.units, USTRINGSIZE (info.units)).assign (USTRING (""));

    info.flags = flags;
    info.id = id;
    info.stepCount = 0;
    info.defaultNormalizedValue = 1.0f;
    info.unitId = Vst::kRootUnitId;

    setNormalized (.5f);
}

//------------------------------------------------------------------------
void VolParameter::toString (Vst::ParamValue normValue, Vst::String128 string) const
{
    char text[32];
    if (normValue > 0.0001)
    {
        snprintf (text, 32, "%.2f", 20 * log10f ((float)normValue));
    }
    else
    {
        strcpy (text, "-oo");
    }

    Steinberg::UString (string, 128).fromAscii (text);
}

//------------------------------------------------------------------------
bool VolParameter::fromString (const Vst::TChar* string, Vst::ParamValue& normValue) const
{
    String wrapper ((Steinberg::Vst::TChar*)string); // don't know buffer size here!
    double tmp = 0.0;
    if (wrapper.scanFloat (tmp))
    {
        // allow only values between -oo and 0dB
        if (tmp > 0.0)
        {
            tmp = -tmp;
        }

        normValue = expf (logf (10.f) * (float)tmp / 20.f);
        return true;
    }
    return false;
}



//------------------------------------------------------------------------

class SrateParameter : public Vst::Parameter
{
public:
    SrateParameter (int32 flags, int32 id);

    void toString (Vst::ParamValue normValue, Vst::String128 string) const SMTG_OVERRIDE;
    //bool fromString (const Vst::TChar* string, Vst::ParamValue& normValue) const SMTG_OVERRIDE;
    //tresult notify (Steinberg::Vst::IMessage* message, Vst::String128 string) const; //SMTG_OVERRIDE;
};

//------------------------------------------------------------------------
SrateParameter::SrateParameter (int32 flags, int32 id)
{
    Steinberg::UString (info.title, USTRINGSIZE (info.title)).assign (USTRING ("Samplerate"));
    Steinberg::UString (info.units, USTRINGSIZE (info.units)).assign (USTRING (""));

    info.flags = flags;
    info.id = id;
    info.stepCount = 0;
    info.defaultNormalizedValue = 0.5f;
    info.unitId = Vst::kRootUnitId;

    setNormalized (.5f);
}

//------------------------------------------------------------------------
void SrateParameter::toString (Vst::ParamValue normValue, Vst::String128 string) const
{
    char text[100];
    int samplesdelay = 8192;
    float samplerate = (float)normValue*1e+6;
    float ms = ((1/samplerate)*samplesdelay)*1000;
    snprintf (text, 100, "latency: %.2f ms @ %.0f hz (%d samples)", ms, samplerate, samplesdelay);

    Steinberg::UString (string, 128).fromAscii (text);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugController::initialize (FUnknown* context)
{
    tresult result = EditController::initialize (context);
    if (result == kResultTrue)
    {
        //---Create Parameters------------
                                 
        parameters.addParameter (STR16 ("in-multiplier"), nullptr, 10, 0,
                                 Vst::ParameterInfo::kCanAutomate,
                                 PulquiLimiterParams::kParamMultiInId);

        auto* inParam = new VolParameter (Vst::ParameterInfo::kCanAutomate,
         PulquiLimiterParams::kParamInId, USTRING ("Input"));
        parameters.addParameter (inParam);                                 

        parameters.addParameter (STR16 ("Feedback"), nullptr, 0, 0,
                                 Vst::ParameterInfo::kCanAutomate,
                                 PulquiLimiterParams::kParamMixId);
                                                                  
        auto* mixParam = new VolParameter (Vst::ParameterInfo::kCanAutomate,
         PulquiLimiterParams::kParamVolId, USTRING ("Volume"));
        parameters.addParameter (mixParam);
        
        parameters.addParameter (STR16 ("LatencyBypass"), nullptr, 1, 0,
                                 Vst::ParameterInfo::kCanAutomate,
                                 PulquiLimiterParams::kParamLatencyBypassId);                                 

        parameters.addParameter (STR16 ("Bypass"), nullptr, 1, 0,
                                 Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsBypass,
                                 PulquiLimiterParams::kBypassId);
                                 
        parameters.addParameter (STR16 ("Vu1L"), nullptr, 0, 0,
                                 Vst::ParameterInfo::kIsReadOnly | Vst::ParameterInfo::kIsHidden,
                                 PulquiLimiterParams::kParamVu1LId);

        parameters.addParameter (STR16 ("Vu1R"), nullptr, 0, 0,
                                 Vst::ParameterInfo::kIsReadOnly | Vst::ParameterInfo::kIsHidden,
                                 PulquiLimiterParams::kParamVu1RId);
                                 
        auto* srateParam = new SrateParameter (Vst::ParameterInfo::kIsReadOnly
         | Vst::ParameterInfo::kIsHidden, PulquiLimiterParams::kParamSrateId);
        parameters.addParameter (srateParam);
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

    float savedParam1 = 0.f;
    if (streamer.readFloat (savedParam1) == false)
        return kResultFalse;
    setParamNormalized (PulquiLimiterParams::kParamMixId, savedParam1);

    // read the bypass
    int32 bypassState;
    if (streamer.readInt32 (bypassState) == false)
        return kResultFalse;
    setParamNormalized (PulquiLimiterParams::kBypassId, bypassState ? 1 : 0);

    int32 LatencyBypassState;
    if (streamer.readInt32 (LatencyBypassState) == false)
        return kResultFalse;
    setParamNormalized (PulquiLimiterParams::kParamLatencyBypassId, LatencyBypassState ? 1 : 0);

    float VolState;
    if (streamer.readFloat (VolState) == false)
        return kResultFalse;
    setParamNormalized (PulquiLimiterParams::kParamVolId, VolState);
    
    float MultiState;
    if (streamer.readFloat (MultiState) == false)
        return kResultFalse;
    setParamNormalized (PulquiLimiterParams::kParamMultiInId, MultiState);
    
    float InState;
    if (streamer.readFloat (InState) == false)
        return kResultFalse;
    setParamNormalized (PulquiLimiterParams::kParamInId, InState);

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
        paramID = PulquiLimiterParams::kParamMixId;

    return (paramID != kNoParamId) ? kResultOk : kResultFalse;
}

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
