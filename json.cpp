/*
 Created on 2020-07-03 by Jacob Bethany
 Purpose: To make converting structured data into JSON easy with c++.
 Last modification (2022-08-14):
   -I've made a new overload of the VARIABLE::parse function that takes a variable
   reference as a parameter and returns a bool that indicates success or failure.
   I didn't like that I was returning a new instance of the VARIABLE class that
   had to be freed via delete.
   See: bool VARIABLE::parse ( VARIABLE &, const char * );
   -Also, I've made it so that the types int64, double, and string can be parsed
   as VARIABLE json objects in and of themselves. (Previously, only types nested
   within [] and {} could be parsed.)

 Remaining tasks:
 [ ] Add std::string VARIABLE::get_last_error ( void ).
 [ ] Make it so that add/set/operator = # isn't ambiguous.
 I'd like for all integer types to be understood to be int64_t without
 explicit casting. (I don't want to achieve this by making overloads for int8_t,
 int16_t, int32_t, uint8_t, ...etc.)

 [X] Add functionality to load from a JSON from a file into the VARIABLE class.
 [X] Add string escaping to the to_json method.
 [X] Subscript overloading. (New keys are created for objects as needed,
 and VARIABLE_TYPE_INVALID variables are set to VARIABLE_TYPE_OBJECT,
 if accessed with a const char * key; arrays accessed by subscript
 will not have keys added, if the subscript is out of bounds and
 improperly typed variables will simply return false (including
 VARIABLE_TYPE_INVALID).)
 [X] Assignment overloading. These are the same as VARIABLE::set in
 all cases, however, only NEW_ARRAY and NEW_OBJECT have been used in
 testing it.

 [C] Add get_integer, get_double, and get_string methods to get values
 from the VARIABLE. Conversions should happen implicitly, as needed.
 Note: Arrays and objects should return 0, 0.0f, and
 "Array ( # of elements )" and "Object ( # of unique keys )," respectively.

 [X] Add two "at" methods and subscript overloads.
 VARIABLE::at ( uint64_t ui64_index )
 VARIABLE::at ( const char *sz_key )
 This way, I can do the following:
 VARIABLE obj = VARIABLE::empty_object (  );
 obj .add ( "some_array", VARIABLE::empty_array (  ) );
 obj .at ( "some_array" ) .add ( 69 ); //If the key doesn't already exist, it will be created with the object type.

 Also:
 VARIABLE obj = VARIABLE::empty_array (  );
 obj .add ( 420 );
 obj .add ( VARIABLE::empty_object (  ) );
 obj .at ( 1 ) .add ( "some_key", "some_value" ); //out of bounds and array type checks would be implicit.

 This will be much simpler than the current method of defining an
 array of variables, filling them, then linking them together within
 the final object.
*/
 #include "json.h"

/*
 This will return false, if the type is not VARIABLE_TYPE_ARRAY
 and/or if the subscript is out of bounds.
*/
 VARIABLE &VARIABLE::operator[] ( uint64_t ui64_index )
{{
 if ( this ->m_ul_type != VARIABLE_TYPE_ARRAY )
      return *this;

 std::vector<VARIABLE *> *lp_vector = (std::vector<VARIABLE *> *) this ->m_lpv_data;
 if ( ! lp_vector )
      return *this;

 if ( lp_vector ->size (  ) <= ui64_index )
      return *this;

 return (*lp_vector ->at ( ui64_index ));
}}

/*
 If the variable type is invalid, it will be set to an object type.
 If the key for which we're looking doesn't already exist, it will
 be added then returned.
*/
/*
 VARIABLE &VARIABLE::operator[] ( const char *sz_key )
{{
 //If this is an invalid variable type, turn it into an object.
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      this ->m_lpv_data = (void *) new std::map<std::string, VARIABLE *> (  );
      this ->m_ul_type = VARIABLE_TYPE_OBJECT;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_OBJECT )
      return *this;

 std::map<std::string, VARIABLE *> *lp_map = (std::map<std::string, VARIABLE *> *) this ->m_lpv_data;
 if ( ! lp_map )
      return *this;

 //If the key doesn't already exist, add it.
 if ( lp_map ->find ( sz_key ) == lp_map ->end (  ) )
      (*lp_map) [ sz_key ] = new VARIABLE (  );

 return *((*lp_map) [ sz_key ]);
}}
*/
 VARIABLE &VARIABLE::operator[] ( std::string key_string )
{{
 //If this is an invalid variable type, turn it into an object.
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      this ->m_lpv_data = (void *) new std::map<std::string, VARIABLE *> (  );
      this ->m_ul_type = VARIABLE_TYPE_OBJECT;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_OBJECT )
      return *this;

 std::map<std::string, VARIABLE *> *lp_map = (std::map<std::string, VARIABLE *> *) this ->m_lpv_data;
 if ( ! lp_map )
      return *this;

 //If the key doesn't already exist, add it.
 if ( lp_map ->find ( key_string ) == lp_map ->end (  ) )
      (*lp_map) [ key_string ] = new VARIABLE (  );

 return *((*lp_map) [ key_string ]);
}}

/*
 This will return false, if the type is not VARIABLE_TYPE_ARRAY
 and/or if the subscript is out of bounds.
*/
 VARIABLE *VARIABLE::at ( uint64_t ui64_index )
{{
 if ( this ->m_ul_type != VARIABLE_TYPE_ARRAY )
      return (VARIABLE *) this;

 std::vector<VARIABLE *> *lp_vector = (std::vector<VARIABLE *> *) this ->m_lpv_data;

 //If the stored pointer is invalid, return a dummy.
 if ( ! lp_vector )
      return (VARIABLE *) this;

 //If the specified index is out of bounds, return a dummy.
 if ( lp_vector ->size (  ) <= ui64_index )
      return (VARIABLE *) this;

 return lp_vector ->at ( ui64_index );
}}

/*
 If the variable type is invalid, it will be set to an object type.
 If the key for which we're looking doesn't already exist, it will
 be added then returned.
*/
 VARIABLE *VARIABLE::at ( const char *sz_key )
{{
 //If this is an invalid variable type, turn it into an object.
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      this ->m_lpv_data = (void *) new std::map<std::string, VARIABLE *> (  );
      this ->m_ul_type = VARIABLE_TYPE_OBJECT;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_OBJECT )
      return (VARIABLE *) this;

 std::map<std::string, VARIABLE *> *lp_map = (std::map<std::string, VARIABLE *> *) this ->m_lpv_data;

 //If the item for which the user is looking doesn't exist, add a new key.
 std::map<std::string, VARIABLE *>::iterator it = lp_map ->find ( sz_key );
 if ( it == lp_map ->end (  ) )
      (*lp_map) [ sz_key ] = new VARIABLE (  ); //assign a new empty variable.

 //Othewise, return the value referenced by this key.
 return (*lp_map) [ sz_key ];
}}

/*
 If the variable type is invalid, it will be set to an object type.
 If the key for which we're looking doesn't already exist, it will
 be added then returned.
*/
 VARIABLE *VARIABLE::at ( std::string key_string )
{{
 return this ->at ( key_string .c_str (  ) );
}}

