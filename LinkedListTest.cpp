/*
 Code by: Jacob A. Bethany.

 NOTE:
 =====
 -Most functions have been renamed to remove LinkedList____ ()
 from the name, now that it's implied by its inclusion in the class.
 The reason for mentioning this is, all of the test code was programmed
 before the renaming, so I'm still recoding little bits and pieces to
 make it comply with the new naming convention... (Nothing has changed
 in the class code though, since the testing).

 Last Update: Wednesday, March 18, 2015 @ 10:49 PM PST
*/
 #include "LinkedList.hpp"

 typedef struct SIMPLE_REGION_S
{
 unsigned short String [ 255 ];
 long Number;
} SIMPLE_REGION;

/*
 Old C-Style callback to return a string from a SIMPLE_REGION structure
 of which a linked list can be comprised.
*/
 unsigned short *GetStringFromSimpleRegion ( void *lpSimpleRegion )
{{
 SIMPLE_REGION *sr = (SIMPLE_REGION *) lpSimpleRegion;

 return sr ->String;
}}

/*
 Old C-Style callback to return a long from a SIMPLE_REGION structure
 of which a linked list can be comprised.
*/
 long GetNumberFromSimpleRegion ( void *lpSimpleRegion )
{{
 SIMPLE_REGION *sr = (SIMPLE_REGION *) lpSimpleRegion;

 return sr ->Number;
}}

/*
 A class which handles a linked list which is comprised soley of SIMPLE_REGION
 structures for the variable blocks of the nodes in its linked list.
*/
 class SimpleListClass : public LINKED_LIST_CLASS
{
 public:

/*
 A function that obsoletes the old C-style callback.
*/
 unsigned short *GetAlphaSortStringFromVariableBlock ( void *lpVariableBlock )
{{
 SIMPLE_REGION *sr = (SIMPLE_REGION *) lpVariableBlock;

 return sr ->String;
}}

/*
 A function that obsoletes the old C-style callback.
*/
 unsigned char GetSortLongFromVariableBlock ( void *lpVariableBlock, long *outLong )
{{
 SIMPLE_REGION *sr = (SIMPLE_REGION *) lpVariableBlock;

 *outLong = sr ->Number;

 return 1;
}}

/*
 Adds a node with a filled SIMPLE_REGION structure to the linked list.
*/
 unsigned char AddSimpleNode ( unsigned short *String, long Number )
{{
 SIMPLE_REGION sr;
 memset ( &sr, 0, sizeof ( sr ) );

 sr .Number = Number;
 memset ( sr .String, 0, 255<<1 );
 wcscpy ( sr .String, String );

 return AddNode ( &sr, sizeof ( sr ) );
}}

/*
 Displays every node's data in the linked list, along with
 a descriptive heading.
*/
 void DisplaySimpleList ( unsigned short *Heading )
{{
 SIMPLE_REGION *sr = 0;

 wprintf ( L"%s", Heading );
 while ( EnumerateByVariableBlock ( (void**) &sr ) )
         wprintf ( L"String [%s] - Number [%ld]\r\n", sr ->String, sr ->Number );
}}

/*
  A callback function to be executed when the linked list is freed
 with any of the Free____Ex() functions contained in the superclass.
*/
 unsigned char FreeCallback ( void *lpComplexRegoin )
{{
 return 1;
}}
}; //End of the SimpleListClass




/*
 A structure containing a non-abstract linked list class,
 to be used in a linked list of a more complex type of
 class.
*/
 typedef struct COMPLEX_REGION_S
{
 SimpleListClass *SimpleList;
 unsigned short *RegionName;
} COMPLEX_REGION;

