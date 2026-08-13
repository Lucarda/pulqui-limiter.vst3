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
    uint32 PLUGIN_API getLatencySamples () SMTG_OVERRIDE { return 0; }

//------------------------------------------------------------------------
    tresult PLUGIN_API setState (IBStream* state) SMTG_OVERRIDE;
    tresult PLUGIN_API getState (IBStream* state) SMTG_OVERRIDE;

    static FUnknown* createInstance (void*) { return (Vst::IAudioProcessor*)new PlugProcessor (); }
    ~PlugProcessor ();




protected:

    template <typename SampleType>
    tresult processAudio (Vst::ProcessData& data);

    tresult (PlugProcessor::*processAudioPtr) (Vst::ProcessData& data);


    struct filter {
        double a0;
        double a1;
        double a2;
        double a3;
        double a4;
        //------------------------------
        double tempx;
        double tempy;
        double xm4;
        double xm3;
        double xm2;
        double xm1;
        double ym4;
        double ym3;
        double ym2;
        double ym1;
    };

    struct Buffer{
        //------------------------------
        double fc; // cutoff frequency
        double pi;
        double srate;  // sample rate
        //------------------------------
        double wc;
        double wc2;
        double wc3;
        double wc4;
        double k;
        double k2;
        double k3;
        double k4;
        double sqrt2;
        double sq_tmp1;
        double sq_tmp2;
        double a_tmp;
        double b1;
        double b2;
        double b3;
        double b4;
        //------------------------------
        struct filter lp;
        struct filter hp;
    };

    Buffer *ch1A = NULL;
    Buffer *ch2A = NULL;

    
    bool mIsStereo = true;
    bool mBypass = false;
    Vst::ParamValue mSplit_1 = 0.05;
    bool mParam_A = true;
    bool mParam_B = true;
  

    double pqcrossover_tilde_lp(Buffer *x, double in);
    double pqcrossover_tilde_hp(Buffer *x, double in);
    void pqcrossover_setup_filter(Buffer *x);
    void pqcrossover_tilde_setcrossf(Buffer *x, double freq, double samplerate);

};



//------------------------------------------------------------------------
} // namespace PulquiLimiter
} // namespace Steinberg
