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
    double f, postmix;

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
            {
                f = self->x_bufsignalout[i + self->x_pulquiblock]*\
                (thresh / self->x_bufpulqui[i + self->x_pulquiblock]);
                postmix = (f * (1 - mMixValue)) + (self->x_bufsignalout[i + \
					self->x_pulquiblock] * mMixValue);
			}
            else
                postmix = self->x_bufsignalout[i + self->x_pulquiblock];
                
            self->x_output[i] = postmix * mVol;
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
