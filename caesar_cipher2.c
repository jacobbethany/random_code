/*
 Created by Jacob Bethany
 Purpose: To show a circularly defined doubly-linked list implementation of a Caesar Cipher.

 To compile:
 gcc caesar_cipher2.c read_line.c -o caesar_cipher2
*/
 #include "stdio.h"
 #include "stdlib.h"
 #include "stdint.h"
 #include "read_line.h" //Custom implementation of basic string functions and read_line (safe dynamically sized stdin reads).

 //This will be a doubly-linked and circular list.
 //Each node points to the node before and after it,
 //except for the first and last nodes of the list
 //which instead point to the last and first nodes
 //of the list, respectively.
 typedef struct CHAR_LIST
{
 char c;
 struct CHAR_LIST *prev;
 struct CHAR_LIST *next;
} CHAR_LIST;

 //This will create and return a new CHAR_LIST node with the specified character value,
 //It will point to the node directly before it in the list, as specified by the caller.
 //If no previous node exists, pass a reference to zero.
 CHAR_LIST *new_char_list_node ( CHAR_LIST *prev, char c )
{{
 CHAR_LIST *node = calloc ( sizeof ( CHAR_LIST ), 1 );
 if ( ! node )
      return 0;

 node ->prev = prev;
 node ->c = c;

 return node;
}}

 //Add a new node to the passed doubly-linked circular list.
 CHAR_LIST *add_char_list_node ( CHAR_LIST **root, char c )
{{
 if ( ! root ) {
   printf ( "Error: A bad reference to the root of a CHAR_LIST was passed to add_char_list_node.\n" );
   return 0;
 }

 if ( ! *root ) {
   if ( ! (*root = new_char_list_node ( 0, c )) )
        return 0;

   (*root) ->prev = *root; //The root node's previous must always point to the end of the list, which is currently this node.
   (*root) ->next = *root; //The end of the list must always point to the start of the list, which is currently this node.

   return *root;
 }

 CHAR_LIST *node = *root;
 while ( node ->next != *root )
         node = node ->next;

 //This will already point the new node's previous reference to the current last node
 //and reassign the previous final node's next reference to the new node, but we'll have
 //to handle the new node's next node (the last node must always wrap back to the first).
 if ( ! (node ->next = new_char_list_node ( node, c )) )
      return 0;

 //Always make the list wrap back on itself.
 node ->next ->next = *root;
 (*root) ->prev = node ->next;

 return node;
}}

//This will free a single node of the list, while attaching the previous and next nodes to one another.
 uint8_t free_char_list_node ( CHAR_LIST *node )
{{
 if ( ! node ) {
   printf ( "A bad node reference was passed to free_char_list_node.\n" );
   return 0;
 }

 //Since this list goes in a circle, there will always be a next and previous node, by definition.
 node ->prev ->next = node ->next;
 node ->next ->prev = node ->prev;

 free ( node );
 return 1;
}}

//This will free the entire character list.
 uint8_t free_char_list ( CHAR_LIST **root )
{{
 if ( ! root ) {
   printf ( "Error: A bad reference to the list to free was passed to free_char_list.\n" );
   return 0;
 }

 //If there are no nodes to free in the list.
 if ( ! *root )
      return 1;

 CHAR_LIST *next = *root, *to_free;
 do {
   to_free = next;
   next = next ->next;
   free ( to_free );
 } while ( next != *root );

 *root = 0;
 return 1;
}}

//Show all of the nodes of the list, starting on the current until we make a full circle back to it.
 void show_char_list ( CHAR_LIST *root )
{{
 uint8_t ui8_i = 0;

 CHAR_LIST *node = root;
 do {
   printf ( "%d: %d\n", ui8_i, node ->c );
   ui8_i ++;

   node = node ->next;
 } while ( node != root );

}}

//This will find the CHAR_LIST node associated with the specified character value.
 CHAR_LIST *find_char_list_node ( CHAR_LIST *root, char c )
{{
 CHAR_LIST *node = root;
 do {
   if ( node ->c == c )
        return node;
   node = node ->next;
 } while ( node != root );

 return 0;
}}

//This will move the passed node forwards or backwards through the list i8_shift_delta times.
//Since the list is a doubly-linked and circular list, this will always work, even for shift
//deltas greater than the size of the list.
 uint8_t iterate_char_list_node ( CHAR_LIST **node, int8_t i8_shift_delta )
{{
 if ( ! node ) {
   printf ( "Error: An invalid node reference was passed to iterate_char_list_node.\n" );
   return 0;
 }

 do {

   //printf ( "We're iterating another %d times.\n", i8_shift_delta );

   if ( i8_shift_delta < 0 )
        *node = (*node) ->prev;
   else *node = (*node) ->next;

   i8_shift_delta += i8_shift_delta < 0 ? 1 : -1;

 } while ( i8_shift_delta != 0 );

 return 1;
}}

