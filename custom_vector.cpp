/*
 Created on 2020-01-15 by Jacob Bethany
 Purpose: To better familiarize myself with templating within C ++.
*/
 #define USE_LINUX 1

 #include "stdio.h"
 #include "stdlib.h"

 #if USE_LINUX
 #include "string.h"
 #else
 #include "conio.h"
 #endif

 #include <vector>




//////start of the custom_vector_iterator
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

 unsigned char operator++ (  ); //custom_vector_iterator::++ prefix
 unsigned char operator-- (  ); //custom_vector_iterator::-- prefix
 unsigned char operator++ ( int dummy ); //custom_vector_iterator::++ postfix
 unsigned char operator-- ( int dummy ); //custom_vector_iterator::-- postfix

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

 int custom_vector_iterator_test ( void )
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

 template<class T>
 unsigned char custom_vector_iterator<T>::operator++ ( int dummy ) //custom_vector_iterator::++ postfix
{{
 this ->increment_iterator (  );
}}

 template<class T>
 unsigned char custom_vector_iterator<T>::operator-- ( int dummy ) //custom_vector_iterator::-- postfix
{{
 this ->decrement_iterator (  );
}}
//////end of the custom_vector_iterator












 template<class T>
 class custom_vector
{
 protected:
 typename std::vector<T> m_vector;
 unsigned long m_ul_size; //Position within the vector of the current iterator.
 //T m_safety_node; //This is what will be returned when the subscript overload is used to access an element that is out of bounds.
 void *m_lp_safety_node; //If we are a vector of objects, it's still not safe to allow random data to be written because the destructor will be corupted.
 //Instead, I've opted to use a block of memory that is equal in size to whatever the type is that this vector is referencing.

 public:
 custom_vector (  );
 ~custom_vector (  );

 unsigned char insert ( T new_node, unsigned long ul_position );
 unsigned char push_back ( T new_node );
 unsigned char push_front ( T new_node );
 unsigned char erase ( typename std::vector<T>::iterator new_node );

 unsigned char clear ( void );
 unsigned char operator+ ( custom_vector<T>& );
 T *operator [] ( unsigned long ul_subscript );
 unsigned char operator = ( const custom_vector &source );

 typename std::vector<T>::iterator begin ( void );
 typename std::vector<T>::iterator end ( void );

 typename std::vector<T>::reverse_iterator rbegin ( void );
 typename std::vector<T>::reverse_iterator rend ( void );

 typename std::vector<T> *get_vector ( void );

 void show_safety_node ( void );
};

//(It should be all zeros unless invalid access was attempt to a subscript)
 template<class T>
 void custom_vector<T>::show_safety_node ( void )
{{
 unsigned long ul_size = sizeof ( T );
 unsigned char *p = (unsigned char *) this ->m_lp_safety_node;

 printf ( "Here's the safety node (%d bytes) within the custom_vector:\n", sizeof ( T ) );

 for ( unsigned long ul_i = 0; ul_i < ul_size; ul_i ++, p ++ )
      {
       if ( *p >= 0x20 && *p <= 0x7E )
           {
            if ( ! ul_i )
                 printf ( "%c", *p );
            else printf ( ", %c", *p );
           }
       else
           {
            if ( ! ul_i )
                 printf ( "%0.2x", *p );
            else printf ( ", %0.2x", *p );
           }
      }
 printf ( "\n" );
}}

 template<class T>
 custom_vector<T>::custom_vector (  )
{{

 this ->m_ul_size = 0;
 try {
   this ->m_lp_safety_node = new char [ sizeof ( T ) ];
   //memset ( this ->m_lp_safety_node, 0, sizeof ( T ) );
   //*((char *) this ->m_lp_safety_node) = 'J';
 } catch ( ... ) {
   printf ( "Warning: The custom_vector couldn't allocate space for a safety-subscript node.\n" );
 }
}}

 template<class T>
 custom_vector<T>::~custom_vector (  )
{{
 delete[] this ->m_lp_safety_node;
}}

//This inserts the element before the position specified.
//This doesn't overwrite the contents at the specified position.
//To do that, use custom_vector::operator[].
 template<class T>
 unsigned char custom_vector<T>::insert ( T new_node, unsigned long ul_position )
{{
 try {
   if ( ul_position > this ->m_ul_size - 1 )
        return this ->push_back ( new_node );

   //!! TODO: try/catch.
   this ->m_vector .insert ( new_node, ul_position );
 }
 catch ( const std::bad_alloc &e ) {
   printf ( "Error: not enough memory for custom_vector.insert().\n" );
 }
 catch ( const std::exception &e ) {
   printf ( "std::exception::what(): \"%s\"\n", e .what (  ) );
   return 0;
   //You can inspect what() for a particular reason, or even try to dynamic_cast<>
   //to find out which specific exception was thrown.
 }
 catch ( ... ) {
   //You have no more type info here, this should be done for last resort (in the
   //outermost scope) only.
   return 0;
 }

 this ->m_ul_size ++;
 return 1;
}}

 template<class T>
 unsigned char custom_vector<T>::push_back ( T new_node )
{{
 try {
    this ->m_vector .push_back ( new_node );
 }
 catch ( const std::bad_alloc &e ) {
   printf ( "Error: not enough memory for custom_vector.push_back().\n" );
 }
 catch ( const std::exception &e ) {
   printf ( "std::exception::what(): \"%s\"\n", e .what (  ) );
   return 0;
   //You can inspect what() for a particular reason, or even try to dynamic_cast<>
   //to find out which specific exception was thrown.
 }
 catch ( ... ) {
   //You have no more type info here, this should be done for last resort (in the
   //outermost scope) only.
   return 0;
 }

 this ->m_ul_size ++;
 return 1;
}}

 template<class T>
 unsigned char custom_vector<T>::push_front ( T new_node )
{{
 try {
    //Insert the node in the front of the list.
    this ->m_vector .insert ( 0, new_node );
 }
 catch ( const std::bad_alloc &e ) {
   printf ( "Error: not enough memory for custom_vector.push_front().\n" );
 }
 catch ( const std::exception &e ) {
   printf ( "std::exception::what(): \"%s\"\n", e .what (  ) );
   return 0;
   //You can inspect what() for a particular reason, or even try to dynamic_cast<>
   //to find out which specific exception was thrown.
 }
 catch ( ... ) {
   //You have no more type info here, this should be done for last resort (in the
   //outermost scope) only.
   return 0;
 }

 this ->m_ul_size ++;
 return 1;
}}

 template<class T>
 unsigned char custom_vector<T>::erase ( typename std::vector<T>::iterator to_delete )
{{
 try {
   this ->vector .erase ( to_delete );
 }
 catch ( ... ) {
   return 0;
 }

 this ->m_ul_size --;
 return 1;
}}

 template<class T>
 unsigned char custom_vector<T>::clear ( void )
{{
 try {
   this ->vector .clear (  );
 }
 catch ( ... ) {
   return 0;
 }

 this ->m_ul_size = 0;
 return 1;
}}

