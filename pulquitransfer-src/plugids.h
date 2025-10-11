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
enum PulquiTransferParams : Vst::ParamID
{
    kBypassId = 100,
    kParamPositiveTreshId = 101,
    kParamPositiveAttId = 102,
    kParamPositiveMakeUpId = 103,
    kParamNegativeTreshId = 104,
    kParamNegativeAttId = 105,
    kParamNegativeMakeUpId = 106,
    kParamSymetricId = 107,
    kGuiViewToggle = 1000,
};


// HERE you have to define new unique class ids: for processor and for controller
// you can use GUID creator tools like https://www.guidgenerator.com/
static const FUID MyProcessorUID (0xDB4DA12F, 0xE67B468E, 0xBD97BF4C, 0xC909372E);
static const FUID MyControllerUID (0x5D3C0EC2, 0xACE04556, 0x8AB2BB5D, 0xC51EF1C9);

//------------------------------------------------------------------------
} // namespace PulquiLimiter
} // namespace Steinberg
