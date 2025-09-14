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

namespace Steinberg {
namespace Vst {

// HERE are defined the parameter Ids which are exported to the host
enum PulquiLimiterParams : Vst::ParamID
{
	kBypassId = 100,

	kParamTreshId = 102,
	kParamSrateId = 103,
	kParamLatencyBypassId = 104,
	kParamMakeUpId = 105,
};


// HERE you have to define new unique class ids: for processor and for controller
// you can use GUID creator tools like https://www.guidgenerator.com/
static const FUID MyProcessorUID (0xF857CCDC, 0xF86D4E53, 0xAB13DE1D, 0x1CD85B61);
static const FUID MyControllerUID (0x2F7F6470, 0x96184F58, 0x827E2057, 0xE685A17E);

//------------------------------------------------------------------------
} // namespace PulquiLimiter
} // namespace Steinberg
