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
    Buffer *ch1B = NULL;
    Buffer *ch1C = NULL;
    Buffer *ch2A = NULL;
    Buffer *ch2B = NULL;
    Buffer *ch2C = NULL;

    
    bool mIsStereo = true;
    bool mBypass = false;
    Vst::ParamValue mSplit_1 = 0.01;
    Vst::ParamValue mSplit_2 = 0.05;
    Vst::ParamValue mSplit_3 = 0.35;   
    bool mParam_A = true;
    bool mParam_B = true;
    bool mParam_C = true;
    bool mParam_D = true;
    bool mParam_F2_asFirstFilter = false;
    bool m_oldParam_F2_asFirstFilter = false;
    double mfade = 1;
    double audiolog;
   

    double pqcrossover_tilde_lp(Buffer *x, double in);
    double pqcrossover_tilde_hp(Buffer *x, double in);
    void pqcrossover_setup_filter(Buffer *x);
    void pqcrossover_tilde_setcrossf(Buffer *x, double freq, double samplerate);

};



//------------------------------------------------------------------------
} // namespace PulquiLimiter
} // namespace Steinberg
