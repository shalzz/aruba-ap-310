/* ----------------------------------------------------------------------- 
 * Name            : rngs.h  (header file for the library file rngs.c) 
 * Author          : Steve Park & Dave Geyer
 * Language        : ANSI C
 * Latest Revision : 09-22-98
 * ----------------------------------------------------------------------- 
 */

#if !defined( _RNGS_ )
#define _RNGS_

uint32_t rand(void);
void   PlantSeeds(void);
void   GetSeed(long *x);
void   PutSeed(void);
void   SelectStream(int index);
void   TestRandom(void);

#endif
