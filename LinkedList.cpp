/*
 Code by: Jacob A. Bethany.

 Last Update: Friday, March 20, 2015 @ 8:58 PM PST
 Last Code Addition: Generic File I/O w/ subclassed functions and via
 generic c-style callback function as parmeter, which was dumped into
 the original C code for these functions as well.

 This file contains code for a generic linked list that is very widely applicable
 in its usage.

 TODO:
 =====
 -For group allocation nodes, make a function to find the first unused.
 IE: SetVariableBlock ( FindNewGroupNode (  ), &VariableBlockStructure, sizeof ( VariableBlockStructure ) );

 -Code FreeNode() / FreeNodeEx() to check to see if the node being freed is a group allocation node and, if so,
 opt not to free any of the information.
 If it's a non root node you could free everything but the variable block without any issues, but if it IS the
 root node, freeing anything would invalidate the list, because it would lose the pointer to the start of the
 variable block that is shared across the entire group. This is essentially just a safety check incase someone
 using this code is doing so incorrectly.

 -Create functions to read files as group lists (IE allocated all of the space needed at once for larger files).

 ___________________________________________________________________________________________

 NOTES:
 =====
 -Allocation types:
 *Group (a cluster of nodes of the same variable block datatype allocated all at once.)
  -This will always memset all of the variable blocks after allocation; this means setting
 the data in the variable block of each node will have to be done after the group allocation.

 *Individual -- One node allocated at a time.
 -->AddUniqueNode() // AddNode()

 *Unique -- Only one node of this 'type' can be allocated. (Current a callback function must be supplied to check for this.)
 -->AddUniqueNode()

 *Associative -- All nodes are defaulted to a blank association; only one associative name can be given per node,
  and only one node may have an associative name. The association can be set as the node is created for indiviaully allocated
  linked lists, or they can be made after the list is already allocated for all list types.
 -->AssociateNode()

 *Pointer Lists -- The datasize is 0 (the variable block is just a pointer to something.)
 -->AddNode() // AddUniqueNode()

 -Useful Subclasses:
 *FreeCallback() --required.
 *GetAlphaSortString() --Optional, needed for AlphaSort(). [not for AlphaSortEx()]
 *GetSortLong() --Optional, needed for LongSort(). [not for LongSortEx()]

 -Freeing:
 *Always call FreeEx(). --This will call the other freeing functions and call backs as needed.
 You should free any linked lists and any other data within your Variable Block during the
 FreeCallback() that you subclassed.

 ___________________________________________________________________________________________

 NOTE: As of Monday, February 23, 2015 @ 9:58 PM PST, all Node ->lpData
 pointers are pushed forward by LINKED_LIST_PTR_SIZE (sizeof(char*)) bytes.
 The reason for this is to include a pointer to the parent in the first (sizeof(char*))
 bytes of each lpData. This makes the function GetParent() possible.
 **This also means that all freeing functions need to call in this fashion now:

 free ( ((char *)FreeNode ->lpData) - sizeof(char*) );

 so keep that in mind if you add any new freeing functions...
 Use: FreeVariableBlock ( LINKED_LIST *Node );

 NOTES: (DEPRECATED)
 ======
 *FreeGroup/FreeGroupEx will now automatically be called by Free()/FreeEx() when needed.
 --So call Free() when you don't need a special callback to free things within the variable
 block of your linked list nodes, and call FreeEx() when you do -- with the FreeCallback()
 function override in place in the extended class you've made for your list that tells the
 functions how to free the complex region of the variable block.

 **FreeGroup() --> for generic linked lists that were allocated in mass by NewGroup(), but also works for any kind
 of generic list, including lists created by NewNode() and lists created by both methods then combined using Append().

 **FreeGroupEx() --> for complex linked lists (ones that have dynamically allocated memory within the structure contained in
 the complex region of the linked list nodes. This function works for ALL kinds of lists that require callbacks.

 **Free() --> for generic individually allocated lists only. (Will NOT work for Group allocated lists or lists that require callbacks to free.)
 **FreeEx() -->for complex individually allocated lists only. (Will NOT work for Group allocated lists.)

 In short, the group freeing functions will always work for callback/non-callback freeing of lists, but will be slower in some cases owing
 to the extensive checking that is required to see what memory is freeable and what memory is not.
*/
/*
 #ifndef IS_LINKED_LIST_LIBRARY_PROJECT
  #define IS_LINKED_LIST_LIBRARY_PROJECT 1
 #endif
*/
 #include "LinkedList.hpp"

 LINKED_LIST_CLASS::LINKED_LIST_CLASS ( void )
{{
 this ->gRoot = 0;
 this ->gNextId = 0;
 memset ( this ->gNoAssocName, 0, 4 );
}}

/*
 Returns the size in memory of the unicode string passed.
*/
 unsigned long LINKED_LIST_CLASS::GetUnicodeStringSize ( unsigned short *String )
{{
 return (wcslen ( String ) + 1) << 1;
}}

/*
 Frees the variable block correctly, and safely by ensuring
 that the pointer is allocated and valid.
*/
 void LINKED_LIST_CLASS::FreeVariableBlock ( LINKED_LIST *Node )
{{
 if ( Node ->ulDataSize )
     {
      if ( Node ->lpData )
           free ( ((char *) Node ->lpData - LINKED_LIST_PTR_SIZE) );
     }
}}

/*
 Allocates memory and pushes the pointer it returns foward by the size of a pointer.
 The begining of the pointer that's invisible to the returned node is a pointer to the
 node itself, which is to be used in the GetParent() function.

 NOTES:
 **The node must be allocated.
 **The Node ->ulDataSize must be set to the appropriate value.
 **This function does NOT free the linked list node on fail!!!!
*/
 unsigned char LINKED_LIST_CLASS::AllocateVariableBlock ( LINKED_LIST *Node )
{{
 //p = pointer to the base of the newly allocated memory block ( where the pointer to (LINKED_LIST *) Node) will be.
 //pp = pointer to the pointer p, this is used to derefrence and manipulate the data at that section to point to (Node).
 char *p = 0, **pp = 0;

 if ( ! (p = (char *) malloc ( Node ->ulDataSize + LINKED_LIST_PTR_SIZE ) ) )
      return 0;

 //Point Node ->lpData to the base of the allocated memory block + LINKED_LIST_PTR_SIZE bytes ---> (char *) deals in 1 byte at a time.
 Node ->lpData = (void *) ((char *)p + LINKED_LIST_PTR_SIZE);

 //Force a pointer to this node at the end of the data of the variable block, so that
 //the parent node can be found through the variable block.
 pp = (char **) ((char *) p); //need to do (char*) to get the address, then (char **) cast to handle the value differently.
 *pp = (char *) Node; //derefrence the pointer to a pointer to manipulate the start of the memory block to be a pointer to the linked list node that contains it.

 return 1;
}}

