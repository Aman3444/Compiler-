/**************************************************************************************************************************************************************************************************
* File name:        scanner.c
* Version:          1.0
* Compiler:		    MS Visual Studio 2015
* Author:           Maryum Awan  (040 854 923)
*					Aman Multani (040 877 727
* Course:           CST 8152 – Compilers, Lab Section: 011
* Assignment:       2
* Submitting Date:	March 30th, 2018
* Due Date:         March 23rd, 2018
* Professor:        Svillen Ranev
* Purpose:          Functions implementing a Lexical Analyzer (Scanner)
* Function list:	scanner_init(Buffer * sc_buf);
*					malar_next_token(Buffer * sc_buf);
*					get_next_state(int state, char c, int *accept);
*					char_class(char c);
*					aa_func02(char lexeme[]);
*					aa_func03(char lexeme[]);
*					aa_func05(char lexeme[])
*					aa_func08(char lexeme[]);
*					aa_func11(char lexeme[]);
*					aa_func12(char lexeme[]);
*					aa_func13(char lexeme[]);
**************************************************************************************************************************************************************************************************/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in Borland compiler projects.
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

/*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
It is defined in platy_st.c */
extern Buffer * str_LTBL; /*String literal table */
int line; /* current line number of the source code */
extern int scerrnum;     /* defined in platy_st.c - run-time error number */

						 /* Local(file) global objects - variables */
static Buffer *lex_buf; /* pointer to temporary lexeme buffer */

					   /* No other global variable declarations/definitiond are allowed */

					   /* scanner.c static(local) function  prototypes */
static int char_class(char c); /* character class function */
static int get_next_state(int, char, int *); /* state machine function */
static int iskeyword(char * kw_lexeme); /*keywords lookup functuion */
static long atolh(char * lexeme); /* converts hexadecimal string to decimal value */

								  /*Initializes scanner */
int scanner_init(Buffer * sc_buf) {
	if (b_isempty(sc_buf)) return EXIT_FAILURE;/*1*/
	/* in case the buffer has been read previously  */
	b_rewind(sc_buf);
	b_clear(str_LTBL);
	line = 1;
	return EXIT_SUCCESS;/*0*/
	/*   scerrnum = 0;  *//*no need - global ANSI C */
}

/***********************************************************************************************************
* Purpose:		   The function performs the token recognition.
* Author:          Maryum Awan  (040 854 923)
*				   Aman Multani (040 877 727)
* Version:         1.0
* Called functions:b_getc()
*				   b_retract()
*				   b_mark()
*				   b_addc()
*				   b_allocate()
*				   b_getc_offset()
*				   b_reset()
*				   b_free()
*				   b_limit()
* Parameters :	   Buffer * sc_buf
* Return value:    t Token 
* Alogrithm:	   1. reads the lexeme from the input stream one character at a time from the input
*				   2. returns a token structure if it finds a token pattern matching within an input (lexemes)
*				   3. when it doesn't get any input it breaks out the loop
*				   4. else loops through, and process the tokens
*				   5. if it is not a token then a transition table is used to determine the token
*				   6. If a token is found return the structure of the token
*				   7. If error occurs then return an error token
***********************************************************************************************************/

