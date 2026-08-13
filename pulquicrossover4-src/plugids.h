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
    k_F2_asFirstFilter_Id = 108,    
};


// HERE you have to define new unique class ids: for processor and for controller
// you can use GUID creator tools like https://www.guidgenerator.com/
static const FUID MyProcessorUID (0x9404534C, 0x75C14A9B, 0xBE82F197, 0xC1FE35C5);
static const FUID MyControllerUID (0xE5E874D3, 0x48494A4F, 0x9710673B, 0x9D6E8F3F);

//------------------------------------------------------------------------
} // namespace PulquiLimiter
} // namespace Steinberg
