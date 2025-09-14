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
