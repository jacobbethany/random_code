/*
 Created on 2020-07-05 by Jacob Bethany
 Purpose: To make converting structured data into JSON easy with c++.
*/
#pragma once
#ifndef JSON_HEADER_DEFINED
#define JSON_HEADER_DEFINED 1
 #include "stdio.h"
 #include "stdlib.h"
 #include "string.h"
 #include "stdint.h"
 #include <string>
 #include <vector>
 #include <map>
 #include <limits> //so that we can return NaN if someone requests a double value of an unconvertable type.

#ifndef VARIABLE_TYPE_OBJECT
#define VARIABLE_TYPE_OBJECT 1
#define VARIABLE_TYPE_ARRAY 2
#define VARIABLE_TYPE_INT64 3
#define VARIABLE_TYPE_DOUBLE 4
#define VARIABLE_TYPE_STRING 5
#define VARIABLE_TYPE_INVALID 6
#define NEW_OBJECT (std::map<std::string, VARIABLE *> *) 0
#define NEW_ARRAY (std::vector<VARIABLE *> *) 0
#define JSON_DEBUG_MODE 0
#define JSON_ENFORCE_SAFE_USAGE 1
#endif

 class VARIABLE
{
 private:
   void *m_lpv_data;
   uint32_t m_ul_type;

 public:

   VARIABLE ( const VARIABLE & ) = delete;

   //Helper constructors (these are the same as VARIABLE v = VARIABLE ( NEW_OBJECT );
   //and VARIABLE *v = new VARIABLE ( NEW_ARRAY ); ...etc.
#if ( ! JSON_ENFORCE_SAFE_USAGE )
   static VARIABLE *new_object ( void );
   static VARIABLE *new_array ( void );
   static VARIABLE empty_object ( void );
   static VARIABLE empty_array ( void );
#endif
   static std::string get_escaped_string ( std::string str );
   static std::string get_escaped_string ( std::string *str );
   static std::string get_minimal_json_string ( const char *sz_json_string );


   static std::string::iterator consume_string (
     std::string::iterator json_string_start_iterator,
     std::string::iterator json_string_end_iterator,
     std::string &item
   );
   static std::string::iterator consume_item (
     std::string::iterator json_string_start_iterator,
     std::string::iterator json_string_end_iterator,
     std::string &item,
     uint32_t &type
   );

   //This will create a local instance of the VARIABLE class, given some JSON, and return it.
   static VARIABLE *parse ( const char *sz_json_string );
   static bool parse ( VARIABLE &variable, const char *sz_json_string );

   VARIABLE ( void ); //empty constructor defaults to VARIABLE_TYPE_INVALID with a null pointer in m_lpv_data.
   VARIABLE ( std::string str );

   VARIABLE ( int64_t ui64_x );
   VARIABLE ( double dbl_x );

   VARIABLE ( std::vector<VARIABLE *> *variable_array );
   VARIABLE ( std::map<std::string, VARIABLE *> *object_map );
   ~VARIABLE (  );

   uint32_t get_type ( void );
   void clear ( void );

   //VARIABLE get ( const char *sz_key_string, ... ); //get ( "some_array", "some_object", "10" ) var["some_array"]["10"].
   //subscript overload [ std::string => "" ]
   VARIABLE &operator[] ( uint64_t ui64_index ); //for arrays.
   //VARIABLE &operator[] ( const char *sz_key ); //for objects. (ambiguous if 0 is used)
   VARIABLE &operator[] ( std::string key_string ); //for objects.

   //!! To do: Add subscript overloads for both of these [ const char * ] and [ uint64_t ]
   VARIABLE *at ( uint64_t ui64_index );
   VARIABLE *at ( const char *sz_key );
   VARIABLE *at ( std::string key_string );

   //This will use a vector as a series of contiguous subscript accesses.
   static VARIABLE *get_node_at_key_path ( VARIABLE *v, std::vector<std::string> &key_vector );

   std::string get_padding ( int64_t i64_spaces );
   std::string to_json ( int64_t i64_depth );
   std::string to_json ( void );
   std::string to_minimal_json ( void );

   //This will make a deep copy of the variable such that objects and arrays of objects, arrays, and primitives will copy.
   VARIABLE *get_copy ( void );

   //If the type is object, this will work.
   unsigned char add ( const char *sz_key, std::string str );
   unsigned char add ( const char *sz_key, int64_t x );
   unsigned char add ( const char *sz_key, double x );
   unsigned char add ( const char *sz_key, std::vector<VARIABLE *> *v );
   unsigned char add ( const char *sz_key, std::map<std::string, VARIABLE *> *m );
   unsigned char add ( const char *sz_key, VARIABLE *lp_variable );

   //If the type is array, this will work.
   unsigned char add ( std::string str );
   unsigned char add ( int64_t x );
   unsigned char add ( double x );
   unsigned char add ( std::vector<VARIABLE *> *v );
   unsigned char add ( std::map<std::string, VARIABLE *> *m );
   unsigned char add ( VARIABLE *lp_variable );

   //If we have a blank variable type (empty constructor used), these will allow us to make assignments.
   unsigned char set ( std::string str );
   unsigned char set ( int64_t x );
   unsigned char set ( double x );
   unsigned char set ( std::vector<VARIABLE *> *v );
   unsigned char set ( std::map<std::string, VARIABLE *> *m );
   unsigned char set ( VARIABLE *lp_variable );

   unsigned char operator= ( std::string str );
   unsigned char operator= ( int64_t x );
   unsigned char operator= ( double x );
   unsigned char operator= ( std::vector<VARIABLE *> *v );
   unsigned char operator= ( std::map<std::string, VARIABLE *> *m );
   unsigned char operator= ( VARIABLE *lp_variable );

   //These will get a double, integer, or string value from the current node, if possible.
   //If these are called on an array or object, the statistics (count) will be returned
   //instead.
   double get_double ( void );
   int64_t get_integer ( void );
   std::string get_string ( void );
};

#endif
