// FILE: handle_signals.h
// AUTHOR: Marc J. Rochkind, Samuel Pinkus
// USERNAME: NA, 13120548
// UNIT: SPD251
// PURPOSE: manage signals.
// REFERENCE: Advanced Unix Programming, M.J. Rochkind
// COMMENTS: this is nearly a complete copy of Rochkind's code available here: http://basepath.com/aup/ex/sigmin_8c-source.html
  //This header file contains the actual function definitions so every .c file including this will  hvae it own copy of the function.
  //Only a .c containing a main fn should include this though.
  //Saves the hassle of having to link in: gcc xx.c sighandle.c.
// LAST MOD: 20/05/08

#ifndef  _HANDLE_SIGNALS_H_
#define _HANDLE_SIGNALS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ec_simple.h"
#include <signal.h>
#include <stdbool.h>

typedef void (* sighandler_t) (int);
extern bool handle_signals(void(*cleanup)());
extern sighandler_t set_sig_handler(int signo, sighandler_t func);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //_HANDLE_SIGNALS_H_
