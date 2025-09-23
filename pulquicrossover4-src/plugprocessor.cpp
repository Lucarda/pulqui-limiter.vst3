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

#include "./plugprocessor.h"
#include "./plugids.h"

#include "public.sdk/source/vst/vstaudioprocessoralgo.h"
#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include <stdlib.h>

namespace Steinberg {
namespace Vst{

#ifndef kPI
#define kPI 3.14159265358979323846
#endif

//-----------------------------------------------------------------------------
PlugProcessor::PlugProcessor ()
{
    // register its editor class
    setControllerClass (MyControllerUID);

    // default init
    processAudioPtr = &PlugProcessor::processAudio<float>;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::initialize (FUnknown* context)
{
    //---always initialize the parent-------
    tresult result = AudioEffect::initialize (context);
    if (result != kResultTrue)
        return kResultFalse;

    //---create Audio In/Out busses------
    // better to create them as mono but we got into troubles with some hosts.
    // also remember to change "bool mIsStereo = true" in the header.
    addAudioInput (STR16 ("Stereo In"), SpeakerArr::kStereo);
    addAudioOutput (STR16 ("Stereo Out"), SpeakerArr::kStereo);

    return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::canProcessSampleSize (int32 symbolicSampleSize)
{
    return ((symbolicSampleSize == Vst::kSample32) || (symbolicSampleSize == Vst::kSample64)) ?
               kResultTrue :
               kResultFalse;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setBusArrangements (Vst::SpeakerArrangement* inputs, int32 numIns,
                                                      Vst::SpeakerArrangement* outputs,
                                                      int32 numOuts)
{

    if (numIns == 1 && numOuts == 1)
    {
        // the host wants Mono => Mono (or 1 channel -> 1 channel)
        if (SpeakerArr::getChannelCount (inputs[0]) == 1 &&
            SpeakerArr::getChannelCount (outputs[0]) == 1)
        {
            auto* bus = FCast<AudioBus> (audioInputs.at (0));
            if (bus)
            {
                // check if we are Mono => Mono, if not we need to recreate the busses
                if (bus->getArrangement () != inputs[0])
                {
                    bus->setArrangement (inputs[0]);
                    bus->setName (STR16 ("Mono In"));
                    if (auto* busOut = FCast<AudioBus> (audioOutputs.at (0)))
                    {
                        busOut->setArrangement (outputs[0]);
                        busOut->setName (STR16 ("Mono Out"));
                    }
                }
                mIsStereo = false;
                return kResultOk;
            }
        }
        // the host wants something else than Mono => Mono, in this case we are always Stereo =>
        // Stereo
        else
        {
            auto* bus = FCast<AudioBus> (audioInputs.at (0));
            if (bus)
            {
                tresult result = kResultFalse;

                // the host wants 2->2 (could be LsRs -> LsRs)
                if (SpeakerArr::getChannelCount (inputs[0]) == 2 &&
                    SpeakerArr::getChannelCount (outputs[0]) == 2)
                {
                    bus->setArrangement (inputs[0]);
                    bus->setName (STR16 ("Stereo In"));
                    if (auto* busOut = FCast<AudioBus> (audioOutputs.at (0)))
                    {
                        busOut->setArrangement (outputs[0]);
                        busOut->setName (STR16 ("Stereo Out"));
                    }
                    result = kResultTrue;
                }
                // the host want something different than 1->1 or 2->2 : in this case we want stereo
                else if (bus->getArrangement () != SpeakerArr::kStereo)
                {
                    bus->setArrangement (SpeakerArr::kStereo);
                    bus->setName (STR16 ("Stereo In"));
                    if (auto* busOut = FCast<AudioBus> (audioOutputs.at (0)))
                    {
                        busOut->setArrangement (SpeakerArr::kStereo);
                        busOut->setName (STR16 ("Stereo Out"));
                    }

                    result = kResultFalse;
                }
                mIsStereo = true;
                return result;
            }
        }
    }
    return kResultFalse;
}


//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setupProcessing (Vst::ProcessSetup& setup)
{
    if (setup.symbolicSampleSize == Vst::kSample64)
    {
        processAudioPtr = &PlugProcessor::processAudio<double>;
    }
    else
    {
        processAudioPtr = &PlugProcessor::processAudio<float>;
    }

    if(1)
    {
        ch1A = new Buffer();
        ch1A->pi = kPI;
        ch1B = new Buffer();
        ch1B->pi = kPI;
        ch1C = new Buffer();
        ch1C->pi = kPI;
    }
    if(mIsStereo)
    {
        ch2A = new Buffer();
        ch2A->pi = kPI;
        ch2B = new Buffer();
        ch2B->pi = kPI;
        ch2C = new Buffer();
        ch2C->pi = kPI;
    }

    return AudioEffect::setupProcessing (setup);
}

//------------------------------------------------------------------------
PlugProcessor::~PlugProcessor()
{
    //delete ch1;
    //delete ch2;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::terminate ()
{
    if(1)
    {
        delete ch1A;
        delete ch1B;
        delete ch1C;
        ch1A = NULL;
        ch1B = NULL;
        ch1C = NULL;
    }
    if(mIsStereo)
    {
        delete ch2A;
        delete ch2B;
        delete ch2C;
        ch2A = NULL;
        ch2B = NULL;
        ch2C = NULL;
    }
    printf("LUCARDA -- terminate called\n");
    return AudioEffect::terminate ();
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setActive (TBool state)
{
    //PlugController::foosamplerate = processSetup.sampleRate;
    /*
    if (state)
    {
        Steinberg::Vst::IMessage* msg = allocateMessage ();
        if (msg)
        {
            msg->setMessageID ("SR");
            msg->getAttributes ()->setFloat ("SSRR", processSetup.sampleRate);
            sendMessage (msg);
            msg->release ();

            //printf("-----message: %.0f",100.0);

        }
    }
    */
    return AudioEffect::setActive (state);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::process (Vst::ProcessData& data)
{
    //--- Read inputs parameter changes-----------
    if (data.inputParameterChanges)
    {
        int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
        for (int32 index = 0; index < numParamsChanged; index++)
        {
            if (Vst::IParamValueQueue* paramQueue =
                    data.inputParameterChanges->getParameterData (index))
            {
                Vst::ParamValue value;
                int32 sampleOffset;
                int32 numPoints = paramQueue->getPointCount ();
                switch (paramQueue->getParameterId ())
                {
                    case PulquiCrossoverParams::kBypassId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mBypass = (value > 0.5f);
                        break;

                    case PulquiCrossoverParams::kParamSplit_1_Id:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mSplit_1= value;
                        break;

                    case PulquiCrossoverParams::kParamSplit_2_Id:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mSplit_2 = value;
                        break;

                    case PulquiCrossoverParams::kParamSplit_3_Id:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mSplit_3 = value;
                        break;


                    case PulquiCrossoverParams::kParam_A_Id:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mParam_A = (value > 0.5f);
                        break;

                    case PulquiCrossoverParams::kParam_B_Id:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mParam_B = (value > 0.5f);
                        break;

                    case PulquiCrossoverParams::kParam_C_Id:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mParam_C = (value > 0.5f);
                        break;

                    case PulquiCrossoverParams::kParam_D_Id:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mParam_D = (value > 0.5f);
                        break;

                    case PulquiCrossoverParams::k_F2_asFirstFilter_Id:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mParam_F2_asFirstFilter = (value > 0.5f);
                        break;

                }
            }
        }
    }

    //--- Process Audio---------------------
    //--- ----------------------------------
    if (data.numInputs == 0 || data.numOutputs == 0 || data.numSamples == 0)
    {
        // nothing to do
        return kResultOk;
    }



    return (this->*processAudioPtr) (data);
}

//------------------------------------------------------------------------
template <typename SampleType>
tresult PlugProcessor::processAudio (Vst::ProcessData& data)
{
    int32 numFrames = data.numSamples;

    uint32 sampleFramesSize = getSampleFramesSizeInBytes (processSetup, numFrames);
    auto** currentInputBuffers =
        (SampleType**)Vst::getChannelBuffersPointer (processSetup, data.inputs[0]);
    auto** currentOutputBuffers =
        (SampleType**)Vst::getChannelBuffersPointer (processSetup, data.outputs[0]);



    //---pulqui---------------------

    SampleType* input1 = currentInputBuffers[0];
    SampleType* output1 = currentOutputBuffers[0];
    SampleType* input2 = currentInputBuffers[1];
    SampleType* output2 = currentOutputBuffers[1];


    #if 1

    double freq1 = (double) mSplit_1 * 20000.;
    double freq2 = (double) mSplit_2 * 20000.;
    double freq3 = (double) mSplit_3 * 20000.;
    double sr = processSetup.sampleRate;
    int ms2samples = (int) sr * (900 / 1000.); // 900ms
    double fadeStep = 1. / ms2samples;

    for (int32 n = 0; n < numFrames; n++)
    {
        if (!mParam_F2_asFirstFilter)
        {
            double in, lpAout, hpAout, lpBout, hpBout, lpCout, hpCout;
            in = input1[n];
            pqcrossover_tilde_setcrossf(ch1A, freq1, sr);
            lpAout = pqcrossover_tilde_lp(ch1A, in);
            hpAout = pqcrossover_tilde_hp(ch1A, in);
            pqcrossover_tilde_setcrossf(ch1B, freq2, sr);
            lpBout = pqcrossover_tilde_lp(ch1B, hpAout);
            hpBout = pqcrossover_tilde_hp(ch1B, hpAout);
            pqcrossover_tilde_setcrossf(ch1C, freq3, sr);
            lpCout = pqcrossover_tilde_lp(ch1C, hpBout);
            hpCout = pqcrossover_tilde_hp(ch1C, hpBout);
            if (mBypass)
                output1[n] = in;
            else
                output1[n] = ((lpAout * mParam_A) + (lpBout * mParam_B) +
                (lpCout * mParam_C) + (hpCout * mParam_D)) * mfade;
            if(mIsStereo)
            {
                in = input2[n];
                pqcrossover_tilde_setcrossf(ch2A, freq1, sr);
                lpAout = pqcrossover_tilde_lp(ch2A, in);
                hpAout = pqcrossover_tilde_hp(ch2A, in);
                pqcrossover_tilde_setcrossf(ch2B, freq2, sr);
                lpBout = pqcrossover_tilde_lp(ch2B, hpAout);
                hpBout = pqcrossover_tilde_hp(ch2B, hpAout);
                pqcrossover_tilde_setcrossf(ch2C, freq3, sr);
                lpCout = pqcrossover_tilde_lp(ch2C, hpBout);
                hpCout = pqcrossover_tilde_hp(ch2C, hpBout);
                if (mBypass)
                    output2[n] = in;
                else
                    output2[n] = ((lpAout * mParam_A) + (lpBout * mParam_B) +
                        (lpCout * mParam_C) + (hpCout * mParam_D)) * mfade;
            }
        }
        else // mParam_F2_asFirstFilter
        {
            double in, lpAout, hpAout, lpBout, hpBout, lpCout, hpCout;
            in = input1[n];
            pqcrossover_tilde_setcrossf(ch1A, freq2, sr);
            lpAout = pqcrossover_tilde_lp(ch1A, in);
            hpAout = pqcrossover_tilde_hp(ch1A, in);
            pqcrossover_tilde_setcrossf(ch1B, freq1, sr);
            lpBout = pqcrossover_tilde_lp(ch1B, lpAout);
            hpBout = pqcrossover_tilde_hp(ch1B, lpAout);
            pqcrossover_tilde_setcrossf(ch1C, freq3, sr);
            lpCout = pqcrossover_tilde_lp(ch1C, hpAout);
            hpCout = pqcrossover_tilde_hp(ch1C, hpAout);
            if (mBypass)
                output1[n] = in;
            else
                output1[n] = ((lpBout * mParam_A) + (hpBout * mParam_B) +
                    (lpCout * mParam_C) + (hpCout * mParam_D)) * mfade;
            if(mIsStereo)
            {
                in = input2[n];
                pqcrossover_tilde_setcrossf(ch2A, freq2, sr);
                lpAout = pqcrossover_tilde_lp(ch2A, in);
                hpAout = pqcrossover_tilde_hp(ch2A, in);
                pqcrossover_tilde_setcrossf(ch2B, freq1, sr);
                lpBout = pqcrossover_tilde_lp(ch2B, lpAout);
                hpBout = pqcrossover_tilde_hp(ch2B, lpAout);
                pqcrossover_tilde_setcrossf(ch2C, freq3, sr);
                lpCout = pqcrossover_tilde_lp(ch2C, hpAout);
                hpCout = pqcrossover_tilde_hp(ch2C, hpAout);
                if (mBypass)
                    output2[n] = in;
                else
                    output2[n] = ((lpBout * mParam_A) + (hpBout * mParam_B) +
                    (lpCout * mParam_C) + (hpCout * mParam_D)) * mfade;
            }
        }
        // fade
        if (mfade < 1.)
        {
            audiolog += fadeStep;
            mfade = audiolog * audiolog;
		}
        if (mfade > 1.)
            mfade = 1.;
    }


    #endif

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setState (IBStream* state)
{
    if (!state)
        return kResultFalse;

    // called when we load a preset or project, the model has to be reloaded

    IBStreamer streamer (state, kLittleEndian);

    float Split_1= 0.f;
    if (streamer.readFloat (Split_1) == false)
        return kResultFalse;

    float Split_2= 0.f;
    if (streamer.readFloat (Split_2) == false)
        return kResultFalse;

    float Split_3= 0.f;
    if (streamer.readFloat (Split_3) == false)
        return kResultFalse;

    int32 BypassState = 0;
    if (streamer.readInt32 (BypassState) == false)
        return kResultFalse;

    int32 Param_A = 0;
    if (streamer.readInt32 (Param_A) == false)
        return kResultFalse;

    int32 Param_B = 0;
    if (streamer.readInt32 (Param_B) == false)
        return kResultFalse;

    int32 Param_C = 0;
    if (streamer.readInt32 (Param_C) == false)
        return kResultFalse;

    int32 Param_D = 0;
    if (streamer.readInt32 (Param_D) == false)
        return kResultFalse;

    int32 Param_F2_first = 0;
    if (streamer.readInt32 (Param_F2_first) == false)
        return kResultFalse;

    mSplit_1 = Split_1;
    mSplit_2 = Split_2;
    mSplit_3 = Split_3;
    mBypass = BypassState > 0;
    mParam_A = Param_A > 0;
    mParam_B = Param_B > 0;
    mParam_C = Param_C > 0;
    mParam_D = Param_D > 0;
    mParam_F2_asFirstFilter = Param_F2_first > 0;


    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::getState (IBStream* state)
{
    // here we need to save the model (preset or project)

    float Split_1 = mSplit_1;
    float Split_2 = mSplit_2;
    float Split_3 = mSplit_3;
    int32 BypassState = mBypass ? 1 : 0;
    int32 Param_A = mParam_A ? 1 : 0;
    int32 Param_B = mParam_B ? 1 : 0;
    int32 Param_C = mParam_C ? 1 : 0;
    int32 Param_D = mParam_D ? 1 : 0;
    int32 Param_F2_first = mParam_F2_asFirstFilter ? 1 : 0;

    IBStreamer streamer (state, kLittleEndian);
    streamer.writeFloat (Split_1);
    streamer.writeFloat (Split_2);
    streamer.writeFloat (Split_3);
    streamer.writeInt32 (BypassState);
    streamer.writeInt32 (Param_A);
    streamer.writeInt32 (Param_B);
    streamer.writeInt32 (Param_C);
    streamer.writeInt32 (Param_D);
    streamer.writeInt32 (Param_F2_first);

    return kResultOk;
}

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
