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
