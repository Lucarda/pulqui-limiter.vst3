//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Examples
// Filename    : public.sdk/samples/vst/panner/source/plugcontroller.cpp
// Created by  : Steinberg, 02/2020
// Description : PulquiLimiter Example for VST 3
//
//-----------------------------------------------------------------------------
// LICENSE
// (c) 2024, Steinberg Media Technologies GmbH, All Rights Reserved
//-----------------------------------------------------------------------------
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
//   * Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation 
//     and/or other materials provided with the distribution.
//   * Neither the name of the Steinberg Media Technologies nor the names of its
//     contributors may be used to endorse or promote products derived from this 
//     software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#include "../include/plugcontroller.h"
#include "../include/plugids.h"

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
class ThreshParameter : public Vst::Parameter
{
public:
	ThreshParameter (int32 flags, int32 id);

	void toString (Vst::ParamValue normValue, Vst::String128 string) const SMTG_OVERRIDE;
	bool fromString (const Vst::TChar* string, Vst::ParamValue& normValue) const SMTG_OVERRIDE;
};


//------------------------------------------------------------------------
// ThreshParameter Implementation
//------------------------------------------------------------------------
ThreshParameter::ThreshParameter (int32 flags, int32 id)
{
	Steinberg::UString (info.title, USTRINGSIZE (info.title)).assign (USTRING ("Threshold"));
	Steinberg::UString (info.units, USTRINGSIZE (info.units)).assign (USTRING (""));

	info.flags = flags;
	info.id = id;
	info.stepCount = 0;
	info.defaultNormalizedValue = 0.5f;
	info.unitId = Vst::kRootUnitId;

	setNormalized (.5f);
}

//------------------------------------------------------------------------
void ThreshParameter::toString (Vst::ParamValue normValue, Vst::String128 string) const
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
bool ThreshParameter::fromString (const Vst::TChar* string, Vst::ParamValue& normValue) const
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
		parameters.addParameter (STR16 ("Bypass"), nullptr, 1, 0,
		                         Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsBypass,
		                         PulquiLimiterParams::kBypassId);
		                         
		parameters.addParameter (STR16 ("LatencyBypass"), nullptr, 1, 0,
		                         Vst::ParameterInfo::kCanAutomate,
		                         PulquiLimiterParams::kParamLatencyBypassId);
		                         
		parameters.addParameter (STR16 ("MakeUp"), nullptr, 1, 0,
		                         Vst::ParameterInfo::kCanAutomate,
		                         PulquiLimiterParams::kParamMakeUpId);

		auto* threshParam = new ThreshParameter (Vst::ParameterInfo::kCanAutomate, PulquiLimiterParams::kParamTreshId);
		parameters.addParameter (threshParam);
		
		auto* srateParam = new SrateParameter (Vst::ParameterInfo::kIsReadOnly , PulquiLimiterParams::kParamSrateId);
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
tresult PLUGIN_API PlugController::getParameterIDFromFunctionName (Vst::UnitID unitID,
                                                                   FIDString functionName,
                                                                   Vst::ParamID& paramID)
{
	using namespace Vst;

	paramID = kNoParamId;

	if (unitID == kRootUnitId && FIDStringsEqual (functionName, FunctionNameType::kPanPosCenterX))
		paramID = PulquiLimiterParams::kParamTreshId;

	return (paramID != kNoParamId) ? kResultOk : kResultFalse;
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
	setParamNormalized (PulquiLimiterParams::kParamTreshId, savedParam1);

	// read the bypass
	int32 bypassState;
	if (streamer.readInt32 (bypassState) == false)
		return kResultFalse;
	setParamNormalized (PulquiLimiterParams::kBypassId, bypassState ? 1 : 0);
	
	int32 LatencyBypassState;
	if (streamer.readInt32 (LatencyBypassState) == false)
		return kResultFalse;
	setParamNormalized (PulquiLimiterParams::kParamLatencyBypassId, LatencyBypassState ? 1 : 0);
	
	int32 MakeUpState;
	if (streamer.readInt32 (MakeUpState) == false)
		return kResultFalse;
	setParamNormalized (PulquiLimiterParams::kParamMakeUpId, MakeUpState ? 1 : 0);

	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