Token malar_next_token(Buffer * sc_buf) {
	Token t = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
	unsigned char c; /* input symbol */
	int state = 0; /* initial state of the FSM */
	short lexstart;  /* start offset of a lexeme in the input char buffer (array) */
	short lexend;    /* end offset of a lexeme in the input char buffer (array) */
	int accept = NOAS; /* type of state - initially not accepting */
	int i = 0; /* index for loop*/
	int err_index = 0; /* index of character in error token */
	int num_retract = 0; /* Counter to store the number of characters are ahead in buffer from '.' */

	/* Part 1: Implementation of token driven scanner */
	/* every token is possessed by its own dedicated code */

	while (1) { /* endless loop broken by token returns it will generate a warning */
		c = b_getc(sc_buf);
		/* checking for the Source-End-Of-File and End-Of-File */
		if (c == SEOF || c == 255 || c == EOF) {
			t.code = SEOF_T;
			return t; /* return token */
		}
		/* checking for the white space */
		if (c == ' ') {
			continue;
		}
		/* checking for the Tab space*/
		if (c == '\t') {
			continue;
		}
		/* checking for the new line */
		if (c == '\n') {
			/* increments line variable*/
			line++;
			continue;
		}
		/* checking for the left bracket */
		if (c == '{') {
			t.code = LBR_T;
			return t;
		}
		/* checking for the right bracket */
		if (c == '}') {
			t.code = RBR_T;
			return t;
		}
		/* checking for the left paranthesis */
		if (c == '(') {
			t.code = LPR_T;
			return t;
		}
		/* checking for the right paranthesis */
		if (c == ')') {
			t.code = RPR_T;
			return t;
		}
		/* checking for the comma */
		if (c == ',') {
			t.code = COM_T;
			return t;
		}
		/* checking for the end of statement */
		if (c == ';') {
			t.code = EOS_T;
			return t;
		}
		/* checking for the string concatenation */
		if (c == '#') {
			t.code = SCC_OP_T;
			return t;
		}
		
		/****************************** ARITHMENTIC OPERATIONS ******************************/
		/* checking for the Addition operator */
		if (c == '+') {
			t.code = ART_OP_T;
			t.attribute.arr_op = PLUS;
			return t;
		}
		/* checking for the Substraction operator */
		if (c == '-') {
			t.code = ART_OP_T;
			t.attribute.arr_op = MINUS;
			return t;
		}
		/* checking for the Multiplication operator */
		if (c == '*') {
			t.code = ART_OP_T;
			t.attribute.arr_op = MULT;
			return t;
		}
		/* checking for the Division operator */
		if (c == '/') {
			t.code = ART_OP_T;
			t.attribute.arr_op = DIV;
			return t;
		}
		/* End of file */
		if (c == EOF) {
			t.code = SEOF_T;
			return t;
		}
		/* checking for Greater-than relational operator */
		if (c == '>') {
			t.code = REL_OP_T;
			t.attribute.rel_op = GT;
			return t;
		}
		/*  checking for Less-than relational operator */
		if (c == '<') {
			c = b_getc(sc_buf);
			if (c == '>') { 
				/* checks if the next character in buffer stream is '>' */
				t.code = REL_OP_T;
				t.attribute.rel_op = NE;
				return t;
			}
			else {
				/* retract to the previous character, if the above condition fails */
				b_retract(sc_buf);
				t.code = REL_OP_T;
				t.attribute.rel_op = LT;
				return t;
			}
		}

		/* checking for equality operators */
		if (c == '=') {
			c = b_getc(sc_buf);
			/* if '=' or '==' sign, set tokens*/
			if (c == '=') {
				/* checks if the next character in buffer stream is '=' */
				t.code = REL_OP_T;
				t.attribute.rel_op = EQ;
				return t;
			}
			else {
				b_retract(sc_buf);
				t.code = ASS_OP_T;
				return t;
			}	
		}

		/****************************** LOGICAL OPERATORS ******************************/
		/* checking for '.' (period) */
		if (c == '.') {
			/* initialize the index of error token */
			err_index = 0; 
			/* at index 0 assigning '.' in err_lex[] */
			t.attribute.err_lex[err_index++] = c; 
			c = b_getc(sc_buf); /* Getting next character */
			num_retract++;

			/* trying to process AND */
			/* Proceed to read the next character if the character is 'A' */
			if (c == 'A') {
				c = b_getc(sc_buf);
				/* Proceed to read the next character if the character is 'N' */
				if (c == 'N') {
					c = b_getc(sc_buf);
					/* Proceed to read the next character if the character is 'D' */
					if (c == 'D') {
						c = b_getc(sc_buf);
					}
					/* token is set to logical operator if the character is '.' */
					if (c == '.') {
						t.code = LOG_OP_T;
					    /* token logical operator set as And */
						t.attribute.log_op = AND;
						return t; /* returns token */
					}
				}
			}

			/* trying to process OR */
			/* Proceed to read the next character if the character is 'O' */
			else if (c == 'O') {
				c = b_getc(sc_buf);
				/* Proceed to read the next character if the character is 'R' */
				if (c == 'R') {
					c = b_getc(sc_buf);
					/* token is set to logical operator if the character is '.' */
					if (c == '.') {
						t.code = LOG_OP_T;
						/* token logical operator set as OR */
						t.attribute.log_op = OR;
						return t; /* return token */
					}
				}
			}
			/* 
			* Since every time buffer increses by one more character to get next character hence
			* if condition fails, then the buffer must retract back to the character after '.'
			*/
			for (i = 0; i < num_retract; i++) {
				b_retract(sc_buf);
			}
			/* Error Token */
			t.code = ERR_T; 
			t.attribute.err_lex[err_index] = '\0';
			return t; /* return token */
		}

		/****************************** COMMENTS ******************************/
		/* checking for '!' */
		if (c == '!') {
			/* setting the mark as the current getc_offset */
			short mark = b_getc_offset(sc_buf);
			b_mark(sc_buf, mark);
			/* getting the new character */
			c = b_getc(sc_buf);
			/* checking for the second '!' */
			if (c == '!') { 
				/* getting the new character */
				c = b_getc(sc_buf);
				/* if it is an Souurce-End-Of-File then return SEOF_T */
				if (c == (unsigned char)SEOF) {
					t.code = SEOF_T;
					return t;
				}
				/* else if it is not SEOF then */
				else {
					t.code = COM_T;
				}
				/* while loop till there is no next line */
				while (c != '\n') {
					/* getting the new character */
					c = b_getc(sc_buf);
					/* if it is an Souurce-End-Of-File then return SEOF_T */
					if (c == (unsigned char)SEOF) {
						t.code = SEOF_T;
						return t;
					}
				}
				/* line increment */
				line++;
				continue;
			}
			/* else returns an error token */
			else {
				t.code = ERR_T;
				t.attribute.err_lex[0] = '!'; /* '!' an index */
				t.attribute.err_lex[1] = c; 
				t.attribute.err_lex[2] = '\0'; /* in index 2 to make c type string */
				while (c != '\n')
					/* Consuming all characters */
					c = b_getc(sc_buf); 
				line++;
				return t;
			}
		}

		/****************************** STRING LITERALs ******************************/
		/* checking for '"' string */
		if (c == '"') {
			/* setting the mark to the begiinning */
			lexstart = b_mark(sc_buf, b_getc_offset(sc_buf));
			int len = 0;
			while (1) {
				/* gets new character */
				c = b_getc(sc_buf);
				/* new line */
				if (c == '\n') {
					/* increment line */
					line++;
				}
				/* checks the End-of-File*/
				else if ((unsigned)c == EOF) {
					lexend = b_getc_offset(sc_buf); 
					len = lexend - lexstart;
					b_reset(sc_buf); /* reset the buffer */
					b_retract(sc_buf); /* retracts the buffer */
					t.code = ERR_T; /* reads an error token */
					/* check if the length is > ERR_LEN */
					if ((len) > ERR_LEN) {
						/* it will return the first 17 characters, */
						for (i = 0; i <= 16; i++)
							t.attribute.err_lex[i] = b_getc(sc_buf);
						/* returning the 3 dots and SEOF */
						t.attribute.err_lex[17] = '.';
						t.attribute.err_lex[18] = '.';
						t.attribute.err_lex[19] = '.';
						t.attribute.err_lex[20] =  SEOF; 
						
						while (i < lexend) {
							b_getc(sc_buf);
							i++; /* increment i */
						}
					}
					/* if the len is less than ERR_LEN */
					else {
						/* it will return the first 17 characters, */
						for (i = 0; i <= 16; i++)
							t.attribute.err_lex[i] = b_getc(sc_buf);
						/* returning the SEOF */
						t.attribute.err_lex[i] = SEOF;
					}
					return t;
				}
				/* if above condition is not true then this will be executed */
				else if (c == '"') {
					b_retract(sc_buf);
					lexend = b_getc_offset(sc_buf);
					len = lexend - lexstart;
					b_reset(sc_buf); /* reset the buffer */
					t.code = STR_T;
					/* string is stored in the string literal table */
					t.attribute.str_offset = b_limit(str_LTBL); 

					for (i = 0; i < len; i++) {
						c = b_getc(sc_buf);
						b_addc(str_LTBL, c);
					}
					b_getc(sc_buf);
					b_addc(str_LTBL, '\0'); /* storing the string to the SEOF */
					return t; /* return token */
				}
			}
		}

		/****************************** TRANISTION TABLE ******************************/
		if (isalpha(c) || isdigit(c)) {
			b_retract(sc_buf);
			/* set the mark at the begining of the lexeme and store it in */
			lexstart = b_mark(sc_buf, b_getc_offset(sc_buf));
			c = b_getc(sc_buf);
			state = 0;/*final state variable  */
			/* gets the next state from transiation table */
			FSM1:state = get_next_state(state, c, &accept);
			/* loops till the false */
			while(accept == NOAS) {
				/* gets the next character */
				c = b_getc(sc_buf);
				goto FSM1;
			}
			/* if accepting state is ASWR then retract */
			if (accept == ASWR) {
				b_retract(sc_buf);
			}
			/* setting lexend to getc_offset */
			lexend = b_getc_offset(sc_buf);
			/* crwating a temporary lexeme buffer */
			lex_buf = b_allocate(lexend - lexstart+1, 0, 'f');
			/* if the temporary lexeme buffer is null */
			if (lex_buf == NULL) {
				scerrnum = 1;
				/* then print this */
				strcpy(t.attribute.err_lex, "RUN TIME ERROR: ");
				t.code = ERR_T;
				return t;
			}
			/* resetting to the first character */
			b_reset(sc_buf);
			/* copies the character to the lex buffer */
			for (i =0; i < lexend-lexstart; i++) {
				b_addc(lex_buf, b_getc(sc_buf));
			}
			/* adding SEOP to the end of lexeme */
			b_addc(lex_buf, '\0');
			/* calling the accepting function */
			t = aa_table[state](b_location(lex_buf, 0));
			b_free(lex_buf); /* freeing the lex_buf */
			return t; 
		}

		/* if the above condition does not work */
		t.attribute.err_lex[0] = c;
		t.attribute.err_lex[1] = '\0';
		t.code = ERR_T; /* set ERR_T */
		return t; /* retun error token */
	}
}

