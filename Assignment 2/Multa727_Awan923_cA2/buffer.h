/**************************************************************************************************************************************************************************************************
* File name:       buffer.h
* Version:         1.0
* Compiler:		   MS Visual Studio 2015
* Author:          Maryum Awan  (040 854 923)
				   Aman Multani (040 877 727)
* Course:          CST 8152 – Compilers, Lab Section: 011
* Assignment:      2
* Submitting Date: March 30th, 2018
* Due Date:        March 23rd, 2018
* Professor:       Svillen Ranev
* Purpose:         To implement a buffer that can operate in three different modes: a "fixed-size buffer", an "additive self-incrementing" buffer, a "multiplicative self-incremting" buffer
* Function list:   b_allocate()			creates a new buffer in memory (on the program heap)
*				   b_addc()				add the character symbol to the character array of the given buffer pointed by pBD
*				   b_clear()			retains the memory space currently allocated to the buffer, but re-initializes all appropiate data members of the given Buffer structure
*				   b_free()				frees the memory occupied by the character buffer and the Buffer structure
*				   b_isfull()			returns 1 if the character buffer is full; it returns 0 otherwise
*				   b_limit()			returns the current limit of the character buffer
*				   b_capacity()			returns the current capacity of the character buffer
*				   b_mark()				sets markc_offset to mark and returns the currently set markc_offset
*				   b_mode()				returns the value of mofe to the calling fuction
*				   b_incfactor()		returns the non-negative value of inc_factor to the calling function
*				   b_load()				loads an open input file specified by fi ino a buffer specified by pBD
*				   b_isempty()			tests if addc_offset is 0, the fuction returns 1 otherwise it returns 0
*				   b_eob()				returns eob to the calling function
*				   b_getc()				checks the argument for validity. If getc_offical and addc_offset are equal, it sets eob to -1 and returns -1. returns the character located at getc_offset
*				   b_print()            prints character by character the contents of the character buffer and returns the number of characters read
*				   b_compact()          uses realloc() to adjust the new capacity and then updates all the necessary memmbers of the buffer descriptor structure
*				   b_rflag()            returns r_flag to the calling fuction
*				   b_retract()			decrements getc_offset by 1
*				   b_reset()			sets getc_offset to the value of the current marck_offset
*				   b_getc_offset()		returns getc_offset to the calling function
*				   b_rewind()			set the getc_offset and markc_offset to 0, so that the buffer can be reread again
*				   b_location()			returns a pointer to a location of the character buffer indicated by loc_offset
**************************************************************************************************************************************************************************************************/

#ifndef BUFFER_H_
#define BUFFER_H_

/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */

/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */
													
/* standard header files */
#include <stdio.h>  /* for exit status defines */
#include <limits.h> /* implementation-defined data type ranges and limits */
#include <malloc.h> /* for dynamic memory allocation*/

/* constant definitions */
/* You may add your own constant definitions here */														   /* constant definitions */
#ifndef TRUE 
#define TRUE 1
#endif

#ifndef FALSE 
#define FALSE 0
#endif
#define RT_FAIL1 -1				/* failure return value -1 */
#define RT_FAIL2 -2				/* failure return value -2*/
#define LOAD_FAIL -2			/* load failure error */
#define SET_R_FLAG 1			/* realloc flag set value */

#define MAX_VALUE SHRT_MAX-1    /*MAX_VALUE set to short_max - 1*/
#define UNSET_R_FLAG 0          /* unset reallocation flag */
#define SET_EOB_FLAG 1          /* set end of buffer flag */
#define UNSET_EOB_FLAG 0        /* unset end of buffer flag */

#define MULTIPLE_OP_MODE -1     /* multiplicative op mode */
#define FIX_MODE 0              /* fixed op mode */
#define ADDITION_OP_MODE 1      /* additive op mode */

#define OFFSET_RESET 0          /* reset character offset */
#define ZERO_INIT_CAPACITY  0   /* indicate the capaciy is zero*/
#define MAX_INIT_CAPACITY -1    /* maximum allowed negative value*/
#define MIN_CAPACITY 1          /* minimum character buffer capacity */

