#include "buffer.h"
/**********************************************************************
Filename:		   buffer.c
Compiler:		   MS Visual Studio 2015
Author:            Ahmad Kaafi, 040 708 958
Course Name/Number: CST8152 - Compilers, Lab Section: [12]
Assignment #:      1
Date:			   Februaray 09 2018
Professor:         Svillen Ranev
Purpose:           You are to implement a buffer that can operate in three
				   different modes: a “fixed-size” buffer, an
				   “additive self-incrementing” buffer, and a
				   “multiplicative self-incrementing” buffer.
Function list:	   b_allocate(); b_addc(); b_clear(); b_free(); b_isfull();
				   b_limit(); b_capacity(); b_mark(); b_mode(); b_incfactor();
				   b_load(); b_isempty(); b_eob(); b_getc(); b_print():
				   b_compact(); b_rflag(); b_retract(); b_reset(); b_getcoffset();
				   b_rewind(); b_location();
*********************************************************************/


/********************************************************************
Purpose:               This function creates a new buffer in memory.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            short init_capacity, char inc_factor , char o_mode
Return value:          NULL, pBuffer
Algorithm:			   Allocate memory for one Buffer using calloc
					   Allocate memory for one dynamic character Buffer
					   Set the buffer's operational mode and inc_factor
					   Copies the givin inc_capacity into the one Buffer
**********************************************************************/
Buffer * b_allocate(short init_capacity, char inc_factor, char o_mode)
{
	Buffer *pBuffer;

	if (init_capacity < 0 || init_capacity > MAX_VALUE) /* checks if init_capacity is within range*/
		return NULL;

	if ((unsigned char)inc_factor< 0 || (unsigned char)inc_factor > MAX_VALUE)/* checks if inc_factor is within range*/
		return NULL;

	else
		pBuffer = (Buffer*)calloc(1, sizeof(Buffer)); /*allocates memory for pBuffer */


	if (!pBuffer)
		return NULL;

	if (0 <= init_capacity || init_capacity <= MAX_VALUE)/*Checks if init_capacity is within range and mallocs cb_head if it is*/
		pBuffer->cb_head = (char*)malloc(init_capacity);

	/*If the o_mode is the symbol F or inc_factor is 0, the mode and the buffer inc_factor are set to number 0 */
	if (o_mode == 'f' || inc_factor == 0)
	{
		if (init_capacity == 0)
			return NULL;

		pBuffer->mode = 0;
		pBuffer->inc_factor = 0;

	}
	/*If the o_mode is the symbol F or inc_factor is not 0, the mode and the buffer inc_factor are set to number 0 */
	else if (o_mode == 'f' && inc_factor != 0)
	{
		if (init_capacity == 0)
			return NULL;

		pBuffer->mode = 0;
		pBuffer->inc_factor = 0;

	}
	/*If the o_mode is the symbol A or inc_factor is range of 1 to 255 inclusive, the mode is set to 1 and the buffer inc_factor is set to inc_factor */
	else if (o_mode == 'a' && (unsigned char)inc_factor >= 1 && (unsigned char)inc_factor <= 255)
	{
		pBuffer->mode = 1;
		pBuffer->inc_factor = (unsigned char)inc_factor;

	}
	/*If the o_mode is the symbol M or inc_factor is range of 1 to 100 inclusive, the mode is set to -1 and the buffer inc_factor is set to inc_factor */
	else if (o_mode == 'm' && (unsigned char)inc_factor >= 1 && (unsigned char)inc_factor <= 100)
	{
		pBuffer->mode = -1;
		pBuffer->inc_factor = (unsigned char)inc_factor;

	}
	else
		return NULL;

	pBuffer->capacity = init_capacity;

	return pBuffer;


}
/********************************************************************
Purpose:               The function resets r_flag to 0 and tries to
					   add the character symbol to the character array
					   of the given buffer pointed by pBD.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            pBuffer const pBD, char	symbol
Return value:          NULL, pBD, tempadd
Algorithm:			   If the Buffer is not full the symbol can be stored.
					   If the Buffer is not full it will resize the Buffer.
					   If the operational mode is 0, it returns NULL.
					   If the operational mode is 1, it tries to increase
					   the current capacity.
					   If the operational mode is -1,  it tries to increase
					   the current capacity by using a formulae.
**********************************************************************/
pBuffer b_addc(pBuffer const pBD, char	symbol)
{
	short newcapacity; /*holds the pBD->capacity + newinc*/
	short space; /*holds the value of MAX_VALUE - pBD->capacity*/
	short newinc; /*holds the value of space* pBD->inc_factor /100*/
	short addcap; /*holds the value of pBD->capacity + pBD->inc_factor*/
	Buffer *tempadd = pBD; /*to hold the new memory allocation*/

	if (!pBD)
		return NULL;


	pBD->r_flag = 0;/*Sets r_flag to 0*/

	if (pBD->addc_offset < pBD->capacity)/*If the buffer is not full add the symbol*/
	{
		pBD->cb_head[pBD->addc_offset] = symbol;
		++pBD->addc_offset;

		return pBD;
	}


	else
	{
		if (pBD->mode == 0)/*if the mode is 0 it returns NULL*/
			return NULL;

		else if (pBD->mode == 1)/*if the mode is 1 it tries to increase the capacity of the buffer*/
		{
			addcap = (pBD->capacity + (unsigned char)pBD->inc_factor); /*Sets the value by adding capacity and inc_factor (converted to bytes)*/

			if (addcap <= 0)/*Checks for overflow*/
				return NULL;

			if (addcap < MAX_VALUE)/*Set capcity to addcap if addcap is less than MAX_VALUE*/
				pBD->capacity = addcap;

			else
				pBD->capacity = MAX_VALUE;
		}

		else if (pBD->mode == -1)/*if the mode is -1 it it tries to increase the capacity of the buffer by using a formulae*/
		{
			if (pBD->capacity == MAX_VALUE)
				return NULL;

			else
			{
				space = MAX_VALUE - pBD->capacity; /*Set remaining space of the capacity to space*/
				newinc = ((space * (unsigned char)pBD->inc_factor) / 100);

				if (newinc < 1)
					newinc = 1;

				newcapacity = (short)(pBD->capacity + newinc);

				tempadd->cb_head = realloc(pBD->cb_head, newcapacity); /*Reallocates memory based on newcapacity*/

				pBD->cb_head = tempadd->cb_head;


				if (pBD->cb_head == NULL)/*checks if the allocation was a success*/
					return NULL;

				if (tempadd->cb_head != pBD->cb_head)/*checks if the memory location are the same and sets r_flag if they are not*/
					pBD->r_flag = SET_R_FLAG;

				tempadd->capacity = (short)newcapacity;

				if (b_isfull(pBD) != 0)/*Checks if the buffer is full*/
					return NULL;

				tempadd->cb_head[pBD->addc_offset] = symbol;/*Adds the symbol to the character buffer*/
				++tempadd->addc_offset;
				return tempadd;
			}

		}
	}

	tempadd->cb_head = (char*)realloc(pBD->cb_head, pBD->capacity);/*Reallocates memory based on pBD->capacity*/

	if (pBD->cb_head == NULL)/*checks if the allocation was a success*/
		return NULL;

	if (tempadd->cb_head != pBD->cb_head)/*checks if the memory location are the same and sets r_flag if they are not*/
		pBD->r_flag = SET_R_FLAG;

	pBD->cb_head[pBD->addc_offset] = symbol;/*Adds the symbol to the character buffer*/

	if (b_isfull(pBD) != 0)/*Checks if the buffer is full*/
		return NULL;

	++pBD->addc_offset;
	return pBD;

}
/********************************************************************
Purpose:               The function retains the memory space currently
					   allocated to the buffer.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:          RT_FAIL1, 1
Algorithm:			   Re-initializes all appropriate data members of the
					   given Buffer structure (buffer descriptor), such
					   that the buffer will appear empty and the next
					   call to b_addc() will put the character at
					   the beginning of the character buffer.
**********************************************************************/
int b_clear(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL1;

	pBD->addc_offset = 0;
	pBD->markc_offset = 0;
	pBD->r_flag = 0;
	pBD->eob = 0;
	pBD->getc_offset = 0;

	return 1;
}