/*DO NOT MODIFY THE CODE OF THIS FUNCTION
YOU CAN REMOVE THE COMMENTS*/

int get_next_state(int state, char c, int *accept){
	int col;
	int next;
	col = char_class(c);
	next = st_table[state][col];
#ifdef DEBUG
		printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
		/*
		The assert(int test) macro can be used to add run-time diagnostic to programs
		and to "defend" from producing unexpected results.
		assert() is a macro that expands to an if statement;
		if test evaluates to false (zero) , assert aborts the program
		(by calling abort()) and sends the following message on stderr:

		Assertion failed: test, file filename, line linenum

		The filename and linenum listed in the message are the source file name
		and line number where the assert macro appears.
		If you place the #define NDEBUG directive ("no debugging")
		in the source code before the #include <assert.h> directive,
		the effect is to comment out the assert statement.
		*/
		assert(next != IS);

		/*
		The other way to include diagnostics in a program is to use
		conditional preprocessing as shown bellow. It allows the programmer
		to send more details describing the run-time problem.
		Once the program is tested thoroughly #define DEBUG is commented out
		or #undef DEBUF is used - see the top of the file.
		*/
#ifdef DEBUG
		if (next == IS) {
			printf("Scanner Error: Illegal state:\n");
			printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
			exit(1);
	}
#endif
		*accept = as_table[next];
		return next;
}