/*
 This allocates a group of nodes all in one giant buffer.
 This also allocates a buffer big enough to house all of the variable blocks.

 Because of the way this function allocates the blocks of memory for the structures
 and the variable blocks, you are unable to change the size of the structures, or
 blocks without recreating the entire list, but this is an extremely fast way of
 allocating the memory needed for large and similar functions.
*/
 unsigned char LINKED_LIST_CLASS::NewGroup ( unsigned long ulDataSize, unsigned long GroupCount )
{{
 if ( ! GroupCount )
      return 0;

 LINKED_LIST *NewGroup = 0;
 void *DataBuffer = 0;
 char *pData = 0;
 char **p = 0;
 unsigned long i = 0;

 if ( ! (DataBuffer = (void *) malloc ( (ulDataSize+LINKED_LIST_PTR_SIZE)*GroupCount ) ) )
      return 0;

 if ( ! (this ->gRoot = (LINKED_LIST *) malloc ( sizeof ( LINKED_LIST ) * GroupCount ) ))
     {
      free ( DataBuffer );
      return 0;
     }

 memset ( DataBuffer, 0, (ulDataSize+LINKED_LIST_PTR_SIZE) *GroupCount );

 NewGroup = this ->gRoot;

 pData = (char *)DataBuffer;
 for ( i = 0; i < GroupCount; i += 1 )
      {
       p = (char **) ((char *) pData);
       *p = (char *) &NewGroup [ i ];

       pData += LINKED_LIST_PTR_SIZE;

       NewGroup [ i ] .AssocName = gNoAssocName;
       NewGroup [ i ] .lpData = (void *) pData;
       NewGroup [ i ] .bFlags = LINKED_LIST_MASS_ALLOCATED;

       NewGroup [ i ] .Next = &NewGroup [i + 1];
       NewGroup [ i ] .Prev = &NewGroup [i - 1];

       NewGroup [ i ] .ulDataSize = ulDataSize;
       NewGroup [ i ] .ulId = gNextId ++;

       //wprintf ( L"This = [%p]\r\n", &NewGroup [ i ] );
       //wprintf ( L"This->Next [%p] =/= NewGroup [ i+1 ]: [%p] \r\n", NewGroup [ i ] .Next, &NewGroup [ i + 1 ] );
       //wprintf ( L"This->Next [%p] =/= NewGroup [ i-1 ]: [%p] \r\n", NewGroup [ i ] .Prev, &NewGroup [ i - 1 ] );

       pData += ulDataSize;
      }

 //wprintf ( L"Final group count allocated: [%ld]\r\nGroup count requested: [%ld]\r\n", i, GroupCount );

 NewGroup [ i - 1 ] .Next = 0;

 NewGroup ->bFlags = LINKED_LIST_FREEABLE_VARIABLE_BLOCK | LINKED_LIST_FREEABLE_STRUCTURE | LINKED_LIST_MASS_ALLOCATED;
 NewGroup ->Prev = 0;

 return 1;
}}

/*
 Pass the list you are searching in for a freeable node.
 Pass the type of freeing you plan to do. (Variable block freeing or Structure freeing).

 Flags (defined in .hpp):
 =====================================
 LINKED_LIST_FREEABLE_VARIABLE_BLOCK
 LINKED_LIST_FREEABLE_STRUCTURE
*/
 LINKED_LIST *LINKED_LIST_CLASS::FindFreeableNode ( LINKED_LIST *StartNode, unsigned long bFlags )
{{
 LINKED_LIST *Node = StartNode;
 while ( Node )
        {
         if ( Node ->bFlags & bFlags )
              return Node;

         //wprintf ( L"Node ->Next [%p]\r\n", Next ->Next );

         Node = Node ->Next;
        }
 return 0;
}}

/*
 If one of the nodes in the linked list were created via NewGroupNode(),
 then we need to free it with FreeGroup() or FreeGroupEx() depending on
 which free function is called! This funciton assists in determining whether
 or not we need to call a group freeing function (method) within this class.
*/
 unsigned char LINKED_LIST_CLASS::ListContainsGroupAllocationNode ( void )
{{
 LINKED_LIST *Node = this ->gRoot;

 while ( Node )
        {
          if ( Node ->bFlags & LINKED_LIST_MASS_ALLOCATED )
               return 1;

         Node = Node ->Next;
        }

 //No node was found within the entire linked list that was group allocated.
 return 0;
}}

/*
 This function frees linked lists of GroupAllocated nodes.
 This function accepts linked lists of multiple GroupAllocated
 nodes attached to one another, and mixed linked lists of both
 individually allcoated and group allocated lists annexed to
 one another...

 NOTE: This function could be called on any type of list and work
 properly.
*/
 unsigned char LINKED_LIST_CLASS::FreeGroup ( void )
{{
 if ( ! (this ->gRoot) )
      return 0;

 //wprintf ( L"Hey, guess what, we're in the FreeGroup() function...\r\n" );

 LINKED_LIST *Next = this ->gRoot, *Free = 0;

 while ( Next )
        {
         //Free the associative name for this node, if it exists.
         if ( Next ->AssocName && Next ->bFlags & LINKED_LIST_HAS_ASSOC_NAME )
             {
              //wprintf ( L"Freeing Node [%ld]'s AssocName...\r\n", Next ->ulId );
              free ( Next ->AssocName );
             }

         Next = Next ->Next;
        }

 //First find and free all freeable variable blocks.
 Next = FindFreeableNode ( this ->gRoot, LINKED_LIST_FREEABLE_VARIABLE_BLOCK );
 Free = Next;

 //While we keep finding new freeable nodes in the list.
 while ( Next )
        {
         //Start on the next node, so we can start looking for the next freeable one.
         Next = FindFreeableNode ( Next ->Next, LINKED_LIST_FREEABLE_VARIABLE_BLOCK );

         if ( Free ->bFlags & LINKED_LIST_FREEABLE_VARIABLE_BLOCK && Free ->ulDataSize )
             {
              //wprintf ( L"Freeing node ID[%ld]'s Variable Block...\r\n", Free ->ulId );

              //Free the last node's data block that we had stored.
              FreeVariableBlock ( Free );
             }

         wprintf ( L"Free [%p] ----> Next [%p]\r\n", Free, Next );

         Free = Next; //Store the node we just found.
        }

 //Secondly, find and free all freeable structures.
 //NOTE: This must be done last, or we'll be trying to recursively search invalid memory addresses...
 Next = FindFreeableNode ( this ->gRoot, LINKED_LIST_FREEABLE_STRUCTURE );
 Free = Next;

 //While we keep finding new freeable nodes in the list.
 while ( Next )
        {
         //Start on the next node, so we can start looking for the next freeable one.
         Next = FindFreeableNode ( Next ->Next, LINKED_LIST_FREEABLE_STRUCTURE );

         if ( Free ->bFlags & LINKED_LIST_FREEABLE_STRUCTURE )
             {
              //wprintf ( L"Freeing node ID[%ld]'s Structure...\r\n", Free ->ulId );

              free ( Free ); //Free the last node block we had stored.
             }

         Free = Next; //Store the node we just found.
        }

 this ->gRoot = 0;
 return 1;
}}

/*
 Same as FreeGroup() but with a user defined callback as well for freeing complex regions of the Variable blocks.
*/
 unsigned char LINKED_LIST_CLASS::FreeGroupEx ( void )
{{
 if ( ! this ->gRoot )
      return 0;

 //wprintf ( L"Hey, guess what, we're in the FreeGroupEx() function...\r\n" );

 LINKED_LIST *Next = this ->gRoot, *Free = 0;

 //Call the Freecallback on every variable block before trying to do any freeing of our own.
 while ( Next )
        {
         //Call the callbackfunction needed to free the data in the complex region that we don't know about.
         FreeCallback ( Next ->lpData );

        //Free the associative name for this node, if it exists.
        if ( Next ->AssocName && Next ->bFlags & LINKED_LIST_HAS_ASSOC_NAME )
            {
             //wprintf ( L"Freeing Node [%ld]'s AssocName...\r\n", Next ->ulId );

             free ( Next ->AssocName );
            }

         Next = Next ->Next;
        }

 //First find and free all freeable variable blocks.
 Next = FindFreeableNode ( this ->gRoot, LINKED_LIST_FREEABLE_VARIABLE_BLOCK );
 Free = Next;

 //While we keep finding new freeable nodes in the list.
 while ( Next )
        {
         //Start on the next node, so we can start looking for the next freeable one.
         Next = FindFreeableNode ( Next ->Next, LINKED_LIST_FREEABLE_VARIABLE_BLOCK );

         if ( Free ->bFlags & LINKED_LIST_FREEABLE_VARIABLE_BLOCK  && Free ->ulDataSize )
             {
              //wprintf ( L"Freeing node ID [%ld]'s Variable Block...\r\n", Free ->ulId );

              //Free the last node's data block that we had stored.
              FreeVariableBlock ( Free );
             }

         Free = Next; //Store the node we just found.
        }

 //Secondly, find and free all freeable structures.
 //NOTE: This must be done last, or we'll be trying to recursively search invalid memory addresses...
 Next = FindFreeableNode ( this ->gRoot, LINKED_LIST_FREEABLE_STRUCTURE );
 Free = Next;

 //While we keep finding new freeable nodes in the list.
 while ( Next )
        {
         //Start on the next node, so we can start looking for the next freeable one.
         Next = FindFreeableNode ( Next ->Next, LINKED_LIST_FREEABLE_STRUCTURE );

         if ( Free ->bFlags & LINKED_LIST_FREEABLE_STRUCTURE )
             {
              //wprintf ( L"Freeing node ID [%ld]'s Structure...\r\n", Free ->ulId );

              free ( Free ); //Free the last node block we had stored.
             }

         Free = Next; //Store the node we just found.
        }

 this ->gRoot = 0;
 return 1;
}}

