#include "../include/plugprocessor.h"

namespace Steinberg {
namespace Vst{

void PlugProcessor::pq_bee32(Buffer* self)
{
    int pos;
    int startpos;
    int endpos;
    t_sample peakIEEE;
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
    t_sample peakIEEE;
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

} // namespace
} // namespace Steinberg