/***********************************************************************************************************
* Purpose:		   This function returns the column index for the column in the transition that represents a character or a character class.
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   char c
* Return value:    int
* Alogrithm:	   1. Scan the input character
*				   2. Matching the character in the transition table
***********************************************************************************************************/

int char_class(char c) {
		/*THIS FUNCTION RETURNS THE COLUMN NUMBER IN THE TRANSITION
		TABLE st_table FOR THE INPUT CHARACTER c.
		SOME COLUMNS MAY REPRESENT A CHARACTER CLASS .
		FOR EXAMPLE IF COLUMN 1 REPRESENTS [A-Z]
		THE FUNCTION RETURNS 1 EVERY TIME c IS ONE
		OF THE LETTERS A,B,...,Z.*/
		
		/* checking for the X to get the Hexadecimal */
		if (c == 'x') {
			return 0;
		}
		/* Checking For the Hexadecimal Values [A-F] */
		else if (c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F') {
			return 2;
		}
		/* Checking for the Letters in thge transition table [] */
		else if (isalpha(c)) {
			return 1;
		}
		/* Checks for the '0' in the transitions table  */
		else if (c == '0') {
			return 3;
		}
		/* checking if the c lies in between the range [1-9]*/
		else if (c >= '1' && c <= '9') {
			return 4;
		}
		/* Checking if the c is'.', then returns 5 */
		else if (c == '.') {
			return 5;
		}
		/* checking if c is '$', then returns the value 6 */
		else if (c == '$') {
			return 6;
		}
		/* if above condition if not setisfied then by default return this value */
		else {
			/* returns value int */
			return 7;
		}
		
}

/*HERE YOU WRITE THE DEFINITIONS FOR YOUR ACCEPTING FUNCTIONS.
************************************************************
ACCEPTING FUNCTION FOR THE arithmentic variable identifier AND keywords(VID - AVID / KW)
REPLACE XX WITH THE CORRESPONDING ACCEPTING STATE NUMBER
*/