/*
 This returns a newly allocated node.
 If you pass a ulDataSize of 0, Node ->lpData is a pointer to lpData.
 If ulDataSize is >0, Node ->lpData is a pointer to a dynamically allocated
 memory block that has the memory of lpData copied to it.

 If the node has a ulDataSize >0 it will free the dynamically allocated memory for
 you when calling Free / FreeEx / FreeNode / FreeNodeEx.
 If the node has ulDataSize = 0, it won't crash by trying to free the memory that isn't there.
 The free functions also check to see if the pointer is still valid (not 0 [NULL]) before freeing.

 -You do NOT have to pass a name for this node, it's just for using functions to call find it
 later like FindNodeByAssoc.
*/
 LINKED_LIST *LINKED_LIST_CLASS::NewNode ( unsigned short *optionalAssocName, void *lpData, unsigned long ulDataSize, LINKED_LIST *Prev )
{{
 LINKED_LIST *NewNode = 0;

 if ( ! (NewNode = (LINKED_LIST *) malloc ( sizeof ( LINKED_LIST_S ) )) )
      return 0;

 //Set all of the flags to FALSE so that we can | them in as needed.
 NewNode ->bFlags = 0;

 //Default to no associative name.
 NewNode ->AssocName = this ->gNoAssocName;

 if ( optionalAssocName )
     {
      if ( ! (NewNode ->AssocName = (unsigned short *) malloc ( this ->GetUnicodeStringSize ( optionalAssocName ) )) )
          {
           free ( NewNode );

           return 0;
          }

      //Copy the associative name over to the node.
      wcscpy ( NewNode ->AssocName, optionalAssocName );

      NewNode ->bFlags |= LINKED_LIST_HAS_ASSOC_NAME;
     }

 //If a size was specified for the data in this node, allocate space for it, and copy it over.
 if ( NewNode ->ulDataSize = ulDataSize )
     {
      if ( ! AllocateVariableBlock ( NewNode ) )
          {
           free ( NewNode );

           return 0;
          }

      memcpy ( NewNode ->lpData, lpData, NewNode ->ulDataSize );
     }
 else NewNode ->lpData = lpData; //Point to the data if no size was specified.

 //Note that LINKED_LIST_FREEABLE_VARIABLE_BLOCK doesn't mean you can free Node ->lpData for sure,
 //Node ->ulDataSize must be > 0 aswell!
 NewNode ->bFlags |= LINKED_LIST_FREEABLE_VARIABLE_BLOCK | LINKED_LIST_FREEABLE_STRUCTURE;
 NewNode ->ulId = this ->gNextId ++;

 NewNode ->Prev = Prev; //The previous can be 0 (NULL) if this is the first node in the list.
 NewNode ->Next = 0; //Next is always 0 (NULL), since the new nodes are always added to the end of the list.

 return NewNode; //Return the new node on success.
}}

/*
 Returns the parent of the variable data block (the node in the linked list.)

 WORKS.

 Example Usage:
 ==============
 wprintf ( L"\r\n\r\n\r\n" );

 wprintf ( L"Trying to obtain the lpData (child) of the linked list node...\r\n" );
 SOURCE_FILE_LIST *ChildTest = ((SOURCE_FILE_LIST *) (gSourceFileList ->lpData));
 wprintf ( L"Here's the Index of [%s]: %ld.\r\n\r\n", ChildTest ->SourceFileName,  gSourceFileList ->ulId );

 wprintf ( L"Trying to obtain the  node (parent) that this lpData (child) is contained within...\r\n" );

 LINKED_LIST *ParentTest = GetParent ( ChildTest, sizeof ( SOURCE_FILE_LIST ) );
 wprintf ( L"Here's the Index of [%s]: %ld.\r\n", ((SOURCE_FILE_LIST *) ParentTest ->lpData) ->SourceFileName, ParentTest ->ulId );

 wprintf ( L"\r\n\r\n\r\n" );
*/
 LINKED_LIST *LINKED_LIST_CLASS::GetParent ( void *lpData )
{{
 //(char **) ((char*) Address - #) -- handle as (char **) but handle arithmetic as a standard (char *) pointer would.
 char **p = (char **) ((char *) lpData - LINKED_LIST_PTR_SIZE);

 //Handle as a linked list pointer, to the location in memory pointed to as a (char *) ---> a derefrenced (char **)...
 return (LINKED_LIST *) *p;
}}

/*
 Add a new node to the end of the list.

 -If ulDataSize is 0 then the NewNode ->lpData is a pointer the lpData passed in this function.
 -If the ulDataSize is > 0 then the NewNode ->lpData is a pointer to a newly allocated block of
 memory that had the data from lpData copied to it.

 -Returns 0 on failure.
 -Returns 1 on success.
*/
 unsigned char LINKED_LIST_CLASS::AddNode ( void *lpData, unsigned long ulDataSize )
{{
 LINKED_LIST *Node = this ->gRoot;

 if ( ! Node )
     {
      if ( this ->gRoot = NewNode ( 0, lpData, ulDataSize, 0 ) )
           return 1;

      return 0;
     }

 //Find the end of the linked list.
 while ( Node ->Next )
         Node = Node ->Next;

 //Attatch a new node to the end of the list.
 if ( Node ->Next = NewNode ( 0, lpData, ulDataSize, Node ) )
      return 1;

 return 0;
}}

/*
 Add a new node to the end of the list.

 -If ulDataSize is 0 then the NewNode ->lpData is a pointer the lpData passed in this function.
 -If the ulDataSize is > 0 then the NewNode ->lpData is a pointer to a newly allocated block of
 memory that had the data from lpData copied to it.

 -Returns 0 on failure.
 -Returns 1 on success.
*/
 unsigned char LINKED_LIST_CLASS::AddAssocNode ( unsigned short *AssocName, void *lpData, unsigned long ulDataSize )
{{
 LINKED_LIST *Node = this ->gRoot;

 if ( ! Node )
     {
      if ( this ->gRoot = NewNode ( AssocName, lpData, ulDataSize, 0 ) )
           return 1;

      return 0;
     }

 //Find the end of the linked list.
 while ( Node ->Next )
         Node = Node ->Next;

 //Attatch a new node to the end of the list.
 if ( Node ->Next = NewNode ( AssocName, lpData, ulDataSize, Node ) )
      return 1;

 return 0;
}}

/*
 Pass this function any node in the list, and it will go backwards
 until it finds the root node, then return a pointer to it.
*/
 LINKED_LIST *LINKED_LIST_CLASS::GetRoot ( LINKED_LIST *SomeNode )
{{
 if ( ! SomeNode )
      return 0;

 LINKED_LIST *Node = SomeNode;

 while ( Node ->Prev )
         Node = Node ->Prev;

 return Node;
}}