//This will encrypt lpsz_string by shifting each of its characters' ASCII codes by i8_shift_delta.
//This shifting occurs by finding the corresponding ASCII code's position within a CHAR_LIST and
//iterating forwards or backwards in the circularly defined doubly-linked list i8_shift_delta
//times, then using the resultant node's corresponding character ASCII code for the current position
//in the buffer being encrypted.
//Note that to decrypt, the shift delta should be the opposite of what it was during the encryption.
//Example: 1 => -1, 2 => -2, 15 => -15, ...etc.
 uint8_t caesar_encrypt ( char *lpsz_string, uint32_t ui32_length, int8_t i8_shift_delta, CHAR_LIST *char_list )
{{
 char *p = lpsz_string, *e = lpsz_string + ui32_length;
 if ( ! lpsz_string ) {
   printf ( "Error: Bad string reference passed to caesar_encrypt.\n" );
   return 0;
 }
 if ( ! char_list ) {
   printf ( "Error: Bad CHAR_LIST reference passed to caesar_encrypt.\n" );
   return 0;
 }

 while ( p != e ) {

   CHAR_LIST *node = find_char_list_node ( char_list, *p );
   if ( ! node ) {
     printf ( "Error: There was a malformed char_list that doesn't contain the character, %c (%d).\n", *p, *p );
     return 0;
   }
//printf ( "%d = %d\n", *p, node ->c );
   iterate_char_list_node ( &node, i8_shift_delta );
//printf ( "E: %d => %d\n", *p, node ->c );
   *p = node ->c;

   p ++;
 }

 return 1;
}}

 //This is the sensible way to do things, but not the intent of this project.
 uint8_t parametric_caesar_encrypt ( char *lpsz_string, uint32_t ui32_length, int8_t i8_shift_delta )
{{
 char *p = lpsz_string, *e = lpsz_string + ui32_length;
 int16_t i16_shifted_value;

 if ( ! lpsz_string )
      return 0;

 while ( p != e ) {

   i16_shifted_value = (int16_t) *p + (int16_t) i8_shift_delta;
   //If we underflow from -128 by 1, we arrive at 127 (0111 1111).
   if ( i16_shifted_value < -128 )
        i16_shifted_value += 128;
   //If we overflow from 127 by 1, we arrive at -128 (1000 0000).
   else if ( i16_shifted_value > 127 )
        i16_shifted_value -= 128;

   *p ++ = (char) i16_shifted_value;

 }

 return 1;
}}

//This is the program's entry point; it accepts no arguments from the command line.
 int main ( int argc, char **argv )
{{
 CHAR_LIST *char_list = 0;
 printf ( "Enter some text to encrypt/decrypt with each shift.\n>> " );
 char *sz_string = read_line (  );
 if ( ! sz_string ) {
   printf ( "Error: Failure reading stdin.\n" );
   return 0;
 }

 uint8_t i8_i = -128;
 uint32_t ui32_length = string_length ( sz_string );

 printf ( "Populating the char list.\n" );
 //Add every character value from 0-255 (or -128 to 127) to the CHAR_LIST.
 while ( 1 ) {

   if ( ! add_char_list_node ( &char_list, i8_i ) ) {
     printf ( "Error: We were unable to add a new CHAR_LIST node.\n" );
     if ( ! free_char_list ( &char_list ) ) {
       printf ( "Error: We couldn't free the CHAR_LIST list.\n" );
     }
     break;
   }

   if ( i8_i == 127 )
        break;
   i8_i ++;
 }

/*
 char x = 127;
 printf ( "x: %d\n", x );
 x += 1;
 printf ( "x+1: %d\n", x );
 x = -128;
 printf ( "x: %d\n", x );
 x -= 1;
 printf ( "x-1: %d\n", x );
*/

 //printf ( "Showing the char list.\n" );
 //show_char_list ( char_list );

 //Show the string before we encrypt it.
 printf ( "Original string: \"%hs\"\n", sz_string );

 //Test all shifting cases with mixed/unmixed methods.
 for ( int16_t i16_i = -128; i16_i <= 127; i16_i ++ ) {

   //Show the string, now that it has been encrypted.
   caesar_encrypt ( sz_string, ui32_length, i16_i, char_list );
   //parametric_caesar_encrypt ( sz_string, ui32_length, i16_u ); //We can mix and match the parametric and circular doubly linked-list methods.
   printf ( "Encrypted string: \"%hs\"\n", sz_string );

   //Show the string, now that it has been decrypted.
   caesar_encrypt ( sz_string, ui32_length, -i16_i, char_list );
   //parametric_caesar_encrypt ( sz_string, ui32_length, -i16_i ); //Don't mix and match parametric and CHAR_LIST versions. (See notes in this function's definition.)
   printf ( "Decrypted String: \"%hs\"\n", sz_string );

 }

 //Free dynamically allocated resources.
 printf ( "Freeing the char list.\n" );
 free_char_list ( &char_list );

 free ( sz_string );

 return 0;
}}