/*
 A class who's linked list's nodes are comprised of
 COMPLEX_REGION structures, which in turn house another
 linked list class each.
*/
 class ComplexListClass : public LINKED_LIST_CLASS
{
 public:

/*
 Subclass the function that tells the linked list class how to find a
 string variable within the Variable Block of the list, in order for
 sorting by them to be possible.
*/
 unsigned short *GetAlphaSortStringFromVariableBlock ( void *lpVariableBlock )
{{
 COMPLEX_REGION *cr = (COMPLEX_REGION *) lpVariableBlock;

 return cr ->RegionName;
}}

/*
 Subclass the function that tells the linked list class how to find the
 long variable within the Variable Block of the list, in order for sorting
 by them to be possible.
*/
 unsigned char GetSortLongFromVariableBlock ( void *lpVariableBlock, long *outLong )
{{
 COMPLEX_REGION *cr = (COMPLEX_REGION *) lpVariableBlock;

 //Return the ID of the LINKED_LIST node within which our complex variable block is contained.
 *outLong = this ->GetParent ( cr ) ->ulId; //Not the usual usage of this type of long sorting function.

 //We were successfull in returning the requested information.
 return 1;
}}

/*
 This function is used to allocate multiple complex list nodes at once.
*/
 unsigned char AddComplexGroup ( unsigned long ulCount )
{{
 return this ->NewGroup ( sizeof ( COMPLEX_REGION ), ulCount );
}}

/*
  Subclass the function that tells the linked list class how to check for
 unique nodes in the list so that we can add only unique nodes.
*/
 unsigned char UniqueCompareCallback ( void *lpVariableBlock1, void *lpVariableBlock2 )
{{
 COMPLEX_REGION *c1, *c2;
 c1 = (COMPLEX_REGION *) lpVariableBlock1;
 c2 = (COMPLEX_REGION *) lpVariableBlock2;

 //If the region name is the same for both the prospective and existing nodes.
 if ( ! wcscmp ( c1 ->RegionName, c2 ->RegionName ) )
      return 1;

 //If the nodes' region names didn't match.
 return 0;
}}

/*
 Add only uniquely named Complex list nodes.
 Add a simple linked list node within the newly added complex list node.

 return 1 on success.
 return 0 on failure.
*/
 unsigned char AddComplexNode ( unsigned short *ComplexNodeName, unsigned short *String, long Number )
{{
 //Define and clear a static structure that will be the VariableBlock of the new linked list's nodes.
 COMPLEX_REGION cr;
 memset ( &cr, 0, sizeof ( cr ) );

 //Allocate enough memory to hold the string that names the region within the variable block.
 if ( ! (cr .RegionName = (unsigned short *) malloc ( this ->GetUnicodeStringSize ( ComplexNodeName ) )) )
      return 0;

 //Clear the buffer to 0 terminate it before copying the string to it.
 memset ( cr .RegionName, 0, this ->GetUnicodeStringSize ( ComplexNodeName ) );
 wcscpy ( cr .RegionName, ComplexNodeName );

 //Create an instance of the SimpleListClass() within the variable block of this complex linked list node.
 if ( ! (cr .SimpleList = new SimpleListClass (  )) )
     {
      free ( cr .RegionName );
      return 0;
     }

 //Try to add a SimpleList node to the class within the complex list.
 if ( ! cr .SimpleList ->AddSimpleNode ( String, Number ) )
     {
      free ( cr .RegionName );
      delete cr .SimpleList;

      return 0;
     }

 //Try to add the complex node to the list, now that it's values are prepared.
 return this ->AddUniqueNode ( &cr, sizeof ( cr ) );
}}

/*
  Displays information about each of this list's nodes, and
 also displays the linked lists within each of this list's
 nodes by their own class display functions.
*/
 void DisplayComplexList ( unsigned short *Heading )
{{
 COMPLEX_REGION *ComplexRegion = 0;

 wprintf ( L"%s", Heading );
 while ( this ->EnumerateByVariableBlock ( (void **) &ComplexRegion ) )
        {
         wprintf ( L"Complex Node [%ld]: ", this ->GetParent ( ComplexRegion ) ->ulId );

         //If the region has a name, display it.
         if ( ComplexRegion ->RegionName )
              wprintf ( L"String = [%s].\r\n", ComplexRegion ->RegionName );

         //If the simple list class exists, use it to show the list inside of it.
         if ( ComplexRegion ->SimpleList )
              ComplexRegion ->SimpleList ->DisplaySimpleList ( L"" );
        }
}}

/*
  Free callback to be called by this ->FreeLinkedListEx (  )
 on each node's variable block.
*/
 unsigned char FreeCallback ( void *lpComplexRegion )
{{
 COMPLEX_REGION *Cr = (COMPLEX_REGION *) lpComplexRegion;

 //To ensure things are freeing properly.
 wprintf ( L"FreeCallback ( \"%s\" )\r\n", Cr ->RegionName );

 //If we have a name for this region, free it.
 if ( Cr ->RegionName )
      free ( Cr ->RegionName );

 //If the class was created.
 if ( Cr ->SimpleList )
     {
      //If the class function failed to free the list it stores.
      if ( ! Cr ->SimpleList ->Free (  ) )
           return 0;

      delete Cr ->SimpleList;
      Cr ->SimpleList = 0;

      return 1;
     }

 return 1;
}}
}; //End of the ComplexListClass

