/*  
    bowedbar.c:

    Copyright (C) 1999 Perry Cook, Georg Essl, John ffitch

    This file is part of Csound.

    The Csound Library is free software; you can redistribute it
    and/or modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    Csound is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Csound; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
*/

/*********************************************/
/*  Bowed Bar model                          */
/*  by Georg Essl, 1999                      */
/*  For details refer to:                    */
/*    G.Essl, P.R.Cook: "Banded Waveguides:  */
/*    Towards Physical Modelling of Bar      */
/*    Percussion Instruments", ICMC'99       */
/*********************************************/

#include "csdl.h"
#include "bowedbar.h"

#ifdef EXTRA_DEBUG
static void print_state(BOWEDBAR *p)
{
    printf("BowTabl\t%f %f\n", p->bowTabl.offSet, p->bowTabl.slope);
    printf("adsr\t%f %f %f %d %f %f %f %f\n",
           p->adsr.value, p->adsr.target, p->adsr.rate, p->adsr.state,
           p->adsr.attackRate, p->adsr.decayRate, p->adsr.sustainLevel,
           p->adsr.releaseRate);
    printf("BiQuad[0]\n%f %f %f %f %f %f %f %f\n", p->bandpass[0].gain,
           p->bandpass[0].inputs[0], p->bandpass[0].inputs[1],
           p->bandpass[0].lastOutput, p->bandpass[0].poleCoeffs[0],
           p->bandpass[0].poleCoeffs[1], p->bandpass[0].zeroCoeffs[0],
           p->bandpass[0].zeroCoeffs[1]);
    printf("BiQuad[1]\n%f %f %f %f %f %f %f %f\n", p->bandpass[1].gain,
           p->bandpass[1].inputs[0], p->bandpass[1].inputs[1],
           p->bandpass[1].lastOutput, p->bandpass[1].poleCoeffs[0],
           p->bandpass[1].poleCoeffs[1], p->bandpass[1].zeroCoeffs[0],
           p->bandpass[1].zeroCoeffs[1]);
    printf("BiQuad[2]\n%f %f %f %f %f %f %f %f\n", p->bandpass[2].gain,
           p->bandpass[2].inputs[0], p->bandpass[2].inputs[1],
           p->bandpass[2].lastOutput, p->bandpass[2].poleCoeffs[0],
           p->bandpass[2].poleCoeffs[1], p->bandpass[2].zeroCoeffs[0],
           p->bandpass[2].zeroCoeffs[1]);
    printf("BiQuad[3]\n%f %f %f %f %f %f %f %f\n", p->bandpass[3].gain,
           p->bandpass[3].inputs[0], p->bandpass[3].inputs[1],
           p->bandpass[3].lastOutput, p->bandpass[3].poleCoeffs[0],
           p->bandpass[3].poleCoeffs[1], p->bandpass[3].zeroCoeffs[0],
           p->bandpass[3].zeroCoeffs[1]);

    printf("maxVelocity\t%f\n", p->maxVelocity);
    printf("modes[4]\t%f %f %f %f\n", p->modes[0], p->modes[1], p->modes[2],
           p->modes[3]);
/*     DLINEN      delay[4]; */
    printf("DLIN[0]\t%f %d %d %d\n", p->delay[0].lastOutput,
           p->delay[0].inPoint, p->delay[0].outPoint, p->delay[0].length);
    printf("DLIN[1]\t%f %d %d %d\n", p->delay[1].lastOutput,
           p->delay[1].inPoint, p->delay[1].outPoint, p->delay[1].length);
    printf("DLIN[2]\t%f %d %d %d\n", p->delay[2].lastOutput,
           p->delay[2].inPoint, p->delay[2].outPoint, p->delay[2].length);
    printf("DLIN[3]\t%f %d %d %d\n", p->delay[3].lastOutput,
           p->delay[3].inPoint, p->delay[3].outPoint, p->delay[3].length);

    printf("freq\t%f\n", p->freq);
    printf("nr_modes\t%d\n", p->nr_modes);
    printf("length\t%d\n", p->length);
    printf("gains[4]\t%f %f %f %f\n", p->gains[0], p->gains[1], p->gains[2],
           p->gains[3]);
    printf("velinput\t%f\n", p->velinput);
    printf("bowvel, bowTarg, lastBowPos\t%f %f %f\n", p->bowvel, p->bowTarg,
           p->lastBowPos);
    printf("lastpos\t%f\n", p->lastpos);
    printf("lastpress\t%f\n", p->lastpress);
    printf("kloop\t%d\n", p->kloop);
}
#endif

/* Number of banded waveguide modes */

