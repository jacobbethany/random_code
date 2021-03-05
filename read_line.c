/*
 Created on 2021-02-21 by Jacob Bethany
 Purpose: To demonstrate how to read dynamically sized input from stdin without worrying about a buffer overflow.
 Note that this has custom implementations of strstr, strchr, strcmp, strlen, and strcat to avoid dependency upon
 string.h or any other string library.

 stdlib.h is required for heap de/allocation with calloc/malloc/realloc/free.
*/
 #include "stdio.h"
 #include "stdlib.h"

//Usage:
//if ( ! string_compare ( sz_string1, sz_string2 ) )
//is the same as
//if ( strstr ( sz_string1, sz_string2 ) == sz_string1 )
 char string_compare ( const char *lpsz_haystack, const char *lpsz_needle )
{{
 const char *p = lpsz_haystack, *e = lpsz_needle;

 //Continue looking while the current character of each string matches.
 while ( *e ++ == *p ++ );

 //If the haystack ends before the needle, then we didn't find the whole string.
 if ( *e )
      return 1;

 //The method below would force both strings to be the same length and match exactly.
 //Continue checking while the strings characters match.
 //while ( *e ++ == *p ++ );

 //If one string ended and the other didn't, fail.
 //if ( *e || *p )
 //     return 0;

 return 0;
}}

//This is the same as strstr.
//Returns a reference to the position in memory of the first instance
//of the string, sz_string, starting at lpsz_haystack bounded by 0.
//If the string, sz_needle, isn't found by the end of the string in
//the block of memory referenced by lpsz_haystack, a reference to zero
//is returned.
 const char *find_string ( const char *lpsz_haystack, const char *sz_needle )
{{
 for ( const char *p = lpsz_haystack; *p; p ++ ) {
   if ( ! string_compare ( p, sz_needle ) )
        return p;
 }
 return 0;
}}

//This is the same as strchr.
//Returns a reference to the position in memory of the first instance
//of the character, c_needle, if it is found before a zero is reached.
//If it isn't found before a zero is reached, a reference to zero is returned.
 const char *find_character ( const char *lpsz_haystack, char c_needle )
{{
 const char *p;
 for ( p = lpsz_haystack; *p && *p != c_needle; p ++ );
 return *p ? p : 0;
}}

//This is the same as strcat.
//This will copy lpsz_string_to_concatenate to lpsz_destination_string,
//starting on lpsz_destination_string's 0 terminator and will continue
//copying until lpsz_to_concatenate's 0 terminator is reached.
 void string_concat ( char *lpsz_destination_string, const char *lpsz_to_concatenate )
{{
 char *d = lpsz_destination_string;
 const char *s = lpsz_to_concatenate;

 //Go to the zero terminator of the destination string.
 while ( *d ) d ++;

 //Keep copying while the character being copied isn't zero.
 while ( *d ++ = *s ++ );
}}

//This is the same as memmove.
//This will copy the ul_byte_count bytes of the contents of lp_source to lp_destination
//in a way that is safe, when the contents of the source and destination blocks overlap.
 void memory_move ( char *lp_destination, char *lp_source, unsigned long ul_byte_count )
{{
 char *d = lp_destination, *s = lp_source, *e = lp_source + ul_byte_count;

 //Don't do anything, if the source and destination references are the same.
 if ( s == d )
      return ;

 //If the source reference is after the destination reference, copy incrementing both references.
 if ( s > d ) {
   while ( s != e )
           *d ++ = *s ++;
   return ;
 }

 //Otherwise, we need to copy from right to left.
 d += ul_byte_count;

 //Continue copying until the start of the destination buffer is reached.
 while ( d >= lp_destination )
         *d -- = *e --;
}}

//This is the same as strlen.
//This will start at the first character of the buffer, and iterate
//forward until a zero terminator is reached.
//The offset from the zero terminator to the start of the string is returned.
 unsigned long string_length ( const char *lpsz_string )
{{
 const char *p = lpsz_string;
 while ( *p )
         p ++;

 return p - lpsz_string;
}}

/*
 This will strip all whitespace from the start and end of the string.
 Returns 0 on failure (bad reference); and 1 on success.
*/
 char trim_string ( char *sz_string )
{{
 //If an invalid reference to the string to trim was passed, fail rather than causing a segmentation fault.
 if ( ! sz_string ) {
   printf ( "Error: A bad string reference was passed to trim_string.\n" );
   return 0;
 }

 char *p = sz_string;
 while ( find_character ( "\r\n\t ", *p ) ) p ++;

 //Move everything after the whitespace to the start of the whitespace.
 memory_move ( sz_string, p, string_length ( p ) );

 //Go to the last character of the string.
 while ( *p ) p ++;
 p --;

 //Look backwards until we find something that isn't whitespace.
 while ( find_character ( "\r\n\t ", *p ) ) {

   *p = 0; //Zero this whitespace at the end of the string.
   p --; //Continue looking backwards.

   //Don't underflow the string while looking backwards from the end.
   if ( p < sz_string )
        break;
 }

 return 1;
}}