/*
 This will reset the variable to be blank, like with an empty constructor having been called.
*/
 void VARIABLE::clear ( void )
{{
 if ( ! this ->m_lpv_data )
     {
      this ->m_ul_type = VARIABLE_TYPE_INVALID;
      return ;
     }

 if ( this ->m_ul_type == VARIABLE_TYPE_OBJECT )
     {
      std::map<std::string, VARIABLE *> *lp_map = (std::map<std::string, VARIABLE *> *) this ->m_lpv_data;
      for ( std::map<std::string, VARIABLE *>::iterator map_iterator = lp_map ->begin (  );
            map_iterator != lp_map ->end (  );
            map_iterator ++
          )
           {
            delete map_iterator ->second;
           }

      delete lp_map;
     }
 else if ( this ->m_ul_type == VARIABLE_TYPE_ARRAY )
     {
      std::vector<VARIABLE *> *lp_vector = (std::vector<VARIABLE *> *) this ->m_lpv_data;
      for ( std::vector<VARIABLE *>::iterator vector_iterator = lp_vector ->begin (  );
            vector_iterator != lp_vector ->end (  );
            vector_iterator ++
          )
           {
            delete *vector_iterator;
           }

      delete lp_vector;
     }
 else if ( this ->m_ul_type == VARIABLE_TYPE_INT64 )
      delete (int64_t *) this ->m_lpv_data;
 else if ( this ->m_ul_type == VARIABLE_TYPE_DOUBLE )
      delete (double *) this ->m_lpv_data;
 else if ( this ->m_ul_type == VARIABLE_TYPE_STRING )
      delete (std::string *) this ->m_lpv_data;

 this ->m_lpv_data = 0;
 this ->m_ul_type = VARIABLE_TYPE_INVALID;
}}

/*
 The destructor will free all contents recursively, as needed.
*/
 VARIABLE::~VARIABLE (  )
{{
 //Let's ensure that we're properly handling recursive deconstruction and avoiding memory leaks.
 if ( JSON_DEBUG_MODE )
      printf ( "Deconstructor: \"%s\"\n", this ->get_string (  ) .c_str (  ) );

 this ->clear (  );
}}

/*
 This will return the type of the variable current set within the class.
 See: #define VARIABLE_TYPE_
*/
 uint32_t VARIABLE::get_type ( void )
{{
 return this ->m_ul_type;
}}

#if ( ! JSON_ENFORCE_SAFE_USAGE )
//This will create a new instance of a VARIABLE class with an empty object (map).
 VARIABLE *VARIABLE::new_object ( void )
{{
 return new VARIABLE ( NEW_OBJECT );
}}

//This will create a new instance of a VARIABLE class with an empty array (vector).
 VARIABLE *VARIABLE::new_array ( void )
{{
 return new VARIABLE ( NEW_ARRAY );
}}

//This will create a locally scoped instance of a VARIABLE class with an empty object (map).
 VARIABLE VARIABLE::empty_object ( void )
{{
 return VARIABLE ( NEW_OBJECT );
}}

//This will create a locally scoped instance of a VARIABLE class with an empty array (vector).
 VARIABLE VARIABLE::empty_array ( void )
{{
 return VARIABLE ( NEW_ARRAY );
}}
#endif

//The empty constructor will set the type to invalid.
//This will allow a value to be assigned later with VARIABLE::at and VARIABLE::add (the vector/array version).
 VARIABLE::VARIABLE ( void )
{{
 this ->m_lpv_data = 0;
 this ->m_ul_type = VARIABLE_TYPE_INVALID;
}}

//This will instantiate the class with a string type and copy of the passed string stored in this ->m_lpv_data.
 VARIABLE::VARIABLE ( std::string str )
{{
 this ->m_lpv_data = (void *) new std::string ( str );
 this ->m_ul_type = VARIABLE_TYPE_STRING;
}}

//This will instantiate the class with a int64_t type and copy of the passed integer stored in this ->m_lpv_data.
 VARIABLE::VARIABLE ( int64_t i64_x )
{{
 this ->m_lpv_data = (void *) new int64_t;
 *((int64_t *) this ->m_lpv_data) = i64_x;
 this ->m_ul_type = VARIABLE_TYPE_INT64;
}}

 VARIABLE::VARIABLE ( double dbl_x )
{{
 this ->m_lpv_data = (void *) new double;
 *((double *) this ->m_lpv_data) = dbl_x;
 this ->m_ul_type = VARIABLE_TYPE_DOUBLE;
}}

//This will instantiate the class with an array type and copy of
//the passed vector stored in this ->m_lpv_data, or an empty vector,
//if the passed reference is null.
 VARIABLE::VARIABLE ( std::vector<VARIABLE *> *variable_array )
{{
 std::vector<VARIABLE *> *v;
 if ( JSON_DEBUG_MODE )
      printf ( "new vector: %p\n", variable_array );

 if ( variable_array )
      v = new std::vector<VARIABLE *> ( *variable_array );
 else v = new std::vector<VARIABLE *> (  );

 this ->m_lpv_data = (void *) v;
 this ->m_ul_type = VARIABLE_TYPE_ARRAY;
}}

//This will instantiate the class with an object type and a copy of
//the passed map, or an empty map, if it is null.
 VARIABLE::VARIABLE ( std::map<std::string, VARIABLE *> *variable_map )
{{
 std::map<std::string, VARIABLE *> *m;

 if ( JSON_DEBUG_MODE )
      printf ( "new map: %p\n", variable_map );

 if ( variable_map )
      m = new std::map<std::string, VARIABLE *> ( *variable_map );
 else m = new std::map<std::string, VARIABLE *> (  );

 this ->m_lpv_data = (void *) m;
 this ->m_ul_type = VARIABLE_TYPE_OBJECT;
}}

//This will return a std::string of i64_spaces if i64_spaces is >= 0.
 std::string VARIABLE::get_padding ( int64_t i64_spaces )
{{
 std::string str;
 for ( int64_t i64_i = 0; i64_i < i64_spaces; i64_i ++ )
       str .append ( " " );

 return str;
}}

