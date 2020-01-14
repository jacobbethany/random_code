/*
 Created on 2020-01-14 by Jacob Bethany.

 Purpose: To see if making an arbitrary function with c++ templating to get the previous iterator of a vector would be possible.

 Results: While it does seem to be possible it also seems unneeded, given that one can do simple addition or subtraction with
 the vector's iterator to get the next or previous node and compare the previous against vector::begin and the next against
 vector::end to ensure that the addition or subtraction hasn't overstepped the vector's start or end.

 Important note: For a vector with zero elements, vector::begin = vector::end.
 Therefore, subtracting from the end of the list to get the penultimate iterator is safe, since we can still check against
 vector::end to see if we've reached an invalid iterator.
*/
 #include "stdio.h"
 #include "stdlib.h"
 #include "string.h"
 #include <vector>
 //#include <map>
 //#include <list>

 //This template line works only for T get_max.
 //The template line will be required, again, before any other templated functions.
 template <class T, class T2>
 unsigned char get_max ( T a, T2 b )
{{
 return (unsigned char) a >= (unsigned char) b ? (unsigned char) a : (unsigned char) b;
}}

 //This template line must precede any function to which it applies.
 template <class T, class T2>
 unsigned char get_min ( T a, T2 b )
{{
 return (unsigned char) a <= (unsigned char) b ? (unsigned char) a : (unsigned char) b;
}}

 void basic_test_one ( void )
{{
 float x = 100.0f;
 unsigned char b = 210;

 printf ( "%d is larger than %d.\n", get_max ( x, b ), get_min ( x, b ) );
}}

/*
 Usage:
 ___________________________________________________________________________________________________________________
 std::vector<_>::iterator previous_iterator;
 if ( get_previous_vector_iterator ( some_vector.begin(), some_vector.end(), current_iterator, previous_iterator ) )
      //use the previous iterator.
 else
      //complain that we can't use the previous iterator, since it couldn't be found.
 ___________________________________________________________________________________________________________________

 This function will return true if the vector was able to be decremented;
 it will return false if the vector couldn't be iterated.

 Failure conditions: The current iterator is the first iterator of the list;
 the iterator points references the end of the vector ( vector .end (  ) ).
*/
 template <class T>
 unsigned char get_previous_vector_iterator (
   T vector_first_iterator, //vector::begin
   T vector_last_iterator,  //vector::end
   T &current_vector_iterator,      //current std::vector<_>::iterator
   T &out_previous_vector_iterator  //desired std::vector<_>::iterator
 )
{{
 //If we're at the end of the vector (not on its last node), see if we can get the last item.
 if ( current_vector_iterator == vector_last_iterator )
     {
      //If the list is empty.
      if ( vector_last_iterator == vector_first_iterator )
           return 0;

      out_previous_vector_iterator = vector_last_iterator - 1; //I'm not sure whether this is safe.
      //does end() - 1 = begin(), if the list has no elements or does it underflow or is its behavior undefined?

      //If the first node of the list exists (it isn't a reference to the end).
      if ( out_previous_vector_iterator != vector_first_iterator )
           return 1;

      //The vector is empty, we couldn't decrement the current node.
      return 0;
     }

 out_previous_vector_iterator = vector_first_iterator;
 for ( out_previous_vector_iterator = vector_first_iterator; out_previous_vector_iterator != vector_last_iterator; out_previous_vector_iterator ++ )
      {
       if ( out_previous_vector_iterator + 1 == current_vector_iterator )
            return 1;
      }

 //We couldn't find the current vector iterator within the source vector.
 return 0;
}}

/*
 //This is how I'd planned to do this.
 //Unfortunately, C++ doesn't seem to understand this sort of syntax.
 template <typename T>
 unsigned char get_previous_vector_iterator (
   std::vector<T> &source_vector,
   std::vector<T>::iterator &current_vector_iterator,
   std::vector<T>::iterator &out_previous_vector_iterator
 )
*/
 //This method works (stick typename before the nested templated type).
 //See the reason here: http://pages.cs.wisc.edu/~driscoll/typename.html
/*
 Usage:
 std::vector<type>::iterator prev;
 if ( get_previous_vector_iterator ( some_vector, current_iterator, prev ) )
      //do something with the previous vector iterator, prev.
 else
      //complain that we can't find the previosu node.
*/
 template <class T>
 unsigned char get_previous_vector_iterator (
   std::vector<T> &source_vector,
   typename std::vector<T>::iterator &current_vector_iterator,
   typename std::vector<T>::iterator &out_previous_vector_iterator
 )
{{
 return get_previous_vector_iterator ( source_vector.begin(), source_vector.end(), current_vector_iterator, out_previous_vector_iterator );
}}

 int main ( int argc, char **argv )
{{
 //basic_test_one (  );
 std::vector<unsigned char> number_vector;

 if ( number_vector .begin (  ) == number_vector .end (  ) )
     {
      printf ( "Checking to see if we're on the terminating iterator of the vector will always work for any sized list.\n" );
     }
 else printf ( "There's probably some other solution required for checking the bounds of the list.\n" );

 number_vector .push_back ( 1 );
 number_vector .push_back ( 2 );
 number_vector .push_back ( 3 );
 number_vector .push_back ( 4 );
 number_vector .push_back ( 5 );

 std::vector<unsigned char>::iterator it = number_vector .begin (  );

 unsigned long ul_iteration_count = 0;
 if ( argc == 2 )
     {
      ul_iteration_count = atoi ( argv [ 1 ] );
      printf ( "We'll iterate %d times, as per your request.\n", ul_iteration_count );
     }

 for ( unsigned long ul_i = 0; ul_i < ul_iteration_count; ul_i ++ )
      {
       it ++;
       if ( it == number_vector .end (  ) )
           {
            printf ( "Note: You've overstepped the vector.\n" );
            break;
           }
      }

 //Manual testing.
 //it ++; //2 works with or without this.
 //it ++; //3
 //it ++; //4
 //it ++; //5
 //it ++; //end()
 printf ( "The current number is: [%d].\n", *it );

 std::vector<unsigned char>::iterator prev;
 //if ( get_previous_vector_iterator ( number_vector .begin (  ), number_vector .end (  ), it, prev ) )
 if ( get_previous_vector_iterator ( number_vector, it, prev ) )
      printf ( "The previous number was: [%d].\n", *prev );
 else
      printf ( "There was no previous number.\n" );

 return 0;
}}
