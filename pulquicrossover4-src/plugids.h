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
    kParamSplit_2_Id = 102,
    kParamSplit_3_Id = 103,
    kParam_A_Id = 104,
    kParam_B_Id = 105,
    kParam_C_Id = 106,
    kParam_D_Id = 107,
};


// HERE you have to define new unique class ids: for processor and for controller
// you can use GUID creator tools like https://www.guidgenerator.com/
static const FUID MyProcessorUID (0x9404534C, 0x75C14A9B, 0xBE82F197, 0xC1FE35C5);
static const FUID MyControllerUID (0xE5E874D3, 0x48494A4F, 0x9710673B, 0x9D6E8F3F);

//------------------------------------------------------------------------
} // namespace PulquiLimiter
} // namespace Steinberg