/*
 This will convert the VARIABLE class into a JSON string, recursively,
 with K&R spacing.
*/
 std::string VARIABLE::to_json ( int64_t i64_depth )
{{
 //std::string s;
 char sz_string [ 255 ] = { 0 };

 if ( this ->m_ul_type == VARIABLE_TYPE_OBJECT )
     {
      //foreach of the variables within this map.
      std::map<std::string, VARIABLE *> *lp_map = (std::map<std::string, VARIABLE *> *) this ->m_lpv_data;
      std::string combined_string = "{\r\n" + get_padding ( i64_depth );
      for ( std::map<std::string, VARIABLE *>::iterator variable_iterator = lp_map ->begin (  );
            variable_iterator != lp_map ->end (  );
            variable_iterator ++
          )
           {
            if ( variable_iterator != lp_map ->begin (  ) )
                {
                 combined_string .append ( ",\r\n" );
                 combined_string .append ( get_padding ( i64_depth ) );
                }

            combined_string .append ( "\"" );
            combined_string .append ( //The key string needs to be escaped, too!
              VARIABLE::get_escaped_string ( variable_iterator ->first )
            );
            combined_string .append ( "\" : " ); //the quotations will be added, automatically, by to_json, if needed.
            combined_string .append ( variable_iterator ->second ->to_json ( i64_depth+2 ) );
           }

      combined_string .append ( "\r\n" );
      combined_string .append ( get_padding ( i64_depth-2 ) );
      combined_string .append ( "}" );

      return combined_string;
     }
 else if ( this ->m_ul_type == VARIABLE_TYPE_ARRAY )
     {
      //foreach of the variables within this map.
      std::vector<VARIABLE *> *lp_vector = (std::vector<VARIABLE *> *) this ->m_lpv_data;
      std::string combined_string = "[\r\n" + get_padding ( i64_depth );
      for ( std::vector<VARIABLE *>::iterator variable_iterator = lp_vector ->begin (  );
            variable_iterator != lp_vector ->end (  );
            variable_iterator ++
          )
           {
            if ( variable_iterator != lp_vector ->begin (  ) )
                {
                 combined_string .append ( ",\r\n" );
                 combined_string .append ( get_padding ( i64_depth ) );
                }

            //Note: We don't always want the quotations around the value, since JSON only uses quotes around strings and keys.
            combined_string .append ( (*variable_iterator) ->to_json ( i64_depth+2 ) );
           }

      combined_string .append ( "\r\n" );
      combined_string .append ( get_padding ( i64_depth-2 ) );
      combined_string .append ( "]" );

      return combined_string;
     }
 else if ( this ->m_ul_type == VARIABLE_TYPE_INT64 )
     {
      //%llu would be uint64_t, but we're dealing with a int64_t (signed).
      snprintf ( sz_string, 255, "%lld", *((int64_t *) this ->m_lpv_data) );
      return std::string ( sz_string );
     }
 else if ( this ->m_ul_type == VARIABLE_TYPE_DOUBLE )
     {
      snprintf ( sz_string, 255, "%f", *((double *) this ->m_lpv_data) );
      return std::string ( sz_string );
     }
 else if ( this ->m_ul_type == VARIABLE_TYPE_STRING )
     {
      return std::string ( "\"" ) +
             VARIABLE::get_escaped_string (
               *((std::string *) this ->m_lpv_data)
             ) +
             std::string ( "\"" );
     }

 return std::string ( "unsupported type." );
}}

/*
 This is a simplified overload to get the JSON string from the VARIABLE class.
*/
 std::string VARIABLE::to_json ( void )
{{
 return this ->to_json ( 2 );
}}

/*
 This is a simplified overload to get the JSON string from the VARIABLE class
 that is also written as concisely as possible.
*/
 std::string VARIABLE::to_minimal_json ( void )
{{
 return VARIABLE::get_minimal_json_string ( this ->to_json (  ) .c_str (  ) );
}}

/*
 This will escape quotations and backslashes for creating valid JSON.
*/
 std::string VARIABLE::get_escaped_string ( std::string str )
{{
 return VARIABLE::get_escaped_string ( &str );
}}

/*
 This will copy the passed string into a new std::string but
 with all backslashes and quotes prefixed with backslashes.
*/
 std::string VARIABLE::get_escaped_string ( std::string *str )
{{
 std::string escaped_string;
 if ( ! str )
      return escaped_string;

 const char *p = (const char *) str ->c_str (  ), *e;
 uint64_t ui64_len = strlen ( p );
 static char sz_character [ 2 ] = { 0 };

 for ( e = p+ui64_len; p != e; p ++ ) {
   if ( *p == '\\' || *p == '\"' ) {
     escaped_string .append ( "\\" );
   }

   *sz_character = *p;
   escaped_string .append ( sz_character );
 }

 return escaped_string;
}}

/*
 This will add a new string variable to an object variable, given some key.
*/
 unsigned char VARIABLE::add ( const char *sz_key, std::string str )
{{
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      this ->m_lpv_data = (void *) new std::map<std::string, VARIABLE *> (  );
      this ->m_ul_type = VARIABLE_TYPE_OBJECT;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_OBJECT )
      return 0;

 //printf ( "Adding this to a map: [%s] => [%s]\n", sz_key, value .to_json (  ) .c_str (  ) );
 std::map<std::string, VARIABLE *> *lp_map = (std::map<std::string, VARIABLE *> *) this ->m_lpv_data;
 (*lp_map) [ sz_key ] = new VARIABLE ( str );

 return 1;
}}

/*
 This will add a new int64 variable to an object variable, given some key.
*/
 unsigned char VARIABLE::add ( const char *sz_key, int64_t x )
{{
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      this ->m_lpv_data = (void *) new std::map<std::string, VARIABLE *> (  );
      this ->m_ul_type = VARIABLE_TYPE_OBJECT;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_OBJECT )
      return 0;

 //printf ( "Adding this to a map: [%s] => [%s]\n", sz_key, value .to_json (  ) .c_str (  ) );
 std::map<std::string, VARIABLE *> *lp_map = (std::map<std::string, VARIABLE *> *) this ->m_lpv_data;
 (*lp_map) [ sz_key ] = new VARIABLE ( x );

 return 1;
}}

/*
 This will add a new double variable to an object variable, given some key.
*/
 unsigned char VARIABLE::add ( const char *sz_key, double x )
{{
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      this ->m_lpv_data = (void *) new std::map<std::string, VARIABLE *> (  );
      this ->m_ul_type = VARIABLE_TYPE_OBJECT;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_OBJECT )
      return 0;

 //printf ( "Adding this to a map: [%s] => [%s]\n", sz_key, value .to_json (  ) .c_str (  ) );
 std::map<std::string, VARIABLE *> *lp_map = (std::map<std::string, VARIABLE *> *) this ->m_lpv_data;
 (*lp_map) [ sz_key ] = new VARIABLE ( x );

 return 1;
}}

/*
 This will add a new array variable to an object variable, given some key.
*/
 unsigned char VARIABLE::add ( const char *sz_key, std::vector<VARIABLE *> *v )
{{
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      printf ( "A blank variable is becoming an object.\n" );

      this ->m_lpv_data = (void *) new std::map<std::string, VARIABLE *> (  );
      this ->m_ul_type = VARIABLE_TYPE_OBJECT;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_OBJECT )
      return 0;

 std::map<std::string, VARIABLE *> *lp_map = (std::map<std::string, VARIABLE *> *) this ->m_lpv_data;
 (*lp_map) [ sz_key ] = new VARIABLE ( v );

 return 1;
}}

/*
 This will add a new object variable to an object variable, given some key.
*/
 unsigned char VARIABLE::add ( const char *sz_key, std::map<std::string, VARIABLE *> *m )
{{
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      this ->m_lpv_data = (void *) new std::map<std::string, VARIABLE *> (  );
      this ->m_ul_type = VARIABLE_TYPE_OBJECT;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_OBJECT )
      return 0;

 std::map<std::string, VARIABLE *> *lp_map = (std::map<std::string, VARIABLE *> *) this ->m_lpv_data;
 (*lp_map) [ sz_key ] = new VARIABLE ( m );

 return 1;
}}

