/*
 Created on 2021-09-09 by Jacob Bethany
 Purpose: To calculate the Nth power of some number, quickly.

 [X] Possible optimization:
 Note that 2^8 = 2^4 * 2^4.
 With that noted, break the desired power into smaller pieces
 and multiply the pieces.

 EG:
 2^1024 ->
   2^2 * 2^2 = 2^4 ->
     2^4 * 2^4 ->
       2^8 * 2^8 ->
         2^16 * 2^16 ->
           ... ->
             2^512 * 2^512 ->
               = 2^1024
 This will happen in log2(1024) steps = 10 steps.

1.00000
2147483647
*/

 #include "stdio.h"
 #include "stdlib.h"
 #include "string.h"
 #include "stdint.h"

/*
 base value
 current value
 desired power
 current power
 Example usage: custom_power_recurse ( 2, 2, 10, 0 );
 (This will raise 2 to the 10th power.)
 NOTE: This handles only postive powers.
 Use custom_power to have this be called, implicitly.
*/
 double custom_power_recurse ( double b, double x, int n, int c )
{{
 printf (
   "{ b : %f, x : %f, n : %ld, c : %ld }\n",
   b, x, n, c
 );

 if ( ! n ) {
   printf ( "Raising to the power of 0 is always 1.\n" );
   return 1;
 }
 if ( c == n ) {
   printf ( "Current power is equal to desired power.\n" );
   return x;
 }

 if ( ! c ) {
   c = 1; //will be 2 when we do <<1 in the passed parameter to the recusive function call.
 }

 if ( c > n ) {
   printf ( "Whoops, we've overshot the desired power!\n" );
   return x;
 }

 if ( (n - c) % 2 ) {
   printf (
     "Desired power is uneven, so we need to handle a single power before doubling.\n"
   );
   c ++;
   x *= b;
   return custom_power_recurse ( b, x, n, c );
 }

 //If we'd overshoot the desired power, just do a loop of single multiplications.
 if ( (c << 1) > n ) {
   for ( ; c < n; c ++ ) {
     x *= b;
   }
   return x;
 }

 printf ( "We're doubling the value.\n" );
 x *= x;
 return custom_power_recurse ( b, x, n, c<<1 );
}}

 double custom_power ( double x, int n )
{{
 //1 to the power of anything is just 1.
 if ( x == 1 ) {
   return 1;
 }

 double dbl_val = custom_power_recurse (
   x, //base value.
   x, //current value.
   n >= 0 ? n : -n, //only pass positive values for the desired power.
   0 //current built-up powers.
 );

 return n >= 0 ? dbl_val : 1 / dbl_val;
}}

 int main ( int argc, char **argv )
{{
 double dbl_bases [  ] = { 2, 2.1, 2, 1 };
 int i_powers [  ] = { 10, 3, -2, 2147483647 };
 //double dbl_val = custom_power_recurse ( 2, 2, 10, 0 );

 for ( int i = 0; i < sizeof(i_powers)/sizeof(int); i ++ ) {
   double dbl_val = custom_power ( dbl_bases [ i ], i_powers [ i ] );
   printf (
     "%f^%ld = \"%f\"\n",
     dbl_bases [ i ],
     i_powers [ i ],
     dbl_val
   );
 }
 return 0;
}}
