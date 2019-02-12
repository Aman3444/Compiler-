/**************************************************************************************************************************************************************************************************
* File name:       buffer.c
* Version:         1.0
* Compiler:		   MS Visual Studio 2015
* Author:          Maryum Awan  (040 854 923)
*				   Aman Multani (040 877 727)
* Course:          CST 8152 – Compilers, Lab Section: 011
* Assignment:      2
* Submitting Date: March 30th, 2018
* Due Date:        March 23rd, 2018
* Professor:       Svillen Ranev
* Purpose:         To implement a buffer that can operate in three different modes: a "fixed-size buffer", an "additive self-incrementing" buffer, a "multiplicative self-incremting" buffer
* Function list:   b_allocate()			creates a new buffer in memory (on the program heap)
*				   b_addc()				add the character s to the character array of the given buffer pointed by pBD
*				   b_clear()			retains the memory space currently allocated to the buffer, but re-initializes all appropiate data members of the given Buffer structure
*				   b_free()				rees the memory occupied by the character buffer and the Buffer structure
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
**************************************************************************************************************************************************************************************************/

#include <stdlib.h>
#include <limits.h>
#include "buffer.h"

/***********************************************************************************************************
* Purpose:		   Function creates a new buffer in memory (on the program heap).
* Version:		   1.0
* Author:		   Aman Multani (040 877 727)
* Called Functions:calloc(),
*				   malloc(),
*				   free()
* Paramenters:	   short init_capacity:	 must be between 1 and the Maximum allowed posutive value - 1
*				   char inc_factor:      The desired increment factor for the buffer
*										 Valid between 1 and 255 in additive mode
*										 Valid between 1 and 100 in multiplicative mode
*				   char o_mode:			 desired buffer operational mode indicator (Valid if 'f','a' or 'm')
* Return value:	   NULL					 when the buffer cannot be allocated
*				   buffer				 when allocation is successful
* Algorithm:	   allocates memory for Buffer structure
*				   allocates memory for dynamic charater buffer
*				   sets buffer operational mode indicator mode
*				   copies the given init_capacity value into the Buffer structure capacity variable
***********************************************************************************************************/

Buffer * b_allocate(short init_capacity, char inc_factor, char o_mode) {
	
	Buffer* buffer = NULL; /* pointer to a new buffer */
	/* if int_capacity is less than 0, greater than -1 and inc _ factor less than 0, greater than -1 */
	if (init_capacity >= 0 && init_capacity <= SHRT_MAX - 1) {
		/* Allocate memory for one buffer structure using calloc() */
		buffer = (Buffer*)calloc(1, sizeof(Buffer));
		/* Checks if the buffer is empty andd returns null */
		if (!buffer)
			return NULL;
		if (o_mode == 'f' && init_capacity != 0) { /*fixed mode wide inc factor 0*/
			buffer->mode = 0;
			buffer->inc_factor = 0;
		}
		else if (o_mode == 'a' && (inc_factor >= 1 && (unsigned char)inc_factor <= MAX_ADD_INC_FACTOR)) {  /* set add mode */
			buffer->mode = 1;
			buffer->inc_factor = (unsigned char)inc_factor;
		}
		/*  If the o_mode is m and inc_factor is in the range of 1 to 100 inclusive, the mode is set to number -1 and the inc_factor vakue is assigned to the buffer inc_factor */
		else if (o_mode == 'm' && inc_factor >= 1 && inc_factor <= MULT_MAX_INC_FACTOR) {  
			buffer->mode = -1;
			buffer->inc_factor = inc_factor;
		}
		else {
			/* if error occured there's not enough so you should free previous struct */
			free(buffer);
			return NULL;
		}
		buffer->capacity = init_capacity;  /* Set capacity to initiale capacity */
		buffer->cb_head = (char*)malloc((init_capacity)*(sizeof(char))); /* Allocate dynamic buffer */
	
		if (!buffer->cb_head)
			/* if cb_head= null then there's not enough memory as you should free previous struct */
			free(buffer);
	}
	return buffer;
}

