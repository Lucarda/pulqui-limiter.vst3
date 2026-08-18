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

    if(!ch1)
        ch1 = new Buffer();
    if(mIsStereo && !ch2)
        ch2 = new Buffer();

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
    if(ch1)
    {
        delete ch1;
        ch1 = NULL;
    }
    if(ch2 && mIsStereo)
    {
        delete ch2;
        ch2 = NULL;
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
                    case PulquiLimiterParams::kParamMixId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mMixValue = value;
                        break;

                    case PulquiLimiterParams::kBypassId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mBypass = (value > 0.5f);
                        break;

                    case PulquiLimiterParams::kParamLatencyBypassId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mLatencyBypass = (value > 0.5f);
                        break;

                    case PulquiLimiterParams::kParamVolId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mVol = value;
                        break;
                        
                    case PulquiLimiterParams::kParamInId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mIn = value;
                        break;
                        
                    case PulquiLimiterParams::kParamMultiInId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mMulti = value;
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

    // skip this
    /*
    // if we have only silence clear the output and do nothing.
    data.outputs->silenceFlags = data.inputs->silenceFlags ? 0x7FFFF : 0;
    if (data.inputs->silenceFlags)
    {
        for (int32 i = 0; i < data.numOutputs; i++)
        {
            memset (currentOutputBuffers[i], 0, sampleFramesSize);
        }
        return kResultOk;
    }
    */

    //---pulqui---------------------

    SampleType* input1 = currentInputBuffers[0];
    SampleType* input2 = currentInputBuffers[1];
    SampleType* output1 = currentOutputBuffers[0];
    SampleType* output2 = currentOutputBuffers[1];


    for (int32 n = 0; n < numFrames; n++)
    {
        ch1->x_input[n] = input1[n];
        if(mIsStereo)
            ch2->x_input[n] = input2[n];
    }

    pulqui(ch1, numFrames);
    if(mIsStereo)
        pulqui(ch2, numFrames);

    for (int32 n = 0; n < numFrames; n++)
    {
        output1[n] = ch1->x_output[n];
        if(mIsStereo)
            output2[n] = ch2->x_output[n];
    }


    //----------------

    double fsamplerate = processSetup.sampleRate;
    //---3) Write outputs parameter changes-----------
    IParameterChanges* outParamChanges = data.outputParameterChanges;
	// latency display
    if (outParamChanges && fsamplrateOld != fsamplerate)
    {
        int32 index = 0;
        IParamValueQueue* paramQueue = outParamChanges->addParameterData (kParamSrateId, index);
        if (paramQueue)
        {
            int32 index2 = 0;
            paramQueue->addPoint (0, (fsamplerate/1e+6), index2);
        }
    }
    fsamplrateOld = fsamplerate;

	// input factor display
    if (outParamChanges && mXMultiDisplayOld != mXMultiDisplay)
    {
        int32 index = 0;
        IParamValueQueue* paramQueue = outParamChanges->addParameterData (kParamInXDisplayId, index);
        if (paramQueue)
        {
            int32 index2 = 0;
            paramQueue->addPoint (0, mXMultiDisplay, index2);
        }
    }
    mXMultiDisplayOld = mXMultiDisplay;
    
	// Vu display
	if (outParamChanges)
    {
        int32 index = 0;
        IParamValueQueue* paramQueue = outParamChanges->addParameterData (kParamVu1LId, index);
        if (paramQueue)
        {
            int32 index2 = 0;
            paramQueue->addPoint (0, ch1->x_vu1, index2);
        }
    }
    if(outParamChanges && mIsStereo)
    {
        int32 index = 0;
        IParamValueQueue* paramQueue = outParamChanges->addParameterData (kParamVu1RId, index);
        if (paramQueue)
        {
            int32 index2 = 0;
            paramQueue->addPoint (0, ch2->x_vu1, index2);
        }
    }


    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setState (IBStream* state)
{
    if (!state)
        return kResultFalse;

    // called when we load a preset or project, the model has to be reloaded

    IBStreamer streamer (state, kLittleEndian);

    float savedMix= 0.f;
    if (streamer.readFloat (savedMix) == false)
        return kResultFalse;

    int32 savedBypass = 0;
    if (streamer.readInt32 (savedBypass) == false)
        return kResultFalse;

    int32 savedBypassLatency = 0;
    if (streamer.readInt32 (savedBypassLatency) == false)
        return kResultFalse;

    float savedVol = 0.f;
    if (streamer.readFloat (savedVol) == false)
        return kResultFalse;

    float savedMulti = 0.f;
    if (streamer.readFloat (savedMulti) == false)
        return kResultFalse;
        
    float savedIn = 0.f;
    if (streamer.readFloat (savedIn) == false)
        return kResultFalse;
        
    mMixValue = savedMix;
    mBypass = savedBypass > 0;
    mLatencyBypass = savedBypassLatency > 0;
    mVol = savedVol;
    mMulti = savedMulti;
    mIn = savedIn;

    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::getState (IBStream* state)
{
    // here we need to save the model (preset or project)

    float toSaveMix = mMixValue;
    int32 toSaveBypass = mBypass ? 1 : 0;
    int32 toSavemLatencyBypass = mLatencyBypass ? 1 : 0;
    float toSaveVol = mVol;
    float toSaveMulti = mMulti;
    float toSaveIn = mIn;


    IBStreamer streamer (state, kLittleEndian);
    streamer.writeFloat (toSaveMix);
    streamer.writeInt32 (toSaveBypass);
    streamer.writeInt32 (toSavemLatencyBypass);
    streamer.writeFloat (toSaveVol);
    streamer.writeFloat (toSaveMulti);
    streamer.writeFloat (toSaveIn);


    return kResultOk;
}

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
