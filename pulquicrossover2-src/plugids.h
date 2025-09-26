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
enum PulquiCrossoverParams : Vst::ParamID
{
    kBypassId = 100,
    kParamSplit_1_Id = 101,
    kParam_A_Id = 102,
    kParam_B_Id = 103,
   
};


// HERE you have to define new unique class ids: for processor and for controller
// you can use GUID creator tools like https://www.guidgenerator.com/
static const FUID MyProcessorUID (0x111C5023, 0xB3514542, 0xAAE6FCEF, 0xC6F8E973);
static const FUID MyControllerUID (0xE0C08C14, 0x30CB4960, 0xAAA0C50E, 0x308D1239);

//------------------------------------------------------------------------
} // namespace PulquiLimiter
} // namespace Steinberg
