/*
 Created on 2022-04-28 by Jacob Bethany
 Purpose: To understand tries and suffix trees.

 TODO:
 [X] Generate suffix trie.
 [X] Matching substring with suffix trie.
 [X] MSVC and GCC compatibility.
 [ ] Use std::vector<TRIE>::insert on TRIE.vec_children to place each
 key at an alphabetical position.
 [ ] Store an extra std::vector<TRIE>* lpvec_first_char_map [ 256 ];
 This will map each possible starting character to the first of each
 list of nodes in the vector mentioned hereinabove for that character.
 It's important that the list be sorted as it is created so that this
 will work. It will speed inserts greatly, as the list grows in size.
 [ ] Implement this trie suffix tree code into a map for its keys.

 See:
   https://www.youtube.com/watch?v=VA9m_l6LpwI

 Note:
  Matches will happen from right to left.
*/
#ifndef _WIN32
 #include "string.h"
#else
 #include "windows.h"
#endif
 #include "stdio.h"
 #include "stdlib.h"
 #include "stdint.h"
 #include "inttypes.h"
 #include <string>
 #include <vector>

 //If set to 1 (true), debugging output will be added.
 #define DEBUG_MODE 0
 #if ( DEBUG_MODE )
   #define debug_printf(...) fprintf ( stdout, __VA_ARGS__ )
 #else
   #define debug_printf(...)
 #endif

typedef struct TRIE {
  std::string str_substring; //either empty or contains some chunk of letters.
  uint32_t ui32_index; //The index of the first character of the substring represented by this node.
  std::vector<TRIE> vec_children; //vector of zero or more children
} TRIE;

/*
It seems like one should keep branching recursively and always match only
the first character to find the starting node. If a starting node is found,
all characters of that starting node past the first should be matched until
no more do. Once the mismatch is found, two children should be created. One
should have the remaining mismatched character(s) of the node that was found,
and the other should contain the remaining mismatched characters of the new
entry.

This means that any clusters of characters in nodes will always be
towards the ends of branches.

Example from the video at the top of the comments:
CAGTCAGG$
---------

  /AG\                /G\ _______/         /CA\         TCAGG$
G$    TCAGG$         $   TCAGG$   G$    GG$    GTCAGG$



Here's an example of the process that I hope to achieve:

Input (parsed from right to left):
CTAGCTAB

1
 B

2
A B

3
T A B

4
CTAB TAB AB B

5
GCTAB CTAB TAB AB B

6
GCTAB CTAB TAB A      B
              B ACTAB

7
GCTAB CTAB TA           A        B
          B  AGTAB     B AGTAB


8
GCTAB      TA           A        B     CTA
          B  AGTAB     B AGTAB        B   GCTAB
*/
 uint8_t add_trie (
   TRIE &trie,
   const char *lpsz_substring,
   uint32_t ui32_index
 )
{{
 if ( ! lpsz_substring ) {
   return 0;
 }
 const char *lp_existing_branch_mismatch_point = 0;
 size_t st_existing_branch_mismatch_index = 0;
 TRIE *node = &trie;
 const char *lpsz_original_substring = lpsz_substring;

 bool b_done;
 //while we match as many characers as possible from lpsz_substring with
 //one of the children of the passed trie.
 do {

   b_done = true;
   debug_printf ( "Viewing nodes of substring: \"%s\"\n", node ->str_substring .c_str (  ) );
   //Match as many characters as possible, then step into the child node.
   for ( auto &t : node ->vec_children ) {
     debug_printf ( "  Viewing node: \"%s\"\n", t .str_substring .c_str (  ) );

     if ( *lpsz_substring == t .str_substring [ 0 ] ) {
       lp_existing_branch_mismatch_point = t .str_substring .c_str (  ) + 1;
       lpsz_substring ++;

       while ( *lpsz_substring && *lp_existing_branch_mismatch_point == *lpsz_substring ) {
         lp_existing_branch_mismatch_point ++, lpsz_substring ++;
       }

       st_existing_branch_mismatch_index = lp_existing_branch_mismatch_point - t .str_substring .c_str (  );

       node = &t;
       b_done = false;
       break;
     }
   }
 } while ( ! b_done );

 //If we can't find a single node that matches the first character,
 //we won't have iterated past the initial reference.

 //Add the entire/remaining substring to the trie.
 TRIE new_node;
 new_node .ui32_index = ui32_index;
 new_node .str_substring = std::string ( lpsz_substring );
 node ->vec_children .push_back ( new_node );

 //If we need to bifurcate the substring that we found in the last node that we matched.
 if ( lp_existing_branch_mismatch_point &&
      *lp_existing_branch_mismatch_point
    ) {

   new_node .str_substring = std::string ( lp_existing_branch_mismatch_point );
   size_t st_characters_to_delete = node ->str_substring .size (  ) - st_existing_branch_mismatch_index;

   debug_printf (
     "Head node deletion index: %zu\n"
     "Head node characters to delete: %zu\n",
     st_existing_branch_mismatch_index,
     st_characters_to_delete
   );

   if ( st_characters_to_delete && st_existing_branch_mismatch_index < node ->str_substring .size (  ) ) {
     node ->str_substring .erase (
       st_existing_branch_mismatch_index,
       st_characters_to_delete
     );
   }
   else {
     fprintf (
       stderr,
       "Error: We can't remove some of the characters from the node from which we're splitting.\n"
     );
   }
   node ->vec_children .push_back ( new_node );
 }

 return 1;
}}