/***********************************************************************************************************
* Purpose:		   This function will check for a keyword and the arithimentic variabke (KW / VID - AVID)
* Author:          Maryum Awan  (040 854 923)
* Version:         1.0
* Called functions:strcmp(), 
*				   strlen()
* Parameters :	   char lexeme[]
* Return value:    t token 
* Alogrithm:	   1. checks if the lexeme is a keyword
*				   2. else set a avid token and return it
***********************************************************************************************************/

Token aa_func02(char lexeme[]) {
	Token t;
	unsigned int i ,j ,k = 0;
	/*WHEN CALLED THE FUNCTION MUST
	1. CHECK IF THE LEXEME IS A KEYWORD.
	IF YES, IT MUST RETURN A TOKEN WITH THE CORRESPONDING ATTRIBUTE
	FOR THE KEYWORD. THE ATTRIBUTE CODE FOR THE KEYWORD
	IS ITS INDEX IN THE KEYWORD LOOKUP TABLE (kw_table in table.h).
	IF THE LEXEME IS NOT A KEYWORD, GO TO STEP 2.*/

		for (i = 0; i < KWT_SIZE; i++) {
			/* comparing lexeme with keyword table string */
			if (strcmp(lexeme, kw_table[i]) == 0) {
				t.code = KW_T;
				t.attribute.kwt_idx = i;
				return t;
			}
		}
		/*	2. SET a AVID TOKEN.
			IF THE lexeme IS LONGER than VID_LEN(see token.h) CHARACTERS,
			ONLY FIRST VID_LEN CHARACTERS ARE STORED
			INTO THE VARIABLE ATTRIBUTE ARRAY vid_lex[](see token.h) .
			ADD \0 AT THE END TO MAKE A C - type STRING.
			*/

		/* Assigning t to AVID_T token */
		t.code = AVID_T; 
		/* checks if the lexemes is greater than VID_LEN characters */
		if (strlen(lexeme)>VID_LEN) {
			for (j = 0; j < VID_LEN; j++) {
				/* This will add the VID_LEN characters */
				t.attribute.vid_lex[j] = lexeme[j]; 
			}
			/* adds the character in the end */
			t.attribute.vid_lex[j] = SEOF;
			return t;
		}
		else {
			/*if the lexemes is less than VID_LEN character */ 
			for (k = 0; k < strlen(lexeme); k++) {
				/* adding the characters */
				t.attribute.vid_lex[k] = lexeme[k]; 
			}
			/* adds the character in the end */
			t.attribute.vid_lex[k] = SEOF;
			return t;
		}
	}

/*ACCEPTING FUNCTION FOR THE string variable identifier(VID - SVID)
REPLACE XX WITH THE CORRESPONDING ACCEPTING STATE NUMBER
*/

/***********************************************************************************************************
* Purpose:		   This function will check for the string variable identifier (VID - SVID)
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:strlen()
* Parameters :	   char *lexeme
* Return value:    SVID token
* Alogrithm:	   1. set a SVID token
*				   2. If the lexeme is longer than VID_LEN 
*				   3. else all the characters from the lexeme will be stored in the token
*			       4. returning SVID token
***********************************************************************************************************/

Token aa_func03(char *lexeme) {
	Token t;
	unsigned int i,j = 0;
		/*WHEN CALLED THE FUNCTION MUST
		1. SET a SVID TOKEN.
		IF THE lexeme IS LONGER than VID_LEN characters,
		ONLY FIRST VID_LEN-1 CHARACTERS ARE STORED
		INTO THE VARIABLE ATTRIBUTE ARRAY vid_lex[],
		AND THEN THE $ CHARACTER IS APPENDED TO THE NAME.
		ADD \0 AT THE END TO MAKE A C-type STRING.*/

			/* Token code is set to the String Varible Identifier */
			t.code = SVID_T; 
			/* checks if the length of lexeme is greater than VID_LEN than */
			if (strlen(lexeme) > VID_LEN) {
				for (i = 0; i < VID_LEN - 1; i++) {
					t.attribute.vid_lex[i] = lexeme[i];
				}
				t.attribute.vid_lex[i++] = '$'; 
				/* adds the character in the end */
				t.attribute.vid_lex[i] = SEOF; 
				return t;
			}
			/*if is not longer than then the string will be stored in the VARIABLE ATTRIBUTE ARRAY vid_lex[]  */
			for (j = 0; j <= strlen(lexeme); j++) {
				t.attribute.vid_lex[j] = lexeme[j];
			}
			t.attribute.vid_lex[j++] = '$';
			/* adds the character in the end */
			t.attribute.vid_lex[j] = SEOF; 
			return t; /* retuen SVID token */
	}

