/*
 Created on 2020-06-05 by Jacob Bethany
 Purpose: To make a cleaner C++ version of the permutation script that I wrote, previously.

 This method fits better within a C++ idiom. I'd need to do some benchmarking, but I'd think
 that checking to see if an item has been seen yet and skipping it is probably faster than
 constantly creating new vector objects with array subsets.

 Note that this method will return a std::vector<std::vector<unsigned char>> immediately.
 No conversion is necessary.
*/
 #include "stdio.h"
 #include "stdlib.h"
 #include "string.h"
 #include <vector> //std::vector
 #include <algorithm> //std::find

/*
 This function will take reference to a set to permute;
 a reference to a vector of items from that set that have already been used by previous calls,
 if any have; and a reference to a vector in which to return the calculated permutations.

 The way that this function works is by continuously picking every item from the set,
 one at a time, so long as the item hasn't already been chosen by a call earlier in the
 chain of recursive calls.
 P ( {abcd} {} ) -> {
   P({abcd}, {a}) -> {
     P({abcd}, {ab}) -> {
       P({abcd}, {abc}) -> {
         P({abcd}, {abcd}) //This would be detected at {adc} and a recursive call wouldn't actually be made.
       },
       P({abcd}, {abd}) -> {
         P({abcd}, {abdc}) //This would be detected at {abd} and a recursive call wouldn't actually be made.
       }
     },
     P({abcd}, {ac}) -> {
       P({abcd}, {acb}) -> {
         P({abcd}, {acbd}) //This would be detected at {adb} and a recursive call wouldn't actually be made.
       },
       P({abcd}, {acd}) -> {
         P({abcd}, {acdb}) //This would be detected at {acd} and a recursive call wouldn't actually be made.
       }
     },
     P({abcd}, {ad}) -> {
       P({abcd}, {adb}) -> {
         P({abcd}, {adbc}) //This would be detected at {adb} and a recursive call wouldn't actually be made.
       },
       P({abcd}, {adc}) -> {
         P({abcd}, {adcb}) //This would be detected at {adc} and a recursive call wouldn't actually be made.
       }
     },
   },
   P({abcd}, {b}) -> { ... },
   P({abcd}, {c}) -> { ... },
   P({abcd}, {d}) -> { ... }
 }
*/
 void permute (
   std::vector<unsigned char> &set,
   std::vector<unsigned char> &chosen,
   std::vector<std::vector<unsigned char>> &permutations
 )
{{
 //For each member of the complete set, look at each of the items.
 for ( std::vector<unsigned char>::iterator member = set .begin (  );
       member != set .end (  );
       member ++
     )
      {
       //Don't pick something from the set that we've already picked.
       if ( std::find ( chosen .begin (  ), chosen .end (  ), *member ) != chosen .end (  ) )
            continue;

       //If we're about to complete the set, then there won't be any other items to choose.
       //We can simply add one item to the current vector, rather than creating a new copy
       //vector and adding the current (only) item there.
       if ( chosen .size (  ) == set .size (  ) - 1 )
           {
            chosen .push_back ( *member );
            permutations .push_back ( chosen );
            return ;
           }

       //Otherwise, we need to copy the vector to a temporary one so that the next selection
       //(there will definitely be one) doesn't have this current loop iteration's selection
       //in it, as well, when we make the recursive call.
       std::vector<unsigned char> branch ( chosen .begin (  ), chosen .end (  ) );

       //Next, let's add the current item to the copy vector.
       branch .push_back ( *member );

       //Finally, let's continue permuting by chosing another item from the set.
       permute ( set, branch, permutations );
      }
}}

//This is an overload of the actual permutation function to simplify calling it.
 std::vector<std::vector<unsigned char>> permute (
   std::vector<unsigned char> &set
 )
{{
 std::vector<std::vector<unsigned char>> permutations;
 std::vector<unsigned char> chosen;
 permute ( set, chosen, permutations );
 return permutations;
}}

//This is intended for primatives like float, double, char, short, dword...etc.
 template <typename T>
 void show_unsigned_character_vector ( std::vector<T> &uc_vector, const char *sz_format )
{{
 for ( typename std::vector<T>::iterator it = uc_vector .begin (  );
       it != uc_vector .end (  );
       it ++
     )
      {
       if ( it != uc_vector .begin (  ) )
            printf ( ", " );

       printf ( sz_format, *it );
      }
 printf ( "\n" );
}}

 int main ( int argc, char **argv )
{{
 unsigned char uc_array [  ] = { 'a', 'b', 'c', 'd' };
 std::vector<unsigned char> set ( uc_array, uc_array + (sizeof(uc_array) / sizeof(unsigned char)) );
 std::vector<std::vector<unsigned char>> permutations = permute ( set );

 for ( std::vector<std::vector<unsigned char>>::iterator it = permutations .begin (  );
       it != permutations .end (  );
       it ++
     )
      {
       //Show each vector of unsigned characters as ASCII characters.
       show_unsigned_character_vector ( *it, "%c" ); //"%u" Show them as their ordinal decimal values.
      }

 return 0;
}}