//Show all of the contents of the trie in stdout for debugging purposes.
 void dump_tries ( const TRIE &trie, uint32_t ui32_depth )
{{
 if ( ! trie .str_substring .empty (  ) ) {
   if ( ui32_depth == 1 ) {
     fprintf (
       stdout,
       "%*c(%s, %" PRIu32 ")",
       ui32_depth<<1,
       ' ',
       trie .str_substring .c_str (  ),
       trie .ui32_index
     );
   }
   else {
     fprintf (
       stdout,
       " => {%" PRIu32 "}(\"%s\", %" PRIu32 ")",
       ui32_depth,
       trie .str_substring .c_str (  ),
       trie .ui32_index
     );
   }
 }

 for ( const auto &t : trie .vec_children ) {
   dump_tries ( t, ui32_depth + 1 );
 }

 if ( ui32_depth == 1 ) {
   fprintf ( stdout, "\n" );
 }
}}

/*
 Returns the one-based index of the match;
 returns 0, if the substring doesn't exist.

 Usage:
 size_t st_index = find_trie_substring ( trie, "test" );
 if ( st_index ) {
   //..use (st_index-1) as the match index.
 }
 NOTE: This match will be from right to left.
 So, "test test test" would match at the final "test" substring at the end.
      0123456789^123 = offset 10
*/
 size_t find_trie_substring (
   const TRIE &trie,
   const char *lpsz_search_substring
 )
{{
 const char *lpsz_substring = lpsz_search_substring, *lpsz_prev_substring;
 const TRIE *node = &trie;
 bool b_stepped_in; //if we've stepped into a child node, this will be true.
 do {
   b_stepped_in = false; //Start false at the start of each child node vector iteration.

   //Store the current reference position so that we can check whether or not we've
   //matched any part of the substring during this loop iteration/recursion level.
   lpsz_prev_substring = lpsz_substring;
   for ( std::vector<TRIE>::const_iterator it_trie = node ->vec_children .cbegin (  );
         it_trie != node ->vec_children .cend (  );
         it_trie ++
       ) {

     const char *lpsz_trie_substring = it_trie ->str_substring .c_str (  );
     while ( *lpsz_substring && *lpsz_trie_substring == *lpsz_substring ) {
       debug_printf ( "'%hc' = '%hc'\n", *lpsz_trie_substring, *lpsz_substring );
       lpsz_trie_substring ++, lpsz_substring ++;
     }

     if ( lpsz_substring != lpsz_prev_substring ) {

       debug_printf (
         "partial match with \"%hs\"\n",
         it_trie ->str_substring .c_str (  )
       );

       //If we've matched every character of the string, return the index of the match.
       if ( ! *lpsz_substring ) {
         return it_trie ->ui32_index;
       }

       debug_printf (
         "there are more characters to match, so we're stepping in.\n"
       );
       debug_printf (
         "prev: \"%hs\"; cur: \"%hs\"\n",
         lpsz_substring,
         lpsz_prev_substring
       );

       node = &(*it_trie);
       b_stepped_in = true;

       break;

     } //if there has been a partial or full match with the curent node.

   } //for each of the child nodes of the current node.

 } while ( b_stepped_in ); //while we're continuing to work with new trie nodes.

 return 0; //the trie is one-based, so returning zero indicates that there was no match.
}}

/*
 Add 1 character, 2 characters, ...etc. from the right side of the string
 to the left side thereof until the start of the string is reached, inclusive.
*/
 void generate_trie ( TRIE &trie, const char *lpsz_string )
{{
 const char *p = lpsz_string;
 while ( *p ) p ++;
 p --;
 for ( ; p >= lpsz_string; p -- ) {
   debug_printf (
     "substring: \"%s\"\n",
     p
   );

   add_trie (
     trie,
     p,
     p - lpsz_string + 1
   );

 } //for each of the substrings, in reverse order.
}}

/*
 Takes two arguments from stdin during application execution:
 1.) The string to match against (haystack).
 2.) A substring to match with the trie that was generated from the first argument.

 Displays the offset of the match, the generated trie, and debugging information if DEBUG_MODE != 0.
*/
 int main ( int argc, char **argv )
{{
 TRIE trie;
 std::vector<std::string> vec_substrings;

 //Prevent the user from invoking the application without any arguments.
 if ( argc < 3 ) {
   fprintf (
     stderr,
     "Error: Two arguments expected:\n"
     "1.) A string to break into a trie.\n"
     "2.) A substring to match with the trie.\n"
   );
   return 0;
 }

 fprintf (
   stdout,
   "Creating trie from string: \"%s\"\n\n",
   argv [ 1 ]
 );

 generate_trie ( trie, argv [ 1 ] );
 dump_tries ( trie, 0 );

 fprintf (
   stdout,
   "Attempting to match the trie with the substring, \"%hs\"\n",
   argv [ 2 ]
 );

 size_t st_index = find_trie_substring (
   trie,
   argv [ 2 ]
 );
 if ( st_index ) {
   fprintf (
     stdout,
     "Match found @ index: %zu.\n",
     st_index - 1 //subtract one because its one-based in the trie.
   );
 }
 else {
   fprintf (
     stderr,
     "Error: The substring, \"%hs,\" couldn't be found.\n",
     argv [ 2 ]
   );
 }

 return 0;
}}