/*
 This will add a new VARIABLE variable to an object variable, given some key.
 This is used internally to attach object and array variables to each other, recursively.
 See VARIABLE::get_copy for more details.

 Note: Do not use like this: VARIABLE::add ( "some_key", VARIABLE::new_array() or VARIABLE::new_object() ).
 This would cause a memory leak.
 Prefer VARIABLE::add ( "some_key", NEW_OBJECT or NEW_ARRAY ), if you don't want to define a variable and reference it.
*/
 unsigned char VARIABLE::add ( const char *sz_key, VARIABLE *lp_variable )
{{
 //If a bad reference was passed.
 if ( ! lp_variable )
      return 0;

 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      printf ( "An invalid variable type is becoming an object.\n" );
#if ( JSON_ENFORCE_SAFE_USAGE )
      this ->m_lpv_data = (void *) new VARIABLE ( NEW_OBJECT );
#else
      this ->m_lpv_data = (void *) VARIABLE::new_object (  );
#endif
      this ->m_ul_type = VARIABLE_TYPE_OBJECT;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_OBJECT )
      return 0;

 //printf ( "Adding this to a map: [%s] => [%s]\n", sz_key, value .to_json (  ) .c_str (  ) );
 std::map<std::string, VARIABLE *> *lp_map = (std::map<std::string, VARIABLE *> *) this ->m_lpv_data;
 //(*lp_map) [ sz_key ] = lp_variable; //we must copy this so that it doesn't get freed multiple times,
                                       //if it's a local variable being passed by reference.
 //This way is always safe, albeit less memory efficient.
 (*lp_map) [ sz_key ] = lp_variable ->get_copy (  );

 return 1;
}}

/*
 This will make a deep copy of a VARIABLE.
 If the type is object or array, it will create a new VARIABLE of the same type
 and recursively add all primitives, arrays, and objects contained within the
 array or object and the object or array's arrays and/or objects, and so forth.
*/
 VARIABLE *VARIABLE::get_copy ( void )
{{
 if ( this ->m_ul_type == VARIABLE_TYPE_INT64 )
      return new VARIABLE ( *((int64_t *) this ->m_lpv_data) );

 if ( this ->m_ul_type == VARIABLE_TYPE_DOUBLE )
      return new VARIABLE ( *((double *) this ->m_lpv_data) );

 if ( this ->m_ul_type == VARIABLE_TYPE_STRING )
      return new VARIABLE ( *((std::string *) this ->m_lpv_data) ); //We don't need to escape the string, since it will already be escaped.

 if ( this ->m_ul_type == VARIABLE_TYPE_ARRAY )
     {
      VARIABLE *v = new VARIABLE ( NEW_ARRAY );
      std::vector<VARIABLE *> *lp_vector = (std::vector<VARIABLE *> *) this ->m_lpv_data;
      for ( std::vector<VARIABLE *>::iterator vector_iterator = lp_vector ->begin (  );
            vector_iterator != lp_vector ->end (  );
            vector_iterator ++
          )
           {
            if ( JSON_DEBUG_MODE )
                 printf ( "Copying item #%d from a vector.\n", (vector_iterator - lp_vector ->begin (  )) + 1 );
            v ->add ( (*vector_iterator) ->get_copy (  ) );
           }
      return v;
     }

 if ( this ->m_ul_type == VARIABLE_TYPE_OBJECT )
     {
      VARIABLE *m = new VARIABLE ( NEW_OBJECT );
      std::map<std::string, VARIABLE *> *lp_map = (std::map<std::string, VARIABLE *> *) this ->m_lpv_data;
      uint64_t ui64_i = 0;
      for ( std::map<std::string, VARIABLE *>::iterator map_iterator = lp_map ->begin (  );
            map_iterator != lp_map ->end (  );
            map_iterator ++
          )
           {
            if ( JSON_DEBUG_MODE )
                {
                 printf (
                   "Copying item #%llu from a map.\n",
                   ++ui64_i
                 );
                }

            m ->add ( map_iterator ->first .c_str (  ), map_iterator ->second ->get_copy (  ) );
           }

      return m;
     }

 if ( JSON_DEBUG_MODE )
      printf ( "returning a string variable because the type is unknown.\n" );

 return new VARIABLE ( "unknown types can't be copied." );
}}

//add a string element to an array or assign a value to invalid variable.
 unsigned char VARIABLE::add ( std::string str )
{{
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      this ->m_lpv_data = (void *) new std::vector<VARIABLE *> (  );
      this ->m_ul_type = VARIABLE_TYPE_ARRAY;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_ARRAY )
      return 0;

 if ( JSON_DEBUG_MODE )
      printf ( "Adding this to a vector: [%s]\n", str .c_str (  ) );

 std::vector<VARIABLE *> *lp_vector = (std::vector<VARIABLE *> *) this ->m_lpv_data;
 lp_vector ->push_back ( new VARIABLE ( str ) );

 return 1;
}}

//add an integer element to an array or assign a value to invalid variable.
 unsigned char VARIABLE::add ( int64_t x )
{{
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      this ->m_lpv_data = (void *) new std::vector<VARIABLE *> (  );
      this ->m_ul_type = VARIABLE_TYPE_ARRAY;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_ARRAY )
      return 0;

 if ( JSON_DEBUG_MODE )
      printf ( "Adding this to a vector: [%lld]\n", x ); //%llu would be uint64_t; this is signed, instead.

 std::vector<VARIABLE *> *lp_vector = (std::vector<VARIABLE *> *) this ->m_lpv_data;
 lp_vector ->push_back ( new VARIABLE ( x ) );

 return 1;
}}

//add a double element to an array or assign a value to invalid variable.
 unsigned char VARIABLE::add ( double x )
{{
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      this ->m_lpv_data = (void *) new std::vector<VARIABLE *> (  );
      this ->m_ul_type = VARIABLE_TYPE_ARRAY;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_ARRAY )
      return 0;

 if ( JSON_DEBUG_MODE )
      printf ( "Adding this to a vector: [%f]\n", x );

 std::vector<VARIABLE *> *lp_vector = (std::vector<VARIABLE *> *) this ->m_lpv_data;
 lp_vector ->push_back ( new VARIABLE ( x ) );

 return 1;
}}

//add an array element to an array or assign a value to invalid variable.
 unsigned char VARIABLE::add ( std::vector<VARIABLE *> *v )
{{
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      this ->m_lpv_data = (void *) new std::vector<VARIABLE *> (  );
      this ->m_ul_type = VARIABLE_TYPE_ARRAY;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_ARRAY )
      return 0;

 if ( JSON_DEBUG_MODE )
      printf ( "Adding a vector to a vector: [%p]\n", v );

 std::vector<VARIABLE *> *lp_vector = (std::vector<VARIABLE *> *) this ->m_lpv_data;
 lp_vector ->push_back ( new VARIABLE ( v ) );

 return 1;
}}

//add an object element to an array or assign a value to invalid variable.
 unsigned char VARIABLE::add ( std::map<std::string, VARIABLE *> *m )
{{
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      this ->m_lpv_data = (void *) new std::vector<VARIABLE *> (  );
      this ->m_ul_type = VARIABLE_TYPE_ARRAY;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_ARRAY )
      return 0;

 if ( JSON_DEBUG_MODE )
      printf ( "Adding a map to a vector: [%p]\n", m );

 std::vector<VARIABLE *> *lp_vector = (std::vector<VARIABLE *> *) this ->m_lpv_data;
 lp_vector ->push_back ( new VARIABLE ( m ) );

 return 1;
}}

