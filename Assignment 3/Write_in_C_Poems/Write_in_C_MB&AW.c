Michael Busby and Andrew Watson, F13
/*********************************************************************************************

// A Simple Repeating XOR
// Writen in C
// By Michael Busby
// My poems are better in C
// For S^R to have fun with
// December 6, 2013.



// With Python snakes and coffee beans,
// And I'm afraid of XML.
// A platypus whispers softly:
// "Write in C"

// The internet is moving fast,
// the GUI's are all around me.
// 0x61 0x38 0x1f 0x4 0x15 0xb 0x44 0x49 0x2 0x49 0x2d 0x47
// "Write in C"

*********************************************************************************************/

#include <stdio.h>

#define LEN 13	 /* The date of our Friday Exam */
void main(void){

	 char key[] = "Command line\0";	/* The Key! */
	 char word[] = {0x61, 0x38, 0x1f, 0x4, 0x15, 0xb, 0x44, 0x49, 0x2, 0x49, 0x2d, 0x47, 0x0}; 	/* What is this? */
	 char result[LEN];	/* a place to store the answer */
	 int i;				/* to help me count */

	 for (i^=i; i<LEN; ++i)	 				/* first we XOR this i */
		  result[i] = word[i] ^ key[i];		/* next we XOR this word */
	 result[LEN-1] = '\0';					/* then we XOR the world!!! */

	 
	printf("With Python snakes and coffee beans,\n");
	printf("And I'm afraid of XML.\n");
	printf("A platypus whispers softly:\n");
	printf("'Write in C'\n");
	printf("\n");
	
	printf("The internet is moving fast,\n");
	printf("the GUI's are all around me.\n");
	printf("%s\n", word);
	printf("'Write in C'\n");
	printf("\n");
	 
	 /* what did the Platypus say? */
	 printf("%s, %s\n%s, oh, %s\n%s is the Key.\n%s\n\n", result, result, result, result, key, result);
}