/***********************************************************************************************************
* Purpose:		   The function adds the character symbol to the character array of the given buffer pointed by pBD
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:realloc()
* Parameters :	   pBuffer const pBD	A pointer to the buffer to be operated on
*										Valid if not null
*				   char symbol			char to be added to the buffer
* Return value:    NULL					function must return NULL on any error
*				   pBD					returns a pointer to the Buffer structure
*				   point
* Alogrithm:	   checks if there is available space in the buffer
*                  checks if buffer is not full, the symbol can be stored in the character buffer
*		           check the operational mode, function returns different value operational mode indicator mode
***********************************************************************************************************/

pBuffer b_addc(pBuffer const pBD, char symbol) {
	/* The change in buffer capacity given multiplicative mode */
	short new_increment = 0;
	/* The usable space available in the buffer */
	short available_space = 0;
	/* Calculated new capacity of the buffer; used if buffer is resized */
	short new_capacity = 0;
	/* Pointer to new character point; used if character buffer is relocated */
	char * point = NULL;

	/* Checks if the buffer is empty and returns null if it is */
	if (pBD == NULL) {
		return NULL;
	}
	/* setting r_flag to 0 */
	pBD->r_flag = ZERO_INIT_CAPACITY;

	/* If there is available space in the buffer */
	if ((short)(pBD->addc_offset * sizeof(char)) != (pBD->capacity)){
		pBD->cb_head[pBD->addc_offset++] = symbol;
		return pBD;
	}
	else {
		/* if the mode is 0 then return NULL */
		if (pBD->mode == FIX_MODE) {
			return NULL;
		}
		/* if the mode is 1 then do this stuff */
		else if (pBD->mode == ADDITION_OP_MODE) {
			/* Calculate the new capacity relative to the buffer's mode */
			new_capacity = (pBD->capacity + (short)sizeof(char)* (unsigned char)pBD->inc_factor);
			 if (new_capacity <= MAX_VALUE) {
				pBD->capacity = new_capacity;
			}
			else {
				pBD->capacity = MAX_VALUE;
			}
		}
		else if (pBD->mode == MULTIPLE_OP_MODE) {
			if (pBD->capacity == MAX_VALUE) {
				return NULL;
			}
			/* Calculate the new capacity relative to the buffer's mode */
			available_space = MAX_VALUE - pBD->capacity;
			/* Calculate new capacity */
			new_increment = (short)((available_space * pBD->inc_factor) / 100.0F);
			if (new_increment == 0)	{
				new_increment = MAX_VALUE;
			}
			else {
				new_capacity = (short)(pBD->capacity + new_increment);
				pBD->capacity = new_capacity;
			}
			/* Increase the capacity by using realloc */
			point = realloc(pBD->cb_head, new_capacity);

			if (pBD->cb_head == NULL) {
				return NULL;
			}
			/* Check if the memory location has moved for the cb_head */
			if (pBD->cb_head != point) {
				pBD->cb_head = point;
				pBD->r_flag = SET_R_FLAG;
			}
		}
			/* Add new character to the newly allocated buffer */
			pBD->cb_head[pBD->addc_offset++] = symbol;
	}
		return pBD;
}
	
/***********************************************************************************************************
* Purpose:		   The function retains the memory space currently allocated to the buffer, but re-initializes all appropiate data members of the given Buffer structure
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
*				   char symbol			char to be added to the buffer
* Return value:    -1 (R_FAIL1)			if run-time error is possible, it will return -1
*				   EXIT_SUCCESS
* Algorithm:	   reset everything in the buffer
*************************************************************************************************/

int b_clear(Buffer * const pBD) {
	if (!pBD) {	/* This will checks if there are any runtime errors ,and it will return -1 if any found */

		return RT_FAIL1;
	}
	/* Clear instruction */
	pBD->addc_offset = 0;
	pBD->eob = 0;
	pBD->getc_offset = 0;
	pBD->markc_offset = 0;
	pBD->r_flag = 0;
	return EXIT_SUCCESS;
}