//Add an VARIABLE element to an array or assign a value to invalid variable.
//This is good for attaching objects and arrays to an array variable.
//See VARIABLE::get_copy for more information.
 unsigned char VARIABLE::add ( VARIABLE *lp_variable )
{{
 if ( ! lp_variable )
      return 0;

 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
     {
      std::vector<VARIABLE *> *lp_vector = (std::vector<VARIABLE *> *) new std::vector<VARIABLE *> (  );
      lp_vector ->push_back ( lp_variable );
      this ->m_lpv_data = (void *) lp_vector;
      this ->m_ul_type = VARIABLE_TYPE_ARRAY;
     }
 else if ( this ->m_ul_type != VARIABLE_TYPE_ARRAY )
      return 0;

 if ( JSON_DEBUG_MODE )
      printf ( "Adding a variable to a vector: [%s]\n", lp_variable ->to_json (  ) .c_str (  ) );

 std::vector<VARIABLE *> *lp_vector = (std::vector<VARIABLE *> *) this ->m_lpv_data;
 //lp_vector ->push_back ( lp_variable );
 lp_vector ->push_back ( lp_variable ->get_copy (  ) );

 return 1;
}}

//This will assign the current VARIABLE instance to the type and value of the passed string.
 unsigned char VARIABLE::set ( std::string str )
{{
 //If this needs to be reset before assigning it to another value, do that.
 if ( this ->m_ul_type != VARIABLE_TYPE_INVALID )
      this ->clear (  );

 this ->m_lpv_data = (void *) new std::string ( str );
 this ->m_ul_type = VARIABLE_TYPE_STRING;

 return 1;
}}

//This will assign the current VARIABLE instance to the type and value of the passed integer.
 unsigned char VARIABLE::set ( int64_t x )
{{
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
      this ->clear (  );

 this ->m_lpv_data = (void *) new int64_t ( x );
 this ->m_ul_type = VARIABLE_TYPE_INT64;

 return 1;
}}

//This will assign the current VARIABLE instance to the type and value of the passed decimal.
 unsigned char VARIABLE::set ( double x )
{{
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
      this ->clear (  );

 this ->m_lpv_data = (void *) new double ( x );
 this ->m_ul_type = VARIABLE_TYPE_DOUBLE;

 return 1;
}}

//This will assign the current VARIABLE instance to an array type and copy the vector, if it has any contents.
 unsigned char VARIABLE::set ( std::vector<VARIABLE *> *v )
{{
 if ( this ->m_ul_type == VARIABLE_TYPE_INVALID )
      this ->clear (  );

 if ( v )
      this ->m_lpv_data = (void *) new std::vector<VARIABLE *> ( *v );
 else
      this ->m_lpv_data = (void *) new std::vector<VARIABLE *> (  );

 this ->m_ul_type = VARIABLE_TYPE_ARRAY;

 return 1;
}}

//This will assign the current VARIABLE instance to an object type and copy the map, if it has any contents.
 unsigned char VARIABLE::set ( std::map<std::string, VARIABLE *> *m )
{{
 if ( this ->m_ul_type != VARIABLE_TYPE_INVALID )
      this ->clear (  );

 this ->m_lpv_data = (void *) new std::map<std::string, VARIABLE *> ( *m );
 this ->m_ul_type = VARIABLE_TYPE_OBJECT;

 return 1;
}}

//This will do a deep-copy of the passed variable, assigning the current class instance to its type and contents.
 unsigned char VARIABLE::set ( VARIABLE *lp_variable )
{{
 if ( ! lp_variable )
      return 0;

 //If this needs to be reset before assigning it to another value, do that.
 if ( this ->m_ul_type != VARIABLE_TYPE_INVALID )
      this ->clear (  );

 this ->m_lpv_data = (void *) lp_variable ->get_copy (  );
 this ->m_ul_type = lp_variable ->get_type (  );

 return 1;
}}

//These assignment overloads are exactly the same as the VARIABLE::set methods.
 unsigned char VARIABLE::operator= ( std::string str )
{{
 return this ->set ( str );
}}

 unsigned char VARIABLE::operator= ( int64_t x )
{{
 return this ->set ( x );
}}

 unsigned char VARIABLE::operator= ( double x )
{{
 return this ->set ( x );
}}

 unsigned char VARIABLE::operator= ( std::vector<VARIABLE *> *v )
{{
 return this ->set ( v );
}}

 unsigned char VARIABLE::operator= ( std::map<std::string, VARIABLE *> *m )
{{
 return this ->set ( m );
}}

 unsigned char VARIABLE::operator= ( VARIABLE *lp_variable )
{{
 return this ->set ( lp_variable );
}}

 double VARIABLE::get_double ( void )
{{
 const char sz_string [ 2048 ] = { 0 };

 //Handle any conversions that we can.
 switch ( this ->m_ul_type ) {

   case VARIABLE_TYPE_DOUBLE:
   return *((double *) this ->m_lpv_data);

   case VARIABLE_TYPE_INT64:
   return (double) *((int64_t *) this ->m_lpv_data);

   case VARIABLE_TYPE_STRING:
     //If this can't be converted into an integer, return zero.
     try {
       return stod ( *((std::string *) this ->m_lpv_data) );
     } catch ( ... ) {
       return 0;
     } //catch
 }

 return std::numeric_limits<double>::quiet_NaN (  );
}}

 int64_t VARIABLE::get_integer ( void )
{{
 const char sz_string [ 2048 ] = { 0 };

 //Handle any conversions that we can.
 switch ( this ->m_ul_type ) {

   case VARIABLE_TYPE_DOUBLE:
   return (int64_t) *((double *) this ->m_lpv_data);

   case VARIABLE_TYPE_INT64:
   return *((int64_t *) this ->m_lpv_data);

   case VARIABLE_TYPE_STRING:
   {
     //If this can't be converted into an integer, return zero.
     try {
       return stoll ( *((std::string *) this ->m_lpv_data) );
     } catch ( ... ) { //std::invalid_argument might be thrown.
       return 0;
     } //catch
   } //case string
 }

 return 0;
}}

/*
 This will get the current variable's value as a string, if possible.
 Integers and doubles will be formatted as ascii string representations of the numbers;
 Strings will be returned verbatim;
 arrays and object's will show the type and number of elements and keys, resepectively;
 invalid and known types will return a string describing such.
*/
 std::string VARIABLE::get_string ( void )
{{
 static char sz_string [ 2048 ] = { 0 };

 //Handle any conversions that we can.
 switch ( this ->m_ul_type ) {

   case VARIABLE_TYPE_DOUBLE:
     {
      std::string s;
      snprintf ( sz_string, 2048, "%f", *((double *) this ->m_lpv_data) );
      s = sz_string;
      return s;
     }

   case VARIABLE_TYPE_INT64:
     {
      std::string s;
      snprintf ( sz_string, 2048, "%lld", *((int64_t *) this ->m_lpv_data) );
      s = sz_string;
      return s;
     }

   case VARIABLE_TYPE_STRING:
     {
      return *((std::string *) this ->m_lpv_data);
     }

   case VARIABLE_TYPE_ARRAY:
     {
      std::string s;
      snprintf ( sz_string, 2048, "Array ( %d )", ((std::vector<VARIABLE *> *) this ->m_lpv_data) ->size (  ) );
      s = sz_string;
      return s;
     }

   case VARIABLE_TYPE_OBJECT:
     {
      std::string s;
      snprintf ( sz_string, 2048, "Object ( %d )", ((std::map<std::string, VARIABLE *> *) this ->m_lpv_data) ->size (  ) );
      s = sz_string;

      return s;
     }

   case VARIABLE_TYPE_INVALID:
     {
      return std::string ( "{empty (invalid) variable object}" );
     }
 }

 return std::string ( "Unknown type." );
}}

