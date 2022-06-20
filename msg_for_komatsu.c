#ifdef _WIN32
 #include "windows.h"
 #include "conio.h"
#else
 #include "string.h"
#endif
 #include "stdio.h"
 #include "stdlib.h"
 #include "stdint.h"
 #include "inttypes.h"

 //Will this parameter be read and not written?
 #define _IN_
 //Will this parameter be modified by the function being called?
 #define _OUT_
 //Will this parameter be read and written to?
 #define _IN_AND_OUT_

 #define DEBUG_MODE 0
 #if ( DEBUG_MODE )
   #define debug_printf(X,...) fprintf ( stdout, X, __VA_ARGS__ )
 #else
   #define debug_printf(...)
 #endif

//Decrypt and return the number of characters in the string contained within the array.
 uint32_t decrypt_and_get_string_length_from_dword_array (
   _IN_AND_OUT_ uint32_t *lpui32_dw_array,
   _IN_ uint32_t ui32_length
 )
{{
 uint32_t ui32_mask;
 uint32_t ui32_string_length = 0;
 for ( uint32_t ui32_i = 0; ui32_i < ui32_length; ui32_i ++ ) {
   lpui32_dw_array [ ui32_i ] ^= ((0xff - (ui32_i % 0xff))*0x01010101);

   ui32_mask = 0xff;
   for ( uint8_t ui8_j = 0; ui8_j < 4; ui8_j ++ ) {

     if ( ! (lpui32_dw_array [ ui32_i ] & ui32_mask) ) {
       return ui32_string_length;
     }
     ui32_string_length ++;
     ui32_mask <<= 8; //0xff -> 0xff00 -> 0xff0000 -> 0xff000000
   }
 }

 return ui32_string_length;
}}

//Decrypt, and extract the string contained within the dword array.
//Returns a reference to an array of characters representing a null-terminated string of text.
 char *get_string_from_dword_array (
   _IN_AND_OUT_ uint32_t *lpui32_dw_array,
   _IN_ uint32_t ui32_length
 )
{{
 uint32_t ui32_mask;
 uint32_t ui32_string_length = decrypt_and_get_string_length_from_dword_array (
   lpui32_dw_array,
   ui32_length
 );
 if ( ! ui32_string_length ) {
   fprintf ( stderr, "Error: A string length couldn't be retrieved from the dword array.\n" );
   return 0;
 }

 debug_printf (
   "The string length is %" PRIu32 " characters.\n",
   ui32_string_length
 );

 //Allocate in such a way so as to include enough space for a zero byte at the
 //end of the string, and pre-zero the entire buffer.
 char *lpsz_string = (char *) calloc ( sizeof(char), ui32_string_length + 1 );
 if ( ! lpsz_string ) {
   fprintf (
     stderr,
     "Error: We couldn't convert a dword array into a string because there wasn't enough memory.\n"
   );
   return 0;
 }

 char *p = lpsz_string;
 for ( uint32_t ui32_i = 0; ui32_i < ui32_length; ui32_i ++ ) {

   ui32_mask = 0xff;
   for ( uint8_t ui8_j = 0; ui8_j < 4; ui8_j ++ ) {
     *p = (lpui32_dw_array [ ui32_i ] & ui32_mask) >> (8*ui8_j);
     p ++;
     if ( ui32_string_length <= p - lpsz_string ) {
       return lpsz_string;
     }
     ui32_mask <<= 8; //0xff -> 0xff00 -> 0xff0000 -> 0xff000000
   }
 }

 return lpsz_string;
}}

 int main (
   _IN_ int argc,
   _IN_ char **argv
 )
{{
 uint32_t ui32_static_array [  ] = {
   0x86df90bb, 0x98de8b91, 0x8e969192, 0x929d8bdc,
   0x948fdb8f, 0x929989da, 0x958c9d9c, 0xd899d89d,
   0x9f949283, 0x97959f98, 0x9b9cd599, 0x82869180,
   0xd384969a, 0x81d28693, 0xd1949c9e, 0x9e999f80,
   0x80cfc39b, 0x8f86ce9c, 0x94cd889b, 0x8dcc9983,
   0x8a8e9987, 0x8cca938e, 0x8d879c86, 0x9bc889c8,
   0x86938e92, 0xc6838a84, 0x818b8486, 0x9085808d,
   0xaae9dc86, 0x8ec286c5, 0xc1848a88, 0x8bc08f94,
   0xffa8b0b1, 0xaafeb1ad, 0xfda9bcb5, 0xbdbffc95,
   0xbebffbb5, 0xbfbeb3b9, 0xbcb1aef9, 0xaabdb0ac,
   0xf7a5b8f7, 0xf6a2b9b8, 0xbda6f59c, 0xb0b8a1bb,
   0xbdbcb0f3, 0xa7bcbba6, 0xbfb8f1b4, 0xa6a2b5a4,
   0xa6b8aaa6, 0xb9eea9a0, 0xeda5b9a4, 0xa9a4b8a3,
   0xb9bbebb9, 0xafbab9a5, 0xbfa0bdaa, 0xa4abe8ad,
   0xb3a9a2ae, 0xa8a7e6b5, 0xb7aaeaa1, 0xa9aba7e4,
   0xaaada2b3, 0xc8ecb1a7, 0xa0e188cb, 0xa5b2b0b0,
   0xcbded6dc, 0xd6ca9edb, 0xd4c99dd8, 0xc89cd9d1,
   0x9bcfdad3, 0x9dcfd5c3, 0xcd99dccf, 0xd6ddd3d9,
   0x97d8c397, 0xc0d8d9d5, 0xd0c6c7d0, 0xc0ddc394,
   0xd6de93db, 0xc7dac692, 0xd0d791c2, 0xd1909cc2,
   0xe68fcbc1, 0xdd8ec389, 0xc1c1c4d9, 0xd9cdcf8c,
   0xdec4c2df, 0x8ad3c6d9, 0xc0ddd9c6, 0xdcdbc1c5,
   0xc687c4ce, 0xd2d3c9c4, 0xc0cdd185, 0xcbd6d484,
   0xc0c6d3d0, 0xcd82d1d6, 0xced681c7, 0xcec9cbd2,
   0xf0f9bff8, 0xf1d5beec, 0xeee9fcf0, 0x9696b2e9,
   0xf8f5f2c8, 0xf6ffe8ff, 0xd393b5e0, 0xfaf7fbf9,
   0xe3f2d5b7, 0xeff8f7fe, 0x95959595
 };

 char *lpsz_string = get_string_from_dword_array (
   ui32_static_array,
   sizeof ( ui32_static_array )
 );
 if ( ! lpsz_string ) {
   fprintf ( stderr, "Error: We couldn't get a string from a static DWORD array.\n" );
   return 0;
 }
 fprintf (
   stdout,
   "%hs\n",
   lpsz_string
 );

 free ( lpsz_string );

 return 0;
}}