static void make_DLineN(ENVIRON *csound, DLINEN *p, long length)
{
    int i;
    /* Writing before reading allows delays from 0 to length-1.
       Thus, if we want to allow a delay of max_length, we need
       a delay-line of length = max_length+1. */
    p->length = length = length+1;
    csound->auxalloc_(length * sizeof(MYFLT), &p->inputs);
    for (i=0; i<length; i++) ((MYFLT*)p->inputs.auxp)[i] = FL(0.0);
    p->inPoint = 0;
    p->outPoint = length >> 1;
    p->lastOutput = FL(0.0);
}

static void DLineN_setDelay(ENVIRON *csound, DLINEN *p, int lag)
{
    if (lag > p->length-1) {                   /* if delay is too big, */
      printf(csound->getstring_(X_1769,
                 "DLineN: Delay length too big ... setting to "
                 "maximum length of %ld.\n"),
             p->length-1);
      p->outPoint = p->inPoint + 1;            /* force delay to max_length */
    }
    else
      p->outPoint = p->inPoint - (long) lag;   /* read chases write */
    while (p->outPoint<0)
      p->outPoint += p->length;                /* modulo maximum length */
}

static void DLineN_tick(DLINEN *p, MYFLT sample) /*  Take one, yield one */
{
    ((MYFLT*)p->inputs.auxp)[p->inPoint++] = sample; /* Input next sample */
    if (p->inPoint == p->length)               /* Check for end condition */
      p->inPoint -= p->length;
    p->lastOutput = ((MYFLT*)p->inputs.auxp)[p->outPoint++]; /* Read nxt value */
    if (p->outPoint>=p->length)                /* Check for end condition */
      p->outPoint -= p->length;
/*     return p->lastOutput; */
}

int bowedbarset(BOWEDBAR *p)
{
    long i;
    MYFLT amplitude = *p->amp * AMP_RSCALE;
    ENVIRON *csound = p->h.insdshead->csound;

    p->modes[0] = FL(1.0);
    p->modes[1] = FL(2.756);
    p->modes[2] = FL(5.404);
    p->modes[3] = FL(8.933);

    make_BiQuad(&p->bandpass[0]);
    make_BiQuad(&p->bandpass[1]);
    make_BiQuad(&p->bandpass[2]);
    make_BiQuad(&p->bandpass[3]);

    ADSR_setAllTimes(csound, &p->adsr, FL(0.02), FL(0.005), FL(0.9), FL(0.01));

    if (*p->lowestFreq>=FL(0.0)) {      /* If no init skip */
      if (*p->lowestFreq!=FL(0.0))
        p->length = (long) (esr / *p->lowestFreq + FL(1.0));
      else if (*p->frequency!=FL(0.0))
        p->length = (long) (esr / *p->frequency + FL(1.0));
      else {
        err_printf(Str(X_512,
                       "unknown lowest frequency for bowed string -- "
                       "assuming 50Hz\n"));
        p->length = (long) (esr / FL(50.0) + FL(1.0));
      }
    }

    p->nr_modes = NR_MODES;
    for (i = 0; i<p->nr_modes; i++) {
      make_DLineN(p->h.insdshead->csound, &p->delay[i], p->length);
      DLineN_setDelay(p->h.insdshead->csound,
                      &p->delay[i], (int)(p->length/p->modes[i]));
      BiQuad_clear(&p->bandpass[i]);
    }
/*     p->gains[0] = FL(0.0); */
/*     p->gains[1] = FL(0.0); */
/*     p->gains[2] = FL(0.0); */
/*     p->gains[3] = FL(0.0); */
    p->adsr.target = FL(0.0);
    p->adsr.value = FL(0.0);
    p->adsr.rate = amplitude * FL(0.001);
    p->adsr.state = ATTACK;
    p->lastBowPos = FL(0.0);
    p->bowTarg = FL(0.0);
    p->freq = -FL(1.0);
    p->lastpos = -FL(1.0);
    p->lastpress = p->bowvel = p->velinput = FL(0.0);
    p->kloop = 0;
    p->bowTabl.offSet = p->bowTabl.slope = FL(0.0);
    return OK;
}