/*
 This will put the passed JSON into the minimal characters as a std::string object.
*/
 std::string VARIABLE::get_minimal_json_string ( const char *sz_json_string )
{{
 std::string minimal_json_string;
 static char sz_character [ 2 ] = { 0 };
 char *p = (char *) sz_json_string;
 unsigned char b_in_quote = 0;
 unsigned char b_last_character_was_a_backslash = 0;
 while ( *p ) {

   if ( *p == '\"' && ! b_last_character_was_a_backslash )
        b_in_quote = ! b_in_quote;

   //If we're in a quote, we can copy spaces, otherwise, if it's not a space, it can be copied
   if ( b_in_quote ||
        ( *p != ' ' && *p != '\r' && *p != '\n' && *p != '\t' )
      )
       {
        *sz_character = *p;
        minimal_json_string .append ( sz_character );
       }

   //Skip string escapes.
   else if ( *p == '\\' )
        b_last_character_was_a_backslash = 1;
   else b_last_character_was_a_backslash = 0;

   p ++;
 }

 //Return a mutable copy of the new string.
 return minimal_json_string;
}}

/*
 This will continue copying characters while ignoring the escaping backslashes
 until a non-escaped quotation mark is reached.
 The pointer passed to this function should start after the first quotation.

 If an item is parsed and there's still more content, a pointer to the position
 after the ending quotation mark will be returned.

 If no unescaped closing quotation mark can be located, an the item will be empty and
 a reference to zero will be returned.
*/
// char *VARIABLE::consume_string ( char *sz_json, std::string &item )
 std::string::iterator VARIABLE::consume_string (
   std::string::iterator json_string_start_iterator,
   std::string::iterator json_string_end_iterator,
   std::string &item
 )
{{
 if ( ! item .empty (  ) )
      item .clear (  );

 static char sz_character [ 2 ] = { 0 };
 std::string::iterator p = json_string_start_iterator;
 unsigned char b_last_character_was_a_backslash = 0;
 while ( *p ) {
//printf ( "Next consume_string character: [%c]\n", *p );
   if ( *p == '\"' && ! b_last_character_was_a_backslash )
        return p + 1;

   else if ( *p == '\\' )
       {
        if ( b_last_character_was_a_backslash )
            {
             *sz_character = *p;
             item .append ( sz_character );
            }
       }
   //If this isn't a quotation mark or a backslash, then we can copy the item verbatim.
   else {
     *sz_character = *p;
     item .append ( sz_character );
   }

   if ( *p == '\\' )
        b_last_character_was_a_backslash = 1;
   else b_last_character_was_a_backslash = 0;
   p ++;
 }

 //This should never happen unless the JSON is malformed.
 std::string error_string = std::string ( json_string_start_iterator, json_string_end_iterator );
 printf ( "No unescaped closing quotation mark could be found in the JSON string starting at:\n>>%p<<\n\n", error_string .c_str (  ) );
 return json_string_end_iterator;
}}

/*
 This will return a pointer to the first character after the parsed item.
 The passed item and type will be filled with the parsed information and type from the string
 at the starting position passed.

 Example:
 while {
   p = consume_item ( p, item, type );
   if ( p >= e ) break;
 }
*/
 //char *VARIABLE::consume_item ( char *sz_json, std::string &item, uint32_t &type )
 std::string::iterator VARIABLE::consume_item (
   std::string::iterator json_string_start_iterator,
   std::string::iterator json_string_end_iterator,
   std::string &item,
   uint32_t &type
 )
{{
 type = VARIABLE_TYPE_INVALID;

 if ( ! item.empty (  ) )
      item .clear (  );

 std::string::iterator p = json_string_start_iterator;
 if ( *p == '\"' )
     {
      type = VARIABLE_TYPE_STRING;
      return VARIABLE::consume_string ( p + 1, json_string_end_iterator, item ); //This returns a pointer to the character after the closing quotation mark.
     }

 //Check all of the characters of the current item until a comma or colon delimiter is reached.
 while ( *p && ! strchr ( ":,]}", *p ) ) {
   if ( *p == '.' )
        type = VARIABLE_TYPE_DOUBLE;

  p ++;
 }
 if ( type == VARIABLE_TYPE_INVALID )
     {
      //If this wasn't a parsable item, then we shouldn't be returning anything.
      if ( p == json_string_start_iterator )
          {
           printf ( "Error: We weren't able to identify a type, let alone parse characters for the item.\n" );
           printf ( ">>>>\n%s\n<<<<\n", p );
           return json_string_end_iterator;
          }

      type = VARIABLE_TYPE_INT64;
     }

 //Create a new string from the start to the position that we've zeroed.
 //We don't need to backup the current character, zero it, copy the substring, then replace the backed-up character, this way.
 item = std::string ( json_string_start_iterator, p );

 return p; //return a reference to the first character after the substring that we parsed.
}}

/*
 This will work like v [ "a" ] [ "b" ] [ "c" ] ...etc.,
 but with a vector that contains a, b, c, ...

 The final resulting node pointer will be returned.
*/
 VARIABLE *VARIABLE::get_node_at_key_path ( VARIABLE *v, std::vector<std::string> &key_vector )
{{
 VARIABLE *node = v;
 for ( std::vector<std::string>::iterator key_iterator = key_vector .begin (  );
       key_iterator != key_vector .end (  );
       key_iterator ++
     )
      {
       node = node ->at ( *key_iterator );
      }

 return node;
}}

