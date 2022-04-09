/*
 Created on 2022-04-08 by Jacob Bethany
 Purpose: To create a generic map in C that is quick,
 rather than based upon some kind of linear lookup.
 This implementation will group similar keys (using
 the first two bytes thereof, or the first byte and
 zero, if only one byte is present in the key string).
 To allow for iterating by insertion order, a vector
 can be exploited within the main structure of the
 pseudo object.
*/
 //Type, Map, Index
 //Example usage: MAP_DATA_AT ( SOME_STRUCTURE, some_map, 0 ) .some_structure_member
 #define MAP_DATA_AT(T,M,I) ( *((T*) M .lpary_doubly_linked_list [ I ] ->lpv_data) )
 #define MAP_KEY_AT(M,I) ( M .lpary_doubly_linked_list [ I ] ->lpsz_key )

 //Usage: SET_MAP_NODE ( map, "some_key", x );
 //Usage: SET_MAP_NODE_NO_ALLOC ( map, "some_key", lp_dynamically_allocated_structure_that_can_be_freed_implicitly );
 #define SET_MAP_NODE(M,K,V) set_map_node__internal ( &M, K, (void *) &V, 0 )
 #define SET_MAP_NODE_NO_ALLOC(M,K,V) set_map_node__internal ( M, K, V, 1 )

 #include "stdio.h"
 #include "stdlib.h"
 #include "stdint.h"
 #include "inttypes.h"
#ifndef _WIN32
 #include "string.h"
#else
 #include "windows.h"
 #include "conio.h"
