/*
 Created on 2022-06-18 by Jacob Bethany
 Purpose: To decrypt and extract a message from an array of dword values
 and display it in stdout.
*/
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
 #include "stdbool.h"

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

//4321 -> 1234 for each dword's bytes.
 bool unscramble_dw_array (
   _IN_AND_OUT_ uint32_t *lpui32_dw_array,
   _IN_ uint32_t ui32_length
 )
{{
 if ( ! lpui32_dw_array || ! ui32_length ) {
   fprintf ( stderr, "Error: An empty or missing array of dwords can't be unscrambled.\n" );
   return 0;
 }

 uint32_t ui32_val;
 for ( uint32_t ui32_i = 0; ui32_i < ui32_length; ui32_i ++ ) {
   ui32_val  = (lpui32_dw_array [ ui32_i ] & 0xff000000) >> 24;
   ui32_val |= (lpui32_dw_array [ ui32_i ] & 0xff0000  ) >> 8;
   ui32_val |= (lpui32_dw_array [ ui32_i ] & 0xff00    ) << 8;
   ui32_val |= (lpui32_dw_array [ ui32_i ] & 0xff      ) << 24;
   lpui32_dw_array [ ui32_i ] = ui32_val;
 }

 return true;
}}

//Decrypt and return the number of characters in the string contained within the array.
 uint32_t decrypt_and_get_string_length_from_dword_array (
   _IN_AND_OUT_ uint32_t *lpui32_dw_array,
   _IN_ uint32_t ui32_length
 )
{{
 uint32_t ui32_mask;
 uint32_t ui32_string_length = 0;

 if ( ! unscramble_dw_array ( lpui32_dw_array, ui32_length ) ) {
   fprintf ( stderr, "Error: We've failed to unscramble the array of dwords.\n" );
   return 0;
 }

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

 debug_printf (
   "Here's the dword array when interpreted as a string via (char *) \"%hs\"\n",
   (char *) lpui32_dw_array //silence any potential compiler warnings about the type mismatch.
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
     if ( p - lpsz_string >= ui32_string_length ) {
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
   0xbb90df86, 0x918bde98, 0x9291968e, 0xdc8b9d92,
   0x8fdb8f94, 0xda899992, 0x9c9d8c95, 0x9dd899d8,
   0x8392949f, 0x989f9597, 0x99d59c9b, 0x80918682,
   0x9a9684d3, 0x9386d281, 0x9e9c94d1, 0x809f999e,
   0x9bc3cf80, 0x9cce868f, 0x9b88cd94, 0x8399cc8d,
   0x87998e8a, 0x8e93ca8c, 0x869c878d, 0xc889c89b,
   0x928e9386, 0x848a83c6, 0x86848b81, 0x8d808590,
   0x86dce9aa, 0xc586c28e, 0x888a84c1, 0x948fc08b,
   0xb1b0a8ff, 0xadb1feaa, 0xb5bca9fd, 0x95fcbfbd,
   0xb5fbbfbe, 0xb9b3bebf, 0xf9aeb1bc, 0xacb0bdaa,
   0xf7b8a5f7, 0xb8b9a2f6, 0x9cf5a6bd, 0xbba1b8b0,
   0xf3b0bcbd, 0xa6bbbca7, 0xb4f1b8bf, 0xa4b5a2a6,
   0xa6aab8a6, 0xa0a9eeb9, 0xa4b9a5ed, 0xa3b8a4a9,
   0xb9ebbbb9, 0xa5b9baaf, 0xaabda0bf, 0xade8aba4,
   0xaea2a9b3, 0xb5e6a7a8, 0xa1eaaab7, 0xe4a7aba9,
   0xb3a2adaa, 0xa7b1ecc8, 0xcb88e1a0, 0xb0b0b2a5,
   0xdcd6decb, 0xdb9ecad6, 0xd89dc9d4, 0xd1d99cc8,
   0xd3dacf9b, 0xc3d5cf9d, 0xcfdc99cd, 0xd9d3ddd6,
   0x97c3d897, 0xd5d9d8c0, 0xd0c7c6d0, 0x94c3ddc0,
   0xdb93ded6, 0x92c6dac7, 0xc291d7d0, 0xc29c90d1,
   0xc1cb8fe6, 0x89c38edd, 0xd9c4c1c1, 0x8ccfcdd9,
   0xdfc2c4de, 0xd9c6d38a, 0xc6d9ddc0, 0xc5c1dbdc,
   0xcec487c6, 0xc4c9d3d2, 0x85d1cdc0, 0x84d4d6cb,
   0xd0d3c6c0, 0xd6d182cd, 0xc781d6ce, 0xd2cbc9ce,
   0xf8bff9f0, 0xecbed5f1, 0xf0fce9ee, 0xe9b29696,
   0xc8f2f5f8, 0xffe8fff6, 0xe0b593d3, 0xf9fbf7fa,
   0xb7d5f2e3, 0xfef7f8ef, 0x95959595
 };

 char *lpsz_string = get_string_from_dword_array (
   ui32_static_array,
   sizeof ( ui32_static_array ) / sizeof ( uint32_t )
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
