//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Examples
// Filename    : public.sdk/samples/vst/panner/include/plugprocessor.h
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

#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"


namespace Steinberg {
namespace Vst{
	
#define PULQUI_SIZE 4096
#define PULQUI_SCAN_SIZE 8192

//-----------------------------------------------------------------------------
class PlugProcessor : public Vst::AudioEffect
{
public:
	PlugProcessor ();


	tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;
	tresult PLUGIN_API setBusArrangements (Vst::SpeakerArrangement* inputs, int32 numIns,
	                                       Vst::SpeakerArrangement* outputs,
	                                       int32 numOuts) SMTG_OVERRIDE;

	tresult PLUGIN_API canProcessSampleSize (int32 symbolicSampleSize) SMTG_OVERRIDE;
	tresult PLUGIN_API setupProcessing (Vst::ProcessSetup& setup) SMTG_OVERRIDE;
	tresult PLUGIN_API setActive (TBool state) SMTG_OVERRIDE;
	tresult PLUGIN_API process (Vst::ProcessData& data) SMTG_OVERRIDE;
	tresult PLUGIN_API terminate () SMTG_OVERRIDE;
	uint32 PLUGIN_API getLatencySamples () SMTG_OVERRIDE { return 8192; }

//------------------------------------------------------------------------
	tresult PLUGIN_API setState (IBStream* state) SMTG_OVERRIDE;
	tresult PLUGIN_API getState (IBStream* state) SMTG_OVERRIDE;

	static FUnknown* createInstance (void*) { return (Vst::IAudioProcessor*)new PlugProcessor (); }
	~PlugProcessor ();
	
	


protected:
	
	template <typename SampleType>
	tresult processAudio (Vst::ProcessData& data);
	
	tresult (PlugProcessor::*processAudioPtr) (Vst::ProcessData& data);
	
	struct Buffer{
		double x_ramchpositive[PULQUI_SCAN_SIZE];
		double x_ramchnegative[PULQUI_SCAN_SIZE];
		double x_ramch[PULQUI_SIZE];
		double x_bufsignal[PULQUI_SIZE];
		double x_bufsignalout[PULQUI_SIZE];
		double x_bufpulqui[PULQUI_SIZE];
		double x_input[PULQUI_SCAN_SIZE]; 
		double x_output[PULQUI_SCAN_SIZE];
		int x_pulquiblock;
	};	
	Buffer *ch1 = NULL;
	Buffer *ch2 = NULL;
		
	Vst::ParamValue mThreshValue = 0.5;
	bool mBypass = false;
	double fsamplrateOld;
	bool mLatencyBypass = false;
	bool mMakeUp = false;
	bool mIsStereo = true;

	

	
	void pq_bee32(Buffer* self);
	void pq_bee32_negative(Buffer* self);
	void pulqui_tilde_do_pulqui(Buffer* self);
	void pulqui(Buffer* self, int32 nSamples);

};



//------------------------------------------------------------------------
} // namespace PulquiLimiter
} // namespace Steinberg