/*
 Simple linked list test, this is the most generic use of
 the linked list class.

 -Employs two separate sorting functions to the list it creates.
*/
 void Test1 (  )
{{
 SimpleListClass *List = new SimpleListClass (  );

 List ->AddSimpleNode ( L"This is only a test...", 0 );
 List ->AddSimpleNode ( L"I like green apples.", 1 );
 List ->AddSimpleNode ( L"Do you like pie?", 2 );
 List ->AddSimpleNode ( L"Zebras have stripes.", 3 );
 List ->AddSimpleNode ( L"Ride a llama; eat a horse.", 4 );

 List ->DisplaySimpleList ( L"Here's the original list:\r\n" );

 List ->AlphaSortEx ( 0, GetStringFromSimpleRegion );
 List ->DisplaySimpleList ( L"\r\nHere's the list sorted by strings.\r\n" );

 List ->LongSortEx ( GetNumberFromSimpleRegion );
 List ->DisplaySimpleList ( L"\r\nHere's the list sorted by numbers.\r\n" );

 wprintf ( L"\r\nFreeing the linked list...\r\n" );
 List ->Free (  );

 delete List;
}}

/*
 This is similar to tests 3 and 4, in that it has linked lists inside of linked lists.
 This function is hardcoded to handle the node additions like this:

 List ->ListRoot ->Data
 List ->ListRoot ->Next ->Data
 List ->ListRoot ->Next ->Next ->Data
 ...

 Where tests3 and 4 take advantage of the enumeration functions built into the
 original abstract class, LINKED_LIST_CLASS().
*/
 void Test2 (  )
{{
 wprintf ( L"Creating the class...\r\n" );
 ComplexListClass *ComplexList = new ComplexListClass (  );

 wprintf ( L"Adding a node to the complex class.\r\n" );
 if ( ! ComplexList ->AddComplexNode ( L"Complex Node1", L"Member1", 0 ) )
      wprintf ( L"Error: Failed to add a node to the complex list class.\r\n" );

 wprintf ( L"Adding 2 nodes inside of the first node of the complex list.\r\n" );
 COMPLEX_REGION *Node = (COMPLEX_REGION *) ComplexList ->gRoot ->lpData;
 Node ->SimpleList ->AddSimpleNode ( L"Member2", 1 );
 Node ->SimpleList ->AddSimpleNode ( L"Member3", 2 );

 wprintf ( L"Adding 2 nodes inside of the second node of the complex list.\r\n" );
 ComplexList ->AddComplexNode ( L"Complex Node2", L"MemberA", 0 );
 Node = (COMPLEX_REGION *) ComplexList ->gRoot ->Next ->lpData;
 Node ->SimpleList ->AddSimpleNode ( L"MemberB", 1 );
 Node ->SimpleList ->AddSimpleNode ( L"MemberC", 2 );

 wprintf ( L"Adding 2 nodes inside of the third node of the complex list.\r\n" );
 ComplexList ->AddComplexNode ( L"Complex Node3", L"MemberX", 0 );
 Node = (COMPLEX_REGION *) ComplexList ->gRoot ->Next ->Next ->lpData;
 Node ->SimpleList ->AddSimpleNode ( L"MemberY", 1 );
 Node ->SimpleList ->AddSimpleNode ( L"MemberZ", 2 );

 wprintf ( L"Attempting to display the complex list.\r\n" );
 ComplexList ->DisplayComplexList ( L"Here's the complex list:\r\n" );

 wprintf ( L"Attempting to free the complex list.\r\n" );
 ComplexList ->FreeEx (  );

 wprintf ( L"Attempting to delete the Complex list class.\r\n" );
 delete ComplexList;
 ComplexList = 0;
}}

/*
 This is a test to see if we can extend an abstract class
 and create the needed functions, then extend that extention
 class and override the functions that used to be abstract
 in the original class.

 tl;dr:
 Class1 { [storage] SomeAbstractFunction ([storage],[storage]) = 0; };
 Class2 { [storage] SomeAbstractFunction ([storage],[storage]) { actually does stuff now. } };
 ThisClass {  [storage] SomeAbstractFunction ([storage],[storage]) { Does other things now. } };

 Yes we can!
*/
 class ReallyComplexListClass : public ComplexListClass
{
 public:

 unsigned char FreeCallback ( void *lpComplexRegion )
{{
 wprintf ( L"Really complex class's freecallback doesn't realy free... sucks for your ram...\r\n" );
 return 1;
}}
}; //End of really complex list class.