/*	ACCEPTING FUNCTION FOR THE floating - point literal (FPL)
*/

/***********************************************************************************************************
* Purpose:		   This function will check for the floating - point literal (FPL)
* Author:          Maryum Awan  (040 854 923)
* Version:         1.0
* Called functions:strtod()
*				   strlen()
*				   strncpy()
* Parameters :	   char *lexeme
* Return value:    FPL token
* Alogrithm:	   1. converting the lexeme into the double value
*				   2. checks the range and returns an error token
*				   3. THREE DOTS ... ARE ADDED TO THE END OF THE err_lex C-type string. 
*				   4. returning FPL token
***********************************************************************************************************/

Token aa_func08(char *lexeme) {
	Token t;
	double Lex_value = strtod(lexeme, NULL); 
		/*THE FUNCTION MUST CONVERT THE LEXEME TO A FLOATING POINT VALUE,
		WHICH IS THE ATTRIBUTE FOR THE TOKEN.
		THE VALUE MUST BE IN THE SAME RANGE AS the value of 4-byte float in C.
		IN CASE OF ERROR (OUT OF RANGE) THE FUNCTION MUST RETURN ERROR TOKEN
		THE ERROR TOKEN ATTRIBUTE IS  lexeme. IF THE ERROR lexeme IS LONGER
		than ERR_LEN characters, ONLY THE FIRST ERR_LEN-3 characters ARE
		STORED IN err_lex. THEN THREE DOTS ... ARE ADDED TO THE END OF THE
		err_lex C-type string. 
		BEFORE RETURNING THE FUNCTION MUST SET THE APROPRIATE TOKEN CODE*/
			
			/* checking the lexeme value is in the range or not */
			if ((Lex_value < FLT_MIN || Lex_value > FLT_MAX) && Lex_value != 0) {
				t.code = ERR_T; /* setting an error token */
				/* checking if the lexeme is greater then ERR_LEN  */
				if (strlen(lexeme) >= ERR_LEN) {
					/* storing the first ERR_LEN -3 character */
					strncpy(t.attribute.err_lex, lexeme, ERR_LEN - 3);
					/* returning the 3 dots */
					t.attribute.err_lex[ERR_LEN - 3] = '.';
					t.attribute.err_lex[ERR_LEN - 2] = '.';
					t.attribute.err_lex[ERR_LEN - 1] = '.';
					/* adds the character in the end */
					t.attribute.err_lex[ERR_LEN] = SEOF;
					return t; 
				}
				/* storing the first ERR_LEN character */
				strncpy(t.attribute.err_lex, lexeme, ERR_LEN);
				return t;
			}
			/* Token code is set to the Floating Point Literal */
			t.code = FPL_T; 
			t.attribute.flt_value = (float)Lex_value; 
			return t; /* returning FPL token */
	}

/*ACCEPTING FUNCTION FOR THE integer literal(IL)-decimal constant(DIL)
*/

/***********************************************************************************************************
* Purpose:		   This function will check for the interger literal (IL) - decimal constamt (DIL)
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:atol()
*				   strlen()
*				   strncpy()
* Parameters :	   char *lexeme
* Return value:    DIL token
* Alogrithm:	   1. converting the lexeme to a decimal constant
*				   2. checks the range and returns an error token
*				   3. checks if the lexemes are greater than or equal to the ERR_LEN
*				   4. THREE DOTS ... ARE ADDED TO THE END OF THE err_lex C-type 
*				   5. returning DIL token
***********************************************************************************************************/