/***********************************************************************************************************
* Purpose:		   The function frees the memory occupied by the character buffer and the Buffer structure
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:free()
* Parameters :	   Buffer * const pBD
* Return value:    NULL
* Alogrithm:	   Checks if the buffer is empty and returns NULL if it is
*				   If the buffer is not empty, the memory of the character buffer array and the buffer struct are freed
***********************************************************************************************************/

void b_free(Buffer * const pBD) {
	/* Checks if the buffer is empty and returns NULL if it is */
	if (pBD == NULL) {
		return;
	}
	/*  If the buffer is not empty, the memory of the character buffer array and the buffer struct are freed */

	/* Free Character Buffer */
	if (!pBD->cb_head)
	free(pBD->cb_head);
	/* Free Buffer Structure */
	free(pBD);
	pBD->cb_head = NULL;
}

/***********************************************************************************************************
* Purpose:		   The function checks if the character buffer it full and returns 1 if the character buffer is full; it returns 0 otherwise
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:	   -1 (RT_FAIL1)			If a run-time error is possible, the function should return –1
*				   0 (FALSE)			if the character buffer is not full it returns FALSE
*				   1 (TRUE)				returns TRUE if the character buffer is full
* Alogrithm:	   check if the character buffer is full or not
check the run-time errors
***********************************************************************************************************/

int b_isfull(Buffer* const pBD) {
	/* Checks if the buffer is empty and returns -1 if it is */
	if (!pBD) {
		return RT_FAIL1;
	}
	/* returns TRUE if the character buffer is full */
	if (pBD->addc_offset == pBD->capacity) {
		return TRUE;
	}
	/* if the character buffer is not full it returns FALSE */
	else {
		return FALSE;
	}
}

/***********************************************************************************************************
* Purpose:		   The function returns the current limit of the character buffer
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:    RT_FAIL1			if runtime error is possible return RT_FAIL1
*				   pBD				returns buffer's addc_offset
* Alogrithm:	   check the run-time errors
***********************************************************************************************************/

short b_limit(Buffer* const pBD) {
	/* Checks if the buffer is empty and returns -1 if it is */
	if (pBD == NULL) {
		return RT_FAIL1;
	}
	/* returns the current limit of the character buffe */
	return pBD->addc_offset;
}

/***********************************************************************************************************
* Purpose:		   The function returns the current capacity of the character buffer
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:    RT_FAIL1   		if runtime error is possible return RT_FAIL1
*				   pBD				returns the current capacity of the character buffer
* Alogrithm:	   check the run-time errors
***********************************************************************************************************/

short b_capacity(Buffer * const pBD) {
	/* Checks if the buffer is empty and returns -1 if it is */
	if (pBD == NULL) {
		return RT_FAIL1;
	}
	/* returns the current capacity of the character buffer */
	return pBD->capacity;
}

/***********************************************************************************************************
* Purpose:		   The function sets markc_offset to mark. The function returns the currently set markc_offset
* Author:          Aman Multani	(040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:    RT_FAIL1   		if runtime error is possible return RT_FAIL1
*				   pBD				returns the markc_offset in the character buffer
* Alogrithm:	   check the run-time errors
***********************************************************************************************************/

short b_mark(pBuffer  const pBD, short mark) {
	/* Checks if the buffer is empty and returns -1 if it is */
	if (!pBD) {
		return RT_FAIL1;
	}
	/* check the mark must be within the current limit of the buffer */
	if (mark < 0 || mark > pBD->addc_offset) {
	/* if runtime error is possible return RT_FAIL1 */
		return RT_FAIL1;
	}
	pBD->markc_offset = mark;
	/* return currently mark_offset */
	return pBD->markc_offset;

}

/***********************************************************************************************************
* Purpose:		   The function returns the value of mode to the calling function.
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:    RT_FAIL2   		if runtime error is possible return RT_FAIL2
*				   pBD				returns the value of the mode
* Alogrithm:	   check the run-time errors
***********************************************************************************************************/

