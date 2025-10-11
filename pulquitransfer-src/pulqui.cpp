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