#endif

 typedef struct DOUBLY_LINKED_LIST {
   char *lpsz_key;
   void *lpv_data;

   struct DOUBLY_LINKED_LIST *lp_next;
   struct DOUBLY_LINKED_LIST *lp_prev;
 } DOUBLY_LINKED_LIST;

 typedef struct TRANCHE {
   DOUBLY_LINKED_LIST *lp_first_node; //first in sorted order (based upon user-defined sorting function).
   DOUBLY_LINKED_LIST *lp_last_node;  //last in sorted order (based upon user-defined sorting function).
   uint32_t ui32_count; //How many nodes are currently in the list?
 } TRANCHE;

 typedef struct MAP {
   //This will be an array of references to the actual nodes within the tranches, in case we want to iterate through the list in the order in which the nodes were inserted.
   DOUBLY_LINKED_LIST **lpary_doubly_linked_list; //DOUBLY_LINKED_LIST*[]. The index is the node starting from lp_first_node and following lp_next until N nodes have been accessed.
   TRANCHE tranches [ 0xFFFF /*65535 or (2^16)-1*/ ]; //[(1<<16)-1] different tranches.
   int8_t (*lpfn_sort_comparator_function)(void *lpv_node1, void *lpv_node2); //-1 = node1 should be left; 0 either is fine; 1 = node1 should be on the right.
   uint8_t (*lpfn_free)(void *lpv_node1); //user-defined free function, in case the data stored in the tranch[n]->lpv_data memory block has dynamicly allocated memory, as well.
   void *(*lpfn_initialize)(void); //user-defined function to return an initialized block of memory of the same arbitrary structure type being used in this map.
   uint32_t ui32_count; //Count of all nodes in all tranches.
   uint32_t ui32_data_size; //Store the size of the data in a user-defined block of a DOUBLY_LINKED_LIST struct, here.

 } MAP;

 //This will allocate memory large enough to hold a new DOUBLY_LINKED_LIST
 //structure node, and fill it with the passed data. The prev and next members
 //can be set to be zero, if needed. They exist only so as to make latent
 //insertion function creation easier.
 DOUBLY_LINKED_LIST *new_doubly_linked_list_node (
   const char *lpsz_key,
   void *lpv_data,
   uint32_t ui32_data_size,
   DOUBLY_LINKED_LIST *lp_prev,
   DOUBLY_LINKED_LIST *lp_next,
   uint8_t b_assign_data_instead_of_allocating_and_copying //don't allocate a new block, just reference the one specified.
 )
{{
 DOUBLY_LINKED_LIST *node = (DOUBLY_LINKED_LIST *) calloc ( sizeof (DOUBLY_LINKED_LIST), 1 );
 if ( ! node ) {
   return 0;
 }

 //If we couldn't allocate memory for the key to store within the structure.
#ifdef _WIN32
 if ( ! (node ->lpsz_key = _strdup ( lpsz_key )) ) {
#else
 if ( ! (node ->lpsz_key = strdup ( lpsz_key )) ) {
#endif
   //Free the memory that we successfully allocated for the structure itself, earlier.
   free ( node );
   return 0;
 }

 if ( b_assign_data_instead_of_allocating_and_copying ) {
   node ->lpv_data = lpv_data;
 }
 else {
   //If we couldn't allocate enough space to hold the contents of this new
   //user data within the structure, free what we've allocated so far and fail.
   if ( ! (node ->lpv_data = calloc(ui32_data_size, 1) ) ) {
     free ( node ->lpsz_key );
     free ( node );
     return 0;
   }

   memcpy (
     node ->lpv_data,
     lpv_data,
     ui32_data_size
   );
 }

 node ->lp_next = lp_next;
 node ->lp_prev = lp_prev;

 return node;
}}

 uint8_t initialize_map (
   MAP *lpm_map,
   uint32_t ui32_data_size,
   void*(*lpfn_initialize)(void),
   int8_t (*lpfn_sort_comparator_function)(void*,void*),
   uint8_t (*lpfn_free)(void*)
 )
{{
 if ( ! lpm_map ) {
   return 0;
 }

 memset (
   lpm_map,
   0,
   sizeof ( MAP )
 );

 lpm_map ->ui32_data_size = ui32_data_size;
 lpm_map ->lpfn_initialize = lpfn_initialize;
 lpm_map ->lpfn_sort_comparator_function = lpfn_sort_comparator_function;
 lpm_map ->lpfn_free = lpfn_free;

 return 1;
}}

 //This will return 0, if no node with the specified key exists, yet.
 //I'm tempted to make a get_map_node version that inserts a new node,
 //if nothing is found. This would invoke some user-defined initialization
 //function that returns a new structure. (MAP would require a new member,
 //void (*lpfn_init)(void);)
 void *find_map_node__internal (
   MAP *lpm_map,
   const char *lpsz_key,
   uint8_t b_return_node_data_instead_of_node
 )
{{
 fprintf (
   stdout,
   "find_map_node__internal: Checking to see whether the referenced map is valid.\n"
 );

 //If the reference to the map is invalid.
 if ( ! lpm_map ) {
   return 0;
 }
 fprintf (
   stdout,
   "find_map_node__internal: Checking to see whether the referenced key is valid.\n"
 );

 //If the key string is an invalid reference.
 if ( ! lpsz_key ) {
   return 0;
 }
 fprintf (
   stdout,
   "find_map_node__internal: Checking to see whether the referenced key is empty.\n"
 );

 //If the key string doesn't have at least one character.
 if ( ! *lpsz_key ) {
   return 0;
 }
 fprintf (
   stdout,
   "find_map_node__internal: Resolving the tranche index.\n"
 );


 //Get the tranche index.
 unsigned short us_tranche_identifier = *((unsigned short *) lpsz_key);
/*
 memcpy ( //us_tranche_idenifier = *((unsigned short *) lpsz_key);
   &us_tranche_identifier,
   lpsz_key,
   2
 );
*/

 fprintf (
   stdout,
   "Looking for \"%s\" in tranche #%" PRIu32 "\n",
   lpsz_key,
   us_tranche_identifier
 );

 //We'll start at the beginning of the tranche list and move forward.
 //Some basic hashing algorithm or tree structure of alphabetically sorted
 //keys within the tranche's linked list could speed things up a bit more,
 //but we'll search linearly, from the first node, for now.
 DOUBLY_LINKED_LIST *lp_doubly_linked_list_node = lpm_map ->tranches [ us_tranche_identifier ] .lp_first_node;
 while ( lp_doubly_linked_list_node ) {

   fprintf (
     stdout,
     "find_map_node__internal: \"%s\" ?= \"%s\"\n",
     lp_doubly_linked_list_node ->lpsz_key,
     lpsz_key
   );

   //If the key of this node matches what we're looking for.
   if ( ! strcmp ( lp_doubly_linked_list_node ->lpsz_key, lpsz_key ) ) {
     if ( b_return_node_data_instead_of_node ) {
       return lp_doubly_linked_list_node ->lpv_data;
     }

     return lp_doubly_linked_list_node;
   }
   else {
     fprintf ( stdout, "find_map_node__internal: They were NOT EQUAL.\n" );
   }

   lp_doubly_linked_list_node = lp_doubly_linked_list_node ->lp_next;
   fprintf (
     stdout,
     "find_map_node__internal: We've followed the next reference to %p.\n",
     lp_doubly_linked_list_node
   );
 }

 //We couldn't find the node.
 return 0;
}}

 void *find_map_node (
   MAP *lpm_map,
   const char *lpsz_key
 )
{{
 return find_map_node__internal (
   lpm_map,
   lpsz_key,
   0 //0 = node ->lpv_data; 1 = node
 );
}}

 void *find_map_node_data (
   MAP *lpm_map,
   const char *lpsz_key
 )
{{
 return find_map_node__internal (
   lpm_map,
   lpsz_key,
   1 //0 = node ->lpv_data; 1 = node
 );
}}

/*
 We could do a slight modification to this to allow prepending.
 1.) The reference list would need to be memmoved up the size of
 one (DOUBLY_LINKED_LIST*).
 2.) The new reference would need to be placed at the first reference
 of that list of refernces. (lpm_map ->lpary_doubly_linked_list)

 Note: The usage of the user-defined functions could allow for
 sorted nodes within the tranches and allow for quicker lookup.
 I'd use a hash of the value to insert/find vs the hashes of the
 first and last nodes in the automatically sorted tranche and
 start from whichever end is closest.
 I could further subdivde this by assuming that some ratio from
 the middle is correct (assuming equal distribution of key values
 which is undoubtedly incorrect in real-world cases) and have another
 array of references specific to each tranche. That is a bit overkill,
 however. An unordered linked list of nodes within each of the tranches
 is fine, since one can iterate from 0 ->n with the main reference array
 without issue.
*/
 uint8_t set_map_node__internal (
   MAP *lpm_map,
   const char *lpsz_key,
   void *lpv_data,
   uint8_t b_assign_data_instead_of_allocating_and_copying //don't allocate a new block, just reference the one specified.
 )
{{
 //If the map reference passed is invalid.
 if ( ! lpm_map ) {
   return 0;
 }
 //If an invalid reference was passed for the string, fail.
 if ( ! lpsz_key ) {
   return 0;
 }
 //If an empty string was passed for the key, fail.
 if ( ! *lpsz_key ) {
   return 0;
 }
 //If we have no way of sorting the list, just fail.
 if ( ! lpm_map ->lpfn_sort_comparator_function ) {
   return 0;
 }
 //If no free function was specified, then fail.
 if ( ! lpm_map ->lpfn_free ) {
   return 0;
 }

 //If the key already exists, just update it.
 void *lpv_preexisting_doubly_linked_list_node_data;
 if ( lpv_preexisting_doubly_linked_list_node_data = find_map_node_data ( lpm_map, lpsz_key ) ) {

   fprintf (
     stdout,
     "set_map_node__internal: Freeing the user-defined data with the user-supplied callback function.\n"
   );

   //Call the user-defined free function on the user-data stored within the pre-existing
   //structure in the node that we've found.
   lpm_map ->lpfn_free ( lpv_preexisting_doubly_linked_list_node_data );

   fprintf (
     stdout,
     "set_map_node__internal: Overwriting %" PRIu32 " bytes of user-data with the newly supplied" \
     " user-data @ %p for this key @ %p.\n",
     lpm_map ->ui32_data_size,
     lpv_data,
     lpv_preexisting_doubly_linked_list_node_data
   );

   //We can safely assume that the user-data is of the same size and copy it in.
   memcpy (
     lpv_preexisting_doubly_linked_list_node_data,
     lpv_data,
     lpm_map ->ui32_data_size
   );

   //We've successfully updated the node.
   return 1;
 }

 fprintf (
   stdout,
   "set_map_node__internal: Getting the tranche identifier.\n"
 );

 //Store the first two characters of the key in a variable
 //to use as a tranche identifier. If the key contains only
 //a single character, that's fine because there will be a
 //terminating byte equal to zero at the end.
 unsigned short us_tranche_identifier = 0;
 memcpy ( &us_tranche_identifier, lpsz_key, 2 );

/*
 DOUBLY_LINKED_LIST *lp_search_doubly_linked_list_node = lpm_map ->tranches [ us_tranche_identifier ] .lp_first_node;
 while ( lp_search_doubly_linked_list_node ->lp_next ) {
   lp_search_doubly_linked_list_node = lp_search_doubly_linked_list_node ->lp_next;
 }
*/

 DOUBLY_LINKED_LIST *lp_search_doubly_linked_list_node = lpm_map ->tranches [ us_tranche_identifier ] .lp_last_node;

 fprintf (
   stdout,
   "set_map_node__internal: Creating a new doubly-linked-list node in tranche #%" PRIu32 ".\n",
   us_tranche_identifier
 );
 DOUBLY_LINKED_LIST *lp_new_doubly_linked_list_node = new_doubly_linked_list_node (
   lpsz_key,
   lpv_data,
   lpm_map ->ui32_data_size,
   lp_search_doubly_linked_list_node, //This is the previous node.
   0, //This is the last node in the list, so there's nothing after.
   b_assign_data_instead_of_allocating_and_copying
 );

 //If we've failed to allocate memory for a new node, fail.
 if ( ! lp_new_doubly_linked_list_node ) {
   return 0;
 }

 fprintf (
   stdout,
   "set_map_node__internal: Attaching the new node to the end of the list.\n"
 );

 //If there is at least one node in the list, already.
 if ( lp_search_doubly_linked_list_node ) {
   fprintf (
     stdout,
     "set_map_node__internal: Attatching this to the end of an existing tranche.\n"
   );

   lp_search_doubly_linked_list_node ->lp_next = lp_new_doubly_linked_list_node;

   //Update the list-end pointer to be this newly added node.
   lpm_map ->tranches [ us_tranche_identifier ] .lp_last_node = lp_new_doubly_linked_list_node;
 }
 //Otherwise, this is the only node in the list,
 //so it's both the first and last node that we need to reference in the tranche.
 else {
   fprintf (
     stdout,
     "set_map_node__internal: Assigning this new node as the first and last node of a previously empty tranche.\n"
   );
   lpm_map ->tranches [ us_tranche_identifier ] .lp_first_node = lp_new_doubly_linked_list_node;
   lpm_map ->tranches [ us_tranche_identifier ] .lp_last_node = lp_new_doubly_linked_list_node;
 }

 //Update the count within the tranche.
 lpm_map ->tranches [ us_tranche_identifier ] .ui32_count ++;

 fprintf (
   stdout,
   "Creating or expanding the reference array buffer.\n"
 );

 void *lpv_new_node_reference_array;

 //If there's no reference array allocated, yet, make a new one.
 if ( ! lpm_map ->lpary_doubly_linked_list ) {
   fprintf (
     stdout,
     "Creating a new reference array.\n"
   );
   lpv_new_node_reference_array = malloc (
     (++ lpm_map ->ui32_count) * sizeof ( DOUBLY_LINKED_LIST **)
   );

 }
 //Otherwise, simply expand the existing one to hold the new node.
 else {

   fprintf (
     stdout,
     "Expanding an existing reference array.\n"
   );

   //Update the overall map count and expand the node reference array.
   lpv_new_node_reference_array = realloc (
     (void *) lpm_map ->lpary_doubly_linked_list,
     (++ lpm_map ->ui32_count) * sizeof ( DOUBLY_LINKED_LIST **)
   );

   fprintf (
     stdout,
     "The reference array has been expanded via realloc.\n"
   );

 }

 //If we couldn't create or extend the reference array to add the new
 //node reference to it, fail.
 if ( ! lpv_new_node_reference_array ) {
   return 0;
 }

/*
 //Copy all of the existing node references to the new array that has one
 //more empty slot. (Not needed for realloc -- just malloc/free method.)
 memcpy (
   lpv_new_node_reference_array,
   lpm_map ->lpary_doubly_linked_list,
   sizeof(DOUBLY_LINKED_LIST**) * (lpm_map ->ui32_count - 1)
 );
*/

 fprintf (
   stdout,
   "set_map_node__internal: Storing the newly re/allocated node reference array address in the map.\n"
 );

 lpm_map ->lpary_doubly_linked_list = (DOUBLY_LINKED_LIST **) lpv_new_node_reference_array;

 fprintf (
   stdout,
   "set_map_node__internal: Storing the new reference at the end of the reference array (zero-based offset #%" PRIu32 ").\n",
   lpm_map ->ui32_count - 1
 );

 lpm_map ->lpary_doubly_linked_list [
   lpm_map ->ui32_count - 1
 ] = lp_new_doubly_linked_list_node;

 return 1;
}}

//This will insert a new default node and return it,
//if the node with the specified key doesn't exist.
//Note: The user should either make his/her default
//initialization function passed to initialize_map
//return a default global node on allocation error,
//or be ready to handle allocation failure via null
//reference return from map ->lpfn_initialize => get_map_node()
//Returns a reference to DOUBLY_LINKED_LIST ->lpv_data if the node exists;
//returns lpm_map ->lpfn_initialize (  ) if it doesn't.
//Note: Returns 0, if lpm_map references 0.
//Note: Returns 0, if lpm_map ->lpfn_initialize() returns 0.
 void *get_map_node ( MAP *lpm_map, const char *lpsz_key )
{{
 //If the map doesn't exist to search for the node.
 if ( ! lpm_map ) {
   return 0;
 }

 //If try to get the user-data from an existing node.
 void *lpv_data = find_map_node_data ( lpm_map, lpsz_key );

 //If we found a node's user-data, then return it.
 if ( lpv_data ) {
   return lpv_data;
 }

 //If we need to initialize a default node and can't, we should fail.
 if ( ! lpm_map ->lpfn_initialize ) {
   return 0;
 }

 lpv_data = lpm_map ->lpfn_initialize (  );
 if ( ! lpv_data ) {
   return 0;
 }

 return set_map_node__internal (
   lpm_map,
   lpsz_key,
   lpv_data,
   1 //don't allocate memory, just assign lpv_data to the same reference.
 ) ? lpv_data : 0;
}}

 uint8_t free_map ( MAP *lpm_map )
{{
 //If the reference to the map is invalid.
 if ( ! lpm_map ) {
   return 0;
 }

 //If there is no memory allocated for a list of references, then the list is empty.
 if ( ! lpm_map ->lpary_doubly_linked_list ) {
   memset ( lpm_map, 0, sizeof ( MAP ) );
   return 0;
 }

 DOUBLY_LINKED_LIST *to_free = 0, *next = *lpm_map ->lpary_doubly_linked_list;
 while ( next ) {
   to_free = next;
   next = next ->lp_next;

   //Call the user-defined free function for any dynamically allocated contents.
   lpm_map ->lpfn_free ( to_free );

   //Free the DOUBLY_LINKED_LIST node itself.
   free ( to_free );
 }

 //Just in case we want to use this map variable again, we'll clear it.
 memset ( lpm_map, 0, sizeof ( MAP ) );

 return 1;
}}

//Free a single DOUBLY_LINKED_LIST node within the map by its key,
//if it exists.
//If the map, key, or user-defined free function are invalid or the
//key doesn't exist, zero is returned.
//Otherwise, one is returned to denote success.
 uint8_t free_map_node ( MAP *lpm_map, const char *lpsz_key )
{{
 //Fail if there is a bad reference to the map containing the key.
 if ( ! lpm_map ) {
   return 0;
 }
 //Fail if there's an invalid reference to the key string.
 if ( ! lpsz_key ) {
   return 0;
 }
 //Fail if the key string is less than one character in length.
 if ( ! *lpsz_key ) {
   return 0;
 }

 //If we can't free the internal data, fail.
 if ( ! lpm_map ->lpfn_free ) {
   return 0;
 }

 //This will return the doubly-linked-list node rather than the ->lpv_data within it.
 DOUBLY_LINKED_LIST *lp_doubly_linked_list_node = (DOUBLY_LINKED_LIST *) find_map_node ( lpm_map, lpsz_key );
 if ( ! lp_doubly_linked_list_node ) {
   return 0;
 }

 fprintf (
   stdout,
   "Found the node, \"%s\" @ %p.\n",
   lpsz_key,
   lp_doubly_linked_list_node
 );

 //Get the tranche to which this key belongs.
 unsigned short us_tranche_identifier = *((unsigned short *) lpsz_key);

 //Decrement the count of nodes stored within this tranche.
 lpm_map ->tranches [ us_tranche_identifier ] .ui32_count --;

 //Keep the nodes around the node we're deleting, if they exist.
 if ( lp_doubly_linked_list_node ->lp_prev ) {
   fprintf ( stdout, "Reassigning the previous node to point to the node after the one being freed.\n" );
   lp_doubly_linked_list_node ->lp_prev ->lp_next = lp_doubly_linked_list_node ->lp_next;
 }
 //This was the first node of the list.
 //We need to find the tranche to which it belongs and update it to start on the next node.
 else {
   fprintf ( stdout, "Reassigning the initial node to point to the node after the one being freed.\n" );

   lpm_map ->tranches [ us_tranche_identifier ] .lp_first_node =
     lp_doubly_linked_list_node ->lp_next;
 }

 if ( lp_doubly_linked_list_node ->lp_next ) {
   fprintf ( stdout, "Reassigning the next node to point to the node before the one being freed.\n" );
   lp_doubly_linked_list_node ->lp_next ->lp_prev = lp_doubly_linked_list_node ->lp_prev;
 }
 //Otherwise, this is the last node in the tranche and we need to update the tranche to
 //say that the last node is the node before this one, instead.
 else {
   fprintf ( stdout, "Reassigning the final node to point to the node before the one being freed.\n" );
   lpm_map ->tranches [ us_tranche_identifier ] .lp_last_node = lp_doubly_linked_list_node ->lp_prev;
 }

 fprintf (
   stdout,
   "Calling the user-defined free function.\n"
 );
 lpm_map ->lpfn_free ( lp_doubly_linked_list_node ->lpv_data );

 fprintf (
   stdout,
   "Calling free on the DOUBLY_LINKED_LIST node.\n"
 );
 free ( lp_doubly_linked_list_node );

 //Find the reference to this DOUBLY_LINKED_LIST node in the reference array of the main map structure.
 for ( uint32_t ui32_i = 0 ; ui32_i < lpm_map ->ui32_count; ui32_i ++ ) {

   fprintf (
     stdout,
     "%p ?= %p\n",
     lp_doubly_linked_list_node,
     lpm_map ->lpary_doubly_linked_list [ ui32_i ]
   );

   //If this a reference to the node that we've just deleted, remove it.
   if ( lp_doubly_linked_list_node == lpm_map ->lpary_doubly_linked_list [ ui32_i ] ) {

     //Move all of the items after this in the array of references down towards one to fill
     //in the deleted node's slot.
     memmove (
       &lpm_map ->lpary_doubly_linked_list [ ui32_i ],
       &lpm_map ->lpary_doubly_linked_list [ ui32_i + 1 ],
       (lpm_map ->ui32_count - ui32_i)*sizeof(DOUBLY_LINKED_LIST**)
     );

     //Reference null at the end of the list for when free_map is called, at some point.
     lpm_map ->lpary_doubly_linked_list [ lpm_map ->ui32_count - 1 ] = (DOUBLY_LINKED_LIST *) 0;
     lpm_map ->ui32_count --; //Update the count for the entire map.

     break;
   }
 }

 return 1;
}}


///user-defined callbacks.
 uint8_t free_map_user_data ( void *lpv_data )
{{
 return 1; //always return successfully, since there's nothing to free.
}}

 int8_t compare_map_user_data ( void *lpv_data1, void *lpv_data2 )
{{
 return 0; //All things can be considered equal, for now.
}}

 void *initialize_user_data ( void )
{{
 return calloc ( sizeof(int8_t), 1 );
}}

 int main ( int argc, char **argv )
{{
 MAP map;
 fprintf (
   stdout,
   "main: Initializing the map.\n"
 );

 initialize_map (
   &map, //The map that we're working with.
   sizeof ( int8_t ), //This is the type of data with which we're working.
   initialize_user_data, //What to return when get_map_node is called and the node doesn't exist.
   compare_map_user_data, //reserved and unused.
   free_map_user_data //called implicitly when free_map is invoked.
 );

 fprintf (
   stdout,
   "main: Preparing some user data.\n"
 );

 int8_t i8_x = 7;

 fprintf (
   stdout,
   "main: Adding the user data to the map.\n"
 );

 SET_MAP_NODE (
   map,
   "some_key",
   i8_x
 );

 fprintf (
   stdout,
   "main: Adding another key to the map.\n"
 );

 i8_x = 29;
 SET_MAP_NODE (
   map,
   "jack",
   i8_x
 );

 fprintf (
   stdout,
   "main: Looking for a key that won't exist, yet.\n"
 );

 int8_t *lpi8_x = (int8_t *) find_map_node_data (
   &map,
   "jacob"
 );

 fprintf (
   stdout,
   "main: We've made it out of find_map_node.\n"
 );

 if ( lpi8_x ) {
   fprintf (
     stdout,
     "main: &map [ \"some_key\" ] = %p; its value is: %" PRIi8 "\n",
     lpi8_x,
     *lpi8_x
   );
 }
 else {
   fprintf (
     stderr,
     "main: Error: We were unable to find the node.\n"
   );
 }

 fprintf (
   stdout,
   "main: Adding a third key to the map.\n"
 );


 //Should should be in the same tranche as "jack"
 SET_MAP_NODE (
   map,
   "jacob",
   i8_x
 );


 //TODO: Find and display this node, again.


 fprintf (
   stdout,
   "main: Updating the third key.\n"
 );

 //Update the value to be 32.
 i8_x = 32;
 set_map_node__internal ( //using it directly for an example...
   &map,
   "jacob",
   (void *) &i8_x,
   0 //We DO want to allocate a buffer and copy the data from &i8_x into it.
 );

 fprintf (
   stdout,
   "Trying to access a key that doesn't exist with get_map_node to create a default one.\n"
 );
 int8_t *lpi8_default_node = (int8_t *) get_map_node (
   &map,
   "doesn't exist"
 );
 fprintf (
   stdout,
   "\"doesn't exist\" => %" PRIi8 " @ %p\n",
   lpi8_default_node ? *lpi8_default_node : 0,
   lpi8_default_node
 );


 fprintf (
   stdout,
   "Attempting to free a key from the map.\n"
 );
 //Freeing nodes in any position is working fine.
 free_map_node (
   &map,
   "jack" //"doesn't exist" //"jacob" //"jack" //"some_key"
 );

 fprintf (
   stdout,
   "main: Showing the user data.\n"
 );

 for ( uint32_t ui32_i = 0; ui32_i < map .ui32_count; ui32_i ++ ) {
   fprintf (
     stdout,
     "main: \"%s\" => \"%" PRIi8 "\"\n",
//     *((int8_t*) map .lpary_doubly_linked_list [ ui32_i ] ->lpv_data)
     MAP_KEY_AT ( map, ui32_i ),
     MAP_DATA_AT ( int8_t, map, ui32_i )
   );
 }

 fprintf (
   stdout,
   "main: Freeing the map.\n"
 );

 free_map ( &map );

 fprintf (
   stdout,
   "main: Terminating.\n"
 );

 return 0;
}}