int b_mode(Buffer * const pBD) {
	/* Checks if the buffer is empty and returns -2 if it is */
	if (pBD == NULL) {
		return RT_FAIL2;
	}
	/* returns the value of mode */
	return pBD->mode;
}

/***********************************************************************************************************
* Purpose:		   The function returns the non-negative value of inc_factor to the calling function.
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:    SIZE_0x100		if runtime error is possible return SIZE_0x100
*				   pBD				returns non-negative value of inc_factor
* Alogrithm:	   check the run-time errors
***********************************************************************************************************/

size_t b_incfactor(Buffer * const pBD) {
	/* Checks if the buffer is empty and returns 0*100 or 256 if it is */
	if (!pBD) {
		return SIZE_0X100;
	}
	/* returns non-negative value of inc_factor */
	return(size_t) (unsigned char)pBD->inc_factor;
}

/***********************************************************************************************************
* Purpose:		   The function loads an open input file specified by fi ino a buffer specified by pBD
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:fof
*				   feof
*				   b_addc
* Parameters :	   Buffer * const pBD
*				   FILE fi
* Return value:    RT_FAIL1   		if runtime error is possible return RT_FAIL1
*				   count			returns how many characters are added into the buffer from the file
* Alogrithm:	   Add character into the buffer until we reach the end of the file
*				   check the run-time errors
***********************************************************************************************************/

int b_load(FILE * const fi, Buffer * const pBD) {
	/* varaible from file to add to buffer */
	char FileChar;
	/* keep track of how many character are added in the buffer */
	short count = 0;
	/* Checks if the buffer is empty and returns -1 if it is */
	if (!fi || !pBD) {
		return RT_FAIL1;
	}
	/* this operation is repeated until the standard macro feof(fi) detects end-of-file on the input file */
	while (TRUE) {
		/* read one character at a time */
		FileChar = (char)fgetc(fi);
		/* to check the end of the file */
		if (feof(fi)) {
			break;
		}
		/* if current character cannot be put in the buffer */
		if (!b_addc(pBD, FileChar)) {
			return LOAD_FAIL;
		}
		count++;
	}
	/* return the number of characters added to the buffer. */
	return count;
}

/***********************************************************************************************************
* Purpose:		   The function checks if the buffer is empty or not
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:    RT_FAIL1   		if runtime error is possible return RT_FAIL1
*				   0 (FALSE)		If the addc_offset is 1, the function returns 0
*				   ONE (TRUE)	    If the addc_offset is 0, the function returns 1
* Alogrithm:	   check the value of add_offset
*				   check the run-time errors
***********************************************************************************************************/

int b_isempty(Buffer * const pBD) {
	/* Checks if the buffer is empty and returns -1 if it is */
	if (!pBD) {
		return RT_FAIL1;
	}
	/* checks if the addc_offset is 1, then returns 0 */
	if (pBD->addc_offset == 0) {
		return TRUE;
	}
	/* checks if the addc_offset is 0, the returns 1 */	
		return FALSE;	
}

/***********************************************************************************************************
* Purpose:		   The function returns eob to the calling function
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:    RT_FAIL1   		if runtime error is possible return RT_FAIL1
*				   pBD				returns eob to the calling function
* Alogrithm:	   return eob to the calling functon
*				   check the run-time errors
***********************************************************************************************************/

int b_eob(Buffer * const pBD) {
	/* Checks if the buffer is empty and returns -1 if it is */
	if (!pBD) {
		return RT_FAIL1;
	}
	/* the function return eob to the calling functon */
	return pBD->eob;
}

/***********************************************************************************************************
* Purpose:		   The function checks the argument for validity and returns the character located at getc_offset
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:    RT_FAIL2   		if runtime error is possible return RT_FAIL2 (-2)
*				   RT_FAIL1   		if runtime error is possible return RT_FAIL1 (-1)
*				   pBD				returns the character located at getc_offset
* Alogrithm:	   checks the argument for validity
*				   returns the character located at getc_offset
*				   check the run-time errors
***********************************************************************************************************/