Token aa_func05(char *lexeme) {
		/*THE FUNCTION MUST CONVERT THE LEXEME REPRESENTING A DECIMAL CONSTANT
		TO A DECIMAL INTEGER VALUE, WHICH IS THE ATTRIBUTE FOR THE TOKEN.
		THE VALUE MUST BE IN THE SAME RANGE AS the value of 2 - byte integer in C.
		IN CASE OF ERROR(OUT OF RANGE) THE FUNCTION MUST RETURN ERROR TOKEN
		THE ERROR TOKEN ATTRIBUTE IS  lexeme.IF THE ERROR lexeme IS LONGER
		than ERR_LEN characters, ONLY THE FIRST ERR_LEN - 3 characters ARE
		STORED IN err_lex.THEN THREE DOTS ... ARE ADDED TO THE END OF THE
		err_lex C - type string.
		BEFORE RETURNING THE FUNCTION MUST SET THE APROPRIATE TOKEN CODE
		*/

			Token t;
			/* converting a string to a long */
			long lex_deciaml = atol(lexeme);
			/* check the range of the lexeme must be 2 byte */
			if (lex_deciaml < -32767 || lex_deciaml > 32767) {
				/* Token code is set to the Error Token */
				t.code = ERR_T;
				/* checks if the lexemes are greater than or equal to the 20 charataters is true */
				if (strlen(lexeme) >= ERR_LEN) {
					/* first three charcterss will be srored */
					strncpy(t.attribute.err_lex, lexeme, ERR_LEN - 3);
					/* the three dors will be stored in the end of the lexemes(type string) */
					t.attribute.err_lex[ERR_LEN - 3] = '.';
					t.attribute.err_lex[ERR_LEN - 2] = '.';
					t.attribute.err_lex[ERR_LEN - 1] = '.';
					/* adds the character in the end */
					t.attribute.err_lex[ERR_LEN] = SEOF;
					return t;
				}
				/* storing the first ERR_LEN character */
				strncpy(t.attribute.err_lex, lexeme, ERR_LEN);
				return t;
			}
			/* Token code is set to the Integer Literal */
			t.code = INL_T;
			t.attribute.int_value = (int)lex_deciaml;
			return t; /* return DIL token */
}

/*ACCEPTING FUNCTION FOR THE integer literal(IL)-hexadecimal constant(HIL)
*/

/***********************************************************************************************************
* Purpose:		   This function will check for the interger literal (IL) - hexadecimal constamt (HIL)
* Author:          Maryum Awan  (040 854 923)
* Version:         1.0
* Called functions:atolh()
*				   strlen()
*				   strncpy()
* Parameters :	   char *lexeme
* Return value:    HIL token
* Alogrithm:	   1. converting the lexeme to a hexadecimal constant
*				   2. checks the range and returns an error token
*				   3. THREE DOTS ... ARE ADDED TO THE END OF THE err_lex C-type string
*				   4. checks if the lexemes are greater than or equal to the ERR_LEN
*				   5. returning HIL token
***********************************************************************************************************/

Token aa_func11(char *lexeme) {

	Token t;
	/* convert the lexeme value to hexadecimal value and storing int long */
	long lex_hex = atolh(lexeme);

	/*THE FUNCTION MUST CONVERT THE LEXEME REPRESENTING AN HEXADECIMAL CONSTANT
		TO A DECIMAL INTEGER VALUE WHICH IS THE ATTRIBUTE FOR THE TOKEN.
		THE VALUE MUST BE IN THE SAME RANGE AS the value of 2 - byte integer in C.
		THIS FUNCTION IS SIMILAR TO THE FUNCTION ABOVE
		THE MAIN DIFFERENCE IE THAT THIS FUNCTION CALLS
		THE FUNCTION atolh(char * lexeme) WHICH CONVERTS AN ASCII STRING
		REPRESENTING AN HEXADECIMAL NUMBER TO INTEGER VALUE
		IN CASE OF ERROR(OUT OF RANGE) THE FUNCTION MUST RETURN ERROR TOKEN
		THE ERROR TOKEN ATTRIBUTE IS  lexeme.IF THE ERROR lexeme IS LONGER
		than ERR_LEN characters, ONLY THE FIRST ERR_LEN - 3 characters ARE
		STORED IN err_lex.THEN THREE DOTS ... ARE ADDED TO THE END OF THE
		err_lex C - type string.
		BEFORE RETURNING THE FUNCTION MUST SET THE APROPRIATE TOKEN CODE
		*/
			/* checks if the range is in 2 bytes or not */
			if (lex_hex > SHRT_MAX || lex_hex <= 0) {
				/* Token code is set to the Error Token*/
				t.code = ERR_T;
				/* checks if the lexemes are greater than or equal to the ERR_LEN is true  */
				if (strlen(lexeme) >= ERR_LEN) {
					/* first err_len -3 charcterss will be srored in the error token  */
					strncpy(t.attribute.err_lex, lexeme, ERR_LEN - 3);
					/* the three dots will be stored in the end of the lexemes(type string) */
					t.attribute.err_lex[ERR_LEN - 3] = '.';
					t.attribute.err_lex[ERR_LEN - 2] = '.';
					t.attribute.err_lex[ERR_LEN - 1] = '.';
					/* adds the character in the end */
					t.attribute.err_lex[ERR_LEN] = SEOF;
					return t;
				}
				/* storing the first ERR_LEN character */
				strncpy(t.attribute.err_lex, lexeme, ERR_LEN);
				return t;
			}
			/* Token code is set to the Integer Literal */
			t.code = INL_T;
			t.attribute.int_value = (int)lex_hex;
			return t; /* returning HIL token */
		}
	