/*
 This will parse some JSON and return the resulting variable.
 On failure, it will return an empty variable.
*/
 VARIABLE *VARIABLE::parse ( const char *sz_json_string )
{{
 VARIABLE *some_variable = new VARIABLE (  ); //VARIABLE_TYPE_INVALID by default.
 std::string minimal_json_string = VARIABLE::get_minimal_json_string ( sz_json_string );

 //If we're parsing an empty string.
 if ( minimal_json_string.empty() ) {
   return some_variable;
 }
 //If this is an integer or floating point number.
 if ( isdigit(minimal_json_string [ 0 ]) ) {
   if ( strchr(minimal_json_string.c_str(), '.') ) {
     *some_variable = (double) atof ( minimal_json_string.c_str() );
   }
   else {
     *some_variable = (int64_t) atoi ( minimal_json_string.c_str() );
   }
   return some_variable;
 }
 //If we're not starting with a { or [ token, and it's not starting with
 //a number, it must be a string data type.
 else if ( ! strchr ( "{[", minimal_json_string[0] ) ) {
   //Assign this to the string because it's a string type.
   //This should automatically update the type, as well.
   *some_variable = minimal_json_string.c_str();
   return some_variable;
 }

 if ( JSON_DEBUG_MODE )
      printf ( "Here's what we're parsing:\n>>>>\n%s\n<<<<\n\n", minimal_json_string .c_str (  ) );

 std::vector<std::string> key_vector;
 std::vector<uint32_t> hierarchy_vector; //is it an object ( VARIABLE_TYPE_OBJECT ) or an array ( VARIABLE_TYPE_ARRAY )?

 std::string key;
 std::string item;
 uint32_t ul_type;
 unsigned char b_need_key = 0;

 std::string::iterator p = minimal_json_string .begin (  ), e = minimal_json_string .end (  );

 while ( p < e ) {

   //We'll store the key in the key vector if and only if we've just encountered a new '{' token.
   unsigned char b_store_key = 0;

   //Skip the delimiter between elements, if we're on one.
   if ( *p == ',' )
        p ++;

   //If we've found an object.
   if ( *p == '{' ) {

     hierarchy_vector .push_back ( VARIABLE_TYPE_OBJECT );
     b_store_key = 1;
     p ++; //skip the '{' token.

   }
   else if ( *p == '[' ) {

     hierarchy_vector .push_back ( VARIABLE_TYPE_ARRAY );
     p ++; //skip the '[' token.
   }

   else if ( *p == ']' ) {

//printf ( "']' located at:\n>>>>\n%s\n<<<<\n", p );
     if ( ! hierarchy_vector .size (  ) )
         {
          printf ( "Error: A ']' or '}' token was found and we aren't within an object or array.\n" );
          break;
         }
     hierarchy_vector .pop_back (  );
     p ++;
     continue;
   }

   else if ( *p == '}' ) {
//printf ( "'}' located at:\n>>>>\n%s\n<<<<\n", p );

     if ( ! hierarchy_vector .size (  ) )
         {
          printf ( "Error: A ']' or '}' token was found and we aren't within an object or array.\n" );
          break;
         }
     hierarchy_vector .pop_back (  );

     //We also need to assign a new key for this pairing level, probably.
     if ( ! key_vector .size (  ) )
         {
          printf ( "Error: A '}' was found but we're not within an object.\n" );
          break;
         }

     if ( JSON_DEBUG_MODE )
          printf ( "Calling key_vector .pop_back (  );\n" );

     key_vector .pop_back (  );
     p ++;
     continue;
   }

   //Use the type for this level of nesting.
   if ( hierarchy_vector .size (  ) )
        b_need_key = hierarchy_vector .back (  ) == VARIABLE_TYPE_OBJECT ? 1 : 0;
   else b_need_key = 0;

   if ( b_need_key ) {
     //Get the key string.
     if ( e == (p = VARIABLE::consume_item ( p, e, key, ul_type )) )
        break;

     if ( JSON_DEBUG_MODE )
          printf ( "Here's the key of the object: \"%s\"\n", key .c_str (  ) );

     if ( *p != ':' )
         {
          printf ( "Error: There was a missing ':' delimiter in a JSON object.\n" );
          return new VARIABLE (  );
         }
     p ++; //skip the ':' token.

     //We only store keys for the starts of new objects. The pairs within them don't need to be stored.
     if ( b_store_key )
         {
          key_vector .push_back ( key );
          VARIABLE *node = VARIABLE::get_node_at_key_path ( some_variable, key_vector );
         }
     //If we don't need to store it, then we need to overwrite the key at this level.
     else key_vector .back (  ) = key;
   }

   //If we have an object or array inside of an object or array, continue from the top.
   if ( *p == '[' || *p == '{' )
        continue;


   //Get the value whether it's an array or associated with some key.
   if ( e == (p = VARIABLE::consume_item ( p, e, item, ul_type )) )
        break;

   if ( JSON_DEBUG_MODE )
        printf ( "Item: \"%s\"\n", item .c_str (  ) );

   //Ensure that the JSON string isn't malformed.
   //It's possible that the string ends after a value, if the JSON consists of a single primitive, hence the p != e exclusion.
   if ( *p != ',' && *p != '\"' && *p != '}' && *p != ']' && p != e )
       {
        printf ( "Found '%c'; expected ',', '\\\"', ']', or '}' after object key & value pair were parsed.\n", *p );
        printf ( "Here's the rest of the JSON:\n>>>>\n%s\n<<<<\n", p );
        break;
       }

   //If we're dealing with an object.
   //Show debugging information:
   if ( JSON_DEBUG_MODE )
       {
        printf ( "some_variable " );
        for ( std::vector<std::string>::iterator key_iterator = key_vector .begin (  );
              key_iterator != key_vector .end (  );
              key_iterator ++
            )
             {
              printf ( " [ \"%s\" ]", key_iterator ->c_str (  ) );
             }
        if ( b_need_key )
             printf ( " = \"%s\";\n", item .c_str (  ) );
        else printf ( " .push_back ( \"%s\" );\n", item .c_str (  ) );
       }

   //Actually add the value to the variable, now.
   VARIABLE *node = VARIABLE::get_node_at_key_path ( some_variable, key_vector );

   int64_t i64_integer;
   double dbl_double;

   //Convert the data to the proper type.
   if ( ul_type == VARIABLE_TYPE_INT64 )
       {
        //printf ( "Trying stoll ( \"%s\" ).\n", item .c_str (  ) );
        i64_integer = stoll ( item ); //stoi = 0->255, stol and stoll are bigger.

        //It's possible that the JSON that we're parsing is just a single primitive.
        if ( key_vector .size (  ) )
            {
             if ( b_need_key )
                  node ->set ( i64_integer ); //should be blank right now.
             else
                  node ->add ( i64_integer );
            }
        else
             node ->set ( i64_integer );
       }
   else if ( ul_type == VARIABLE_TYPE_DOUBLE )
       {
        dbl_double = stod ( item );

        //It's possible that the JSON that we're parsing is just a single primitive.
        if ( key_vector .size (  ) )
            {
             if ( b_need_key )
                  node ->set ( dbl_double ); //should be blank right now.
             else
                  node ->add ( dbl_double );
            }
        else
             node ->set ( dbl_double );
       }
   else if ( ul_type == VARIABLE_TYPE_STRING )
       {
        //It's possible that the JSON that we're parsing is just a single primitive.
        if ( key_vector .size (  ) )
            {
             if ( b_need_key )
                  node ->set ( item ); //should be blank right now.
             else
                  node ->add ( item );
            }
        else
             node ->set ( item );
       }

   //printf ( "Current object:\n%s\n\n", some_variable ->to_json (  ) .c_str (  ) );
 }

 return some_variable;
}}

