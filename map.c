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

 #include "stdio.h"
 #include "stdlib.h"
 #include "stdint.h"
 #include "inttypes.h"
 #include "string.h"

 typedef struct DOUBLY_LINKED_LIST {
   char *lpsz_key;
   void *lpv_data;
   uint32_t ui32_data_size;

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
   uint32_t ui32_count; //Count of all nodes in all tranches.
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
   DOUBLY_LINKED_LIST *lp_next
 )
{{
 DOUBLY_LINKED_LIST *node = (DOUBLY_LINKED_LIST *) calloc ( sizeof (DOUBLY_LINKED_LIST), 1 );
 if ( ! node ) {
   return 0;
 }

 //If we couldn't allocate memory for the key to store within the structure.
 if ( ! (node ->lpsz_key = strdup ( lpsz_key )) ) {
   //Free the memory that we successfully allocated for the structure itself, earlier.
   free ( node );
   return 0;
 }

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

 node ->lp_next = lp_next;
 node ->lp_prev = lp_prev;

 return node;
}}

 uint8_t initialize_map (
   MAP *lpm_map,
   int8_t (*lpfn_sort_comparator_function)(void*,void*),
   uint8_t (*lpfn_free)(void*)
 )
{{
 if ( ! lpm_map ) {
   return 0;
 }

 lpm_map ->ui32_count = 0;
 memset (
   &lpm_map ->tranches,
   0,
   sizeof ( TRANCHE ) * 0xffff
 );
 lpm_map ->lpfn_sort_comparator_function = lpfn_sort_comparator_function;
 lpm_map ->lpfn_free = lpfn_free;

 return 1;
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
 uint8_t add_map_node (
   MAP *lpm_map,
   const char *lpsz_key,
   void *lpv_data,
   uint32_t ui32_data_size
 )
{{
 fprintf (
   stdout,
   "Entered the function, \"add_map_node\"\n"
 );

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

 fprintf (
   stdout,
   "Getting the tranche identifier.\n"
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
   "Creating a new doubly-linked-list node in tranche #%" PRIu32 ".\n",
   us_tranche_identifier
 );
 DOUBLY_LINKED_LIST *lp_new_doubly_linked_list_node = new_doubly_linked_list_node (
   lpsz_key,
   lpv_data,
   ui32_data_size,
   lp_search_doubly_linked_list_node, //This is the previous node.
   0 //This is the last node in the list, so there's nothing after.
 );

 //If we've failed to allocate memory for a new node, fail.
 if ( ! lp_new_doubly_linked_list_node ) {
   return 0;
 }

 fprintf (
   stdout,
   "Attaching the new node to the end of the list.\n"
 );

 //If there is at least one node in the list, already.
 if ( lp_search_doubly_linked_list_node ) {
   fprintf (
     stdout,
     "Attatching this to the end of an existing tranche.\n"
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
     "Assigning this new node as the first and last node of a previously empty tranche.\n"
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

   lpv_new_node_reference_array = malloc (
     (++ lpm_map ->ui32_count) * sizeof ( DOUBLY_LINKED_LIST **)
   );

 }
 //Otherwise, simply expand the existing one to hold the new node.
 else {

   //Update the overall map count and expand the node reference array.
   lpv_new_node_reference_array = realloc (
     lpm_map ->lpary_doubly_linked_list,
     (++ lpm_map ->ui32_count) * sizeof ( DOUBLY_LINKED_LIST **)
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
   "Storing the newly re/allocated node reference array address in the map.\n"
 );

 lpm_map ->lpary_doubly_linked_list = (DOUBLY_LINKED_LIST **) lpv_new_node_reference_array;

 fprintf (
   stdout,
   "Storing the new reference at the end of the reference array (zero-based offset #%" PRIu32 ").\n",
   lpm_map ->ui32_count - 1
 );

 lpm_map ->lpary_doubly_linked_list [
   lpm_map ->ui32_count - 1
 ] = lp_new_doubly_linked_list_node;

 return 1;
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

 void *find_map_node ( MAP *lpm_map, const char *lpsz_key )
{{
 fprintf (
   stdout,
   "find_map_node: Checking to see whether the referenced map is valid.\n"
 );

 //If the reference to the map is invalid.
 if ( ! lpm_map ) {
   return 0;
 }
 fprintf (
   stdout,
   "find_map_node: Checking to see whether the referenced key is valid.\n"
 );

 //If the key string is an invalid reference.
 if ( ! lpsz_key ) {
   return 0;
 }
 fprintf (
   stdout,
   "find_map_node: Checking to see whether the referenced key is empty.\n"
 );

 //If the key string doesn't have at least one character.
 if ( ! *lpsz_key ) {
   return 0;
 }
 fprintf (
   stdout,
   "find_map_node: Resolving the tranche index.\n"
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
   "Looking for \"%s\" in tranche #" PRIu32 "\n",
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
     "\"%s\" ?= \"%s\"\n",
     lp_doubly_linked_list_node ->lpsz_key,
     lpsz_key
   );

   //If the key of this node matches what we're looking for.
   if ( ! strcmp ( lp_doubly_linked_list_node ->lpsz_key, lpsz_key ) ) {
     return lp_doubly_linked_list_node ->lpv_data;
   }

   lp_doubly_linked_list_node = lp_doubly_linked_list_node ->lp_next;
 }

 //We couldn't find the node.
 return 0;
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

 int main ( int argc, char **argv )
{{
 MAP map;
 fprintf (
   stdout,
   "Initializing the map.\n"
 );

 initialize_map (
   &map,
   compare_map_user_data,
   free_map_user_data
 );

 fprintf (
   stdout,
   "Preparing some user data.\n"
 );

 int8_t i8_x = 7;

 fprintf (
   stdout,
   "Adding the user data to the map.\n"
 );

 add_map_node (
   &map,
   "some_key",
   (void *) &i8_x,
   sizeof(int8_t)
 );

 i8_x = 32;
 add_map_node (
   &map,
   "jacob",
   (void *) &i8_x,
   sizeof(int8_t)
 );

 fprintf (
   stdout,
   "Showing the user data.\n"
 );

 for ( uint32_t ui32_i = 0; ui32_i < map .ui32_count; ui32_i ++ ) {
   fprintf (
     stdout,
     "Some value within the block: \"%" PRIi8 "\"\n",
//     *((int8_t*) map .lpary_doubly_linked_list [ ui32_i ] ->lpv_data)
     MAP_DATA_AT ( int8_t, map, ui32_i )
   );
 }

 int8_t *lpi8_x = find_map_node (
   &map,
   "jacob"
 );
 if ( lpi8_x ) {
   fprintf (
     stdout,
     "&map [ \"some_key\" ] = %p; its value is: %" PRIi8 "\n",
     lpi8_x,
     *lpi8_x
   );
 }
 else {
   fprintf (
     stderr,
     "Error: We were unable to find the node.\n"
   );
 }

 fprintf (
   stdout,
   "Freeing the map.\n"
 );

 free_map ( &map );

 fprintf (
   stdout,
   "Terminating.\n"
 );

 return 0;
}}
