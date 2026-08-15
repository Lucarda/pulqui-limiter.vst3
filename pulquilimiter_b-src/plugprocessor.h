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
        double x_vu1;
        int x_pulquiblock;
    };
    Buffer *ch1 = NULL;
    Buffer *ch2 = NULL;

    Vst::ParamValue mThreshValue = 0.998;
    Vst::ParamValue mMixValue = 0.0;
    Vst::ParamValue mVol = 1.0;
    bool mBypass = false;
    double fsamplrateOld;
    bool mLatencyBypass = false;
    bool mIsStereo = true;
    Vst::ParamValue mIn = 0;
	Vst::ParamValue mMulti = 0;




    void pq_bee32(Buffer* self);
    void pq_bee32_negative(Buffer* self);
    void pulqui_tilde_do_pulqui(Buffer* self);
    void pulqui(Buffer* self, int32 nSamples);

};



//------------------------------------------------------------------------
} // namespace PulquiLimiter
} // namespace Steinberg 
