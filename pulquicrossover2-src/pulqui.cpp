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

    x->lp.tempy=x->lp.a0*x->lp.tempx+x->lp.a1*x->lp.xm1+x->lp.a2*
        x->lp.xm2+x->lp.a3*x->lp.xm3+x->lp.a4*x->lp.xm4-x->b1*
        x->lp.ym1-x->b2*x->lp.ym2-x->b3*x->lp.ym3-x->b4*x->lp.ym4;
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

    x->hp.tempy=x->hp.a0*x->hp.tempx+x->hp.a1*x->hp.xm1+x->hp.a2*
        x->hp.xm2+x->hp.a3*x->hp.xm3+x->hp.a4*x->hp.xm4-x->b1*
        x->hp.ym1-x->b2*x->hp.ym2-x->b3*x->hp.ym3-x->b4*x->hp.ym4;
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
    if (freq < 20. || freq > 20000.)
        return;
    
    if (freq != x->fc || samplerate != x->srate)
    {
        x->fc = freq;
        x->srate = samplerate;
        pqcrossover_setup_filter(x);
    }
}


} // namespace
} // namespace Steinberg