//Copy the vector to the end of ours.
 template<class T>
 unsigned char custom_vector<T>::operator + ( custom_vector &source )
{{
 for ( typename std::vector<T>::iterator it = source .begin (  );
       it != source .end (  );
       it
     )
      {
       if ( ! this ->push_back ( *it ) )
            return 0;

       this ->m_ul_size ++;
      }

 return 1;
}}

//This returns a reference to the value in the vector at the specified position,
//if the subscript index is within the bounds of the vector's beginning and end.
//(I'm not sure what the type is for the return value.)
 template<class T>
 T *custom_vector<T>::operator [] ( unsigned long ul_subscript )
{{
 if ( ul_subscript >= this ->m_ul_size - 1 )
      return (T *) m_lp_safety_node;
//      return &m_safety_node;

 //return &this ->m_vector [ ul_subscript ];
 return &this ->m_vector .at ( ul_subscript );
}}

 template<class T>
 unsigned char custom_vector<T>::operator = ( const custom_vector &source )
{{
 this ->m_vector = source;
}}

 template<class T>
 typename std::vector<T>::iterator custom_vector<T>::begin ( void )
{{
 return this ->m_vector .begin (  );
}}

 template<class T>
 typename std::vector<T>::iterator custom_vector<T>::end ( void )
{{
 return this ->m_vector .end (  );
}}

 template<class T>
 typename std::vector<T>::reverse_iterator custom_vector<T>::rbegin ( void )
{{
 return this ->m_vector .rbegin (  );
}}

 template<class T>
 typename std::vector<T>::reverse_iterator custom_vector<T>::rend ( void )
{{
 return this ->m_vector .rend (  );
}}

/*
 This returns a reference to the vector stored inside the custom_vector class.
*/
 template<class T>
 typename std::vector<T> *custom_vector<T>::get_vector ( void )
{{
 return &this ->m_vector;
}}

 int main ( int argc, char **argv )
{{
 custom_vector<char> v;
 v .push_back ( 't' );
 v .push_back ( 'e' );
 v .push_back ( 's' );
 v .push_back ( 't' );
 v .push_back ( '.' );
 printf ( "Test.\n" );

 //Show that custom_vector::m_safety_node is defaulted to 0.
 v .show_safety_node (  );

 for ( std::vector<char>::iterator it = v .begin (  );
       it != v .end (  );
       it ++
     )
      {
       printf ( "%c", *it );
      }
 printf ( "\n" );

 printf ( "Element 0: %c\n", *v [ 0 ] );
 printf ( "Element 1: %c\n", *v [ 1 ] );
 printf ( "Element 100: %0.3d\n", *v [ 100 ] ); //returns a reference to custom_vector::m_safety_node.
 printf ( "Element -1: %0.3d\n", *v [ -1 ] ); //returns a reference to custom_vector::m_safety_node.

 //Show that custom_vector::m_safety_node is defaulted to 0.
 v .show_safety_node (  );

 printf ( "Attempting *v[0] = 'x'\n" );
 *v [ 0 ] = 'x'; //v[0] returns &(char) 't' (a reference to the value 't')
 printf ( "%c\n", *v [ 0 ] );

 *v [ 1 ] = 'y'; //We're reassigning the value at the returned index by manipulating the returned reference.
 printf ( "%c\n", *v [ 1 ] );

 if ( v [ 100 ] )
     {
      *v [ 100 ] = 'z';
      printf ( "%c\n", *v [ 5 ] ); //test.z
     }
 else
      printf ( "The subscript was out of bounds.\n" );

 //Show that we set custom_vector::m_safety_node to 'z'.
 v .show_safety_node (  );

 printf ( "" );
 for ( std::vector<char>::reverse_iterator it = v .rbegin (  ); it != v .rend (  ); it ++ )
      {
       printf ( "%c", *it );
      }
 printf ( "\n" );

 //Same as custom_vector_iterator<char> it = custom_vector_iterator<char> ( v .get_vector (  ) );
 for ( custom_vector_iterator<char> it ( v .get_vector (  ) ); ++ it; )
       printf ( "Test: [%c]\n", *it );

 printf ( "\n" );
 for ( custom_vector_iterator<char> it ( v .get_vector (  ) ); it ++; )
       printf ( "Test: [%c]\n", *it );

 //custom_vector_iterator_test (  );

 return 0;
}}