/*
 Pass this function any node in the list, and it will go backwards
 until it finds the root node, then return a pointer to it.
*/
 LINKED_LIST *LINKED_LIST_CLASS::GetEnd ( void )
{{
 LINKED_LIST *Node = this ->gRoot;

 while ( Node ->Next )
         Node = Node ->Next;

 return Node;
}}

/*
 Finds the node with the association you pass.
*/
 LINKED_LIST *LINKED_LIST_CLASS::FindNodeByAssoc ( unsigned short *AssocName )
{{
 LINKED_LIST *Node = this ->gRoot;

 while ( Node )
        {
         if ( Node ->bFlags & LINKED_LIST_HAS_ASSOC_NAME )
             {
              if ( ! wcscmp ( Node ->AssocName, AssocName ) )
                   return Node;
             }

         Node = Node ->Next;
        }

 //Doesn't exist in the list.
 return 0;
}}

/*
 Adds an association between this node and some name.
*/
 unsigned char LINKED_LIST_CLASS::AssociateNode ( LINKED_LIST *Node, unsigned short *AssocName )
{{
 if ( ! Node )
      return 0;

 unsigned long Size = GetUnicodeStringSize ( AssocName );

 if ( ! this ->gRoot )
     {
      MessageBoxW ( NULL, L"Error: Couldn't find the root of the list to check if an association exists already.", L"Error:", MB_OK );
      return 0;
     }

 //If we couldn't add the association becuase it already exists in the linked list...
 if ( FindNodeByAssoc ( AssocName ) )
     {
      MessageBoxW ( NULL, L"Error: Couldn't associate a linked list node with a name, becuse the name already exists in the linked list.", L"Error:", MB_OK );

      return 0;
     }

 //If this node already has an association with another name, free it.
 if ( Node ->AssocName && Node ->bFlags & LINKED_LIST_HAS_ASSOC_NAME )
      free ( Node ->AssocName );

 if ( ! AssocName )
     {
      Node ->AssocName = gNoAssocName;
      Node ->bFlags &= ~LINKED_LIST_HAS_ASSOC_NAME;

      return 1;
     }

 //Try and allocate memory to hold the new associatiave name.
 if ( ! (Node ->AssocName = (unsigned short *) malloc ( Size )) )
      return 0;

 //Clear the newly allocated buffer.
 memset ( Node ->AssocName, 0, Size );

 //Copy the name over.
 wcscpy ( Node ->AssocName, AssocName );

 //Set a flag so that we know to free the association later.
 Node ->bFlags |= LINKED_LIST_HAS_ASSOC_NAME;

 return 1;
}}

/*
 This sets the Node ->lpData pointer to what you specify.

 -If a block of memory is already allocated and being pointed to
 by Node ->lpData:
 *The variable memory block is freed.

 If ( ulDataSize > 0 ) New memory is allocated to hold the data.
 Next, the data is copied to the newly allocated memory block.

 If ( ulDataSize == 0 ) -->The pointer, Node ->lpData is redirected
 to the lpData parameter passed.

 The Node ->ulDataSize variable is always set to whatever is specified.
*/
 unsigned char LINKED_LIST_CLASS::SetNodeVariableBlock ( LINKED_LIST *Node, void *lpData, unsigned long ulDataSize )
{{
 if ( (Node ->bFlags & LINKED_LIST_MASS_ALLOCATED) &&
      ulDataSize != Node ->ulDataSize )
     {
      //MessageBoxW ( NULL, L"Error: You may not call SetNodeVariableBlock()\r\n" \
      //                    L"on a node that was allocated in mass via NewGroup().\r\n\r\n", L"Error:", MB_OK );
      wprintf ( L"You may not call SetNodeVariableBlock() if your node is a group allocated node, and the datasizes are different.\r\n" );

      return 0;
     }

 //If we had memory allocated we need to free it before making more or before redirecting the pointer.
 if ( ! (Node ->bFlags & LINKED_LIST_MASS_ALLOCATED) )
      FreeVariableBlock ( Node );

 //If we need to allocate new memory:
 if ( Node ->ulDataSize = ulDataSize )
     {
      if ( ! AllocateVariableBlock ( Node ) )
           return 0;

      memcpy ( Node ->lpData, lpData, ulDataSize );

      return 1;
     }

 //Redirect the pointer.
 Node ->lpData = lpData;

 return 1;
}}

/*
 This sets the Node ->lpData pointer to what you specify.

 IMPORTANT NOTE: This function will only call the FreeCallback
 function if the Node ->ulDataSize member is > 0. This means
 if you make a linked list of pointers to another linked list,
 calling this function will not free the complex regions of
 the node before setting the Node ->lpData and Node ->ulDataSize
 members.

 tl;dr: This does not free the complex regions of the nodes
 that are arbitrary pointers to something; it only frees the
 complex regions of nodes whose Node ->lpData points to an
 allocated block and whose Node ->ulDataSize is > 0 to show
 that this is the case.

 USAGE NOTE: This function is intended to be used with Complex
 linked lists that have dynamically allocated members within
 the structure of the Node ->lpData to be handled outside of
 the basic  functions.

 -If a block of memory is already allocated and being pointed to
 by Node ->lpData:
 *The FreeCallback function is called to handle any dynamically
  allocated memory stored within the block.
 *The variable memory block is freed.

 If ( ulDataSize > 0 ) New memory is allocated to hold the data.
 Next, the data is copied to the newly allocated memory block.

 If ( ulDataSize == 0 ) -->The pointer, Node ->lpData is redirected
 to the lpData parameter passed.

 The Node ->ulDataSize variable is always set to whatever is specified.
*/
 unsigned char LINKED_LIST_CLASS::SetNodeVariableBlockEx ( LINKED_LIST *Node, void *lpData, unsigned long ulDataSize )
{{
 if ( (Node ->bFlags & LINKED_LIST_MASS_ALLOCATED) &&
      Node ->ulDataSize != ulDataSize )
     {
      MessageBoxW ( NULL, L"Error: You may not call SetNodeVariableBlockEx()\r\n" \
                          L"on a node that was allocated in mass via NewGroup().\r\n\r\n", L"Error:", MB_OK );

      return 0;
     }

 //If we had memory allocated we need to free it before making more or before redirecting the pointer.
 if ( Node ->ulDataSize )
     {
      if ( ! FreeCallback ( Node ->lpData ) )
           return 0;

      if ( ! (Node ->bFlags & LINKED_LIST_MASS_ALLOCATED) )
           FreeVariableBlock ( Node );
      //else return WAS_ALREADY_FREED; //maybe have a warning for undefined behavior?
     }

 //If we need to allocate new memory:
 if ( Node ->ulDataSize = ulDataSize )
     {
      if ( ! AllocateVariableBlock ( Node ) )
           return 0;

      memcpy ( Node ->lpData, lpData, ulDataSize );

      return 1;
     }

 //Redirect the pointer.
 Node ->lpData = lpData;

 return 1;
}}

/*
 This copies the variable block (Node ->lpData) to the data location specified.

 Example usage:
 ==============
 VARIABLE_LIST VariableNode = 0;
 GetNodeVariableBlock ( SomeVariableList, &VariableNode );

 wprintf ( L"Variable Name: \"%s\"\n", VariableNode .VariableName );
*/
 void LINKED_LIST_CLASS::GetNodeVariableBlock ( LINKED_LIST *Node, void *outlpData )
{{
 //Copy the data over to the location specified.
 memcpy ( outlpData, Node ->lpData, Node ->ulDataSize );
}}

