/*
 Code by: Jacob A. Bethany
 Last Modification: Sunday, April 19, 2015

 Note: offset_of () macro was taken from some stack overflow post.
 It essentially pretends there's a pointer to a structure at (struct *) 0;
 then subtracts the location of it's hypothetical member to get the offset.

 This project was to see how viable an automated approach to dereferencing
 could potentially be in a large scale project.

 While this does look potentially possible and useful in some very obscure
 cases, its convoluted nature would most likely make this sort of approach
 error prone, and further, unreadable by any new programmers staffed on the
 project in question. As such, this technique will probably be avoided, other
 than as a conversational piece in the future unless some debuging of running
 processes is ever needed, as the applications of dereferencing by offset,
 rather than by compiler defined operations in ones' own project can be useful.

 Applications in remote debugging:
 -Inject a dll that uses code something like this.
 -Search for variables by offsets of some sort (while being able to ignore the FILO structure thanks to dereferencing by offset/datasize).
 -Enumerate linked lists in memory by base pointers and offsets to the NEXT/PREV pointers stored in the structures.
 NOTE: Normally you'd need a structure something like this to enumerate:
 typedef struct PADDING_S
{
 unsigned char UnknownMemberPadding [ 96 ]; //random assortment of variables that could exist above the next pointer.
 PADDING_S *Next; //Pointer to the next node in the linked list.

 UnknownMemberPadding [ 72 ]; //random assortment of variables that could exist between the next and prev pointers.
 PADDING_S *Prev; //pointer to the previous node in the linked list.

 unsigned char UnknownMemberPadding2 [ 20 ]; //random assortment of variables that could exist below the prev pointer.
} PADDING;

 PADDING *p = FindFirstListNodeInMemorySomeHow ( ... );
 while ( p )
        {
         DumpData ( p, sizeof ( PADDING ) );

         p = p ->Next;
        }

 With the dereference by offset, you wouldn't need to make padded filler structures
 to navigate the prudent information while debugging.

 Instead you could just do something like this:
 while ( Next = ExtractValue ( Next, 0, 96 ) )
         DumpData ( *Next, 96+72+20+sizeof(char*)*2 ); //note the sizes are just taken from the random example above.
*/
 #ifndef offset_of
  #define offset_of(TYPE , MEMBER) ((size_t) &((TYPE*)0)->MEMBER)
 #endif

 typedef void (*EmptyFunction)(void);

 #include "windows.h"
 #include "conio.h"
 #include "stdlib.h"
 #include "stdio.h"

 typedef struct SOME_STRUCT_S
{
 long a;
 long b;
 long c;
} SOME_STRUCT;

 typedef struct ABSTRACT_STRUCT_S
{
 SOME_STRUCT *p;
 SOME_STRUCT s;
} ABSTRACT_STRUCT;

 void Test1 ( void )
{{
 long x = 17;
 long *px = &x;
 long **ppx = &px; //(long **) (long *) px; fails...
 long *ex = *ppx;

 printf ( "%ld = %ld = %ld...%ld\r\n", x, *px, **ppx, *ex );
}}

 void Test2 ( void )
{{
 long x = 17;
 long *px = &x;
 long **ppx = &px;
 long ***pppx = &ppx;

 printf ( "%ld = %ld = %ld = %ld\r\n", x, *px, **ppx, ***pppx );
}}

 void *DereferenceOnce ( void *Ptr )
{{
 void **p = (void **) Ptr;

 return (void *) *p;
}}

 void *Dereference ( void *Ptr, unsigned long Times )
{{
 unsigned long i = 0;
 void *p = Ptr;

 for ( i = 0; i < Times; i += 1 )
       p = DereferenceOnce ( p );

 return p;
}}

/*
  This function is used to dereference a pointer a set number of times,
 then add an offset to the pointer and return the location in memory that
 results from all of this.

  You may then dereference the location in memory returned in order to extract
 the data from the location.

 NOTE: You must cast the datatype you are retrieving as a pointer to the
 returned location, then dereference the location to get the actual data
 stored in that location.

 AS AN EXAMPLE:
 ==============
 DATA_TYPE DataStoredInMemory = *(DATA_TYPE *) ExtractValue ( POINTER_TO_STRUCTURE_OF_X_REFERENCES, X_REFERENCES-1, offset_of ( SOME_STRUCTURE, SOME_MEMBER_OF_THE_STRUCTURE ) );
*/
 void *ExtractValue ( void *Ptr, unsigned long Dereferences, unsigned long Offset )
{{
 char *p = ((char *) Dereference ( Ptr, Dereferences ));
 p += Offset;

 return (void *) p;
}}

