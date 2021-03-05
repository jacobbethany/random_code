/*
 Created on 2021-02-19 by Jacob Bethany
 Purpose: To explain a method of implementing a Caesar Cipher.
*/
 #include "stdio.h"
 #include "stdlib.h"
 #include "stdint.h"
 #include "string.h"

 //This method is intended to work only with alphabetical characters (case-insensitive).
 //Any out of bounds characters may result in undefined behavior.
 //Also, any shifting that takes place that would cause the current character code to become zero
 //will cause the decryption to fail, since it will look for the first zero to stop.
 void alpha_caesar_encrypt ( char *lpsz_string, int8_t ui8_shift_delta, uint8_t b_decrypt )
{{
 char *p = (char *) lpsz_string;
 while ( *p ) {
   uint8_t b_lowercase = *p >= 'a' && *p <= 'z';
   *p += b_decrypt ? -ui8_shift_delta : ui8_shift_delta;

   if ( b_lowercase ) {
     if ( *p < 'a' ) {
       *p = 'z' - ('a' - *p); //However far below the first character we are is how far from the last character we should be.
     }
     else if ( *p > 'z' ) {
       *p = 'a' + ( *p - 'z' ); //However far above the last character we are is how far from the first character we should be.
     }
   }
   else if ( *p >= 'A' && *p <= 'Z' ) {
     if ( *p < 'A' ) {
       *p = 'Z' - ('A' - *p); //However far below the first character we are is how far from the last character we should be.
     }
     else if ( *p > 'Z' ) {
       *p = 'A' + ( *p - 'Z' ); //However far above the last character we are is how far from the first character we should be.
     }
   }
   //Otherwise, for spaces and other symbols, don't take any corrective action as the behavior should be undefined.

   p ++;
 }
}}

//This subroutine is an improvement on the above because it is binary safe.
//When called to encrypt, the ui32_original_length argument will be filled.
//When called to decrypt, the ui32_original_length argument will be used.
 uint8_t caesar_encrypt ( char *lpsz_string, int8_t ui8_shift_delta, uint8_t b_decrypt, uint32_t *lpui32_original_length )
{{
 if ( ! lpui32_original_length ) {
   printf ( "Error: An invalid reference to a uin32_t to hold the original length of a string to encrypt was passed to the caesar_encrypt subroutine.\n" );
   return 0;
 }

 char *p = (char *) lpsz_string;
 while ( b_decrypt ? p - lpsz_string < *lpui32_original_length : *p ) {
   int16_t i16_value = *p;
   i16_value += b_decrypt ? -ui8_shift_delta : ui8_shift_delta;
   if ( i16_value > 127 ) { //(2^(8-1))-1 = (2^7)-1 = 128-1 = 127
     i16_value -= 256;
   }
   else if ( i16_value < -128 ) {
     i16_value += 256;
   }
   else if ( ! i16_value ) {
     printf ( "Warning: One of the values in the string will be shifted to 0 which will make it unable to be decrypted unless the original length is stored.\n" );
   }
   *p = (char) i16_value & 0xff; //Take only the first byte of the word (which should be safely rotated in bounds).

   p ++;
 }
 if ( ! b_decrypt ) {
   *lpui32_original_length = p - lpsz_string; //We stopped on the zero.
 }

 //Return success.
 return 1;
}}

/*
 This subroutine takes a reference to a char[256] array intended to map
 starting values with ending values, given the specified shift delta.
 This is a much faster way to encrypt a buffer, since it performs a simple lookup,
 rather than doing a series of mathematical operations and conditionals for wrapping
 values left to right and right to left as necessary. It also avoids the need to cast
 from i8_t (char) to i16_t for over/underflows.
*/
 uint8_t generate_caesar_table ( char *lpc_shift_map, int8_t ui8_shift_delta, uint8_t b_decrypt )
{{
 if ( ! lpc_shift_map ) {
   printf ( "Error: An invalid reference to a forward shift map was passed to the generate_caesar_table subroutine.\n" );
   return 0;
 }

 //Setup the shift array such that lpc_shift_map [ i ] = i+ui8_shift_delta within [-128 -> 127], inclusive.
 //Note that 127+any non-zero shift will overflow, which will be pushed back in bounds by wrapping around.
 //I.E. 127+1 = -128.
 //This should mean that we deduct 256 during overflows.
 //Likewise, if -128 -1 = 127, we should add 256 during underflows.
 char *p = lpc_shift_map;
 int8_t i8_i = 0;

 //Note: This loop starts with i8_i at 0 and overflows from 127 to -128, and continues until it reaches -1 ((uint8_t) 255).
 while ( 1 ) {

   int16_t i16_value = (int16_t) i8_i;
   i16_value += b_decrypt ? -ui8_shift_delta : ui8_shift_delta;

   //128 - 256 = -128
   if ( i16_value > 127 ) //(2^(8-1))-1 = (2^7)-1 = 128-1 = 127
        i16_value -= 256;
   //-129 + 255 = 127
   else if ( i16_value < -128 )
        i16_value += 256;

   //printf ( "%d: %d+(%d) => %d.\n", p-lpc_shift_map, i8_i, ui8_shift_delta, i16_value );

   *p = (char) i16_value; //Take only the first byte of the word (which should be safely rotated in bounds).
   if ( i8_i == -1 )
        break;
   i8_i ++;
   p ++;

 }

 //Return success.
 return 1;
}}