/*
 Example Usage:
 ==============
 //Initialize the variables to 0.
 LINKED_LIST *EnumNode = 0;

 //Pass the root of the list to enumerate, a pointer to the enumeration node, and cast
 //a >>> (void **) & <<< to the Node you want returned.
 while ( EnumerateByVariableBlock ( SomeListToEnumerate, &EnumNode ) )
        {
         SOME_LIST *SomeListNode = (SOME_LIST *) EnumNode ->lpData;

         wprintf ( L"\"%s\" --> \"%s\"\r\n\r\n", SomeListNode ->SomeName, SomeListNode ->SomeString );
        }
*/
 unsigned char LINKED_LIST_CLASS::Enumerate ( LINKED_LIST **EnumerationNode )
{{
 if ( ! (*EnumerationNode) )
     {
      (*EnumerationNode) = this ->gRoot;

      if ( this ->gRoot )
           return 1;

      return 0;
     }

 if ( (*EnumerationNode) ->Next )
     {
      (*EnumerationNode) = (*EnumerationNode) ->Next;

      return 1;
     }
 return 0;
}}

/*
 Example Usage:
 ==============
 //Initialize the variables to 0.
 SOME_LIST *SomeListNode = 0;

 //Pass the root of the list to enumerate, and cast (void **) &Node you want returned.
 while ( EnumerateByVariableBlock ( SomeListToEnumerate, (void **) &SomeListNode ) )
         wprintf ( L"\"%s\" --> \"%s\"\r\n\r\n", SomeListNode ->SomeName, SomeListNode ->SomeString );
*/
 unsigned char LINKED_LIST_CLASS::EnumerateByVariableBlock ( void **lpVariableBlock )
{{
 if ( ! (*lpVariableBlock) )
     {
      if ( (*lpVariableBlock) = this ->gRoot ->lpData )
           return 1;
      return 0;
     }

 LINKED_LIST *Parent = GetParent ( (*lpVariableBlock) );
 if ( ! Parent )
      return 0;

 if ( ! Parent ->Next )
      return 0;

 (*lpVariableBlock) = Parent ->Next ->lpData;

 if ( (*lpVariableBlock) )
      return 1;

 return 0;
}}

/*
 NOTE: This version of the function passes the linked list node itself.

 Calls the callback function for each node in the list, to eliminate the need to
 keep creating the same while-loop format for every recursive function with the
 linked lists.
*/
 void LINKED_LIST_CLASS::Foreach ( ForeachCallback ForeachFunction )
{{
 LINKED_LIST *Node = this ->gRoot;

 while ( Node )
        {
         ForeachFunction ( Node );

         Node = Node ->Next;
        }
}}

/*
 NOTE: This version of the function passes the complex regions to the callback
 function.

 Calls the callback function for each node in the list, to eliminate the need to
 keep creating the same while-loop format for every recursive function with the
 linked lists.
*/
 void LINKED_LIST_CLASS::ForeachByVariableBlock ( ForeachCallback ForeachFunction )
{{
 LINKED_LIST *Node = this ->gRoot;

 while ( Node )
        {
         ForeachFunction ( Node ->lpData );

         Node = Node ->Next;
        }
}}

/*
 Returns the node in the list with the ID you specify.
 Returns 0 on failure to find the node with the ID requested.
*/
 LINKED_LIST *LINKED_LIST_CLASS::FindNodeById ( unsigned long ulId )
{{
 LINKED_LIST *Node = this ->gRoot;
 while ( Node )
        {
         if ( Node ->ulId == ulId )
              return Node;

         Node = Node ->Next;
        }

 return 0;
}}

/*
 If the compare function passed ever returns true before the list ends,
 the node that caused this action to happen on is returned.
*/
 LINKED_LIST *LINKED_LIST_CLASS::FindNode ( void *SearchData, CompareCallback CompareCallback )
{{
 LINKED_LIST *Node = this ->gRoot;

 //Loop until a node who passes the user-defined callback function's test is found.
 while ( Node )
        {
         if ( CompareCallback ( Node ->lpData, SearchData ) )
              return Node;

         Node = Node ->Next;
        }

 return 0;
}}

/*
 This function ensures that you don't add the same type of item to the linked list twice by mistake.
 Simply pass the same parameters you normally would to AddNode() but with a callback function
 used to compare the contents VariableBlock of existing nodes with the one you think you may want to add.

 If no match is found it will add it, otherwise it will return success without adding it.
*/
 unsigned char LINKED_LIST_CLASS::AddUniqueNodeEx ( void *lpData, unsigned long ulDataSize, CompareCallback CompareCallback )
{{
 if ( FindNode ( lpData, CompareCallback ) )
      return ERROR_NODE_EXISTS;

 return AddNode (lpData, ulDataSize );
}}

/*
 Uses UniqueCompareCallback() to see if a node exists in the linked list.
 returns 1 if it does.
 returns 0 if it doesn't.
*/
 unsigned char LINKED_LIST_CLASS::NodeExists ( void *lpVariableBlock )
{{
 LINKED_LIST *Node = this ->gRoot;

 //Loop until a node who passes the user-defined callback function's test is found.
 while ( Node )
        {
         if ( UniqueCompareCallback ( Node ->lpData, lpVariableBlock ) )
              return 1;

         Node = Node ->Next;
        }

 return 0;
}}

/*
 This should be subclassed before calling AddUniqueNode().
*/
 unsigned char LINKED_LIST_CLASS::UniqueCompareCallback ( void *lpVariableBlock1, void *lpVariableBlock2 )
{{
/*
 //Example subclassed function code.
 SOME_STRUCT *a, *b;

 a = (SOME_STRUCT *) lpVariableBlock1;
 b = (SOME_STRUCT *) lpVariableBlock2;

 if ( ! strcmp ( a ->Name, b ->Name ) )
      return 1;

 return 0;
*/

 MessageBoxW ( NULL, L"UniqueCompareCallback was not subclassed, so all nodes are currently considered unique.", L"Note:", MB_OK );

 return 0;
}}

/*
 This function ensures that you don't add the same type of item to the linked list twice by mistake.
 Simply pass the same parameters you normally would to AddNode() but with a callback function
 used to compare the contents VariableBlock of existing nodes with the one you think you may want to add.

 If no match is found it will add it, otherwise it will return success without adding it.
*/
 unsigned char LINKED_LIST_CLASS::AddUniqueNode ( void *lpData, unsigned long ulDataSize )
{{
 if ( NodeExists ( lpData ) )
     {
      FreeCallback ( lpData );
      return ERROR_NODE_EXISTS;
     }

 return AddNode ( lpData, ulDataSize );
}}

/*
 Adds the root node of ToAdd to the last node of Target ->Next.

 This also redirects ToAdd to the start of the Target list once
 the two linked lists are combined.

 NOTE: This function MUST be called with two linked lists of the
 same type. If the Node ->ulDataSize is different, the function
 is programmed to automatically fail!
*/
 unsigned char LINKED_LIST_CLASS::Append ( LINKED_LIST **Target, LINKED_LIST **ToAdd )
{{
 LINKED_LIST *Node = (*Target);

 //If both lists exist.
 if ( (*Target) && (*ToAdd) )
     {
      //If their payloads are of different sizes (types).
      if ( (*Target) ->ulDataSize != (*ToAdd) ->ulDataSize )
          {
           #if ( DEBUG_MODE )
                 wprintf ( L"Append() was called on lists of two differnt data sizes,\r\nwhich probably means two differnt types; failing for safety.\r\n" );
           #endif

           //Mixing different list types is dangerous so fail.
           return 0;
          }
     }

 //If the root of the first list is 0, start the first at the second.
 if ( ! Node )
     {
      (*Target) = (*ToAdd);

      return 1;
     }

 //If the first list exists, find the end of it.
 while ( Node ->Next )
         Node = Node ->Next;

 //Attatch the start of the second list to the end of the first.
 Node ->Next = (*ToAdd);

 //Redirect the second list to point to the start of the newly
 //attatched list.
 (*ToAdd) = (*Target);

 //Return success.
 return 1;
}}

