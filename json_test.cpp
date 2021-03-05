/*
 Created on 2020-07-05 by Jacob Bethany
 Purpose: To move the tests out of the main JSON source file.
 Ask for permission before using.

 To compile:
 g++ json.cpp json_test.cpp -o json
*/
 #include "json.h"

 void json_minimize_test ( void )
{{
 const char *sz_string = "{ \"test\" : [ 1, 2, 3 ] }";
 std::string minimized_json_string = VARIABLE::get_minimal_json_string ( sz_string );
 printf ( "Minimized json: \"%s\"\n", minimized_json_string .c_str (  ) );
}}

 void test1 ( void )
{{
 VARIABLE tim_vector = VARIABLE::empty_array (  ); //same as //VARIABLE jacob_vector ( NEW_ARRAY );
 auto jacob_vector = VARIABLE::empty_array (  ); //same as //VARIABLE tim_vector ( std::vector<VARIABLE *>() );
 jacob_vector .add ( "guitar" );
 jacob_vector .add ( "gayming" );
 jacob_vector .add ( "frolicing in the nude" );
 jacob_vector .add ( "programming" );

 tim_vector .add ( "programming" );
 tim_vector .add ( "guitar" );
 tim_vector .add ( "complaining" );
 tim_vector .add ( "trolling conservatives on Facebook" );
 tim_vector .add ( "photoshop" );

 VARIABLE jacob_object ( NEW_OBJECT );
 VARIABLE tim_object ( NEW_OBJECT );
 jacob_object .add ( "hobbies", &jacob_vector );
 tim_object .add ( "hobbies", &tim_vector );

 //printf ( "%s\n", jacob_vector .to_json (  ) .c_str (  ) );

 VARIABLE main_object = VARIABLE::empty_object (  ); //same as VARIABLE main_object ( NEW_OBJECT ); or VARIABLE main_object ( std::map<std::string, VARIABLE *>() );
 main_object .add ( "Jacob", &jacob_object );
 main_object .add ( "Tim", &tim_object );
 printf ( "%s\n\n", main_object .to_json (  ) .c_str (  ) );
}}

 void test_at (  )
{{
 //Create a new object.
 VARIABLE obj = VARIABLE::empty_object (  );
 printf ( "A new object was created.\n" );

 //Start filling the object.
 //Use an out-of-bounds index for the object (it's actually not even an array).
 //Upon failure, this will return the same object (obj) by returning (VARIABLE *) this.
 VARIABLE *lp_variable = obj .at ( 10 );
 printf ( "VARIABLE::at was used.\n" );

 //Add astring at the key test, but after intentionally using a bad index.
 obj .at ( 9999 ) ->at ( "test" ) ->add ( std::string ( "This is only a test..." ) );
 printf ( "VARIABLE::at(10) =>\n%s\n\n", lp_variable ->to_json (  ) .c_str (  ) );
}}

 void test2 (  )
{{
/*
 {
  "Jacob" : {
    "age" : 30,
    "hobbies" : [
      "guitar", "programming", "gayming", "frolicing in the nude"
    ]
  },
  "Tim" : {
    "age" : 48,
    "hobbies" : [
      "complaining", "guitar", "programming", "trolling conservatives on Facebook", "photoshop"
    ]
  }
 }
*/

 VARIABLE obj = VARIABLE::empty_object (  );
 obj .add ( "Jacob", VARIABLE::new_object (  ) );
 obj .at ( "Jacob" ) ->add ( "Age", (int64_t) 30 );
 obj .at ( "Jacob" ) ->add ( "Hobbies", VARIABLE::new_array (  ) );
 obj .at ( "Jacob" ) ->at ( "Hobbies" ) ->add ( "Guitar" );
 obj .at ( "Jacob" ) ->at ( "Hobbies" ) ->add ( "Programming" );
 obj .at ( "Jacob" ) ->at ( "Hobbies" ) ->add ( "Gayming" );
 obj .at ( "Jacob" ) ->at ( "Hobbies" ) ->add ( "frolicing in the nude" );
 obj .at ( "Jacob" ) ->at ( "Hobbies" ) ->add ( "\"Creating strings\" that \\need\\ to be \\e\"s\"\\c\\ap\\e\\d'`~" );

 obj .add ( "Tim", VARIABLE::new_object (  ) );
 obj .at ( "Tim" ) ->add ( "Age", (int64_t) 48 );
 obj .at ( "Tim" ) ->add ( "Hobbies", VARIABLE::new_array (  ) );
 obj .at ( "Tim" ) ->at ( "Hobbies" ) ->add ( "Complaining" );
 obj .at ( "Tim" ) ->at ( "Hobbies" ) ->add ( "Guitar" );
 obj .at ( "Tim" ) ->at ( "Hobbies" ) ->add ( "Programming" );
 obj .at ( "Tim" ) ->at ( "Hobbies" ) ->add ( "Trolling conservatives on Facebook" );
 obj .at ( "Tim" ) ->at ( "Hobbies" ) ->add ( "Photoshop" );
 std::string json_string = obj .to_json (  );
 printf ( "%s\n", json_string .c_str (  ) );
}}

 void test3 ( void )
{{
 VARIABLE obj;
/*
 printf ( "New object:\n%s\n", obj .to_json (  ). c_str (  ) );
 obj .add ( "test", NEW_ARRAY );
 obj .at ( "test" ) ->add ( "a" );
 obj .at ( "test" ) ->add ( "b" );
 obj .at ( "test" ) ->add ( "c" );
*/

 //obj .add ( "test", (int64_t) 9 );

 //obj .add ( "key", "value" );

/*
 obj .add ( "key", NEW_OBJECT );
 obj .at ( "key" ) ->add ( "k", "v" );
 obj .at ( "key" ) ->add ( "k2", NEW_ARRAY );
 obj .at ( "key" ) ->at ( "k2" ) ->add ( (int64_t) 1 );
 obj .at ( "key" ) ->at ( "k2" ) ->add ( 2.0 );
 obj .at ( "key" ) ->at ( "k2" ) ->add ( "three" );
*/

 //obj is invalid =>becomes an object ->"Jacob" is added.
 //obj [ "Jacob" ] is invalid =>becomes an object ->"Hobbies" is added.
 //object [ "Jacob" ] [ "Hobbies" ] is invalid =>becomes an array -> 1, 2.0, and "three" are added to the array.
/*
 obj [ "Jacob" ] [ "Hobbies" ] .add ( NEW_ARRAY );
 obj [ "Jacob" ] [ "Hobbies" ] .add ( (int64_t) 1 );
 obj [ "Jacob" ] [ "Hobbies" ] .add ( 2.0 );
 obj [ "Jacob" ] [ "Hobbies" ] .add ( "three" );
*/

 obj [ "Jacob" ] [ "Age" ] = (int64_t) 30;
// obj [ "Jacob" ] [ "Hobbies" ] = NEW_ARRAY; //= operator overload is the same as .set().
 obj [ "Jacob" ] [ "Hobbies" ] .add ( "playing guitar" );
 obj [ "Jacob" ] [ "Hobbies" ] .add ( "playing games" );
 obj [ "Jacob" ] [ "Hobbies" ] .add ( "frolicing in the nude" );
 obj [ "Jacob" ] [ "Hobbies" ] .add ( "programming" );

 obj [ "Tim" ] [ "Age" ] = (int64_t) 48;
 //= NEW_ARRAY is unneeded. It will happen implicitly when the VARIABLE created with the blank constructor when [ "Hobbies" ] was accessed
 //has its add method called with the single parameter overload (vector/array) which will turn it into an array type VARIABLE object.
// obj [ "Tim" ] [ "Hobbies" ] = NEW_ARRAY; //same as obj [ "Tim" ] [ "Hobbies" ] .set ( NEW_ARRAY );
 obj [ "Tim" ] [ "Hobbies" ] .add ( "playing guitar" );
 obj [ "Tim" ] [ "Hobbies" ] .add ( "using photoshop" );
 obj [ "Tim" ] [ "Hobbies" ] .add ( "trolling conservatives" );
 obj [ "Tim" ] [ "Hobbies" ] .add ( "complaining" );

 obj [ "An unborn person" ] [ "Age" ] = (int64_t) -1234;
 obj [ "An unborn person" ] [ "Favorite numbers" ] .add ( (int64_t) -10 );
 obj [ "An unborn person" ] [ "Favorite numbers" ] .add ( (int64_t) 0 );
 obj [ "An unborn person" ] [ "Favorite numbers" ] .add ( 0.1 );
 obj [ "\\a\\ key that requires \"escaping\"" ] = "A \\value\\ that \"requires\" escaping.";

 //std::string json_string = obj .to_json (  );
 std::string json_string = obj .to_minimal_json (  );
 printf ( "Final object:\n%s\n", json_string. c_str (  ) );

 VARIABLE *json_object = VARIABLE::parse ( json_string .c_str (  ) );

 printf ( "Here's the parsed json object:\n%s\n\n", json_object ->to_json (  ) .c_str (  ) );

 printf ( "The destructor is about to be called.\n" );
 delete json_object;
 printf ( "json_object is now freed, but the local scope of obj will now end and call an identical destructor.\n" );
}}

