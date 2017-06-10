#ifndef AU_TYPED_H
#define AU_TYPED_H

#include <assert.h>
#include "au.h"

typedef enum {
  T_VOID,
  T_DBL,
  T_ADD,
  T_MUL,
  T_OSC,
  T_SQUARE,
  T_DELAY,
  MAX_TYPE
} e_type;

typedef struct typed {
  e_type        t;
  t_frame       frame;
} s_typed;

#define assert_typed(x) assert(x); assert(x->t >= 0); assert(x->t < MAX_TYPE)
#define assert_typed_type(x, y) assert(x); assert(x->t == y)

#define MAX_LIST 64
void list_append (s_typed *list[], s_typed *item);

typedef struct dbl {
  e_type        t;
  t_frame       frame;
  double        d;
} s_dbl;

typedef struct add {
  e_type        t;
  t_frame       frame;
  double        sum;
  s_typed      *list[MAX_LIST];
} s_add;


typedef struct mul {
  e_type        t;
  t_frame       frame;
  double        product;
  s_typed      *list[MAX_LIST];
} s_mul;

typedef struct osc {
  e_type        t;
  t_frame       frame;
  double        sinus;
  double        phase;
  s_typed      *freq;
} s_osc;

typedef struct square {
  e_type        t;
  t_frame       frame;
  signed char   s;
  double        phase;
  s_typed      *freq;
} s_square;

typedef struct delay {
  e_type        t;
  t_frame       frame;
  s_typed      *input;
  double        delay;
  double        feedback;
  double       *buffer;
  unsigned int  buffer_frames;
  unsigned int  index;
} s_delay;

/* init functions */

void init_dbl    (s_dbl *dbl, double d);
void init_add    (s_add *add);
void init_mul    (s_mul *mul);
void init_osc    (s_osc *osc, s_typed *freq);
void init_square (s_square *s, s_typed *freq);
void init_delay  (s_delay *d, s_typed *input, double delay, double feedback);

/* to_double cast methods */

double  typed_to_double (s_typed *typed, t_frame frame);
double   void_to_double (s_typed *typed, t_frame frame);
double    dbl_to_double (s_typed *typed, t_frame frame);
double    add_to_double (s_typed *typed, t_frame frame);
double    mul_to_double (s_typed *typed, t_frame frame);
double    osc_to_double (s_typed *typed, t_frame frame);
double square_to_double (s_typed *typed, t_frame frame);
double  delay_to_double (s_typed *typed, t_frame frame);

typedef double (*f_typed_to_double) (s_typed *typed, t_frame frame);

static const f_typed_to_double typed_to_double_[] = {
    void_to_double,
     dbl_to_double,
     add_to_double,
     mul_to_double,
     osc_to_double,
  square_to_double,
   delay_to_double
};

#endif /* AU_TYPED_H */