/*
 Frees the entire linked list.
*/
 unsigned char LINKED_LIST_CLASS::Free ( void )
{{
 if ( ! this ->gRoot )
      return 0;

 if ( ListContainsGroupAllocationNode (  ) )
     {
      FreeGroup (  );
      return 0;
     }

 LINKED_LIST *Next = this ->gRoot, *Free = 0;

 while ( Next )
        {
         Free = Next;
         Next = Next ->Next;

         //Use a specialized function to free() the variable block because of the way it was allocated.
         this ->FreeVariableBlock ( Free );

         //Free the associative name for this node, if it exists.
         if ( Free ->AssocName && Free ->bFlags & LINKED_LIST_HAS_ASSOC_NAME )
             {
              //wprintf ( L"Freeing the association in node [%ld].\r\n", Free ->ulId );

              free ( Free ->AssocName );
             }

         //Free this linked list node itself.
         free ( Free );
        }
 this ->gRoot = 0;
 return 1;
}}

/*
 Frees a single node in the linked list, and retains the integrity of
 the linked list while doing so.
*/
 unsigned char LINKED_LIST_CLASS::FreeNode ( LINKED_LIST **Node )
{{
 if ( ! Node )
      return 0;

 LINKED_LIST *FreeNode = (*Node);

 if ( ! FreeNode )
      return 0;

 //Point the previous node to the next node, if the previous node exists.
 if ( FreeNode ->Prev )
      FreeNode ->Prev ->Next = FreeNode ->Next;
 else (*Node) = (*Node) ->Next;

 //Point the next node to the previous, if the next node exists.
 if ( FreeNode ->Next )
      FreeNode ->Next ->Prev = FreeNode ->Prev;

 //Free the associative name for this node, if it exists.
 if ( FreeNode ->AssocName && FreeNode ->bFlags & LINKED_LIST_HAS_ASSOC_NAME )
      free ( FreeNode ->AssocName );

 //Free the lpData parameter (the variable data) within this node, if any memory was allocated for it.
 FreeVariableBlock ( FreeNode );

 //Free this node itself.
 free ( FreeNode );

 return 1;
}}

/*
 Frees the entire complex linked list.
*/
 unsigned char LINKED_LIST_CLASS::FreeEx ( void )
{{
 if ( ! this ->gRoot )
      return 0;

 if ( ListContainsGroupAllocationNode (  ) )
     {
      FreeGroupEx (  );
      return 0;
     }

 LINKED_LIST *Next = this ->gRoot, *Free = 0;
 unsigned char Fail = 0;

 while ( Next )
        {
         Free = Next;
         Next = Next ->Next;

         //Let the user-defined callback free any dynamically allocated memory within the complex region.
         if ( ! this ->FreeCallback ( Free ->lpData ) )
              Fail = 1;

         //Free the associative name for this node, if it exists.
         if ( Free ->AssocName && Free ->bFlags & LINKED_LIST_HAS_ASSOC_NAME )
              free ( Free ->AssocName );

         //Use a specialized function to free() the variable block because of the way it was allocated.
         FreeVariableBlock ( Free );

         //Free this linked list node itself.
         free ( Free );
        }

 this ->gRoot = 0;

 return Fail;
}}

/*
 Frees a single node in the complex linked list, and retains the integrity of
 the linked list while doing so.
*/
 unsigned char LINKED_LIST_CLASS::FreeNodeEx ( LINKED_LIST **Node )
{{
 if ( ! Node )
      return 0;

 LINKED_LIST *FreeNode = (*Node);
 unsigned char Fail = 0;

 if ( ! FreeNode )
      return 0;

 //Point the previous node to the next node, if the previous node exists.
 if ( FreeNode ->Prev )
      FreeNode ->Prev ->Next = FreeNode ->Next;
 else (*Node) = (*Node) ->Next;

 //Point the next node to the previous, if the next node exists.
 if ( FreeNode ->Next )
      FreeNode ->Next ->Prev = FreeNode ->Prev;

 //Let the user-defined callback free any dynamically allocated memory within the complex region.
 if ( ! this ->FreeCallback ( FreeNode ->lpData ) )
      Fail = 1;

 //Free the associative name for this node, if it exists.
 if ( FreeNode ->AssocName && FreeNode ->bFlags & LINKED_LIST_HAS_ASSOC_NAME )
      free ( FreeNode ->AssocName );

 //Free the lpData parameter (the variable data) within this node, if any memory was allocated for it.
 FreeVariableBlock ( FreeNode );

 //Free this node itself.
 free ( FreeNode );

 return Fail;
}}

/*
 O.M.G this was the hardest thing in the world to code!

 *This first swaps the memory locations of the two nodes in question that
 are embedded into the first LINKED_LIST_PTR_SIZE bytes of the complex region.
 *Next is swaps the pointers to the complex variable block (after the parent
 pointer nodes).
 *Lastly it swaps the datasize variables, since they may each point to different
 sized data now.
*/
 void LINKED_LIST_CLASS::SwapNodes ( LINKED_LIST *Node1, LINKED_LIST *Node2 )
{{
 if ( Node1 == Node2 )
      return ;

 //Store all of Node1's attributes for the swapping.
 LINKED_LIST *Temp = Node1;
 unsigned long ulDataSize = Node1 ->ulDataSize;
 void *lpData = Node1 ->lpData;
 unsigned long bFreeableVariableBlock = (Node1 ->bFlags & LINKED_LIST_FREEABLE_VARIABLE_BLOCK);
 unsigned long bHasAssoc = (Node1 ->bFlags &LINKED_LIST_HAS_ASSOC_NAME);

 //Swap the parent pointers before the variable block.
 char **p = (char **)((char *) Node1 ->lpData - LINKED_LIST_PTR_SIZE);
 *p = (char *) Node2;
 p = (char **)((char *) Node2 ->lpData - LINKED_LIST_PTR_SIZE);
 *p = (char *) Temp;

 //Swap the data blocks.
 Node1 ->lpData = Node2 ->lpData;
 Node2 ->lpData = lpData;

 //Swap the data size variables.
 Node1 ->ulDataSize = Node2 ->ulDataSize;
 Node2 ->ulDataSize = ulDataSize;

 //Swap the allocation status of the two nodes.
 Node1 ->bFlags &= ~LINKED_LIST_FREEABLE_VARIABLE_BLOCK; //Unflag the ability to free the variable block.
 Node1 ->bFlags |= Node2 ->bFlags & LINKED_LIST_FREEABLE_VARIABLE_BLOCK; //Set the flag back if it exists in the swapnode.

 Node2 ->bFlags &= ~LINKED_LIST_FREEABLE_VARIABLE_BLOCK; //Unflag the ability to free the variable block.
 Node2 ->bFlags |= bFreeableVariableBlock; //Set the flag back if it exists in the swapnode.

 //Swap the assoc name pointers.
 unsigned short *AssocName = Node1 ->AssocName;
 Node1 ->AssocName = Node2 ->AssocName;
 Node2 ->AssocName = AssocName;

 //Swap the association flags of the two nodes.
 Node1 ->bFlags &= ~LINKED_LIST_HAS_ASSOC_NAME;
 Node1 ->bFlags |= Node2 ->bFlags & LINKED_LIST_HAS_ASSOC_NAME;

 Node2 ->bFlags &= ~LINKED_LIST_HAS_ASSOC_NAME;
 Node2 ->bFlags |= bHasAssoc;
}}