/*
 This will concatenate a string with another string, allocating or reallocating
 the destination string to be able to fit the new string.
 Returns 1 on success; returns 0 on error.
*/
 char concatenate_string ( char **lplpsz_string, char *lpsz_to_concatenate )
{{
 //If a bad reference to a string to concatenate was passed, exit with success,
 //since concatenating nothing is the same as doing nothing, anyway.
 if ( ! lpsz_to_concatenate )
      return 1;

 //If the reference to the variable pointing to the string to be concatenated is invalid,
 //exit with failure status rather than crashing to a memory access violation.
 if ( ! lplpsz_string ) {
   printf ( "Error: A bad reference to a pointer variable to hold the string was passed.\n" );
   return 0;
 }

 //If the string hasn't been allocated, yet.
 if ( ! *lplpsz_string ) {

   //Determine the size required to hold the string and the null terminator.
   unsigned long ul_required_bytes = string_length ( lpsz_to_concatenate ) + 1;
   *lplpsz_string = calloc ( ul_required_bytes, 1 );

   //If we were unable to allocate a buffer large enough to hold the string to concatenate,
   //exit with an error code to inform the caller not to use the returned buffer.
   if ( ! *lplpsz_string ) {
     printf ( "Error: strdup failed.\n" );
     return 0;
   }

   //Copy the string to concatenate to the new block of memory.
   string_concat ( *lplpsz_string, lpsz_to_concatenate );

   //Return success.
   return 1;
 }

 //Determine the size needed to hold the old and new strings with a null terminator at the end.
 unsigned long ul_required_bytes = string_length ( *lplpsz_string ) + string_length ( lpsz_to_concatenate ) + 1;

 //Reallocate a buffer the size of both strings plus a null terminator at the end.
 void *lpv_new_block = realloc ( *lplpsz_string, ul_required_bytes );

 //If we've failed to reallocate the block to hold both strings, exit with failure status.
 if ( ! lpv_new_block ) {
   printf ( "Error: We insufficient memory to reallocate a block large enough to fit the old string and string to concatenate.\n" );
   return 0;
 }

 //Since realloc guarantees that the contents of the old block
 //will exist in the new block, we can simply concatenate the
 //new string to it.
 string_concat ( lpv_new_block, lpsz_to_concatenate );

 //Make the passed reference to a string reference point to the start of the new block
 //(in case the current one wasn't expanded, but a new block was allocated and the contents
 //of the old block were copied, instead).
 *lplpsz_string = lpv_new_block;

 //Return with success status.
 return 1;
}}

/*
 This will read a single line of text from stdin of any length and strip any whitespace from the beginning and end.
 Returns a pointer to a buffer containing all of the variably sized text from stdin on success;
 returns a reference to 0 on failure.
*/
 char *read_line ( void )
{{
 //Note: This is static, so it isn't thread safe, but it'd be weird for multiple threads to be tying up
 //stdin simultaneously on the same machine.
 static char sz_string [ 256 ] = { 0 }; //Define a buffer of 256 bytes to hold the stdin input.
 char *lpsz_final_string = 0; //Initialize this to zero so that the first concatenation allocates the buffer.
 char b_done; //This doesn't need to be initialized to any value, since the completion state will be determined during each loop iteration.

 do {

   //Grab up to 255 characters from stdin.
   fgets ( sz_string, 255, stdin );

   //If this contains a line break, then this is the end of the input from the user.
   b_done = (find_character ( sz_string, '\n' ) != 0);

   //Trim off any whitespace from the beginning and end of the string.
   trim_string ( sz_string );

   if ( ! concatenate_string ( &lpsz_final_string, sz_string ) ) {
     if ( lpsz_final_string ) {
       free ( lpsz_final_string );
     }
     printf ( "Error: We can't read the line because we've run out of memory.\n" );
     return 0;
   }

 } while ( ! b_done );

 //Return a reference to the allocated block of memory containing the entire contents of the user's input within stdin.
 return lpsz_final_string;
}}

//This test was created to ensure that the trim_string function works as expected.
 void trim_test ( void )
{{
 //Define a string with a bunch of whitespace at the beginning and end to be trimed.
 char sz_string [  ] = " \tThis is only a test... \n \r";

 //Show the string in its original state.
 printf ( "Before: \"%hs\"\n", sz_string );

 //Trim the whitespace from the beginning and end of the string.
 trim_string ( sz_string );

 //Show the new trimmed string.
 printf ( "After: \"%hs\"\n", sz_string );
}}

 char read_line_test ( void )
{{
 //Ask the user for some input.
 printf ( "Please, enter a string followed by return: " );

 //Grab the input from the user.
 char *lpsz_string = read_line (  );

 //Ensure that the input was retrieved successfully, before continuing.
 if ( ! lpsz_string ) {
   printf ( "An error occurred while reading the line, exiting.\n" );
   return 0;
 }

 //Since we didn't fail earlier, we'll show the user what s/he typed.
 printf ( "This is what was typed:\n\"%hs\"\n", lpsz_string );

 //Free the dynamically allocated block on the heap.
 free ( lpsz_string );

 return 1;
}}

/*
//This is the program's entry point.
 int main ( int argc, char **argv )
{{
 read_line_test (  );
 //Return without error.
 return 0;
}}
*/
