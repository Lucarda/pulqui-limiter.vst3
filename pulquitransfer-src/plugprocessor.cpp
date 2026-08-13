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


    return AudioEffect::setupProcessing (setup);
}

//------------------------------------------------------------------------
PlugProcessor::~PlugProcessor()
{

}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::terminate ()
{

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
                    case PulquiTransferParams::kBypassId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mBypassId = (value > 0.5f);
                        break;
                    case PulquiTransferParams::kParamPositiveTreshId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mParamPositiveTreshId = value;
                        break;
                    case PulquiTransferParams::kParamPositiveAttId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mParamPositiveAttId = value;
                        break;
                    case PulquiTransferParams::kParamPositiveMakeUpId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mParamPositiveMakeUpId = (value > 0.5f);
                        break;
                    case PulquiTransferParams::kParamNegativeTreshId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mParamNegativeTreshId = value;
                        break;
                    case PulquiTransferParams::kParamNegativeAttId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mParamNegativeAttId = value;
                        break;
                    case PulquiTransferParams::kParamNegativeMakeUpId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mParamNegativeMakeUpId = (value > 0.5f);
                        break;
                    case PulquiTransferParams::kParamSymetricId:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mParamSymetricId = (value > 0.5f);
                        break;
                    case PulquiTransferParams::kGuiViewToggle:
                        if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
                            kResultTrue)
                            mParamGUIAB = (value > 0.5f);
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
    SampleType* input2 = currentInputBuffers[1];
    SampleType* output1 = currentOutputBuffers[0];
    SampleType* output2 = currentOutputBuffers[1];

    if (mBypassId)
    {
        for (int32 n = 0; n < numFrames; n++)
        {
            output1[n] = input1[n];
            if(mIsStereo)
            {
                output2[n] = input2[n];
            }
        }
    }
    else
    {
        double sample;
        for (int32 n = 0; n < numFrames; n++)
        {
            sample = input1[n];
            output1[n] = transfer(sample);
            if(mIsStereo)
            {
                sample = input2[n];
                output2[n] = transfer(sample);
            }
        }
    }


    //----------------


    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setState (IBStream* state)
{
    if (!state)
        return kResultFalse;

    // called when we load a preset or project, the model has to be reloaded

    IBStreamer streamer (state, kLittleEndian);

    int32 SmBypassId = 0;
    if (streamer.readInt32 (SmBypassId) == false)
        return kResultFalse;
    float SmParamPositiveTreshId= 0.f;
    if (streamer.readFloat (SmParamPositiveTreshId) == false)
        return kResultFalse;
    float SmParamPositiveAttId= 0.f;
    if (streamer.readFloat (SmParamPositiveAttId) == false)
        return kResultFalse;
    int32 SmParamPositiveMakeUpId = 0;
    if (streamer.readInt32 (SmParamPositiveMakeUpId) == false)
        return kResultFalse;
    float SmParamNegativeTreshId= 0.f;
    if (streamer.readFloat (SmParamNegativeTreshId) == false)
        return kResultFalse;
    float SmParamNegativeAttId= 0.f;
    if (streamer.readFloat (SmParamNegativeAttId) == false)
        return kResultFalse;
    int32 SmParamNegativeMakeUpId = 0;
    if (streamer.readInt32 (SmParamNegativeMakeUpId) == false)
        return kResultFalse;
    int32 SmParamSymetricId = 0;
    if (streamer.readInt32 (SmParamSymetricId) == false)
        return kResultFalse;
    int32 SmParamGUIAB = 0;
    if (streamer.readInt32 (SmParamGUIAB) == false)
        return kResultFalse;

    mBypassId = SmBypassId > 0;
    mParamPositiveTreshId = SmParamPositiveTreshId;
    mParamPositiveAttId = SmParamPositiveAttId;
    mParamPositiveMakeUpId = SmParamPositiveMakeUpId > 0;
    mParamNegativeTreshId = SmParamNegativeTreshId;
    mParamNegativeAttId = SmParamNegativeAttId;
    mParamNegativeMakeUpId = SmParamNegativeMakeUpId > 0;
    mParamSymetricId = SmParamSymetricId > 0;
    mParamGUIAB = SmParamGUIAB > 0;


    return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::getState (IBStream* state)
{
    // here we need to save the model (preset or project)

    int32 SmBypass = mBypassId ? 1 : 0;
    float SmParamPositiveTreshId = mParamPositiveTreshId;
    float SmParamPositiveAttId = mParamPositiveAttId;
    int32 SmParamPositiveMakeUpId = mParamPositiveMakeUpId ? 1 : 0;
    float SmParamNegativeTreshId = mParamNegativeTreshId;
    float SmParamNegativeAttId = mParamNegativeAttId;
    int32 SmParamNegativeMakeUpId = mParamNegativeMakeUpId ? 1 : 0;
    int32 SmParamSymetricId = mParamSymetricId ? 1 : 0;
    int32 SmParamGUIAB = mParamGUIAB ? 1 : 0;


    IBStreamer streamer (state, kLittleEndian);

    streamer.writeInt32 (SmBypass);
    streamer.writeFloat (SmParamPositiveTreshId);
    streamer.writeFloat (SmParamPositiveAttId);
    streamer.writeInt32 (SmParamPositiveMakeUpId);
    streamer.writeFloat (SmParamNegativeTreshId);
    streamer.writeFloat (SmParamNegativeAttId);
    streamer.writeInt32 (SmParamNegativeMakeUpId);
    streamer.writeInt32 (SmParamSymetricId);
    streamer.writeInt32 (SmParamGUIAB);


    return kResultOk;
}

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