#define FIX_INC_FACTOR 0        /* fixed increment factor constant */
#define MIN_INC_FACTOR 1        /* minimum additive increment factor constant */
#define MIN_MUL_INC_FACTOR 1    /* minumum multiplicative increment factor constant */
#define MAX_ADD_INC_FACTOR 255  /* maximum additive increment factor constant */
#define MULT_MAX_INC_FACTOR 100 /* maximum multiplicative increment factor constant */
#define SIZE_0X100 0x100		/* return value in case of error on reading increment factor */

/* user data type declarations */
typedef struct BufferDescriptor {
	char *cb_head;      /* pointer to the beginning of character array (character buffer) */
	short capacity;     /* current dynamic memory size (in bytes) allocated to character buffer */
	short addc_offset;  /* the offset (in chars) to the add-character location */
	short getc_offset;  /* the offset (in chars) to the get-character location */
	short markc_offset; /* the offset (in chars) to the mark location */
	char  inc_factor;   /* character array increment factor */
	char  r_flag;       /* reallocation flag */
	char  mode;         /* operational mode indicator*/
	int   eob;          /* end-of-buffer flag */
} Buffer, *pBuffer;     /*typedef Buffer *pBuffer;*/

/* function declarations */
Buffer* b_allocate(short init_capacity, char inc_factor, char o_mode);  /* creates a new buffer in memory (on the program heap) */
pBuffer b_addc(pBuffer const pBD, char symbol);							/* add the character symbol to the character array of the given buffer pointed by pBD */
int     b_clear(Buffer * const pBD);									/* retains the memory space currently allocated to the buffer, but re-initializes all appropiate data members of the given Buffer structure */
void    b_free(Buffer * const pBD);										/* frees the memory occupied by the character buffer and the Buffer structure */
int     b_isfull(Buffer * const pBD);									/* returns 1 if the character buffer is full; it returns 0 otherwise */
short   b_limit(Buffer * const pBD);									/* returns the current limit of the character buffer */
short   b_capacity(Buffer * const pBD);									/* returns the current capacity of the character buffer */
short   b_mark(pBuffer const pBD, short mark);							/* sets markc_offset to mark and returns the currently set markc_offset */
int     b_mode(Buffer * const pBD);										/* returns the value of mofe to the calling fuction */
size_t  b_incfactor(Buffer * const pBD);								/* returns the non-negative value of inc_factor to the calling function*/
int     b_load(FILE * const fi, Buffer * const pBD);				    /* loads an open input file specified by fi ino a buffer specified by pBD */
int     b_isempty(Buffer * const pBD);									/* tests if addc_offset is 0, the fuction returns 1 otherwise it returns 0 */
int     b_eob(Buffer * const pBD);									    /* returns eob to the calling function */
char    b_getc(Buffer * const pBD);									    /* checks the argument for validity. If getc_offical and addc_offset are equal, it sets eob to -1 and returns -1. returns the character located at getc_offset */
int     b_print(Buffer * const pBD);                                    /* prints character by character the contents of the character buffer and returns the number of characters read */
Buffer* b_compact(Buffer * const pBD, char symbol);                     /* uses realloc() to adjust the new capacity and then updates all the necessary memmbers of the buffer descriptor structure */
char    b_rflag(Buffer * const pBD);                                    /* returns r_flag to the calling fuction */
short   b_retract(Buffer * const pBD);								    /* decrements getc_offset by 1 */
short   b_reset(Buffer * const pBD);									/* sets getc_offset to the value of the current marck_offset */
short   b_getc_offset(Buffer * const pBD);								/* returns getc_offset to the calling function*/
int		b_rewind(Buffer * const pBD);									/* set the getc_offset and markc_offset to 0, so that the buffer can be reread again*/
char*   b_location(Buffer * const pBD, short loc_offset);				/* returns a pointer to a location of the character buffer indicated by loc_offset*/

#endif