/*
 This function is like ExtractValue(), but instead of having
 you dereference the data yourself before manipulating it
 and/or storing it somewhere, it simply stores it for you.

 Example Usage:
 ==============
 MEMBER_STORAGE_TYPE VariableToStoreTheDataIn;
 memset ( &b, 0, sizeof ( MEMBER_STORAGE_TYPE ) );

 CopyValue ( POINTER_TO_STRUCTURE_OF_X_REFERENCES, X_REFERENCES-1, offset_of ( SOME_STRUCTURE, SOME_MEMBER_OF_THE_STRUCTURE ), &VariableToStoreTheDataIn, sizeof ( MEMBER_STORAGE_TYPE ) );

 printf ( "s .b copied -> %ld\r\n\r\n", VariableToStoreTheDataIn );
*/
 void CopyValue ( void *Ptr, unsigned long Dereferences, unsigned long Offset, void *lpOut, unsigned long OutSize )
{{
 char *p = ((char *) Dereference ( Ptr, Dereferences ));
 p += Offset;

 memcpy ( lpOut, p, OutSize );
}}

 void Test3 ( void )
{{
 long x = 17;
 long *px = &x;
 long **ppx = &px;
 long ***pppx = &ppx;

 printf ( "%ld = %ld = %ld = %ld\r\n",
          x,
          (long) Dereference ( px, 1 ), //note that the (long) is implied with "%ld".
          (long) Dereference ( ppx, 2 ), //it's just good practice, and often required for general usage
          (long) Dereference ( pppx, 3 ) ); //to specifiy the storage type after the dereference.
}}

 void Test4 ( void )
{{
 SOME_STRUCT s;
 s .a = 17;
 s .b = 27;
 s .c = 172;

 void *ps = (void *) &s;
 void **pps = (void **) &ps;
 void ***ppps = (void ***) &pps;

 printf ( "Location of the Structure variable: %p\r\n { %p, %p, %p --> %ld }\r\nPointer to structure variable: {%p}\r\n\r\n",
          &s,
          &s .c, //0xAC
          ExtractValue ( ppps, 2, offset_of ( SOME_STRUCT, c ) ), //0x19
          *(long *) ExtractValue ( ppps, 2, offset_of ( SOME_STRUCT, c ) ), //0x19
          ExtractValue ( ppps, 3, 0x9B ),
          ps ); //0x11 + 0x9B = 0xAC

 printf ( "%ld, %ld, %ld\r\n",
          *(long *) ExtractValue ( ps, 0, offset_of ( SOME_STRUCT, a ) ),      //(long ) *((char *) ps + offset_of ( SOME_STRUCT, a )),
          *(long *) ExtractValue ( pps, 1, offset_of ( SOME_STRUCT, b ) ),
          *(long *) ExtractValue ( ppps, 2, offset_of ( SOME_STRUCT, c ) ) );

 long b = 0;
 CopyValue ( ppps, 2, offset_of ( SOME_STRUCT, b ), &b, sizeof ( long ) );
 printf ( "s .b copied -> %ld\r\n\r\n", b );
}}

 void Test5 ( void )
{{
 SOME_STRUCT s;
 ABSTRACT_STRUCT a;

 s .a = 3;
 s .b = 2;
 s .c = 1;

 a .p = &s;
 a .s .a = 10;
 a .s .b = 11;
 a .s .c = 12;

 void *pa = &a;
 void *ppa = &pa;
 void *pppa = &ppa;

 //Note: Abstract_struct ->p = (some_struct *), so we need to dereference an extra time,
 //either inside the extract function, or outside of it as shown below:
 //SOME_STRUCT p1 = **(SOME_STRUCT **) ExtractValue ( ppa, 1, offset_of ( ABSTRACT_STRUCT, p ) ); //works.
 SOME_STRUCT p1 = *(SOME_STRUCT *) ExtractValue ( ppa, 2, offset_of ( ABSTRACT_STRUCT, p ) ); //also works, but is less intuitive that the member of the structure is actually a pointer to another structure.

 //This also works... treat the derefrenced value as what it is... a pointer to a structure, and use it normally.
 SOME_STRUCT *p3 = (SOME_STRUCT *) ExtractValue ( ppa, 2, offset_of ( ABSTRACT_STRUCT, p ) );

 SOME_STRUCT *p2 = (SOME_STRUCT *) ExtractValue ( pppa, 2, offset_of ( ABSTRACT_STRUCT, s ) );

 printf ( "a: %ld, %ld, %ld\r\n" \
          "b: %ld, %ld, %ld\r\n\r\n",
          p1 .a, //access this as a local variable would be accessed.
          (*p3) .b, //access this as a dereferenced pointer to a structure, then add the offset to get the value.
          p3 ->c, //use the pointer to the member of the function directly from a pointer to the structure.
          (*p2) .a, //Repeat of above potential notations.
          p2 ->b, //Another repeat of one of the above notations.
          *(long *) ExtractValue ( p2, 0, offset_of ( SOME_STRUCT, c ) ) //Lastly, extract using the extraction function once more, as a strange alternative...
        );
}}

/*
 Ever want to call a dynamicly sized chain of functions
 one after another all within one call? Me neither... but
 I was curious enough to try and see if it were possible...
*/
 void CallFunctions ( unsigned long FunctionCount, ... )
{{
 unsigned long i = 0;
 char *p = (char *) &FunctionCount + sizeof ( unsigned long );

 EmptyFunction *TestFunction = (EmptyFunction *) p;

 for ( i = 0; i < FunctionCount; i += 1 )
      {
       printf ( "Calling Function %ld...\r\n", i );
       printf ( "=================================\r\n\r\n" );

       EmptyFunction Tmp = *TestFunction;
       Tmp (  );

       TestFunction ++;
      }
}}

 void main ( void )
{{
 CallFunctions ( 5, &Test1, &Test2, &Test3, &Test4, &Test5 );
}}