/*
 Gets a string of a set length with random letters in each byte
 from 'a' to 'Z'
*/
 void GetRandomString ( unsigned short *String, unsigned long Count )
{{
 unsigned short Buffer [ 255 ];
 memset ( Buffer, 0, 255 );
 unsigned long i = 0;
 for ( i = 0; i < Count; i += 1 )
       Buffer [ i ] = L'a' + rand()%26;

 for ( i = 0; i < Count; i += 1 )
      {
       if ( rand()%2 )
            Buffer [ i ] = towupper ( Buffer [ i ] );
      }

 wcscpy ( String, Buffer );
}}

/*
 Adds 10 linked lists, each with 10 linked lists inside of them.
 Displays them.
 Frees them.
 -Also tests overriding class functions that were originally abstract 2 classes ago.

 Works fine.
 (Note that not actually freeing memory was intentional in the overriden FreeCallback()).
*/
 void Test3 (  )
{{
 ComplexListClass *SomeList = new ComplexListClass (  );

 //This will compile and work, but the freecallback was left blank intentionally to see if the override would work.
 //tl;dr this will cause an intentional memory leak for testing...
 ReallyComplexListClass *List = new ReallyComplexListClass (  );

 List ->AddComplexNode ( L"Fruit", L"Pineapple", 0 );
 ((COMPLEX_REGION *)List ->gRoot ->lpData) ->SimpleList ->AddSimpleNode ( L"Pear", 1 );

 List ->DisplayComplexList ( L"Here's the complex list:\r\n" );

 wprintf ( L"Adding a group of 10 nodes to SomeList.\r\n" );
 SomeList ->AddComplexGroup ( 10 );

 COMPLEX_REGION *ComplexRegion = 0;
 long i = 0, j = 0;
 while ( SomeList ->EnumerateByVariableBlock ( (void **) &ComplexRegion ) )
        {
         ComplexRegion ->RegionName = (unsigned short *) malloc ( SomeList ->GetUnicodeStringSize ( L"COMPLEX SPAM." ) );
         wcscpy ( ComplexRegion ->RegionName, L"COMPLEX SPAM." );

         ComplexRegion ->SimpleList = new SimpleListClass (  );

         for ( j = 0; j < 10; j += 1 )
               ComplexRegion ->SimpleList ->AddSimpleNode ( L"SIMPLE SPAM.", j );
        }

 SomeList ->DisplayComplexList ( L"Displaying the complex group list...\r\n" );

 wprintf ( L"Freeing the group of nodes.\r\n" );
 SomeList ->FreeEx (  ); //SomeList ->FreeGroupEx (  );
 List ->FreeEx (  );

 delete List;
}}

/*
 Essentially the same as Test3() unless you uncomment the SetLinkedListNodeVariableBlock()
 to the offset datasize version, inwhich case it tests the ability of the function to check
 for bad data sizes, specifically when working with Group allocation nodes of NEW datasizes.

 Works fine.
*/
 void Test4 ( void )
{{
 ComplexListClass *List = new ComplexListClass (  );

 LINKED_LIST *Node = 0;
 COMPLEX_REGION ComplexRegion;
 memset ( &ComplexRegion, 0, sizeof ( COMPLEX_REGION ) );

 //Add 10 nodes to the list, all at once.
 List ->AddComplexGroup ( 10 );

 while ( List ->Enumerate ( &Node ) )
        {
         if ( ! (ComplexRegion .RegionName = (unsigned short *) malloc ( List ->GetUnicodeStringSize ( L"COMPLEX SPAM." ) )) )
              break;

         if ( ! (ComplexRegion .SimpleList = new SimpleListClass (  )) )
             {
              free ( ComplexRegion .RegionName );
              break;
             }

         for ( long j = 0; j < 10; j += 1 )
              {
               if ( ! ComplexRegion .SimpleList ->AddSimpleNode ( L"SIMPLE SPAM.", j ) )
                    break;
              }

         if ( ! List ->SetNodeVariableBlock ( Node, &ComplexRegion, sizeof ( COMPLEX_REGION ) ) )
         //if ( ! List ->SetNodeVariableBlock ( Node, &ComplexRegion, sizeof ( COMPLEX_REGION )-5 ) ) //Doesn't crash, it just disallows you to modify the data.
             {
              free ( ComplexRegion .RegionName );
              ComplexRegion .SimpleList ->Free (  );

              delete ComplexRegion .SimpleList;
             }
        }

 List ->DisplayComplexList ( L"SetLinkedListNodeVariableBlock() testing...\r\n" );

 //Free the entire list and all of the entire linked lists within each of the nodes.
 List ->FreeEx (  );
 //List ->FreeGroupEx (  );
 delete List;
}}