char b_getc(Buffer * const pBD) {
	/* Checks if the buffer is empty and returns -2 if it is */
	if (pBD == NULL) {
		return RT_FAIL2;
	}
	if (b_isempty(pBD))
		return RT_FAIL2;
	/* if getc_offset and addc_offset are equal, it sets eob to 1 and returns -1;
	otherwise it sets eob to 0; */
	if (pBD->getc_offset == pBD->addc_offset) {
		/* End of buffer (eob) */
		pBD->eob = 1;
		return RT_FAIL1;
	}
	else {
		/* Reseting the flag */
		pBD->eob = 0;
		/* Increment get offset by 1 */
	}
	return pBD->cb_head[pBD->getc_offset++];
}

/***********************************************************************************************************
* Purpose:		   The function prints character by character the contents of the character buffer to the standard output (stdout).
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:b_eob()
*				   b_getc()
* Parameters :	   Buffer * const pBD
* Return value:    RT_FAIL1   		if runtime error is possible return RT_FAIL1 (-1)
*				   count			returns the character located at getc_offset
* Alogrithm:	   print new line character
*				   if buffer is empty, print buffer empty string
*				   print the values within it by calling the b_getc() function
*				   check the run-time errors
***********************************************************************************************************/

int b_print(Buffer * const pBD) {
	/* buffer to print */
	char numChar;
	/* catch the character counter */
	int count = 0;
	/* Checks if the buffer is empty and returns -1 if it is */
	if (!pBD) {
		return RT_FAIL1;
	}
	/* Checking the addc_offset is 0 or not */
	 if (pBD->addc_offset == 0) {
		/* prints the following message Empty buffer. adding a new line at the end and returns */
		printf("Empty buffer.\n");
		return RT_FAIL1;
	}
		numChar = b_getc(pBD);
		/* While not the end of the buffer */
		while (!b_eob(pBD)) {
			printf("%c", (char)numChar);		
			numChar = b_getc(pBD); /* numChar to grab the first chararcter before the while loop */
			count++;
		}
		/* Printing a new line */
		printf("\n");
	return count;
}

/***********************************************************************************************************
* Purpose:		   The function uses realloc() to adjust the new capacity and then updates all the necessary memmbers of the buffer descriptor structure
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:realloc()
* Parameters :	   Buffer * const pBD
* Return value:    NULL  			if the buffer struct is null
*				   point			returning a pointer to Buffer
* Alogrithm:	   check the run-time errors
*                  set buffer capacity to new_Space
*				   set r_flag to SET_R_FLAG (1)
*				   adds symbol to the end of the buffer
*				   define and adjust memory of the new capacity
***********************************************************************************************************/

Buffer * b_compact(Buffer * const pBD, char symbol) {
	char * point = NULL;
	/* set buffer capacity to n_Space */
	short newCapacity = 0;

	/* Checks if the buffer is empty and returns  NULL if it is */
	if (pBD == NULL) {
		return NULL;
	}
	if (pBD->addc_offset == SHRT_MAX)return  NULL;
	pBD->r_flag = 0;
	/* updates all the necessary members of the buffer descriptor structure */
	newCapacity = sizeof(char)*(pBD->addc_offset + 1);
	/* new capacity is addc_offset + 1 converted to bytes */
	point = (char *)realloc(pBD->cb_head, newCapacity);

	/* checks if the head is null or not */
	if (point == NULL) {
		return NULL;
	}

	/* set the buffer r_flag appropriately */
	else if (point!= pBD->cb_head) {
		pBD->r_flag = SET_R_FLAG; /* setting r_flag to 1 */
		pBD->cb_head = point;

	}
	/* adds the symbol to the end of the character buffer*/
	pBD->capacity = newCapacity;
	pBD->cb_head[pBD->addc_offset++] = symbol;
	/* returning a pointer to Buffer */
	return pBD;
}

/***********************************************************************************************************
* Purpose:		   The function returns r_flag to the calling function
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:    RT_FAIL1   		if runtime error is possible return RT_FAIL1 (-1)
*				   pBD				returns the flag variable of buffer structure
* Alogrithm:	   check the run-time errors
***********************************************************************************************************/