/*
 This will parse some JSON and return the resulting variable.
 On failure, it will return an empty variable.
*/
 bool VARIABLE::parse ( VARIABLE &some_variable, const char *sz_json_string )
{{
 //Reset any lingering data from a previous parse.
 some_variable .clear (  );
 std::string minimal_json_string = VARIABLE::get_minimal_json_string ( sz_json_string );

 //If we're parsing an empty string.
 if ( minimal_json_string.empty() ) {
   return false;
 }
 //If this is an integer or floating point number.
 if ( isdigit(minimal_json_string [ 0 ]) ) {
   if ( strchr(minimal_json_string.c_str(), '.') ) {
     some_variable = (double) atof ( minimal_json_string.c_str() );
   }
   else {
     some_variable = (int64_t) atoi ( minimal_json_string.c_str() );
   }
   return true;
 }
 //If we're not starting with a { or [ token, and it's not starting with
 //a number, it must be a string data type.
 else if ( ! strchr ( "{[", minimal_json_string[0] ) ) {
   //Assign this to the string because it's a string type.
   //This should automatically update the type, as well.
   some_variable = minimal_json_string .c_str (  );
   return true;
 }

 if ( JSON_DEBUG_MODE )
      printf ( "Here's what we're parsing:\n>>>>\n%s\n<<<<\n\n", minimal_json_string .c_str (  ) );

 std::vector<std::string> key_vector;
 std::vector<uint32_t> hierarchy_vector; //is it an object ( VARIABLE_TYPE_OBJECT ) or an array ( VARIABLE_TYPE_ARRAY )?

 std::string key;
 std::string item;
 uint32_t ul_type;
 unsigned char b_need_key = 0;

 std::string::iterator p = minimal_json_string .begin (  ), e = minimal_json_string .end (  );

 while ( p < e ) {

   //We'll store the key in the key vector if and only if we've just encountered a new '{' token.
   unsigned char b_store_key = 0;

   //Skip the delimiter between elements, if we're on one.
   if ( *p == ',' )
        p ++;

   //If we've found an object.
   if ( *p == '{' ) {

     hierarchy_vector .push_back ( VARIABLE_TYPE_OBJECT );
     b_store_key = 1;
     p ++; //skip the '{' token.

   }
   else if ( *p == '[' ) {

     hierarchy_vector .push_back ( VARIABLE_TYPE_ARRAY );
     p ++; //skip the '[' token.
   }

   else if ( *p == ']' ) {

//printf ( "']' located at:\n>>>>\n%s\n<<<<\n", p );
     if ( ! hierarchy_vector .size (  ) )
         {
          printf ( "Error: A ']' or '}' token was found and we aren't within an object or array.\n" );
          break;
         }
     hierarchy_vector .pop_back (  );
     p ++;
     continue;
   }

   else if ( *p == '}' ) {
//printf ( "'}' located at:\n>>>>\n%s\n<<<<\n", p );

     if ( ! hierarchy_vector .size (  ) )
         {
          printf ( "Error: A ']' or '}' token was found and we aren't within an object or array.\n" );
          break;
         }
     hierarchy_vector .pop_back (  );

     //We also need to assign a new key for this pairing level, probably.
     if ( ! key_vector .size (  ) )
         {
          printf ( "Error: A '}' was found but we're not within an object.\n" );
          break;
         }

     if ( JSON_DEBUG_MODE )
          printf ( "Calling key_vector .pop_back (  );\n" );

     key_vector .pop_back (  );
     p ++;
     continue;
   }

   //Use the type for this level of nesting.
   if ( hierarchy_vector .size (  ) )
        b_need_key = hierarchy_vector .back (  ) == VARIABLE_TYPE_OBJECT ? 1 : 0;
   else b_need_key = 0;

   if ( b_need_key ) {
     //Get the key string.
     if ( e == (p = VARIABLE::consume_item ( p, e, key, ul_type )) )
        break;

     if ( JSON_DEBUG_MODE )
          printf ( "Here's the key of the object: \"%s\"\n", key .c_str (  ) );

     if ( *p != ':' )
         {
          printf ( "Error: There was a missing ':' delimiter in a JSON object.\n" );
          some_variable.clear();
          return false;
         }
     p ++; //skip the ':' token.

     //We only store keys for the starts of new objects. The pairs within them don't need to be stored.
     if ( b_store_key )
         {
          key_vector .push_back ( key );
          VARIABLE *node = VARIABLE::get_node_at_key_path ( &some_variable, key_vector );
         }
     //If we don't need to store it, then we need to overwrite the key at this level.
     else key_vector .back (  ) = key;
   }

   //If we have an object or array inside of an object or array, continue from the top.
   if ( *p == '[' || *p == '{' )
        continue;


   //Get the value whether it's an array or associated with some key.
   if ( e == (p = VARIABLE::consume_item ( p, e, item, ul_type )) )
        break;

   if ( JSON_DEBUG_MODE )
        printf ( "Item: \"%s\"\n", item .c_str (  ) );

   //Ensure that the JSON string isn't malformed.
   //It's possible that the string ends after a value, if the JSON consists of a single primitive, hence the p != e exclusion.
   if ( *p != ',' && *p != '\"' && *p != '}' && *p != ']' && p != e )
       {
        printf ( "Found '%c'; expected ',', '\\\"', ']', or '}' after object key & value pair were parsed.\n", *p );
        printf ( "Here's the rest of the JSON:\n>>>>\n%s\n<<<<\n", p );
        break;
       }

   //If we're dealing with an object.
   //Show debugging information:
   if ( JSON_DEBUG_MODE )
       {
        printf ( "some_variable " );
        for ( std::vector<std::string>::iterator key_iterator = key_vector .begin (  );
              key_iterator != key_vector .end (  );
              key_iterator ++
            )
             {
              printf ( " [ \"%s\" ]", key_iterator ->c_str (  ) );
             }
        if ( b_need_key )
             printf ( " = \"%s\";\n", item .c_str (  ) );
        else printf ( " .push_back ( \"%s\" );\n", item .c_str (  ) );
       }

   //Actually add the value to the variable, now.
   VARIABLE *node = VARIABLE::get_node_at_key_path ( &some_variable, key_vector );

   int64_t i64_integer;
   double dbl_double;

   //Convert the data to the proper type.
   if ( ul_type == VARIABLE_TYPE_INT64 )
       {
        //printf ( "Trying stoll ( \"%s\" ).\n", item .c_str (  ) );
        i64_integer = stoll ( item ); //stoi = 0->255, stol and stoll are bigger.

        //It's possible that the JSON that we're parsing is just a single primitive.
        if ( key_vector .size (  ) )
            {
             if ( b_need_key )
                  node ->set ( i64_integer ); //should be blank right now.
             else
                  node ->add ( i64_integer );
            }
        else
             node ->set ( i64_integer );
       }
   else if ( ul_type == VARIABLE_TYPE_DOUBLE )
       {
        dbl_double = stod ( item );

        //It's possible that the JSON that we're parsing is just a single primitive.
        if ( key_vector .size (  ) )
            {
             if ( b_need_key )
                  node ->set ( dbl_double ); //should be blank right now.
             else
                  node ->add ( dbl_double );
            }
        else
             node ->set ( dbl_double );
       }
   else if ( ul_type == VARIABLE_TYPE_STRING )
       {
        //It's possible that the JSON that we're parsing is just a single primitive.
        if ( key_vector .size (  ) )
            {
             if ( b_need_key )
                  node ->set ( item ); //should be blank right now.
             else
                  node ->add ( item );
            }
        else
             node ->set ( item );
       }

   //printf ( "Current object:\n%s\n\n", some_variable ->to_json (  ) .c_str (  ) );
 }

 return true;
}}

/*
 int main ( int argc, char **argv )
{{
 //test1 (  );
 //test2 (  );
 test3 (  );
 //test4 (  );
 //test5 (  );
 //test6 (  );

 return 0;
}}
*/