/*
 This function checks to see if String is before OtherString.
*/
 unsigned long LINKED_LIST_CLASS::StringIsFirst ( unsigned short *String, unsigned short *OtherString )
{{
 unsigned long i = 0;
 unsigned long Len = wcslen ( String );
 unsigned long OtherLen = wcslen ( OtherString );

 if ( OtherLen < Len )
     {
      for ( i = 0; i < OtherLen; i += 1 )
           {
            if ( OtherString [ i ] < String [ i ] )
                 return 0;
            if ( OtherString [ i ] > String [ i ] )
                 return 1;

            //If they are equal, keep looping.
           }

      return 1;
     }

 for ( i = 0; i < Len; i += 1 )
      {
       if ( OtherString [ i ] < String [ i ] )
            return 0;
       if ( OtherString [ i ] > String [ i ] )
            return 1;

       //If they are equal, keep looping.
      }

 return 1;
}}

/*
 Same as StringIsFirst, but it will consider I the same as i,
 so if you have some lowercase and upercase things to sort
 you wont have two lists going back to back...
*/
 unsigned long LINKED_LIST_CLASS::StringIsFirstInsensitive ( unsigned short *String, unsigned short *OtherString )
{{
 unsigned long i = 0;
 unsigned long Len = wcslen ( String );
 unsigned long OtherLen = wcslen ( OtherString );

 if ( OtherLen < Len )
     {
      for ( i = 0; i < OtherLen; i += 1 )
           {
            if ( towupper ( OtherString [ i ] ) < towupper ( String [ i ] ) )
                 return 0;
            if ( towupper ( OtherString [ i ] ) > towupper ( String [ i ] ) )
                 return 1;

            //If they are equal, keep looping.
           }

      return 1;
     }

 for ( i = 0; i < Len; i += 1 )
      {
       if ( towupper ( OtherString [ i ] ) < towupper ( String [ i ] ) )
            return 0;
       if ( towupper ( OtherString [ i ] ) > towupper ( String [ i ] ) )
            return 1;

       //If they are equal, keep looping.
      }

 return 1;
}}

/*
 You pass this function the list to sort, and a function that returns a
 string to sort with from the inside of the variable block structure that
 the generic linked list functions have no clue about.

 You can chose to sort case insensitve or case sensitively.
*/
 void LINKED_LIST_CLASS::LongSortEx ( LongFromVariableBlockCallback Callback )
{{
 LINKED_LIST *Node = 0;
 long Num1 = 0, Num2 = 0;
 unsigned char bSwapHappend = 1;

 //Leep looping until a swap doesn't happen.
 while ( bSwapHappend )
        {
         bSwapHappend = 0;
         Node = 0;

         //Enumerate through the list, and swap any out-of-order nodes on the way through.
         while ( Enumerate ( &Node ) )
                {
                 //If this is the end of the list, this node can't be out of order with the one behind it...
                 if ( ! Node ->Next )
                      break;

                 Num1 = Callback ( Node ->lpData );
                 Num2 = Callback ( Node ->Next ->lpData );

                 //wprintf ( L"Comparing to see if [%ld] comes before [%ld].\r\n", Num1, Num2 );

                 //If the string that is first in the list shouldn't be, then swap it with the one ahead of it.
                 if ( Num1 > Num2 )
                     {
                      //wprintf ( L"As it turns out, it shouldn't, attempting to swap the nodes.\r\n" );

                      //Swap the out-of-order nodes.
                      SwapNodes ( Node, Node ->Next );

                      //Remember that a swap happened so that we can keep looping.
                      bSwapHappend = 1;
                     }
                 //else wprintf ( L"It appears that we were fine and didn't need to swap after all...\r\n" );
                }
        }
}}


/*
 You pass this function the list to sort, and a function that returns a
 string to sort with from the inside of the variable block structure that
 the generic linked list functions have no clue about.

 You can chose to sort case insensitve or case sensitively.
*/
 void LINKED_LIST_CLASS::LongSort ( void )
{{
 LINKED_LIST *Node = 0;
 long Num1 = 0, Num2 = 0;
 unsigned char bSwapHappend = 1;

 //Leep looping until a swap doesn't happen.
 while ( bSwapHappend )
        {
         bSwapHappend = 0;
         Node = 0;

         //Enumerate through the list, and swap any out-of-order nodes on the way through.
         while ( Enumerate ( &Node ) )
                {
                 //If this is the end of the list, this node can't be out of order with the one behind it...
                 if ( ! Node ->Next )
                      break;

                 if ( ! GetSortLongFromVariableBlock ( Node ->lpData, &Num1 ) )
                      return ;

                 if ( ! GetSortLongFromVariableBlock ( Node ->Next ->lpData, &Num2 ) )
                      return ;

                 //wprintf ( L"Comparing to see if [%ld] comes before [%ld].\r\n", Num1, Num2 );

                 //If the string that is first in the list shouldn't be, then swap it with the one ahead of it.
                 if ( Num1 > Num2 )
                     {
                      //wprintf ( L"As it turns out, it shouldn't, attempting to swap the nodes.\r\n" );

                      //Swap the out-of-order nodes.
                      SwapNodes ( Node, Node ->Next );

                      //Remember that a swap happened so that we can keep looping.
                      bSwapHappend = 1;
                     }
                 //else wprintf ( L"It appears that we were fine and didn't need to swap after all...\r\n" );
                }
        }
}}

/*
 You pass this function the list to sort, and a function that returns a
 string to sort with from the inside of the variable block structure that
 the generic linked list functions have no clue about.

 You can chose to sort case insensitve or case sensitively.
*/
 void LINKED_LIST_CLASS::AlphaSortEx ( unsigned char bCaseSensitiveSort, StringFromVariableBlockCallback GetStringFromVariableBlock )
{{
 LINKED_LIST *Node = 0;
 unsigned char bSwapHappend = 1;

 //Leep looping until a swap doesn't happen.
 while ( bSwapHappend )
        {
         bSwapHappend = 0;
         Node = 0;

         //Enumerate through the list, and swap any out-of-order nodes on the way through.
         while ( Enumerate ( &Node ) )
                {
                 //If this is the end of the list, this node can't be out of order with the one behind it...
                 if ( ! Node ->Next )
                      break;

                 unsigned short *String1, *String2;
                 String1 = GetStringFromVariableBlock ( Node ->lpData );
                 String2 = GetStringFromVariableBlock ( Node ->Next ->lpData );

                 //wprintf ( L"Comparing to see if [%s] comes before [%s].\r\n", String1, String2 );

                 //If the string that is first in the list shouldn't be, then swap it with the one ahead of it.
                 if ( bCaseSensitiveSort )
                     {
                      if ( ! StringIsFirst ( String1, String2 ) )
                          {
                           //Swap the out-of-order nodes.
                           SwapNodes ( Node, Node ->Next );

                           //Remember that a swap happened so that we can keep looping.
                           bSwapHappend = 1;
                          }
                     }
                 else if ( ! StringIsFirstInsensitive ( String1, String2 ) )
                     {
                      //wprintf ( L"As it turns out, it shouldn't, attempting to swap the nodes.\r\n" );

                      //Swap the out-of-order nodes.
                      SwapNodes ( Node, Node ->Next );

                      //Remember that a swap happened so that we can keep looping.
                      bSwapHappend = 1;
                     }
                 //else wprintf ( L"It appears that we were fine and didn't need to swap after all...\r\n" );
                }
        }
}}

