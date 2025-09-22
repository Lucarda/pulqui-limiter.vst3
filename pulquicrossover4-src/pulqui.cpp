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


/*
 * 4th order Linkwitz-Riley filters
 * taken from pseudo code in:
 * https://www.musicdsp.org/en/latest/Filters/266-4th-order-linkwitz-riley-filters.html
 * (the first example)
 */

#include "./plugprocessor.h"
#include <math.h>

namespace Steinberg {
namespace Vst{


double PlugProcessor::pqcrossover_tilde_lp(Buffer *x, double in)
{
    x->lp.tempx=in;

    x->lp.tempy=x->lp.a0*x->lp.tempx+x->lp.a1*x->lp.xm1+x->lp.a2*x->lp.xm2+x->lp.a3*x->lp.xm3+x->lp.a4*x->lp.xm4-x->b1*x->lp.ym1-x->b2*x->lp.ym2-x->b3*x->lp.ym3-x->b4*x->lp.ym4;
    x->lp.xm4=x->lp.xm3;
    x->lp.xm3=x->lp.xm2;
    x->lp.xm2=x->lp.xm1;
    x->lp.xm1=x->lp.tempx;
    x->lp.ym4=x->lp.ym3;
    x->lp.ym3=x->lp.ym2;
    x->lp.ym2=x->lp.ym1;
    x->lp.ym1=x->lp.tempy;

    return (x->lp.tempy);
}


double PlugProcessor::pqcrossover_tilde_hp(Buffer *x, double in)
{
    x->hp.tempx=in;

    x->hp.tempy=x->hp.a0*x->hp.tempx+x->hp.a1*x->hp.xm1+x->hp.a2*x->hp.xm2+x->hp.a3*x->hp.xm3+x->hp.a4*x->hp.xm4-x->b1*x->hp.ym1-x->b2*x->hp.ym2-x->b3*x->hp.ym3-x->b4*x->hp.ym4;
    x->hp.xm4=x->hp.xm3;
    x->hp.xm3=x->hp.xm2;
    x->hp.xm2=x->hp.xm1;
    x->hp.xm1=x->hp.tempx;
    x->hp.ym4=x->hp.ym3;
    x->hp.ym3=x->hp.ym2;
    x->hp.ym2=x->hp.ym1;
    x->hp.ym1=x->hp.tempy;

    return (x->hp.tempy);
}

void PlugProcessor::pqcrossover_setup_filter(Buffer *x)
{
    //------------------------------
    x->wc=2*x->pi*x->fc;
    x->wc2=x->wc*x->wc;
    x->wc3=x->wc2*x->wc;
    x->wc4=x->wc2*x->wc2;
    x->k=x->wc/tan(x->pi*x->fc/x->srate);
    x->k2=x->k*x->k;
    x->k3=x->k2*x->k;
    x->k4=x->k2*x->k2;
    x->sqrt2=sqrt(2);
    x->sq_tmp1=x->sqrt2*x->wc3*x->k;
    x->sq_tmp2=x->sqrt2*x->wc*x->k3;
    x->a_tmp=4*x->wc2*x->k2+2*x->sq_tmp1+x->k4+2*x->sq_tmp2+x->wc4;

    x->b1=(4*(x->wc4+x->sq_tmp1-x->k4-x->sq_tmp2))/x->a_tmp;
    x->b2=(6*x->wc4-8*x->wc2*x->k2+6*x->k4)/x->a_tmp;
    x->b3=(4*(x->wc4-x->sq_tmp1+x->sq_tmp2-x->k4))/x->a_tmp;
    x->b4=(x->k4-2*x->sq_tmp1+x->wc4-2*x->sq_tmp2+4*x->wc2*x->k2)/x->a_tmp;

    //================================================
    // low-pass
    //================================================
    x->lp.a0=x->wc4/x->a_tmp;
    x->lp.a1=4*x->wc4/x->a_tmp;
    x->lp.a2=6*x->wc4/x->a_tmp;
    x->lp.a3=x->lp.a1;
    x->lp.a4=x->lp.a0;
    //=====================================================
    // high-pass
    //=====================================================
    x->hp.a0=x->k4/x->a_tmp;
    x->hp.a1=-4*x->k4/x->a_tmp;
    x->hp.a2=6*x->k4/x->a_tmp;
    x->hp.a3=x->hp.a1;
    x->hp.a4=x->hp.a0;
    //------------------------------
}


void PlugProcessor::pqcrossover_tilde_setcrossf(Buffer *x, double freq, 
                                                        double samplerate)
{
    if (freq < 20. || freq > 20000.) return;
    if (freq != x->fc || samplerate != x->srate || mParam_F2_asFirstFilter != m_oldParam_F2_asFirstFilter)
    {
        x->fc = freq;
        x->srate = samplerate;
        m_oldParam_F2_asFirstFilter = mParam_F2_asFirstFilter;
        pqcrossover_setup_filter(x);
    }
}


} // namespace
} // namespace Steinberg
