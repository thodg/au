#include <stdio.h>
#include <err.h>

#include "portaudio.h"

#include "au.h"
#include "typed.h"

const double    g_2pi = M_PI * 2.0;
unsigned long   g_freq;
double          g_2pi_by_freq;

typedef struct state
{
  PaStream *stream;
  s_typed *out[2];
} s_state;

int au_cb (const void *inputBuffer, void *outputBuffer,
	   unsigned long framesPerBuffer,
	   const PaStreamCallbackTimeInfo *timeInfo,
	   PaStreamCallbackFlags statusFlags,
	   void *userData)
{
  float *outBuf = (float*) outputBuffer;
  unsigned long i = 0;
  s_state *state = (s_state*) userData;
  (void) inputBuffer;
  (void) timeInfo;
  (void) statusFlags;
  for (i = 0; i < framesPerBuffer; i++) {
    *outBuf++ = typed_to_double(state->out[0], i);
    *outBuf++ = typed_to_double(state->out[1], i);
  }
  return 0;
}

void pa_check_err (PaError e)
{
  if (e != paNoError)
    errx(e, "PortAudio: %s", Pa_GetErrorText(e));
}

void pa_check_warn (PaError e)
{
  if (e != paNoError)
    warnx("PortAudio: %s", Pa_GetErrorText(e));
}

void au_init (s_state *state)
{
  pa_check_err(Pa_Initialize());

  g_freq = 44100;
  g_2pi_by_freq = g_2pi / (double) g_freq;

  pa_check_err(Pa_OpenDefaultStream(&state->stream,
				    0, /* input */
				    2, /* stereo output */
				    paFloat32,
				    g_freq,
				    paFramesPerBufferUnspecified,
				    au_cb,
				    state));
}

void au_deinit (s_state *state)
{
  Pa_AbortStream(state->stream);
  pa_check_warn(Pa_Terminate());
}

void ev_loop (s_state *state)
{
  int c;
  pa_check_err(Pa_StartStream(state->stream));
 loop:
  c = fgetc(stdin);
  if (c == 'q' || c == 'Q' || c == 27 || c == EOF) {
    Pa_StopStream(state->stream);
    return;
  } else {
    goto loop;
  }
}

int main (int argc, char *argv[])
{
  s_dbl dbl[3];
  s_add add[1];
  s_mul mul[1];
  s_osc osc[3];
  s_square sqr;
  s_state state;
  s_delay delay;
  (void) argc;
  (void) argv;
  init_dbl(&dbl[0], 200.0);
  init_dbl(&dbl[1], 0.5);
  init_dbl(&dbl[2], 100.0);
  init_add(&add[0]); {
    list_append(add[0].list, (s_typed*) &dbl[0]);
    list_append(add[0].list, (s_typed*) &mul[0]);
  }
  init_mul(&mul[0]); {
    list_append(mul[0].list, (s_typed*) &dbl[2]);
    list_append(mul[0].list, (s_typed*) &osc[2]);
  }
  init_osc(&osc[0], (s_typed*) &dbl[0]);
  init_osc(&osc[1], (s_typed*) &add[0]);
  init_osc(&osc[2], (s_typed*) &dbl[1]);
  init_square(&sqr, (s_typed*) &dbl[0]);
  init_delay(&delay, (s_typed*) &osc[1], 0.2, 0.5);
  state.out[0] = (s_typed*) &sqr;
  state.out[1] = (s_typed*) &delay;
  au_init(&state);
  ev_loop(&state);
  au_deinit(&state);
  return 0;
}
