/*
 Created on 2020-06-04 by Jacob Bethany.
 Purpose: To convert the permutation script that I wrote in Perl into C++.

 Note: This could probably be done a bit better, but I wanted to try making
 this work similarly to the Perl script that I wrote, before.
*/
 #include "stdio.h"
 #include "stdlib.h"
 #include "string.h"
 #include "stdint.h"
 #include <vector>

 typedef struct PERM {
   unsigned char uc_head; //Start of the branch (see the slopes in show_permutations).
   std::vector<PERM> body_vector; //The dis/connected limb of the branch that shows the variations.
 } PERM;

//This will convert the permutations to an vector of std::vector<unsigned char> for easier presentation.
 void convert_permutations (
   std::vector<PERM> &permutations,
   std::vector<std::vector<unsigned char>> &output_vector,
   uint32_t ul_indentation,
   std::vector<unsigned char> previous_vector //should be empty when first passed.
 )
{{
 for ( std::vector<PERM>::iterator it = permutations .begin (  );
       it != permutations .end (  );
       it ++
     )
      {
       //We need to pass a temporary vector, since the variations will accumulate garbage that we don't want, otherwise.
       std::vector<unsigned char> tmp_previous_vector ( previous_vector .begin (  ), previous_vector .end (  ) );
       tmp_previous_vector .push_back ( it ->uc_head );

       //If we've reached the end of the branch, copy the entire permutation into the vector of vectors.
       if ( permutations .size (  ) == 1 )
            output_vector.push_back ( tmp_previous_vector );

       //If we're able to make a recursive call to follow a slope, do it.
       if ( it ->body_vector .size (  ) )
           {
            convert_permutations (
              it ->body_vector,
              output_vector,
              ul_indentation + 1,
              tmp_previous_vector
            );
           }

       //If we're in the root call, we should clear the list so that we don't have abcd, -> abcdbacd for the second loop iteration.
       if ( ! ul_indentation )
            previous_vector .clear (  );
      } //while passed permutations.
}}

//This is just a function overload to simply the call.
 void convert_permutations (
   std::vector<PERM> &permutations,
   std::vector<std::vector<unsigned char>> &output_vector
 )
{{
 std::vector<unsigned char> previous_vector;
 return convert_permutations ( permutations, output_vector, 0, previous_vector );
}}

/*
 This will print the passed number of spaces to stdout.
*/
 void indent ( uint32_t ul_spaces )
{{
 //Prepad ul_spaces spaces before the string constant string, "" which has no length.
 //So, essentially, this will always prepad the full amount since there's no space taken
 //by the pseudo string.
 printf ( "% *s", ul_spaces, "" );
}}

/*
 This will show the permutations calculated by get_permutations in a series
 of slopes with indentation to show how they relate to one another.
*/
 void show_permutations ( std::vector<PERM> &v, uint32_t ul_indentation )
{{
 for ( std::vector<PERM>::iterator it = v .begin (  );
       it != v .end (  );
       it ++
     )
      {
       indent ( ul_indentation );
       printf ( "%c\n", it ->uc_head );
       if ( it ->body_vector .size (  ) )
            show_permutations ( it ->body_vector, ul_indentation + 1 );
      }
}}

/*
 This will show a vector of unsigned characters as a comma delimited list
 of the ascii character values within the vector.
*/
 void show_char_vector ( std::vector<unsigned char> &char_vector )
{{
 for ( std::vector<unsigned char>::iterator it = char_vector .begin (  );
       it != char_vector .end (  );
       it ++
     )
      {
       if ( it != char_vector .begin (  ) )
            printf ( ", " );

       printf ( "%c", *it );
      }
 printf ( "\n" );
}}

//Pass a reference to an array for which you'd like the permutations.
 std::vector<PERM> get_permutations ( std::vector<unsigned char> &set )
{{
 std::vector<PERM> permutations;

 for ( std::vector<unsigned char>::iterator it = set .begin (  );
       it != set .end (  );
       it ++
     )
      {

       //printf ( "Iteration #%ld:\n", (it - set .begin())+1 );

       //Either copy everything before this iterator or nothing.
       //Note: This will stop when it reaches the end point rather than copying it.
       std::vector<unsigned char> perm_head ( set .begin (  ), it );

       //Either copy everything after this iterator or nothing.
       //Note: This will copy starting on the node after the one we're currently on until but not including the end pointer.
       std::vector<unsigned char> perm_foot ( (it != set .end (  )) ? it + 1 : set .end (  ), set .end (  ) );

       //printf ( "This is the head vector:\n" );
       //show_char_vector ( perm_head );

       //printf ( "This is the foot vector.\n" );
       //show_char_vector ( perm_foot );

       //Pass everything to the recursive call except the current iterator node.
       std::vector<unsigned char> tmp_vector ( perm_head .begin (  ), perm_head .end (  ) );
       tmp_vector .insert ( tmp_vector .end (  ), perm_foot .begin (  ), perm_foot .end (  ) );
       std::vector<PERM> ret = get_permutations ( tmp_vector );

       //Each PERM structure will contain the branch's root node and a vector of limb sequence variations.
       PERM permutation;
       permutation .uc_head = *it;

       //Now that we've added the head, we need to prepare a PERM structure with a vector of
       //PERM structures from the recursive call (of which there may be many).
       for ( std::vector<PERM>::iterator ret_iterator = ret .begin (  );
             ret_iterator != ret .end (  );
             ret_iterator ++
           )
            {
             permutation .body_vector .push_back ( *ret_iterator );
            }

       //Add a PERM structure filled with the branch root item and the variations thereform.
       permutations.push_back ( permutation );

      } //foreach of the array members.

 return permutations;
}}

 int main ( int argc, char **argv )
{{
 //Define a set to permute.
 const unsigned char uc_set [  ] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g' };

 //Initialize a vector with the set to permute. (This is just cleaner than repeating .push_back() for each item.)
 std::vector<unsigned char> set ( uc_set, uc_set + (sizeof(uc_set) / sizeof(unsigned char)) );

 //Get a vector of PERM structures that describe each of the permutations of the set.
 std::vector<PERM> permutations = get_permutations ( set );

 //Show the permutations that we've calculated as a series of slopes.
 show_permutations ( permutations, 0 );

 //Convert the permutations to a more simplistic list, a vector of vectors.
 std::vector<std::vector<unsigned char>> simple_permutations;
 convert_permutations ( permutations, simple_permutations );

 //Show the simplified list of permutations in a clean format.
 for ( std::vector<std::vector<unsigned char>>::iterator it = simple_permutations .begin (  );
       it != simple_permutations .end (  );
       it ++
     )
      {
       //The format will be like this: "#0001: a, b, c, d, e, f, g"
       printf ( "#%04d: ", (it - simple_permutations .begin (  ))+1 );
       show_char_vector ( *it ); //This will print to stdout something like implode/join with ", "
      }

 return 0;
}}