//This leverages pregenerated shift tables to optimize the en/decryption routines.
 uint8_t caesar_translate ( char *lpsz_buffer, uint32_t ui32_length, char *lpc_shift_table )
{{
 if ( ! lpsz_buffer ) {
   printf ( "Error: An invalid buffer reference was passed to caesar_translate.\n" );
   return 0;
 }
 if ( ! lpc_shift_table ) {
   printf ( "Error: An invalid reference was passed to the shift table to use in caesar_translate.\n" );
   return 0;
 }

 char *p = lpsz_buffer, *e = lpsz_buffer + ui32_length;
 while ( p != e )
         *p ++ = lpc_shift_table [ (uint8_t) *p ]; //We must cast the subscript to uint8_t because we'll be using negative offsets and accessing invalid regions of memory, otherwise.

 //Return success.
 return 1;
}}

//This function was created to handle the display of encrypted text.
//Since character codes < 0x20 aren't all intended for display, this
//replaces all of them with '?' characters. This avoids issues with
//extra characters being places in stdin and beeping, when some special
//characters would have been displayed.
 void dump_buffer ( const char *lpc_buffer, uint32_t ui32_length )
{{
 const char *e = lpc_buffer + ui32_length;
 printf ( "Buffer: \"" );
 for ( const char *p = lpc_buffer; p != e; p ++ ) {
   printf ( "%hc", *p >= 0x20 ? *p : '?' );
 }
 printf ( "\"\n" );
}}

 int main ( int argc, char **argv )
{{
 char sz_string [  ] = "This is only a test...";
 printf ( "String: \"%hs\"\n", sz_string );

 char c_forward_shift_table [ 256 ];
 char c_reverse_shift_table [ 256 ];

 //Note: Generating these tables each iteration of the loop is actually going to be slower than the method below.
 //This shifting optimization is intended for when the same shifting routine/delta is used for many different strings.
 int8_t i8_i = -128;
 uint32_t ui32_length = strlen ( sz_string );
 while ( 1 ) {

   if ( ! generate_caesar_table ( c_forward_shift_table, i8_i, 0 ) ) {
      printf ( "Error: We were unable to generate the forward caesar shift table for a shift delta of %d.\nExiting.\n", i8_i );
      return 0;
   }
   if ( ! generate_caesar_table ( c_reverse_shift_table, i8_i, 1 ) ) {
      printf ( "Error: We were unable to generate the reverse caesar shift table for a shift delta of %d.\nExiting.\n", i8_i );
      return 0;
   }

   //Show the source and destination values stored within the maps.
   //for ( uint16_t i = 0; i < 256; i ++ ) {
   //  printf ( "%d => %d\n", i, c_forward_shift_table [ i ] );
   //  printf ( "%d => %d\n", i, c_reverse_shift_table [ i ] );
   //}

   if ( ! caesar_translate ( sz_string, ui32_length, c_forward_shift_table ) ) {
     printf ( "Note: Forward shifting (encryption) of the string has failed.\n" );
     return 0;
   }
   dump_buffer ( sz_string, ui32_length );

   if ( ! caesar_translate ( sz_string, ui32_length, c_reverse_shift_table ) ) {
     printf ( "Note: Reverse shifting (decryption) of the string has failed.\n" );
     return 0;
   }
   dump_buffer ( sz_string, ui32_length );

   if ( i8_i == 127 ) //-128 )
        break;
   i8_i ++;
 }

 //return 0;

 //This method works just the same as the above method, but it
 //doesn't leverage the lookup tables, instead opting to do the
 //math for each character of the string.
 i8_i = -128;
 while ( 1 ) {
   uint32_t ui32_original_length;
   if ( ! caesar_encrypt ( sz_string, i8_i, 0, &ui32_original_length ) ) {
     printf ( "Note: We've failed to encrypt the string with shift delta %d.\n", i8_i );
   }
   dump_buffer ( sz_string, ui32_original_length );

   if ( ! caesar_encrypt ( sz_string, i8_i, 1, &ui32_original_length ) ) {
     printf ( "Note: We've failed to decrypt the string with shift delta %d.\n", i8_i );
   }
   dump_buffer ( sz_string, ui32_original_length );

  if ( i8_i == 127 )
       break;

  i8_i ++;
 }

 return 0;
}}
