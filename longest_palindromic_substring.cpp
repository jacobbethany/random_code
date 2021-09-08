/*
 Created on 2021-09-08 by Jacob Bethany
 Purpose: To find the longest palindrome within a substring of a larger string.
*/
 #include "stdio.h"
 #include "stdlib.h"
 #include "string.h"
 #include <vector>
 #include <string>
 #include <algorithm>

//This will expand a string in to all possible substrings and return each of them
//within a vector.
//Example:
//ABC: -> { "A", "AB", "ABC", "B", "BC", "C" }
//Note: In the event of a repeated value, it will be displayed only once.
//Example:
//ABA: -> { "A", "AB", "ABA", "B", "BA", [A omitted] }
 std::vector<std::string> expand ( const char *lpsz_string )
{{
 std::vector<char> vec_seen;
 std::vector<std::string> vec_substrings;
 for ( const char *s = lpsz_string; *s; s ++ ) {
   for ( const char *p = s; *p; p ++ ) {
     //For each character added, add the current set of characters.
     vec_seen .push_back ( *p );

     std::string str_substring = std::string (
       vec_seen .begin (  ),
       vec_seen .end (  )
     );

     //If we haven't already added this substring.
     if ( std::find (
            vec_substrings .begin (  ),
            vec_substrings .end (  ),
            str_substring
          ) == vec_substrings .end (  )
        ) {
       vec_substrings .push_back ( str_substring );
     }
   }
   vec_seen .clear (  );
 }
 return vec_substrings;
}}

/*
 This would work, if we used all string lengths at all starting positions.
 ABC would make the following pairs for each index:

  0: A, AB, ABC
  1: B, BC
  2: C
*/
 bool is_palindrome ( const char *lpsz_string )
{{
 //Note: ABA = 3/2 = 1.5 = 1.
 int i_len = strlen ( lpsz_string );
 int i_remainder = i_len % 2;
 int i_middle = i_len >> 1;


 //ABA --> { a : 1, b : 1 } = 3/2, 3/2
 //012
 //ABBA --> { a : 1, b : 2 } = ((4/2) -1), (4/2)
 //0123

 const char *a = lpsz_string + i_middle;
 const char *b = a;
 if ( ! i_remainder ) {
   a --;
 }

 //for ( ; a >= lpsz_string; a -- ) {
 for ( ;; ) {
   //if ( ! *b ) {
   if ( *b != *a ) { //including null terminator check at the end of the string.
     return false;
   }

   printf ( "{ a : '%c', b : '%c'}\n", *a, *b );
   b ++;

   if ( a <= lpsz_string ) {
     break;
   }
   a --;
 }

 return true;
}}

//See is_palindrome for how this works.
//This is a very suboptimal method.
 std::string slow_glp ( const char *lpsz_string )
{{
 std::vector<std::string> vec_substrings = expand ( lpsz_string );
 int i_longest_palindrome_length = 0;
 int i_longest_palindrome_index = 0;
 for ( std::vector<std::string>::const_iterator it = vec_substrings .cbegin (  );
       it != vec_substrings .cend (  );
       it ++
     ) {
   printf ( "Substring: \"%s\"\n", it ->c_str (  ) );
   if ( is_palindrome ( it ->c_str (  ) ) ) {
     if ( it ->length (  ) > i_longest_palindrome_length ) {
       i_longest_palindrome_length = it ->length (  );
       i_longest_palindrome_index = it - vec_substrings .cbegin (  );
     }
   }
 }

 return vec_substrings [ i_longest_palindrome_index ];
}}

//This will start at the specified a and b points and
//recursively expand until a =/= b or a < s or b > e.
 std::string get_palindrome (
   const char *s, //string start.
   const char *a,
   const char *b
 )
{{
 //printf ( "String from a: \"%s\"\n", a );
 //This should also account for a null terminator check on *b.
 if ( *a != *b ) {
   printf ( "%c =/= %c @off: { %ld, %ld }\n", *a, *b, a-s, b-s );
   //Return everything starting from the previous a until the character
   //before the current b.
   return std::string ( a+1, b );
 }
 //The characters were equal, but we've reached the start of the string,
 //we'd underflow the buffer, if we continued to recurse.
 if ( a == s ) {
   //In this case, everything from the CURRENT A to B (INCLUSIVE) are valid.
   return std::string ( a, b + 1 ); //maybe - 1?
 }

 //Otherwise, recurse.
 return get_palindrome ( s, a-1, b+1 );
}}