/*ACCEPTING FUNCTION FOR THE ERROR TOKEN
*/

/***********************************************************************************************************
* Purpose:		   This function will check for error token
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:strlen()
* Parameters :	   char *lexeme
* Return value:    ERR_T token
* Alogrithm:	   1. set the ERROR TOKEN.lexeme[]
*				   2. checks if the lexemes are greater than or equal to the ERR_LEN
*				   3. THREE DOTS ... ARE ADDED TO THE END OF THE err_lex C-type string
*				   4. returning ERR_T token
***********************************************************************************************************/

Token aa_func12(char *lexeme) {
	Token t;
	unsigned int i = 0;
		/* Token code is set to the error token */
		t.code = ERR_T; 

			/*THE FUNCTION SETS THE ERROR TOKEN.lexeme[] CONTAINS THE ERROR
				THE ATTRIBUTE OF THE ERROR TOKEN IS THE lexeme ITSELF
				AND IT MUST BE STORED in err_lex.IF THE ERROR lexeme IS LONGER
				than ERR_LEN characters, ONLY THE FIRST ERR_LEN - 3 characters ARE
				STORED IN err_lex.THEN THREE DOTS ... ARE ADDED TO THE END OF THE
				err_lex C - type string.
				BEFORE RETURNING THE FUNCTION MUST SET THE APROPRIATE TOKEN CODE*/

			/* checks the lexemes length more than error length(ERR_LEN) */						
			if (strlen(lexeme) > ERR_LEN) {
				for (i = 0; i < ERR_LEN-3; i++) {
					/* only the err_len -3 will stored in the error token */
					t.attribute.err_lex[i] = lexeme[i];
				}
				/* the three dots will be stored in the end of the lexemes(type string)*/
				t.attribute.err_lex[ERR_LEN - 3] = '.';
				t.attribute.err_lex[ERR_LEN - 2] = '.';
				t.attribute.err_lex[ERR_LEN - 1] = '.';
				/* adds the character in the end */
				t.attribute.err_lex[i] = SEOF;
				return t;
			}		
			/* if above condition is not true then the lexeme size will be stored in error token */
			for (i = 0; i < strlen(lexeme); i++) {
				t.attribute.err_lex[i] = lexeme[i];
			}
			t.attribute.err_lex[i] = SEOF;
			return t; /* returning error token*/
		}
		
/*CONVERSION FUNCTION:
THE FUNCTION CONVERTS AN ASCII STRING
REPRESENTING AN HEXADECIMAL INTEGER CONSTANT TO INTEGER VALUE*/

/***********************************************************************************************************
* Purpose:		   This function converts an ASCII string representing an hexademical integer constant to interger value
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:strlol()
* Parameters :	   char *lexeme
* Return value:    hex_value
* Alogrithm:	   1. converting the hex value to an integer value
***********************************************************************************************************/

long atolh(char * lexeme) {
	long hex_value = strtol(lexeme, NULL, 16);
	/* converting the hex value to an integer value */
	return (int) hex_value; 
}

/*HERE YOU WRITE YOUR ADDITIONAL FUNCTIONS(IF ANY).
FOR EXAMPLE*/

/***********************************************************************************************************
* Purpose:		   This function determines if the word is a keyword
* Author:          Maryum Awan  (040 854 923)
* Version:         1.0
* Called functions:strcmp()
* Parameters :	   char * kw_lexeme
* Return value:    RT_FAIL1
* Alogrithm:	   1. scan the keyword array till the match is found 
*				   2. return its position or else false (-1)
***********************************************************************************************************/

int iskeyword(char * kw_lexeme) {
	int i;
		/* go through the keyword array to look for a match */
		for (i = 0; i < KWT_SIZE; i++) {
		if (strcmp(kw_lexeme, kw_table[i]) == 0) {
		return i; 
		}
	}
	/* returns False (-1) */
	return RT_FAIL1;
}