/*
 This is a test to ensure that the get_[type] functions are working, properly.
*/
 void test4 (  )
{{
 VARIABLE v;
 v [ "array" ] .add ( (int64_t) 0 );
 v [ "array" ] .add ( 1.0 );
 v [ "array" ] .add ( "two" );
 v [ "array" ] .add ( NEW_OBJECT );
 v [ "array" ] .add ( "four" );
 v [ "array" ] [ 3 ] [ "test" ] = "thing";

 //int64_t to string.
 //printf ( "v [ \"array\" ] [ 0 ] .get_string (  ) = >>>> %s <<<<\n", v [ "array" ] [ (uint64_t) 0 ] .get_string (  ) .c_str (  ) );
 printf ( "v [ \"array\" ] [ 0 ] .get_string (  ) = >>>> %s <<<<\n", v [ "array" ] [ 0 ] .get_string (  ) .c_str (  ) );

 //double to string.
 printf ( "v [ \"array\" ] [ 1 ] .get_string (  ) = >>>> %s <<<<\n", v [ "array" ] [ 1 ] .get_string (  ) .c_str (  ) );

 //string to string.
 printf ( "v [ \"array\" ] [ 2 ] .get_string (  ) = >>>> %s <<<<\n", v [ "array" ] [ 2 ] .get_string (  ) .c_str (  ) );

 //object to string.
 printf ( "v [ \"array\" ] [ 3 ] .get_string (  ) = >>>> %s <<<<\n", v [ "array" ] [ 3 ] .get_string (  ) .c_str (  ) );

 //array to string.
 printf ( "v [ \"array\" ] .get_string (  ) = >>>> %s <<<<\n", v [ "array" ] .get_string (  ) .c_str (  ) );

 int64_t integer = v [ "array" ] [ 0 ] .get_integer (  );
 printf ( "v [ \"array\" ] [ 1 ] .get_integer (  ) = %lld.\n", integer );

 integer = v [ "array" ] [ 1 ] .get_integer (  );
 printf ( "v [ \"array\" ] [ 1 ] .get_integer (  ) = %lld.\n", integer );

 integer = v [ "array" ] [ 2 ] .get_integer (  );
 printf ( "v [ \"array\" ] [ 2 ] .get_integer (  ) = %lld.\n", integer );

 integer = v [ "array" ] [ 3 ] .get_integer (  );
 printf ( "v [ \"array\" ] [ 3 ] .get_integer (  ) = %lld.\n", integer );

 integer = v [ "array" ] .get_integer (  );
 printf ( "v [ \"array\" ] .get_integer (  ) = %lld.\n", integer );


 //This was ambiguous before the subscript overload was changed from const char * to std::string
 //double decimal = v [ "array" ] [ (uint64_t) 0 ] .get_double (  );
 double decimal = v [ "array" ] [ 0 ] .get_double (  );
 printf ( "v [ \"array\" ] [ 0 ] .get_double (  ) = %f;\n", decimal );

 decimal = v [ "array" ] [ (uint64_t) 1 ] .get_double (  );
 printf ( "v [ \"array\" ] [ 1 ] .get_double (  ) = %f;\n", decimal );

 decimal = v [ "array" ] [ (uint64_t) 2 ] .get_double (  );
 printf ( "v [ \"array\" ] [ 2 ] .get_double (  ) = %f;\n", decimal );

 decimal = v [ "array" ] [ (uint64_t) 3 ] .get_double (  );
 printf ( "v [ \"array\" ] [ 3 ] .get_double (  ) = %f;\n", decimal );

 decimal = v [ "array" ] .get_double (  );
 printf ( "v [ \"array\" ] .get_double (  ) = %f;\n", decimal );

 //Show the object, itself.
 printf ( "%s\n", v .to_json (  ) .c_str (  ) );
}}