int bowedbar(BOWEDBAR *p)
{
    MYFLT       *ar = p->ar;
    long        nsmps = ksmps;
    MYFLT       amp = (*p->amp)*AMP_RSCALE; /* Normalise */
    long k;
    int i;
    MYFLT       maxVelocity;
    MYFLT       integration_const = *p->integration_const;
    ENVIRON    *csound = p->h.insdshead->csound;

    if (p->lastpress != *p->bowPress)
      p->bowTabl.slope = p->lastpress = *p->bowPress;
    if (p->freq != *p->frequency) {
      p->freq = *p->frequency;
      if (p->freq > FL(1568.0)) p->freq = FL(1568.0);

      p->length = (int)(esr/p->freq);
      p->nr_modes = NR_MODES;   /* reset for frequency shift */
      for (i = 0; i<p->nr_modes; i++) {
        if((int)(p->length/p->modes[i]) > 4)
          DLineN_setDelay(p->h.insdshead->csound,
                          &p->delay[i], (int)(p->length/p->modes[i]));
        else    {
          p->nr_modes = i;
          break;
        }
      }

      for (i=0; i<p->nr_modes; i++) {
        MYFLT R = FL(1.0) - p->freq * p->modes[i] * pidsr;
        BiQuad_clear(&p->bandpass[i]);
        BiQuad_setFreqAndReson(p->bandpass[i], p->freq * p->modes[i], R);
        BiQuad_setEqualGainZeroes(p->bandpass[i]);
        BiQuad_setGain(p->bandpass[i], (FL(1.0)-R*R)*FL(0.5));
      }
    }
                                /* Bow position as well */
    if (*p->position != p->lastpos) {
      double temp2 = (double)*p->position * PI_F;
      p->gains[0] = (MYFLT)fabs(sin(temp2 *0.5)) /*  * pow(0.9,0))*/;
      p->gains[1] = (MYFLT)fabs(sin(temp2) * 0.9);
      p->gains[2] = (MYFLT)fabs(sin(temp2 * 1.5) * 0.9*0.9);
      p->gains[3] = (MYFLT)fabs(sin(temp2 * 2) * 0.9 * 0.9 * 0.9);
      p->lastpos = *p->position;
    }
    if (*p->bowposition != p->lastBowPos) { /* Not sure what this control is? */
      p->bowTarg += FL(0.02) * (*p->bowposition - p->lastBowPos);
      p->lastBowPos = *p->bowposition;
      ADSR_setTarget(csound, &p->adsr, p->lastBowPos);
      p->lastBowPos = *p->bowposition;
    }
    if (p->kloop>0 && p->h.insdshead->relesing) p->kloop=1;
    if ((--p->kloop) == 0) {
      ADSR_setReleaseRate(csound, &p->adsr, (FL(1.0) - amp) * FL(0.005));
      p->adsr.target = FL(0.0);
      p->adsr.rate = p->adsr.releaseRate;
      p->adsr.state = RELEASE;
    }
    maxVelocity = FL(0.03) + (FL(0.5) * amp);

    do {
      MYFLT data = FL(0.0);
      MYFLT input = FL(0.0);
      if (integration_const == FL(0.0))
        p->velinput = FL(0.0);
      else
        p->velinput = integration_const * p->velinput;

      for (k=0; k<p->nr_modes; k++) {
        p->velinput += *p->GAIN * p->delay[k].lastOutput;
      }

      if (*p->trackVel) {
        p->bowvel *= FL(0.9995);
        p->bowvel += p->bowTarg;
        p->bowTarg *= FL(0.995);
      }
      else
        p->bowvel = ADSR_tick(&p->adsr)*maxVelocity;

      input = p->bowvel - p->velinput;
      input = input * BowTabl_lookup(csound, &p->bowTabl, input);
      input = input/(MYFLT)p->nr_modes;

      for (k=0; k<p->nr_modes; k++) {
        BiQuad_tick(&p->bandpass[k],
                    input*p->gains[k] + *p->GAIN * p->delay[k].lastOutput);
        DLineN_tick(&p->delay[k], p->bandpass[k].lastOutput);
        data += p->bandpass[k].lastOutput;
      }

      *ar++ = data * FL(4.0) * AMP_SCALE * FL(20.0);
    } while (--nsmps);
    return OK;
}

/*

void BowedBar_controlChange(BOWEDBAR *p, int number, MYFLT value)
{
    if (number == __SK_BowPressure_)
      {
        bowTabl->setSlope(FL(10.0) - (FL(9.0) * value * NORM_7));
        slope = FL(10.0) - (FL(9.0) * value * NORM_7);
      }
    else if (number == __SK_BowPosition_)
      {
        this->setStrikePosition(value*NORM_7);
      }
    else if (number == __SK_Balance_)   {
      bowPos = value * NORM_7;
      bowTarg += FL(0.02) * (bowPos - lastBowPos);
      lastBowPos = bowPos;
      adsr->setTarget(bowPos);
    }
    else if (number == __SK_AfterTouch_Cont_)
      {
        bowPos = value * NORM_7;
        bowTarg += 0.02 * (bowPos - lastBowPos);
        lastBowPos = bowPos;
        adsr->setTarget(bowPos);
      }
    else if (number == __SK_ModWheel_)
      {
        GAIN = FL(0.809) + (FL(0.2) * (value * NORM_7));
      }
    else if(number == __SK_ModFrequency_)
      {
        integration_const_ = value * NORM_7;

      }
}
 */

#define S       sizeof

static OENTRY localops[] = {
  { "wgbowedbar", S(BOWEDBAR), 5, "a", "kkkkkoooo",
    (SUBR)bowedbarset, NULL, (SUBR)bowedbar }
};

LINKAGE