/*
 You must have called this ->SetAlphaSortStringOffset (  ) for
 this function to work.

 You can chose to sort case insensitve or case sensitively.
*/
 unsigned char LINKED_LIST_CLASS::AlphaSort ( unsigned char bCaseSensitiveSort )
{{
 LINKED_LIST *Node = 0;
 unsigned char bSwapHappend = 1;

 //Leep looping until a swap doesn't happen.
 while ( bSwapHappend )
        {
         bSwapHappend = 0;
         Node = 0;

         //Enumerate through the list, and swap any out-of-order nodes on the way through.
         while ( Enumerate ( &Node ) )
                {
                 //If this is the end of the list, this node can't be out of order with the one behind it...
                 if ( ! Node ->Next )
                      break;

                 unsigned short *String1, *String2;
                 String1 = GetAlphaSortStringFromVariableBlock ( Node ->lpData );
                 //wprintf ( L"Comparing to see if [%s] ", String1 );

                 String2 = GetAlphaSortStringFromVariableBlock ( Node ->Next ->lpData );
                 //wprintf ( L"comes before [%s].\r\n", String2 );

                 //Don't crash to null string sorting.
                 if ( ! String1 || ! String2 )
                      return 0;

                 //wprintf ( L"Comparing to see if [%s] comes before [%s].\r\n", String1, String2 );

                 //If the string that is first in the list shouldn't be, then swap it with the one ahead of it.
                 if ( bCaseSensitiveSort )
                     {
                      if ( ! StringIsFirst ( String1, String2 ) )
                          {
                           //Swap the out-of-order nodes.
                           SwapNodes ( Node, Node ->Next );

                           //Remember that a swap happened so that we can keep looping.
                           bSwapHappend = 1;
                          }
                     }
                 else if ( ! StringIsFirstInsensitive ( String1, String2 ) )
                     {
                      //wprintf ( L"As it turns out, it shouldn't, attempting to swap the nodes.\r\n" );

                      //Swap the out-of-order nodes.
                      SwapNodes ( Node, Node ->Next );

                      //Remember that a swap happened so that we can keep looping.
                      bSwapHappend = 1;
                     }
                 //else wprintf ( L"It appears that we were fine and didn't need to swap after all...\r\n" );
                }
        }
 return 1;
}}

/*
 This function should be subclassed.
*/
 unsigned short *LINKED_LIST_CLASS::GetAlphaSortStringFromVariableBlock ( void *lpVariableBlock )
{{
/*
 //Code should look like this in the subclass:

 SOME_STRUCTURE *pStruct = (SOME_STRUCTURE *) lpVariableBlock;
 return pStruct ->StringToSortWith;
*/
 MessageBoxW ( NULL, L"Error: GetAlphaSortStringFromVariableBlock() was not overwritten by the subclass;\r\nwe are unable to retrieve the string from the block.", L"Error:", MB_OK );

 return 0;
}}

/*
 This function should be subclassed.
*/
 unsigned char LINKED_LIST_CLASS::GetSortLongFromVariableBlock ( void *lpVariableBlock, long *outLong )
{{
/*
 //Code should look like this in the subclass:

 SOME_STRUCTURE *pStruct = (SOME_STRUCTURE *) lpVariableBlock;
 return pStruct ->LongToSortWith;
*/
 MessageBoxW ( NULL, L"Error: GetSortLongFromVariableBlock() was not overwritten by the subclass;\r\nwe are unable to retrieve the long from the block.", L"Error:", MB_OK );

 *outLong = 0;
 return 0;
}}

 unsigned long LINKED_LIST_CLASS::CountListNodes ( void )
{{
 LINKED_LIST *Node = 0;
 unsigned long i = 0;

 while ( Enumerate ( &Node ) )
         i += 1;

 return i;
}}

/*
 Saves a generic linked list to a file.

 *This is from the old c-style callback function parameter form.
*/
 unsigned char LINKED_LIST_CLASS::SaveListEx ( unsigned short *FileName, LinkedListWriteCallback SomeWriteCallback )
{{
 FILE *out = 0;
 LINKED_LIST *Node = 0;
 unsigned long ListLength = 0;

 //Try to open the file specified.
 if ( ! (out = _wfopen ( FileName, L"wb" )) )
      return 0;

 //Count the number of nodes in the linked list that we need to write out.
 ListLength = this ->CountListNodes (  );

 //Write out the number of nodes that we're saving to the file.
 fwrite ( &ListLength, sizeof ( unsigned long ), 1, out );

 //While we still have nodes to write, call the write callback to do so.
 while ( Enumerate ( &Node ) )
         SomeWriteCallback ( out, Node ->lpData );

 //We're done with the file, close it.
 fclose ( out );

 return 1;
}}

/*
 Loads a generic linked list from a file.

 *This is from the old c-style callback function parameter form.
*/
 unsigned char LINKED_LIST_CLASS::LoadListEx ( unsigned short *FileName, unsigned long StructureSize, LinkedListReadCallback SomeReadCallback )
{{
 FILE *in = 0;
 void *pVariableBlock = 0;
 unsigned long ListLength = 0, i = 0;

 //Try to open the file passed to us.
 if ( ! (in = _wfopen ( FileName, L"rb" )) )
      return 0;

 //Read in the number of linked list nodes that were saved to this file.
 fread ( &ListLength, sizeof ( unsigned long ), 1, in );

 for ( i = 0; i < ListLength; i += 1 )
      {
       if ( ! (pVariableBlock = SomeReadCallback ( in )) )
            break;

       if ( ! AddNode ( pVariableBlock, StructureSize ) )
           {
            free ( pVariableBlock );

            break;
           }
      }

 fclose ( in );

 return 1;
}}

/*
 This must be subclassed with relevant data before calling this ->SaveList (  );
*/
 unsigned char LINKED_LIST_CLASS::WriteCallback ( FILE *out, void *lpData )
{{
 MessageBoxW ( NULL, L"Error: You never subclassed WriteCallback in the LINKED_LIST Class.\r\n" \
               L"We have no way of knowing how to write your customly defined structure to a file until you do so.", L"Error:", MB_OK );

 return 0;
}}

/*
 This must be subclassed with relevant data before calling this ->LoadList (  );
*/
 void *LINKED_LIST_CLASS::ReadCallback ( FILE *in )
{{
 MessageBoxW ( NULL, L"Error: You never subclassed ReadCallback in the LINKED_LIST Class.\r\n" \
               L"We have no way of knowing how to read your customly defined structure from a file until you do so.", L"Error:", MB_OK );

 return 0;
}}

/*
 Saves a generic linked list to a file.

 *This is from the old c-style callback function parameter form.
*/
 unsigned char LINKED_LIST_CLASS::SaveList ( unsigned short *FileName )
{{
 FILE *out = 0;
 LINKED_LIST *Node = 0;
 unsigned long ListLength = 0;

 //Try to open the file specified.
 if ( ! (out = _wfopen ( FileName, L"wb" )) )
      return 0;

 //Count the number of nodes in the linked list that we need to write out.
 ListLength = this ->CountListNodes (  );

 //Write out the number of nodes that we're saving to the file.
 fwrite ( &ListLength, sizeof ( unsigned long ), 1, out );

 //While we still have nodes to write, call the write callback to do so.
 while ( Enumerate ( &Node ) )
        {
         if ( ! WriteCallback ( out, Node ->lpData ) )
              break;
        }

 //We're done with the file, close it.
 fclose ( out );

 return 1;
}}

/*
 Loads a generic linked list from a file.

 *This is from the old c-style callback function parameter form.
*/
 unsigned char LINKED_LIST_CLASS::LoadList ( unsigned short *FileName, unsigned long StructureSize )
{{
 FILE *in = 0;
 void *pVariableBlock = 0;
 unsigned long ListLength = 0, i = 0;

 //Try to open the file passed to us.
 if ( ! (in = _wfopen ( FileName, L"rb" )) )
      return 0;

 //Read in the number of linked list nodes that were saved to this file.
 fread ( &ListLength, sizeof ( unsigned long ), 1, in );

 for ( i = 0; i < ListLength; i += 1 )
      {
       if ( ! (pVariableBlock = ReadCallback ( in )) )
            break;

       if ( ! AddNode ( pVariableBlock, StructureSize ) )
           {
            free ( pVariableBlock );

            break;
           }
      }

 fclose ( in );

 return 1;
}}
