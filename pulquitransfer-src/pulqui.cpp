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

namespace Steinberg {
namespace Vst{

double PlugProcessor::transfer_symetric(double in)
{

    int sign =  (in < 0) ? -1 : 1;
    double sample = (in < 0) ? in*-1 : in;
    if (sample > mParamPositiveTreshId)
        sample = ((sample - mParamPositiveTreshId) * mParamPositiveAttId)
                                                 + mParamPositiveTreshId;
    if (sign == -1)
        sample = sample * sign;

    if (mParamPositiveMakeUpId)
    {
        double factor = ((1. - mParamPositiveTreshId) * mParamPositiveAttId)
                                                 + mParamPositiveTreshId;
        sample = sample*(0.998/factor);
    }

    return sample;
}

double PlugProcessor::transfer_positive(double in)
{
    double sample = in;
    if (sample > mParamPositiveTreshId)
        sample = ((sample - mParamPositiveTreshId) * mParamPositiveAttId)
                                                 + mParamPositiveTreshId;
    if (mParamPositiveMakeUpId)
    {
        double factor = ((1. - mParamPositiveTreshId) * mParamPositiveAttId)
                                                 + mParamPositiveTreshId;
        sample = sample*(0.998/factor);
    }

    return sample;
}

double PlugProcessor::transfer_negative(double in)
{
    double sample = in*-1;
    if (sample > mParamNegativeTreshId)
        sample = ((sample - mParamNegativeTreshId) * mParamNegativeAttId)
                                                 + mParamNegativeTreshId;
    if (mParamNegativeMakeUpId)
    {
        double factor = ((1. - mParamNegativeTreshId) * mParamNegativeAttId)
                                                 + mParamNegativeTreshId;
        sample = sample*(0.998/factor);
    }

    return sample*-1;
}

double PlugProcessor::transfer(double in)
{
    if (mParamSymetricId)
        return transfer_symetric(in);
    if (in < 0)
        return transfer_negative(in);
    else
        return transfer_positive(in);
}

} // namespace
} // namespace Steinberg
