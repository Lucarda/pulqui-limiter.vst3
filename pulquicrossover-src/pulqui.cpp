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

void PlugProcessor::pq_bee32(Buffer* self)
{
    int pos;
    int startpos;
    int endpos;
    double peakIEEE;
    startpos = 0;
    endpos = 0;
    pos = 0;

    LOOP:while (pos < PULQUI_SCAN_SIZE)
    {
        if ( self->x_ramchpositive[pos] > 0.0001) break;
        pos++;
    }
    startpos = pos;
    peakIEEE = 0;
    while (pos < PULQUI_SCAN_SIZE)
    {
        if (self->x_ramchpositive[pos] > peakIEEE) peakIEEE = self->x_ramchpositive[pos];
        if ( self->x_ramchpositive[pos] < 0.0001) break;
        pos++;
    }
    endpos = pos;
    for (pos = startpos; pos < endpos ; pos++)
    {
        self->x_ramchpositive[pos] = peakIEEE;
    }
    //endpos = pos;
    if (pos < PULQUI_SCAN_SIZE) goto LOOP;
}

void PlugProcessor::pq_bee32_negative(Buffer* self)
{
    int pos;
    int startpos;
    int endpos;
    double peakIEEE;
    startpos = 0;
    endpos = 0;
    pos = 0;

    LOOP:while (pos < PULQUI_SCAN_SIZE)
    {
        if ( self->x_ramchnegative[pos] < -0.0001) break;
        pos++;
    }
    startpos = pos;
    peakIEEE = 0;
    while (pos < PULQUI_SCAN_SIZE)
    {
        if (self->x_ramchnegative[pos] < peakIEEE) peakIEEE = self->x_ramchnegative[pos];
        if ( self->x_ramchnegative[pos] > -0.0001) break;
        pos++;
    }
    endpos = pos;
    for (pos = startpos; pos < endpos ; pos++)
    {
        self->x_ramchnegative[pos] = peakIEEE;
    }
    //endpos = pos;
    if (pos < PULQUI_SCAN_SIZE) goto LOOP;
}

void PlugProcessor::pulqui_tilde_do_pulqui(Buffer* self)
{
    int i;
    for (i = 0; i < PULQUI_SIZE; i++)
    {
         self->x_ramchpositive[PULQUI_SIZE + i] = self->x_ramch[i];
         self->x_ramchnegative[PULQUI_SIZE + i] = self->x_ramch[i];
    }

    pq_bee32(self);
    pq_bee32_negative(self);

    for (i = 0; i < PULQUI_SIZE; i++)
    {
        self->x_bufsignalout[i] = self->x_bufsignal[i];
        if (self->x_ramchpositive[i] >  0.0001)
        {
            self->x_bufpulqui[i] = self->x_ramchpositive[i];
        }
        else if (self->x_ramchnegative[i] <  -0.0001)
        {
            self->x_bufpulqui[i] = self->x_ramchnegative[i] * -1;
        }
        else
        {
            self->x_bufpulqui[i] = 1;
        }
    }

    for (i = 0; i < PULQUI_SIZE; i++)
    {
         self->x_ramchpositive[i] = self->x_ramchpositive[PULQUI_SIZE + i];
         self->x_ramchnegative[i] = self->x_ramchnegative[PULQUI_SIZE + i];
    }

    for (i = 0; i < PULQUI_SIZE; i++)
    {
        self->x_bufsignal[i] = self->x_ramch[i];
    }
}


void PlugProcessor::pulqui(Buffer* self, int32 nSamples)
{
	int n_samples = (int)nSamples;
	double thresh = mThreshValue;
	double f;
	
	for (int i = 0; i < n_samples; i++)
    {
        self->x_ramch[i + self->x_pulquiblock] = self->x_input[i];
        if(mLatencyBypass)
        {
            self->x_output[i] = self->x_bufsignalout[i + self->x_pulquiblock];
        }
        else
        {
            if (self->x_bufpulqui[i + self->x_pulquiblock] > \
            thresh)
                f = self->x_bufsignalout[i + self->x_pulquiblock]*\
                (thresh / self->x_bufpulqui[i + self->x_pulquiblock]);
            else 
                f = self->x_bufsignalout[i + self->x_pulquiblock];
            if (mMakeUp)
                self->x_output[i] = f*(0.998/thresh);
            else
            self->x_output[i] = f;
        }
    }

    if(self->x_pulquiblock > ((PULQUI_SIZE - n_samples) - 1))
    {
        pulqui_tilde_do_pulqui(self);
        self->x_pulquiblock = 0;
    }
    else self->x_pulquiblock += n_samples;
}

} // namespace
} // namespace Steinberg
