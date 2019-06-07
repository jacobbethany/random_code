/*
 Code written on 2019_06_07 by Jacob Bethany.
 This is an equivalent to the subset_sum_finder3.php script that I wrote earlier today,
 but written in c. I usee the cpp extension for compilation reasons, on Linux.
 This takes a char * array of the same length as the uint32_t array of values and uses
 it as a mask. 1 = true, 0 = false.
 I've created the same functions to increment the mask array, like with binary addition.
*/
 #include "stdio.h"
 #include "stdlib.h"
 #include "string.h"
 #include "stdint.h"

 char *create_array_mask ( uint32_t array_length )
{{
 char *mask = (char*) calloc ( 1, array_length );

 return mask;
}}

/*
 This performs a faux binary addition upon the mask array to emulate a bitmask of variadic size.
 This could have been optimized by having this work like a real bit mask, just across any number
 of bytes, but this was more simplistic than converting a bit number to a bit and bit number and
 doing the necessary array access and bit manipulations. I may do this later, though, for fun.
*/
 void increment_array_mask ( char *lp_mask, uint32_t ul_length )
{{
 char *p = lp_mask + ul_length - 1;
 while ( p >= lp_mask )
        {
         if ( *p )
             {
              *p = ! (*p);
              p --;
              continue;
             }

         *p = ! (*p);
         break;
        }
}}

/*
 This function can check to see if the mask is completely full of 1s or 0s, depending upon what you pass for b_true.
*/
 unsigned char mask_is_all ( char *lp_mask, uint32_t ul_length, char b_true )
{{
 uint32_t i;
 for ( i = 0; i < ul_length; i ++ )
      {
       if ( ! (lp_mask [ i ] == b_true ) )
            return 0;
      }

 //If we didn't find an instance of a byte in the array that didn't match the request true or false status, then they all matched..
 return 1;
}}

/*
 This will show the current mask array as though it were a bitmask (a binary representation of the array).
*/
 void show_array_mask ( char *lp_mask, uint32_t ul_length )
{{
 for ( uint32_t i = 0; i < ul_length; i ++ )
       printf ( "%d", lp_mask [ i ] );
 printf ( "\n" );

/*
 //This is in reverse.
 for ( char *p = lp_mask + ul_length - 1; p >= lp_mask; p -- )
       printf ( "%d", *p );
 printf ( "\n" );
*/
}}

/*
 This will add all values of the source array that have true counter parts (1s) in the mask array (at the same index).
*/
 uint32_t combine_masked_array_subset ( uint32_t *lp_array, char *lp_mask, uint32_t ul_length )
{{
 uint32_t ul_total = 0, i;
 for ( i = 0; i < ul_length; i ++ )
      {
       if ( lp_mask [ i ] )
            ul_total += lp_array [ i ];
      }

 return ul_total;
}}

/*
 This will show the pieces of the array that were/would be combined, given hte current mask array.
*/
 void show_masked_array_subset ( uint32_t *lp_array, char *lp_mask, uint32_t ul_length )
{{
 uint32_t i;
 unsigned char b_first = 1;
 for ( i = 0; i < ul_length; i ++ )
      {
       if ( lp_mask [ i ] )
           {
            if ( b_first )
                 b_first = 0;
            else
                 printf ( ", " );

            printf ( "%d", lp_array [ i ] );
           }
      }

 printf ( "\n" );
}}

 int main ( int argc, char *argv[] )
{{
 uint32_t ul_required_value = 12;
 uint32_t ul_array [  ] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
 const uint32_t ul_array_length = sizeof(ul_array)/sizeof(uint32_t);
 //char mask [ ul_array_length ] = { 0 };

 char *lp_mask = create_array_mask ( ul_array_length );
 if ( ! lp_mask )
     {
      printf ( "Error: We couldn't allocate memory for the array mask.\n" );
      return 0;
     }

 //Keep looping until we wrap back around to all zeros (overflow).
 do
   {
    increment_array_mask ( lp_mask, ul_array_length );

    //If the combined masked region of the array totals to the required value, show the subset of the array by the mask that was used for the subset combination.
    if ( combine_masked_array_subset ( ul_array, lp_mask, ul_array_length ) == ul_required_value )
         show_masked_array_subset ( ul_array, lp_mask, ul_array_length );

    //show_array_mask ( lp_mask, ul_array_length );
   } while ( ! mask_is_all ( lp_mask, ul_array_length, 0 ) );

 free ( lp_mask );
 return 0;
}}