/********************************************************************
Purpose:               The function de-allocates (frees) the memory
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:          None
Algorithm:			   The function de-allocates (frees) the memory
					   occupied by the character buffer and the Buffer
					   structure (buffer descriptor).
**********************************************************************/
void b_free(Buffer * const pBD)
{
	if (!pBD)
		return;

	free(pBD->cb_head);
	free(pBD);


}
/********************************************************************
Purpose:               The function  returns 1 if the character buffer
					   is full.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   b_capacity();
Parameters:            Buffer * const pBD
Return value:          RT_FAIL1, 1, 0
Algorithm:			   The function returns 1 if the character buffer is full.
					   It returns 0 otherwise. If a run-time error is
					   possible, the function should return–1.
**********************************************************************/
int b_isfull(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL1;

	if (pBD->addc_offset == b_capacity(pBD))
		return 1;

	else
		return 0;


}
/********************************************************************
Purpose:               The function returns the current limit of the
					   character buffer.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:          RT_FAIL1, pBD->addc_offset
Algorithm:			   The current limit is the amount of space measured
					   in chars that is currently being used by all
					   added (stored) characters. If a run-time error
					   is possible, the function should return–1.
**********************************************************************/
short b_limit(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL1;

	return pBD->addc_offset;

}
/********************************************************************
Purpose:               The funnction returns the current capacity of
					   the character buffer.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:          RT_FAIL1, pBD->capacity
Algorithm:			   The funnction returns the current capacity of
					   the character buffer. If a run -time error is
					   possible, the function should return–1.
**********************************************************************/
short b_capacity(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL1;

	return pBD->capacity;
}
/********************************************************************
Purpose:               The function sets markc_offset to mark.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD, short mark
Return value:          RT_FAIL1, pBD->markc_offset
Algorithm:			   The parameter mark must be within the current
					   limit of the buffer (0 to addc_offset inclusive).
					   The function returns the currently set markc_offset.
					   If a run-time error is possible, the function should
					   return-1.
**********************************************************************/
short b_mark(pBuffer const pBD, short mark)
{
	if (!pBD)
		return RT_FAIL1;

	if (pBD->markc_offset >= 0 && pBD->markc_offset <= pBD->addc_offset)
	{
		pBD->markc_offset = mark;
		return pBD->markc_offset;
	}
	else
		return RT_FAIL1;


}
/********************************************************************
Purpose:               The function returns the value of mode
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:          RT_FAIL2, pBD->mode
Algorithm:			   The function returns the value of mode to the
					   calling function. If a run-time error is possible,
					   the function should notify the calling function
					   about the failure.
**********************************************************************/
int b_mode(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL2;

	return pBD->mode;

}
/********************************************************************
Purpose:               The function returns the non-negative value
					   of inc_factor
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:          (unsigned char)pBD->inc_factor, 256
Algorithm:			   The function returns the non-negative value of
					   inc_factor to the calling function. If a run-time
					   error is possible, the function should return 0x100.
**********************************************************************/
size_t  b_incfactor(Buffer * const pBD)
{
	if (!pBD)
		return 256;

	return (unsigned char)pBD->inc_factor;
}
/********************************************************************
Purpose:               The function loads (reads) an open input file
					   specified by fi
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   b_eob(); b_addc();
Parameters:            FILE * const fi, Buffer * const pBD
Return value:          RT_FAIL1, -2, numchar
Algorithm:			   The function loads (reads) an open input file
					   specified by fi into a buffer specified by pBD.
					   The function must use the standard function fgetc(fi)
					   to read one character at a time and the function
					   b_addc() to add the character to the buffer.
**********************************************************************/
int b_load(FILE * const fi, Buffer * const pBD)
{
	int numchar = 0;
	char charbuffer;

	if (!pBD || !fi)
		return RT_FAIL1;

	while (!feof(fi))
	{
		charbuffer = (char)fgetc(fi);

		if (feof(fi))/*Checks if its EOF*/
			break;

		else if (b_addc(pBD, charbuffer) == NULL) /*Adds the characters into the buffer, if NULL returns -2*/
			return -2;

		else if (b_eob(pBD) == 1)/*Checks for the end of buffer flag*/
			break;

	}
	++numchar;
	return numchar;
}
/********************************************************************
Purpose:               If the addc_offset is 0, the function returns 1
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:          RT_FAIL1, 0, 1
Algorithm:			   If the addc_offset is 0, the function returns 1.
					   Otherwise it returns 0. If a run -time error is
					   possible, it should return –1.
**********************************************************************/
int b_isempty(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL1;

	if (pBD->addc_offset == 0)
		return 1;

	else
		return 0;


}
/********************************************************************
Purpose:               The function returns eob.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:	       RT_FAIL1, pBD->eob
Algorithm:			   The function returns eob to the calling function.
					   If a run-time error is possible, it should return –1
**********************************************************************/
int b_eob(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL1;

	return pBD->eob;


}
/********************************************************************
Purpose:               The function check if getc_offset and addc_offset are equal.
					   and returns the character at getc_offset.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:		   RT_FAIL1, RT_FAIL2, getc
Algorithm:			   Checks the argument for validity (possible run -time error).
					   If it is not valid, it returns-2.
					   If getc_offset and addc_offset are equal, it sets eob to 1
					   and returns -1. Otherwise it sets eob to 0.
					   Returns the character located at getc_offset.
					   Before returning it increments getc_offset by 1.
**********************************************************************/
char b_getc(Buffer * const pBD)
{
	char getc;

	if (!pBD)
		return RT_FAIL2;

	else if (pBD->getc_offset == pBD->addc_offset)/*checks if its full*/
	{
		pBD->eob = 1;
		return RT_FAIL1;
	}
	else
	{
		pBD->eob = 0;
		getc = pBD->cb_head[pBD->getc_offset];
		++pBD->getc_offset;
		return getc;
	}

}
/********************************************************************
Purpose:               Using the printf() library function the function prints
					   character by character the contents of the character buffer.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   b_isempty(); b_eob();
Parameters:            Buffer * const pBD
Return value:		   RT_FAIL1, numchar
Algorithm:			   Before printing the content the function checks if
					   the buffer is empty, and if it is, it prints the
					   following message Empty buffer. Adding a new
					   line at the end and returns. Next, the function
					   prints the content calling b_getc() in a loop and using
					   b_eob() to detect the end of the buffer content.
					   Finally, it prints a new line character.
					   It returns the number of characters printed.
					   The function returns –1 on failure.
**********************************************************************/
int b_print(Buffer * const pBD)
{
	short getc = 0;
	char charbuffer;
	int numchar = 0;

	if (!pBD)
		return RT_FAIL1;

	else if (b_isempty(pBD) == 1) /*Checks if the buffer is empty*/
		printf("Empty buffer.\n");

	else
	{
		getc = pBD->getc_offset;
		pBD->getc_offset = 0;
		charbuffer = b_getc(pBD); /*gets the first charater before adding*/

		while (b_eob(pBD) == 0)
		{

			printf("%c", charbuffer);/*prints each character*/
			++numchar;
			charbuffer = b_getc(pBD);
		}

		printf("\n");


	}
	pBD->getc_offset = getc; /*sets getc_offset to its orignal poistion*/
	return numchar;

}
/********************************************************************
Purpose:               For all operational modes of the buffer the
					   function shrinks (or in some cases may expand)
					   the buffer to a new capacity.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD, char symbol
Return value:		   NULL, tempbuffer
Algorithm:			   The new capacity is the current limit plus a space
					   for one more character. In other words the new capacity
					   is addc_offset + 1 converted to bytes. The function uses
					   realloc() to adjust the new capacity, and then updates
					   all the necessary members of the buffer descriptor
					   structure. Before returning a pointer to Buffer,
					   the function adds the symbol to the end of the character
					   buffer (do not use b_addc(), use addc_offset to add
					   the symbol) and increments addc_offset. The function
					   must return NULL if for some reason it cannot to perform
					   the required operation. It must set the buffer r_flag
					   appropriately.
**********************************************************************/
Buffer * b_compact(Buffer * const pBD, char symbol)
{
	Buffer *tempBuffer = pBD; /*to hold the new memory allocation*/
	short newcapacity; /*holds the value of addc_offset + 1*/

	if (!pBD)
		return NULL;

	newcapacity = (unsigned)(pBD->addc_offset + 1) * sizeof(char); /*Sets new capacity to addc_offset +1*/


	tempBuffer->cb_head = (char *)realloc(pBD->cb_head, newcapacity); /*reallocates memory for cb_head*/

	if (tempBuffer->cb_head == NULL)
		return NULL;
	else
		pBD->cb_head = tempBuffer->cb_head;

	if (tempBuffer->cb_head != pBD->cb_head)/*checks if the memory location are the same and sets r_flag if they are not*/
		pBD->r_flag = SET_R_FLAG;


	tempBuffer->capacity = (short)newcapacity; /*sets capacity to newcapacity*/

	tempBuffer->cb_head[pBD->addc_offset] = symbol;/*Adds the symbol*/

	++tempBuffer->addc_offset;

	return tempBuffer;


}
/********************************************************************
Purpose:               The function returns r_flag.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:		   RT_FAIL1, pBD->r_flag
Algorithm:			   The function returns r_flag to the calling function.
					   If a run-time error is possible, it should return –1.
**********************************************************************/
/*The function returns r_flag to the calling function. If a
run-time error is possible, it should return –1.*/
char b_rflag(Buffer * const pBD)
{
	if (!pBD) 
		return RT_FAIL1;

	return pBD->r_flag;

}
/********************************************************************
Purpose:               The function decrements getc_offset by 1
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:		   RT_FAIL1, pBD->getc_offset
Algorithm:			   The function decrements getc_offset by 1. If a
					   run-time error is possible, it should return –1;
					   otherwise it returns getc_offset.
**********************************************************************/
short b_retract(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL1;

	--pBD->getc_offset;

	return pBD->getc_offset;
}
/********************************************************************
Purpose:               The function sets getc_offset to the value of the current
					   markc_offset.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:		   RT_FAIL1, pBD->getc_offset
Algorithm:			   The function sets getc_offset to the value of the current
					   markc_offset. If a run-time error is possible, it should
					   return –1; otherwise it returns getc_offset.
**********************************************************************/
short b_reset(Buffer * const pBD)
{
	if (!pBD || pBD->markc_offset < 0 || pBD->markc_offset>pBD->capacity) 
		return RT_FAIL1;
	
	pBD->getc_offset = pBD->markc_offset;

	return pBD->getc_offset;

}
/********************************************************************
Purpose:               The function returns getc_offset
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:		   RT_FAIL1, pBD->getc_offset
Algorithm:			   The function returns getc_offset to the calling function.
					   If a run-time error is possible, it should return –1.
**********************************************************************/
short b_getcoffset(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL1;

	return pBD->getc_offset;

}
/********************************************************************
Purpose:			   The function set the getc_offset and markc_offset
					   to 0.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD
Return value:		   RT_FAIL1, 0
Algorithm:			   The function set the getc_offset and markc_offset
					   to 0, so that the buffer can be reread again. If a
					   run-time error is possible, it should return –1;
					   otherwise it returns 0;
**********************************************************************/
int b_rewind(Buffer * const pBD)
{
	if (!pBD)
		return RT_FAIL1;

	pBD->getc_offset = 0;
	pBD->markc_offset = 0;

	return 0;
}
/********************************************************************
Purpose:               The function returns a pointer to a location of
					   the character buffer.
Version:               1.0
Author:				   Ahmad Kaafi
Called functions:	   None
Parameters:            Buffer * const pBD, short loc_offset
Return value:		   NULL, pBD->cb_head + loc_offset
Algorithm:			   The function returns a pointer to a location of
					   the character buffer indicated by loc_offset. loc_offset
					   is the distance (measured in chars) from the beginning
					   of the character array (cb_head). If a run-time error
					   is possible, it should return NULL.
**********************************************************************/
char * b_location(Buffer * const pBD, short loc_offset)
{
	if (!pBD || loc_offset < 0 || loc_offset >= pBD->addc_offset) {
		return NULL;
	}

	return pBD->cb_head + loc_offset;


}