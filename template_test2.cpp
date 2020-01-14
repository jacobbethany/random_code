/*
 Created on 2020_01_14 by Jacob Bethany.
 Purpose: To try to wrap the std::vector in such a way so as to extend the functionality of the existing std::vector::iterator.
 Results: It seems possible, but I'm getting a real lesson in templating that I never wanted.

 Tasks left to do:
 -Try instantiating the custom_vector_iterator with new and freeing it with delete to see if the access to the iterator's contents
 is suitably coherent.

 Notes:
 -When making a templated class, it's method definitions (not the prototypes within the class definition)
 need to be defined thusly:
 template<class T>
 class_name<T>::method_name ( typename std::vector<T> ){{...}}
           ^^^
 We're generically defining all possible types of this method of this class.

 -Operator overloads are janky.
 For prefix and postfix ++ and --, the left hand and right hand are denoted by dummy variables.
 See this post: https://stackoverflow.com/questions/17886220/overloading-postfix-operator
 TL;DR:
 void operator++(Number&);      //prefix
 void operator++(Number&, int); //postfix

 Other notable reading:
 http://pages.cs.wisc.edu/~driscoll/typename.html
 https://www.tutorialspoint.com/cplusplus/cpp_overloading.htm
 http://www.cplusplus.com/doc/oldtutorial/templates/
*/
 #include "stdio.h"
 #include "stdlib.h"
 #include "string.h"
 #include <vector>

 template<class T>
 class custom_vector_iterator
{
 private:
 typename std::vector<T> *m_vector_reference; //The vector_reference.
 typename std::vector<T>::iterator m_iterator; //The current iterator of the list, this ->m_vector_reference.
 unsigned long m_ul_offset = 0;

 public:

 custom_vector_iterator ( typename std::vector<T> *vector_through_which_to_iterate );
 ~custom_vector_iterator ( void );

 unsigned char goto_first_iterator ( void );
 unsigned char goto_last_iterator ( void );

 unsigned char increment_iterator ( void );
 unsigned char decrement_iterator ( void );
 unsigned char operator++ (  ); //custom_vector_iterator::++
 unsigned char operator-- (  );

 T& operator* (  ); //this is the dereference overload (*custom_vector_iterator).
 typename std::vector<T>::iterator get ( void );
};

 template<class T>
 custom_vector_iterator<T>::custom_vector_iterator ( typename std::vector<T> *vector_through_which_to_iterate )
{{
 this ->m_vector_reference = vector_through_which_to_iterate;
 this ->m_iterator = vector_through_which_to_iterate ->begin (  );
 this ->m_ul_offset = 0;
}}

 template<class T>
 custom_vector_iterator<T>::~custom_vector_iterator ( void )
{{
}}

 template<class T>
 unsigned char custom_vector_iterator<T>::goto_first_iterator ( void )
{{
 this ->m_iterator = this ->m_vector_reference .begin (  );
 this ->m_ul_offset = 0;
}}

 template<class T>
 unsigned char custom_vector_iterator<T>::goto_last_iterator ( void )
{{
 //This this safe for a list of size zero?
 this ->m_iterator = this ->m_vector_reference .end (  ) - 1;
 this ->m_ul_offset = this ->m_vector_reference .size (  ) - 1;
}}

//Usage: if ( custom_vector_iterator::increment_iterator (  ) ) use_the_iterator (  );
 template<class T>
 unsigned char custom_vector_iterator<T>::increment_iterator ( void )
{{
 if ( this ->m_iterator == this ->m_vector_reference ->end (  ) )
      return 0;

 this ->m_iterator ++;
 this ->m_ul_offset ++;
 if ( this ->m_iterator == this ->m_vector_reference ->end (  ) )
      return 0;

 return 1;
}}

//Usage: if ( custom_vector_iterator::decrement_iterator (  ) ) use_the_iterator (  );
 template<class T>
 unsigned char custom_vector_iterator<T>::decrement_iterator ( void )
{{
 //If we're already on the first node of the vector, don't try to decrement.
 if ( ! this ->m_ul_offset )
      return 0;

 this ->m_iterator = this ->m_vector_reference ->begin (  ) + this ->m_ul_offset - 1;
 this ->m_ul_offset --;

 return 1;
}}

//Usage: if ( custom_vector_iterator::++ ) use_the_iterator (  );
 template<class T>
 unsigned char custom_vector_iterator<T>:: operator++ ( void )
{{
 return this ->increment_iterator (  );
}}

//Usage: if ( custom_vector_iterator::++ ) use_the_iterator (  );
 template<class T>
 unsigned char custom_vector_iterator<T>::operator -- ( void )
{{
 return this ->decrement_iterator (  );
}}

 template<class T>
 typename std::vector<T>::iterator custom_vector_iterator<T>::get ( void )
{{
 return this ->m_iterator;
}}

 template<class T>
 T& custom_vector_iterator<T>::operator* (  )
{{
 return *(this ->m_iterator);
}}

 int main ( int argc, char **argv )
{{
 std::vector<unsigned char> number_vector;

 for ( unsigned char uc_i = 0; uc_i < 10; uc_i ++ )
       number_vector .push_back ( uc_i );


 //Instantiated test:
 custom_vector_iterator<unsigned char> *it = new custom_vector_iterator<unsigned char>( &number_vector );
 do
   {
    //printf ( "Here's the number: [%d].\n", *(it ->get (  )) );
    printf ( "Here's the number: [%d].\n", *(*it) );
   } while ( ++(*it) );

 while ( --(*it) )
         printf ( "Reverse: [%d].\n", *(*it) );

 delete it;


 //Basic test:
 custom_vector_iterator<unsigned char> basic_iterator = custom_vector_iterator<unsigned char> ( &number_vector );
 do
   {
    //printf ( "Here's the number: [%d].\n", *(basic_iterator .get (  )) );
    printf ( "Here's the number: [%d].\n", *basic_iterator );
   } while ( ++basic_iterator );

 while ( --basic_iterator )
         printf ( "Reverse: [%d].\n", *basic_iterator );


 return 0;
}}