char b_rflag(Buffer * const pBD) {
	/* Checks if the buffer is empty and returns -1 if it is */
	if (pBD == NULL) {
		return RT_FAIL1;
	}	
	/* returns r_flag to the calling function */
	return pBD->r_flag;
}

/***********************************************************************************************************
* Purpose:		   The function decrements getc_offset by 1
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:    RT_FAIL1   		if runtime error is possible return RT_FAIL1 (-1)
*				   pBD				returns the character located at getc_offset
* Alogrithm:	   check the run-time errors
***********************************************************************************************************/

short b_retract(Buffer * const pBD) {
	/* Checks if the buffer is empty and returns -1 if it is */
	if (pBD == NULL) {
		return RT_FAIL1;
	}
	/* If offset cannot retract */
	if (pBD->getc_offset == 0)
	return RT_FAIL1;
	/* decrements getc_offset by 1 */
		pBD->getc_offset--;
	return pBD->getc_offset;
}

/***********************************************************************************************************
* Purpose:		   The function sets getc_offset to the value of the current markc_offset .
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:    RT_FAIL1   		if runtime error is possible return RT_FAIL1 (-1)
*				   pBD				returns the character located at getc_offset
* Alogrithm:	   set the value of getc_offset to current markc_offset
*				   check the run-time errors
***********************************************************************************************************/

short b_reset(Buffer * const pBD) {
	/* Checks if the buffer is empty and returns -1 if it is */
	if (pBD == NULL) {
		return RT_FAIL1;
	}
		/* setting up getc_offset to the value of the current markc_offset */
		pBD->getc_offset = pBD->markc_offset;
		/* returns the located at getc_offset */
		return pBD->getc_offset;	
}

/***********************************************************************************************************
* Purpose:		   The function returns getc_offset to the calling function
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:    RT_FAIL1   		if runtime error is possible return RT_FAIL1 (-1)
*				   pBD				returns the character getc_offset to the calling fuction
* Alogrithm:	   set the value of getc_offset to current markc_offset
*				   check the run-time errors
***********************************************************************************************************/

short b_getc_offset(Buffer * const pBD) {
	/* Checks if the buffer is empty and returns -1 if it is */
	if (pBD == NULL || !pBD->getc_offset) {
		return RT_FAIL1;
	}
	/* returns the getc_offset to the calling fuction */
	return pBD->getc_offset;
}

/***********************************************************************************************************
* Purpose:		   The function set the getc_offset and markc_offset to 0, so that the buffer can be reread again
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:    RT_FAIL1   		if runtime error is possible return RT_FAIL1 (-1)
*				   0
* Alogrithm:	   set the getc_offset and markc_offset to 0
*				   check the run-time errors
***********************************************************************************************************/

int b_rewind(Buffer * const pBD) {
	/* Checks if the buffer is empty and returns -1 if it is */
	if (!pBD) {
		return RT_FAIL1;
	}
	/* set the getc_offset to 0, so that the buffer can read agian */
	pBD->getc_offset = OFFSET_RESET;
	/* set the markc_offical to 0, so that the buffer can read again */
	pBD->markc_offset = OFFSET_RESET;
		return 0;
}

/***********************************************************************************************************
* Purpose:		   The function returns a pointer to a location of the character buffer indicated by loc_offset
* Author:          Aman Multani (040 877 727)
* Version:         1.0
* Called functions:None
* Parameters :	   Buffer * const pBD
* Return value:    NULL				failure
*				   pD				returns if no run-timr errors happen
* Alogrithm:	   returns pointer to a location of the character buffer indicated by loc_offset
*				   check the run-time errors
***********************************************************************************************************/

char * b_location(Buffer * const pBD, short loc_offset) {
	/*Checks if the buffer is empty and returns  NULL if it is */
	if (!pBD) {
		return NULL;
	}
	/* returns a pointer to a location of the character buffer */
	return  pBD->cb_head + loc_offset;
}