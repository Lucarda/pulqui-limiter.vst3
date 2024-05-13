//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Examples
// Filename    : public.sdk/samples/vst/panner/source/plugprocessor.cpp
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

#include "../include/plugprocessor.h"
#include "../include/plugids.h"

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
	// we want a stereo Input and a Stereo Output
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
					getAudioInput (0)->setArrangement (inputs[0]);
					getAudioInput (0)->setName (STR16 ("Mono In"));
					getAudioOutput (0)->setArrangement (outputs[0]);
					getAudioOutput (0)->setName (STR16 ("Mono Out"));
				}
				mIsStereo = false;
				return kResultOk;
			}
		}
		// the host wants something else than Mono => Mono,
		// in this case we are always Stereo => Stereo
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
					getAudioInput (0)->setArrangement (inputs[0]);
					getAudioInput (0)->setName (STR16 ("Stereo In"));
					getAudioOutput (0)->setArrangement (outputs[0]);
					getAudioOutput (0)->setName (STR16 ("Stereo Out"));
					result = kResultTrue;
				}
				// the host want something different than 1->1 or 2->2 : in this case we want stereo
				else if (bus->getArrangement () != SpeakerArr::kStereo)
				{
					getAudioInput (0)->setArrangement (SpeakerArr::kStereo);
					getAudioInput (0)->setName (STR16 ("Stereo In"));
					getAudioOutput (0)->setArrangement (SpeakerArr::kStereo);
					getAudioOutput (0)->setName (STR16 ("Stereo Out"));
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
	delete ch1;
	delete ch2;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::terminate ()
{
	//delete ch1;
	//delete ch2;
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
					case PulquiLimiterParams::kParamTreshId:
						if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
						    kResultTrue)
							mThreshValue = value;
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
						
					case PulquiLimiterParams::kParamMakeUpId:
						if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) ==
						    kResultTrue)
							mMakeUp = (value > 0.5f);
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
	// a new value of VuMeter will be send to the host
	// (the host will send it back in sync to our controller for updating our editor)
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


	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setState (IBStream* state)
{
	if (!state)
		return kResultFalse;

	// called when we load a preset or project, the model has to be reloaded

	IBStreamer streamer (state, kLittleEndian);

	float savedPan= 0.f;
	if (streamer.readFloat (savedPan) == false)
		return kResultFalse;

	int32 savedBypass = 0;
	if (streamer.readInt32 (savedBypass) == false)
		return kResultFalse;
		
	int32 savedBypassLatency = 0;
	if (streamer.readInt32 (savedBypassLatency) == false)
		return kResultFalse;
		
	int32 savedMakeup = 0;
	if (streamer.readInt32 (savedMakeup) == false)
		return kResultFalse;

	mThreshValue = savedPan;
	mBypass = savedBypass > 0;
	mLatencyBypass = savedBypassLatency > 0;
	mMakeUp = savedMakeup > 0;

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::getState (IBStream* state)
{
	// here we need to save the model (preset or project)

	float toSavePan = mThreshValue;
	int32 toSaveBypass = mBypass ? 1 : 0;
	int32 toSavemLatencyBypass = mLatencyBypass ? 1 : 0;
	int32 toSavemMakeUp = mMakeUp ? 1 : 0;

	IBStreamer streamer (state, kLittleEndian);
	streamer.writeFloat (toSavePan);
	streamer.writeInt32 (toSaveBypass);
	streamer.writeInt32 (toSavemLatencyBypass);
	streamer.writeInt32 (toSavemMakeUp);
	

	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace
} // namespace Steinberg
