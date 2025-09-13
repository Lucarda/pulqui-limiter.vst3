/*
 * Copyright (C) 2024-2025 Lucas Cordiviola
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

#include "vstgui/plugin-bindings/vst3editor.h"
#include "public.sdk/source/vst/vsteditcontroller.h"
#include "pluginterfaces/vst/ivstparameterfunctionname.h"

namespace Steinberg {
namespace Vst {

//-----------------------------------------------------------------------------
class PlugController : public Vst::EditController,
                       public VSTGUI::VST3EditorDelegate,
                       public Vst::IParameterFunctionName
{
public:
//------------------------------------------------------------------------
	// create function required for plug-in factory,
	// it will be called to create new instances of this controller
//------------------------------------------------------------------------
	static FUnknown* createInstance (void*)
	{
		return (Vst::IEditController*)new PlugController ();
	}

	//---from IPluginBase--------
	tresult PLUGIN_API initialize (FUnknown* context) SMTG_OVERRIDE;

	//---from EditController-----
	IPlugView* PLUGIN_API createView (const char* name) SMTG_OVERRIDE;
	tresult PLUGIN_API setComponentState (IBStream* state) SMTG_OVERRIDE;

	//---from IParameterFunctionName----
	tresult PLUGIN_API getParameterIDFromFunctionName (Vst::UnitID unitID, FIDString functionName,
	                                                   Vst::ParamID& paramID) override;


	OBJ_METHODS (PlugController, Vst::EditController)
	DEFINE_INTERFACES
		DEF_INTERFACE (Vst::IParameterFunctionName)
	END_DEFINE_INTERFACES (Vst::EditController)
	DELEGATE_REFCOUNT (Vst::EditController)
	

//-----------------------------------------------------------------------------
protected:
	double rsamplerate;

};

//------------------------------------------------------------------------
} // namespace PulquiLimiter
} // namespace Steinberg
