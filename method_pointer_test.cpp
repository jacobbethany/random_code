/*
 Created on 2021-02-12 by Jacob Bethany.
 Purpose: To get a pointer to a method within an instantiated object and use it.
 
 Reference definition:
 [return value] ([optional additional declarator] [class name]::*[method_offset_variable_name])(argument1, argument2, ...);

 Reference usage:
 ([class_instance] ->*[method_offset_variable_name]) ( argument1, argument2, ... );

 This is very useful for something like a message loop within WINAPI, in conjunction with
 /^SetWindowLongPtr[AW]$/ ( hWnd, GWLP_USERDATA, This ) within the WM_NCCREATE message. One just
 needs to pass a reference to "this" class instance in the (LPVOID) final parameter of
 /^CreateWindowEx[AW]$/.

 The last thing to note is that one can make an array of offsets and dynamically call methods from
 an instantiated object, as I've done in this code. The single call version is commented for
 reference, in case I forget the syntax and want something more simplistic to review.
*/
 #include "stdio.h"
 #include "stdlib.h"
 #include "string.h"
 #include "stdint.h"

 class some_class
{
 public:

 some_class (  )
{{
}}

 ~some_class (  )
{{
}}

 void some_callback ( int *a, int b, int c, int d )
{{
 printf ( "a: %d; b: %d, c: %d, d: %d.\n", a, b, c, d );
}}

 void some_callback2 ( int *a, int b, int c, int d )
{{
 printf ( "a: %d; b: %d, c: %d, d: %d.\n", a, b, c, d );
}}

 void some_callback3 ( int *a, int b, int c, int d )
{{
 printf ( "a: %d; b: %d, c: %d, d: %d.\n", a, b, c, d );
}}

 static void passthrough ( int *a, int b, int c, int d )
{{
 some_class *This = (some_class *) a;
 if ( ! This ) {
   printf ( "Error: A bad reference to the class instance was passed.\n" );
   return ;
 }

/*
 //This works...
 //Get some kind of offset in a variable so that we can reference the method of an instantiated object of this type.
 void (some_class::*method_offset)(int *, int, int, int) = &some_class::some_callback;

 //Call the method at the specified offset.
 (This ->*method_offset) ( a, b, c, d );
*/

 //Fill an array with offsets to specific methods within the class.
 void (some_class::*method_offset_array[])(int *,int,int,int) = {
   &some_class::some_callback,
   &some_class::some_callback2,
   &some_class::some_callback3
 };

 //sizeof(method_offset_array) with 3 items = 48 bytes. 48/3 = 16 bytes per offset. That's a bit unexpected.
 //Here I'm dividing the size of the array of method offsets by the size of each offset.
 uint8_t ui8_callback_count = sizeof ( method_offset_array ) / sizeof ( void(some_class::*)(int*,int,int,int) );
 printf ( "There are %d callback functions within the array.\n", ui8_callback_count );

 //Loop through each of the callback functions, using each one in the process.
 for ( uint8_t ui8_i = 0; ui8_i < ui8_callback_count; ui8_i ++ ) {

   printf (
     "Callback offset: %p\n",
     method_offset_array [ ui8_i ]
   );
   (This ->*method_offset_array [ ui8_i ]) ( a, b + ui8_i, c, d );
 }
}}
};

 int main ( int argc, char **argv )
{{
 some_class cls; //local variable of this class type.
 cls .passthrough ( (int *) &cls, 1, 2, 3 );

 return 0;
}}