/*
 Test for applications of sorting algorithyms via string offset.
*/
 void Test5 (  )
{{
 SimpleListClass *cls = new SimpleListClass (  );
 unsigned short String [ 255 ];

 for ( int i = 0; i < 10; i += 1 )
      {
       memset ( String, 0, 255 );
       GetRandomString ( String, 10 );

       cls ->AddSimpleNode ( String, i );
      }

 cls ->DisplaySimpleList ( L"Here's the list(unsorted):\r\n" );

 if ( ! cls ->AlphaSort ( 0 ) )
      wprintf ( L"Error: Bad offset for the string inside the complex region.\r\n" );

 cls ->DisplaySimpleList ( L"Here's the list(sorted):\r\n" );

 cls ->FreeGroupEx (  );
 delete cls;
 cls = 0;
}}

/*
 This function creates 4 associative linked lists of
 10 linked lists.

 Every list of every list node is sorted alphabetically
 using the new string callback function...

 Every list of every list node is sorted numerically
 using the new long callback function...

 Adding the nodes use the new UniqueCallback function.
 -You can see the freecallback being called when the
 second instance of a complex node called "Zebra" fails
 owing to it's prior existence.

 Works.
*/
 void Test6 (  )
{{
 long i = 0; //a generic counter variable.
 COMPLEX_REGION *cr = 0; //a pointer to a variable block of a complex linked list node.
 ComplexListClass *cls = new ComplexListClass (  ); //an associative linked list of linked lists.
 unsigned short String [ 255 ]; //A buffer to house a randomly generated string.

 //Make an array of Complex list names to add.
 unsigned short *Names[] = { L"Zebra", L"Elephant", L"Mosquito", L"Turtle", L"Zebra" };

 //For each of the Names[] of complex lists, add a complex list by the name associated with that index.
 for ( i = 0; i < 5; i += 1 )
      {
       //Clear the buffer that will hold the random string.
       memset ( String, 0, 255 );

       //Dump a randomly generated string in the buffer with a length of 20 characters.
       GetRandomString ( String, 20 );

       //If we can't add a complex list node, complain!
       if ( ! cls ->AddComplexNode ( Names [ i ], String, 0 ) )
            wprintf ( L"Error: Failed to add the complex node named: [%s].\r\n", Names [ i ] );
      }

 //Start with an invalid structure pointer, so the function knows to start at the root of the linked list.
 cr = 0;
 while ( cls ->EnumerateByVariableBlock ( (void **) &cr ) )
        {
         //For every complex list node, add 10 simple list nodes.
         for ( i = 1; i < 10; i += 1 )
              {
               //Clear the buffer that will hold random strings.
               memset ( String, 0, 255 );

               //Generate a random string 20 characters long, and store it in the buffer.
               GetRandomString ( String, 20 );

               //If we can't add a simple linked list node, complain!
               if ( ! cr ->SimpleList ->AddSimpleNode ( String, i ) )
                    wprintf ( L"Error: Failed to add a simple node to one of the complex list nodes.\r\n" );
              }

         //Alphabetically (case insensitively) sort the list of randomly generated strings we just added to the current complex list node.
         if ( ! cr ->SimpleList ->AlphaSort ( 0 ) )
              wprintf ( L"Error: Bad offset for the string inside the complex region of the simple node." );
        }

 //Sort the complex linked list by the associative name of each reigon.
 if ( ! cls ->AlphaSort ( 0 ) )
      wprintf ( L"Error: Bad offset for the string inside the complex region of the complex node.\r\n" );

 //Print out all of the complex list nodes, along with their contained simple linked lists.
 cls ->DisplayComplexList ( L"Here's the complex list(sorted by string):\r\n" );

 //Resort the linked lists within the complex linked list by some numbers.
 cr = 0;
 while ( cls ->EnumerateByVariableBlock ( (void **) &cr ) )
         cr ->SimpleList ->LongSort (  );

 //This will do nothing, since we're using the LINKED_LIST id, rather than our own id in the variable block,
 //so while we swapped the node data for the alpha sort, the id's of the LINKED_LIST nodes stayed the same.
 cls ->LongSort (  );

 //Print out all of the complex list nodes, along with their contained simple linked lists.
 cls ->DisplayComplexList ( L"Here's the complex list(sorted by id):\r\n" );

 //Free the lists. (The simple linked lists within the complex list's nodes will be freed in the callback function for each complex node.)
 cls ->FreeEx (  );

 //Delete the memory taken by the class itself.
 delete cls;

 //Invalidate the complex class pointer.
 cls = 0;
}}

 //Call some test function to ensure stability of this LinkedList class.
 void main ( void )
{{
 Test6 (  );
}}
