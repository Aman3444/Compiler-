!!This program contains many lexical errors
!!It tests mainly your transition table
!! Use MY output to adjust
!! YOUR error state transitions in YOUR transition table 
!!You should create your own test file with more errors 
!>Wrong comment
PLATYPUs {
i=000;  !! legal zero decimal literal
n=001;  !! illegal decimal literal
j=0x;   !! illegal hexadecimal literal
k=2I;  !! possible error
k=0O; !! digit 0 followed by a letter O
a% = 0.0L; !! possible error
b. = 0.L   !! possible error
USING> = .0 !! illegal floating point literal
Read(ElSe)
If=0.l  !! letter l not digit 1
o=0x1Ga  !! possible error
b10 = 1O1.0; !! letter O follows 1
3C=7.0O2;    !! digit in VID and letter O precedes 2 
name$S = Last$ << " S.#" !! wrong string VID
WHILE FALSE(b10 > 0x00 OR. <= b10.AND a != o.o)LOOP{
  a=a+1..;
  c=001.1; !! leading zero not allowed in floating point
}
WRITE$("No  Luck  Today);
WRITE (""My Scanner has gone astray");
}
"There is always one more bug