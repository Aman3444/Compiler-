/*************************************************************************************************************************************************************************************************
* File name:        table.h
* Version:          1.0
* Compiler:		    MS Visual Studio 2015
* Author:           Maryum Awan  (040 854 923)
					Aman Multani (040 877 727)
* Course:           CST 8152 – Compilers, Lab Section: 011
* Assignment:       2
* Submitting Date:	March 30th, 2018
* Due Date:         March 23rd, 2018
* Professor:        Svillen Ranev
* Purpose:          Functions implementing a Lexical Analyzer (Scanner)
* Function List:    aa_func02
*                   aa_func03
*                   aa_func05
*                   aa_func08
*                   aa_func11
*                   aa_func12
**************************************************************************************************************************************************************************************************/

#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef TOKEN_H_
#include "token.h"
#endif

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif


/*   So urce end-of-file (SEOF) sentinel symbol
*    '\0' or one of 255,0xFF,EOF
*/
#define SEOF '\0'
/*  Special case tokens processed separately one by one
*  in the token-driven part of the scanner
*  '=' , ' ' , '(' , ')' , '{' , '}' , == , <> , '>' , '<' , ';',
*  white space
*  !!comment , ',' , '"' , ';' , '-' , '+' , '*' , '/', # ,
*  .AND., .OR. , SEOF, 'illigal symbol',
*/

#define ES 12 /* Error State */
#define IS -1 /* Illegal State */

/* State transition table definition */
#define TABLE_COLUMNS 8	/* Number of columns in the transition table.*/
#define TABLE_ROWS   14     /* Number of rows in the transition table. */
/*transition table - type of states defined in separate table */

int st_table[][TABLE_COLUMNS] = {
	/*              X  [a-zA-Z]  [A-F]     0    [1-9]   .      $       other */
	/* State 0  */{ 1,		1,     1,     6,     4,    ES,   ES,     ES }, /* NOAS */
	/* State 1  */{ 1,		1,     1,     1,     1,     2,    3,      2 }, /* NOAS */
	/* State 2  */{ IS,    IS,    IS,    IS,    IS,    IS,   IS,     IS }, /* ASWR */
	/* State 3  */{ IS,    IS,    IS,    IS,    IS,    IS,	 IS,     IS }, /* ASNR */
	/* State 4  */{ 5,     ES,    ES,     4,     4,     7,    5,      5 }, /* NOAS */
	/* State 5  */{ IS,    IS,    IS,    IS,    IS,    IS,	 IS,     IS }, /* ASWR */
	/* State 6  */{ 9,     ES,    ES,     5,    ES,     7,   ES,      5 }, /* NOAS */
	/* State 7  */{ ES,     8,    ES,     7,     7,     8,    8,      8 }, /* NOAS */
	/* State 8  */{ IS,    IS,    IS,    IS,    IS,    IS,    IS,    IS }, /* ASWR */
	/* State 9  */{ ES,    ES,    10,    10,    10,    ES,    ES,    12 }, /* NOAS */
	/* State 10 */{ ES,    ES,    10,    10,    10,    ES,    ES,    11 }, /* NOAS */
	/* State 11 */{ IS,    IS,    IS,    IS,    IS,    IS,    IS,	 IS }, /* ASWR */
	/* State 12 */{ IS,    IS,    IS,    IS,    IS,    IS,    IS,	 IS }, /* ASNR */
	/* State 13 */{ IS,    IS,    IS,    IS,    IS,    IS,    IS,	 IS }  /* ASWR */
	
};

/* Accepting state table definition */
#define ASWR     0  /* accepting state with retract */
#define ASNR     1  /* accepting state with no retract */
#define NOAS     2  /* not accepting state */

int as_table[] = {
	/* State 0 */ NOAS,
	/* State 1 */ NOAS,
	/* State 2 */ ASWR,
	/* State 3 */ ASNR,
	/* State 4 */ NOAS,
	/* State 5 */ ASWR,
	/* State 6 */ NOAS,
	/* State 7 */ NOAS,
	/* State 8 */ ASWR,
	/* State 9 */ NOAS,
	/* State 10*/ NOAS,
	/* State 11*/ ASWR,
	/* State 12*/ ASNR
};

/* Accepting action function declarations */

/* Accepting state with retract - AVID*/
Token aa_func02(char lexeme[]);
/* Accepting state without retract - SVID */
Token aa_func03(char *lexeme);
/* Accepting state with retract - DIL */
Token aa_func05(char *lexeme);
/* Accepting state with retract - FPL */
Token aa_func08(char *lexeme);
/* Accepting state with retract - HIL */
Token aa_func11(char *lexeme);
/* Accepting state with retract - ES */
Token aa_func12(char *lexeme);


/* defining a new type: pointer to function (of one char * argument)
returning Token
*/

typedef Token(*PTR_AAF)(char *lexeme);

/* Accepting function (action) callback table (array) definition */
/* If you do not want to use the typedef, the equvalent declaration is:
* Token (*aa_table[])(char lexeme[]) = {
*/

PTR_AAF aa_table[] = {
	/* State 0 */ NULL,
	/* State 1 */ NULL,
	/* State 2 */ aa_func02,
	/* State 3 */ aa_func03,
	/* State 4 */ NULL,
	/* State 5 */ aa_func05,
	/* State 6 */ NULL,
	/* State 7 */ NULL,
	/* State 8 */ aa_func08,
	/* State 9 */ NULL,
	/* State 10*/ NULL,
	/* State 11*/ aa_func11,
	/* State 12*/ aa_func12
};

/* Keyword lookup table (.AND. and .OR. are not keywords) */

#define KWT_SIZE  10

char * kw_table[] =
{
	"ELSE",/*0*/
	"FALSE",/*1*/
	"IF",/*2*/
	"PLATYPUS",/*3*/
	"READ",/*4*/
	"REPEAT",/*5*/
	"THEN",/*6*/
	"TRUE",/*7*/
	"WHILE",/*8*/
	"WRITE"/*9*/
};

#endif