/*
 This will test having a json object with the base being an array.
*/
 void test5 ( void )
{{
 VARIABLE ary;
 ary .add ( (int64_t) 0 );
 ary .add ( 1.0 );
 ary .add ( "two" );
 ary .add ( NEW_OBJECT );
 ary [ 3 ] [ "some_key" ] = "some_value";
 ary .add ( "4" );
 printf ( "%s\n", ary .to_json (  ) .c_str (  ) );
}}

/*
 Test json objects consisting only of primitive types.
*/
 void test6 ( void )
{{
 VARIABLE integer ( (int64_t) 777 );
 printf ( "%s\n", integer .to_json (  ) .c_str (  ) );
 VARIABLE *json = VARIABLE::parse ( integer .to_json (  ) .c_str (  ) );
 printf ( "%s\n", json ->to_json (  ) .c_str (  ) );
 delete json;


 VARIABLE double_variable ( 7.77 );
 printf ( "%s\n", double_variable .to_json (  ) .c_str (  ) );
 json = VARIABLE::parse ( double_variable .to_json (  ) .c_str (  ) );
 printf ( "%s\n", json ->to_json (  ) .c_str (  ) );
 delete json;


 VARIABLE string_variable ( "This is only a test." );
 printf ( "%s\n", string_variable .to_json (  ) .c_str (  ) );
 json = VARIABLE::parse ( string_variable .to_json (  ) .c_str (  ) );
 printf ( "%s\n", json ->to_json (  ) .c_str (  ) );
 delete json;
}}

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