//Get longest palindromic substring by attempting
//to get all palindromes from all points within the string.
//Note: Since patterns like ABBA need to start on indices
//1 and 2, whereas patterns like ABA need to start on indices
//1 and 1, to calls to get_palindrome are made per string position.
//Returns the longest palindromic substring found within the string
//in the form of an std::string.
 std::string glp ( const char *lpsz_string )
{{
 int i_len = strlen ( lpsz_string );
 std::string str_longest_palindrome;
 for ( const char *s = lpsz_string; *s; s ++ ) {

   //Check for ABA pattern.
   std::string str_palindrome = get_palindrome ( lpsz_string, s, s );
   printf ( "Current (s,s) palindrome: \"%s\"\n", str_palindrome.c_str (  ) );
   if ( str_palindrome .length (  ) > str_longest_palindrome .length (  ) ) {
     str_longest_palindrome = str_palindrome;
   }

   //CHECK for ABBA pattern, too.
   str_palindrome = get_palindrome ( lpsz_string, s, s+1 );
   printf ( "Current (s,s+1) palindrome: \"%s\"\n", str_palindrome.c_str (  ) );
   if ( str_palindrome .length (  ) > str_longest_palindrome .length (  ) ) {
     str_longest_palindrome = str_palindrome;
   }

 }
 if ( str_longest_palindrome .length (  ) ) {
   return str_longest_palindrome;
 }
 return std::string ( lpsz_string, lpsz_string + 1 );
}}

 int main ( int argc, char **argv )
{{
 //This way is very slow, but it works.
 std::vector<std::string> vec_substrings = expand ( "ABA" );
 int i_longest_palindrome_length = 0;
 int i_longest_palindrome_index = 0;
 for ( std::vector<std::string>::const_iterator it = vec_substrings .cbegin (  );
       it != vec_substrings .cend (  );
       it ++
     ) {
   printf ( "Substring: \"%s\"\n", it ->c_str (  ) );
   if ( is_palindrome ( it ->c_str (  ) ) ) {
     if ( it ->length (  ) > i_longest_palindrome_length ) {
       i_longest_palindrome_length = it ->length (  );
       i_longest_palindrome_index = it - vec_substrings .cbegin (  );
     }
   }
 }
 printf (
   "The longest palindrome is: \"%s\"\n",
   vec_substrings [ i_longest_palindrome_index ] .c_str (  )
 );
//return 0;

 std::vector<std::string> vec_test_strings;
 vec_test_strings .push_back ( "babad" );
 vec_test_strings .push_back ( "cbbd" );
 vec_test_strings .push_back ( "a" );
 vec_test_strings .push_back ( "ac" );
 vec_test_strings .push_back ( "cc" );
 vec_test_strings .push_back ( "ccc" );
 vec_test_strings .push_back ( "civilwartestingwhetherthatnaptionoranynartionsoconceivedandsodedicatedcanlongendureWeareqmetonagreatbattlefiemldoftzhatwarWehavecometodedicpateaportionofthatfieldasafinalrestingplaceforthosewhoheregavetheirlivesthatthatnationmightliveItisaltogetherfangandproperthatweshoulddothisButinalargersensewecannotdedicatewecannotconsecratewecannothallowthisgroundThebravelmenlivinganddeadwhostruggledherehaveconsecrateditfaraboveourpoorponwertoaddordetractTgheworldadswfilllittlenotlenorlongrememberwhatwesayherebutitcanneverforgetwhattheydidhereItisforusthelivingrathertobededicatedheretotheulnfinishedworkwhichtheywhofoughtherehavethusfarsonoblyadvancedItisratherforustobeherededicatedtothegreattdafskremainingbeforeusthatfromthesehonoreddeadwetakeincreaseddevotiontothatcauseforwhichtheygavethelastpfullmeasureofdevotionthatweherehighlyresolvethatthesedeadshallnothavediedinvainthatthisnationunsderGodshallhaveanewbirthoffreedomandthatgovernmentofthepeoplebythepeopleforthepeopleshallnotperishfromtheearth" );

 for ( std::vector<std::string>::const_iterator it = vec_test_strings .cbegin (  );
       it != vec_test_strings .cend (  );
       it ++
     ) {
   printf ( "Finding the substrings of \"%s\"\n", it ->c_str (  ) );


   std::string str_longest_palindrome = glp ( it ->c_str (  ) );
   printf (
     "The longest palindromic substring is \"%s\"\n",
     str_longest_palindrome .c_str (  )
   );

   printf (
     "Iteration #%ld complete.\n",
     it - vec_test_strings .cbegin (  )
   );
 }

 return 0;
}}
