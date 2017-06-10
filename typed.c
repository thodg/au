#include <err.h>
#include <stdlib.h>
#include "typed.h"

void list_append (s_typed *list[], s_typed *item)
{
  int i;
  for (i = 0; i < MAX_LIST - 1 && list[i]; i++)
    ;
  if (i >= MAX_LIST - 1 || list[i])
    err(1, "MAX_LIST reached");
  list[i] = item;
  list[i + 1] = 0;
}

/* init functions */

void init_typed  (s_typed *typed, e_type t)
{
  assert(typed);
  typed->t = t;
  typed->frame = -1;
}

void init_dbl    (s_dbl *dbl, double d)
{
  init_typed((s_typed*) dbl, T_DBL);
  dbl->d = d;
}

void init_add    (s_add *add)
{
  int i;
  init_typed((s_typed*) add, T_ADD);
  add->sum = 0;
  for (i = 0; i < MAX_LIST; i++)
    add->list[i] = 0;
}

void init_mul    (s_mul *mul)
{
  int i;
  init_typed((s_typed*) mul, T_MUL);
  mul->product = 0;
  for (i = 0; i < MAX_LIST; i++)
    mul->list[i] = 0;
}

void init_osc    (s_osc *osc, s_typed *freq)
{
  init_typed((s_typed*) osc, T_OSC);
  osc->sinus = 0;
  osc->phase = 0;
  osc->freq = freq;
}

void init_square (s_square *s, s_typed *freq)
{
  init_typed((s_typed*) s, T_SQUARE);
  s->s = 1;
  s->phase = 0;
  s->freq = freq;
}

void init_delay (s_delay *d, s_typed *input, double delay, double feedback)
{
  init_typed((s_typed*) d, T_DELAY);
  d->input = input;
  d->delay = delay;
  d->feedback = feedback;
  d->buffer_frames = (unsigned int) (delay * g_freq) + 1;
  d->buffer = malloc(d->buffer_frames * sizeof(double));
  d->index = 0;
}

/* to_double cast methods */

double  typed_to_double (s_typed *typed, t_frame frame)
{
  assert_typed(typed);
  f_typed_to_double to_double = typed_to_double_[typed->t];
  return to_double(typed, frame);
}

double   void_to_double (s_typed *typed, t_frame frame)
{
  assert_typed(typed);
  (void) frame;
  return (double) 0.0;
}

double    dbl_to_double (s_typed *typed, t_frame frame)
{
  assert_typed(typed);
  s_dbl *d = (s_dbl*) typed;
  (void) frame;
  return d->d;
}

double    add_to_double (s_typed *typed, t_frame frame)
{
  assert_typed_type(typed, T_ADD);
  s_add *add = (s_add*) typed;
  if (add->frame != frame) {
    int i;
    add->sum = 0.0;
    for (i = 0; i < MAX_LIST && add->list[i]; i++) {
      add->sum += typed_to_double(add->list[i], frame);
    }
  }
  return add->sum;
}

double    mul_to_double (s_typed *typed, t_frame frame)
{
  assert_typed_type(typed, T_MUL);
  s_mul *mul = (s_mul*) typed;
  if (mul->frame != frame) {
    int i;
    mul->product = 1.0;
    for (i = 0; i < MAX_LIST && mul->list[i]; i++) {
      mul->product *= typed_to_double(mul->list[i], frame);
    }
  }
  return mul->product;
}

double    osc_to_double (s_typed *typed, t_frame frame)
{
  s_osc *osc;
  assert_typed_type(typed, T_OSC);
  osc = (s_osc*) typed;
  if (osc->frame != frame) {
    double freq = typed_to_double(osc->freq, frame);
    osc->phase += freq * g_2pi_by_freq;
    osc->phase = fmod(osc->phase, g_2pi);
    osc->sinus = sin(osc->phase);
  }
  return osc->sinus;
}

double    square_to_double (s_typed *typed, t_frame frame)
{
  s_square *sqr;
  assert_typed_type(typed, T_SQUARE);
  sqr = (s_square*) typed;
  if (sqr->frame != frame) {
    double freq = typed_to_double(sqr->freq, frame);
    sqr->phase += freq * g_2pi_by_freq;
    sqr->phase = fmod(sqr->phase, g_2pi);
    if (sqr->phase < M_PI)
      sqr->s = -1;
    else
      sqr->s = 1;
  }
  return sqr->s;
}

double delay_to_double (s_typed *typed, t_frame frame)
{
  s_delay *d;
  assert_typed_type(typed, T_DELAY);
  d = (s_delay*) typed;
  if (d->frame != frame) {
    d->buffer[d->index] = d->buffer[d->index] * d->feedback +
      typed_to_double(d->input, frame);
    d->index++;
    if (d->index > d->buffer_frames)
      d->index = 0;
  }
  return d->buffer[d->index];
}
