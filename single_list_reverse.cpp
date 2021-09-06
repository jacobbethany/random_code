/*
 Created by Jacob Bethany on 2021-09-06
 Purpose: To try to reverse a singly linked list, efficiently.

 Results:
 By pointing each node's next pointer to its previous, starting on the
 second node until the end of the list, then reassigning the old root's
 next to zero and the last node found's next to the new start node of
 the list, this works in one pass.

 Another method that might have worked would be swapping
 n with count(list)-n, repeatedly until the mid-point.

 Yet another way might be to create a new list using the last
 item of the previous list and deleting from the old list each
 time. This would take N, N-1, N-2, ..., 1 dereferences to complete,
 as well as having the overhead of de/allocation for the old/new
 lists, respectively.
*/
 #include "stdio.h"
 #include "stdlib.h"
 #include "string.h"
 #include "stdint.h"

typedef struct NODE {
 unsigned char uc_id;
 NODE *lp_next;
} NODE;

//Returns a reference to a newly allocated NODE structure on the heap,
//or zero on error.
 NODE *new_node ( unsigned char uc_id )
{{
 NODE *node = (NODE *) calloc ( sizeof ( NODE ), 1 );
 if ( ! node ) {
   return 0;
 }

 node ->uc_id = uc_id;
 node ->lp_next = 0;

 return node;
}}

//Returns a reference to a newly allocated NODE structure on the heap,
//or zero on error.
 NODE *add_node ( NODE **root, unsigned char uc_id )
{{
 //Bad reference to the variable that holds the first node of the list.
 if ( ! root ) {
   return 0;
 }

 //An empty list was passed.
 if ( ! *root ) {
   return ((*root) = new_node ( uc_id ));
 }

 NODE *node = *root;
 while ( node ->lp_next ) {
   node = node ->lp_next;
 }

 return (node ->lp_next = new_node ( uc_id ));
}}

//Frees the entire singly-linked list of NODE structures.
 unsigned char free_nodes ( NODE **root )
{{
 if ( ! root ) {
   return 1;
 }

 NODE *next = *root, *to_free;
 while ( next ) {
   to_free = next;
   next = next ->lp_next;
   free ( to_free );
 }

 //Invalidate the list to prevent any further action thereon.
 *root = 0;

 return 1;
}}

//Shows the entire singly linked list of NODE structures.
 void show_nodes ( NODE *root )
{{
 for ( NODE *node = root; node; node = node ->lp_next ) {
   printf (
     "Here's the number: %u.\n",
     node ->uc_id
   );
 }
}}

//Counts the number of nodes within the singly linked list.
 uint32_t count_nodes ( NODE *root )
{{
 uint32_t ui32_count = 0;
 for ( NODE *node = root; node; node = node ->lp_next ) {
   ui32_count ++;
 }

 return ui32_count;
}}

//This will reverse all of the nodes of the singly linked list in one pass.
 unsigned char reverse_nodes ( NODE **root )
{{
 if ( ! root ) {
   return 0;
 }

 NODE *last = *root, *current = (*root) ->lp_next, *next;
 while ( current ) {

   //Store the next node so that we can iterate to it later.
   next = current ->lp_next;

   //Assign the previous node's next pointer to the previous node.
   current ->lp_next = last;

   //Record the last node so that we can reassign its next pointer.
   last = current;

   //Move to the next node in the list.
   current = next;
 }

 (*root) ->lp_next = 0; //End the list on a null.
 *root = last; //The last node recorded is the new first node.

 return 1;
}}

 int main ( int argc, char **argv )
{{
 //Define an empty singly linked list of NODE structures.
 NODE *node = 0;

 //Add 100 nodes to the singly linked list of NODE structures.
 for ( uint8_t ui8_i = 0; ui8_i < 100; ui8_i ++ ) {
   printf ( "Adding node #%u.\n", ui8_i );

   //If we couldn't add the node, break out, early.
   if ( ! add_node ( &node, ui8_i ) ) {
     fprintf (
       stderr,
       "Error: We couldn't allocate node #%u.\n",
       ui8_i
     );
     free_nodes ( &node );

     return 0;
   }
 }

 //Force the output regarding the node addition to be displayed.
 fflush ( stdout );

 //Show all of the nodes that we've added, in the order
 //that we've added them.
 printf ( "Showing all of the nodes.\n" );
 show_nodes ( node );
 fflush ( stdout );

 //Reverse the singly linked list of NODE structures.
 printf ( "Reversing the nodes of the list.\n" );
 fflush ( stdout );

 reverse_nodes ( &node );

 //Show the singly linked list of NODE structures, again.
 show_nodes ( node );
 fflush ( stdout );

 //Free all of the nodes within the singly linked list.
 printf ( "Freeing all of the nodes.\n" );
 fflush ( stdout );

 free_nodes ( &node );

 //Show that we didn't have a segmentation fault owing to
 //some kind of deallocation error and that we are terminating,
 //successfully.
 printf ( "Terminating the application.\n" );
 fflush ( stdin );

 //Return to the OS without an error.
 return 0;
}}
