/*
 Code by: Jacob A. Bethany.
*/
#ifndef LINKED_LIST_HEADER
#define LINKED_LIST_HEADER

 //We'll have a definition in .cpp that gets
 //compiled in the library with this flag set.
 //This way it will allready try to include the library
 //with this header if needed, and won't if not.
/*
 #ifndef IS_LINKED_LIST_LIBRARY_PROJECT
  #pragma comment ( lib, "Lib.lib" )
 #endif
*/

 #include "stdio.h"
 #include "stdlib.h"
 #include "windows.h"

 //This will allow you to continue adding nodes, since it's non zero,
 //but will also allow you to free when needed.
 #ifndef ERROR_NODE_EXISTS
  #define ERROR_NODE_EXISTS 2
 #endif

 #ifndef offset_of
  #define offset_of(TYPE , MEMBER) ((size_t) &((TYPE*)0)->MEMBER)
 #endif

 #ifndef LINKED_LIST_PTR_SIZE
  #define LINKED_LIST_PTR_SIZE sizeof ( char * )
 #endif

 //This node's structure can be freed, it is either individually allocated, or the root of a mass allocation list.
 //1
 #ifndef LINKED_LIST_FREEABLE_STRUCTURE
  #define LINKED_LIST_FREEABLE_STRUCTURE 1
 #endif

 //This node's variable block can be freed if the Node ->ulDataSize is > 0
 //10
 #ifndef LINKED_LIST_FREEABLE_VARIABLE_BLOCK
  #define LINKED_LIST_FREEABLE_VARIABLE_BLOCK 2
 #endif

 //This node was one of the many (including the root) allocated by NewGroup().
 //100
 #ifndef LINKED_LIST_MASS_ALLOCATED
  #define LINKED_LIST_MASS_ALLOCATED 4
 #endif

 //This node has a name associated with it. (Used for freeing.)
 //1000
 #ifndef LINKED_LIST_HAS_ASSOC_NAME
  #define LINKED_LIST_HAS_ASSOC_NAME 8
 #endif

 typedef struct LINKED_LIST_S
{
 void *lpData; //The unique variable data contained in this node. (It could be a structure or a single variable.)
 unsigned short *AssocName; //Name to associate with this node. (Can be null if unused.)
 unsigned long ulDataSize; //Size of the data being pointed to by lpData.
 unsigned long ulId; //Identifier for this node.

 //This would be the node to free, in order to free the entire linked list of nodes.
 unsigned long bFlags; //Is not a node in a group-allocated buffer other than the root.

 LINKED_LIST_S *Prev; //Previous node in the linked list.
 LINKED_LIST_S *Next; //Next node in the linked list.
} LINKED_LIST;

 //This attempts to emulate the foreach ( a as b => c ) {} php loops with a callback for each node.
 typedef void (*ForeachCallback)(void*);

 //Callback function for comparing the complex regions of two nodes within a linked list.
 //This can be useful for adding only unique items to a linked list by comparing the existing
 //items with the one to be added first; or for finding a particular node in the list.
 typedef unsigned char (*CompareCallback)(void*, void*);

 //This callback function is used for sorting, but could also be used for finding linked list nodes by name in the future,
 //you simply return a string from inside your variable data block when the callback function recieves a pointer to a node's
 //variable block.
 typedef unsigned short *(*StringFromVariableBlockCallback)(void*lpVariableBlock);

 //This callback function should return a long from the variable block.
 typedef long (*LongFromVariableBlockCallback)(void*lpVariableBlock);

 //tested and works.

 //Complex
 class LINKED_LIST_CLASS
{
 //private:
 public:
 unsigned long gNextId; //identifier for the next node to be added in the linked list.
 unsigned short gNoAssocName [ 2 ]; //Default value for non associtiave lists.
 LINKED_LIST *gRoot; //a pointer to the root of the linked list for this class.

 void LINKED_LIST_CLASS::FreeVariableBlock ( LINKED_LIST *Node );
 unsigned char LINKED_LIST_CLASS::AllocateVariableBlock ( LINKED_LIST *Node );
 LINKED_LIST *LINKED_LIST_CLASS::FindFreeableNode ( LINKED_LIST *StartNode, unsigned long bFlags );
 LINKED_LIST *LINKED_LIST_CLASS::NewNode ( unsigned short *optionalAssocName, void *lpData, unsigned long ulDataSize, LINKED_LIST *Prev );
 unsigned long LINKED_LIST_CLASS::StringIsFirst ( unsigned short *String, unsigned short *OtherString );
 unsigned long LINKED_LIST_CLASS::StringIsFirstInsensitive ( unsigned short *String, unsigned short *OtherString );

 public:
 LINKED_LIST_CLASS::LINKED_LIST_CLASS ( void );

 virtual unsigned char FreeCallback ( void *lpComplexRegion ) = 0; //You define this when you extend this class.
 virtual unsigned short *LINKED_LIST_CLASS::GetAlphaSortStringFromVariableBlock ( void *lpVariableBlock );
 virtual unsigned char LINKED_LIST_CLASS::GetSortLongFromVariableBlock ( void *lpVariableBlock, long *outLong );
 virtual unsigned char LINKED_LIST_CLASS::UniqueCompareCallback ( void *lpVariableBlock1, void *lpVariableBlock2 );

 //Sorting callbacks.
 //unsigned short *GetStringCallback ( void *lpComplexRegion ); //Your callback that returns a string from the complex region.
 //unsigned long GetLongCallback ( void *lpComplexRegion ); //Your callback that returns a long from the complex region.

 unsigned char LINKED_LIST_CLASS::AddNode ( void *lpData, unsigned long ulDataSize );
 unsigned char LINKED_LIST_CLASS::NodeExists ( void *lpVariableBlock );
 unsigned char LINKED_LIST_CLASS::AddUniqueNode ( void *lpData, unsigned long ulDataSize );
 unsigned char LINKED_LIST_CLASS::AddUniqueNodeEx ( void *lpData, unsigned long ulDataSize, CompareCallback CompareCallback );
 unsigned char LINKED_LIST_CLASS::AddAssocNode ( unsigned short *AssocName, void *lpData, unsigned long ulDataSize );
 unsigned char LINKED_LIST_CLASS::SetNodeVariableBlock ( LINKED_LIST *Node, void *lpData, unsigned long ulDataSize );
 unsigned char LINKED_LIST_CLASS::Free ( void );
 unsigned char LINKED_LIST_CLASS::FreeEx ( void );
 unsigned char LINKED_LIST_CLASS::FreeNode ( LINKED_LIST **Node );
 unsigned char LINKED_LIST_CLASS::FreeNodeEx ( LINKED_LIST **Node );

 unsigned char LINKED_LIST_CLASS::ListContainsGroupAllocationNode ( void );
 unsigned char LINKED_LIST_CLASS::NewGroup ( unsigned long ulDataSize, unsigned long GroupCount );
 unsigned char LINKED_LIST_CLASS::FreeGroup (  );
 unsigned char LINKED_LIST_CLASS::FreeGroupEx (  );

 LINKED_LIST *LINKED_LIST_CLASS::FindNodeByAssoc ( unsigned short *AssocName );
 LINKED_LIST *LINKED_LIST_CLASS::FindNode ( void *SearchData, CompareCallback CompareCallback );
 unsigned char LINKED_LIST_CLASS::SetNodeVariableBlockEx ( LINKED_LIST *Node, void *lpData, unsigned long ulDataSize );
 unsigned char LINKED_LIST_CLASS::AssociateNode ( LINKED_LIST *Node, unsigned short *AssocName );

 void LINKED_LIST_CLASS::Foreach ( ForeachCallback ForeachFunction );
 void LINKED_LIST_CLASS::ForeachByVariableBlock ( ForeachCallback ForeachFunction );

 unsigned char LINKED_LIST_CLASS::Enumerate ( LINKED_LIST **EnumerationNode );
 unsigned char LINKED_LIST_CLASS::EnumerateByVariableBlock ( void **lpVariableBlock );

 unsigned char LINKED_LIST_CLASS::Append ( LINKED_LIST **Target, LINKED_LIST **ToAdd );

 LINKED_LIST *LINKED_LIST_CLASS::GetParent ( void *lpData );
 LINKED_LIST *LINKED_LIST_CLASS::GetRoot ( LINKED_LIST *SomeNode );
 LINKED_LIST *LINKED_LIST_CLASS::GetEnd ( void );
 LINKED_LIST *LINKED_LIST_CLASS::FindNodeById ( unsigned long ulId );

 void LINKED_LIST_CLASS::GetNodeVariableBlock ( LINKED_LIST *Node, void *outlpData );

 void LINKED_LIST_CLASS::SwapNodes ( LINKED_LIST *Node1, LINKED_LIST *Node2 );
 unsigned char LINKED_LIST_CLASS::AlphaSort ( unsigned char bCaseSensitiveSort );
 void LINKED_LIST_CLASS::AlphaSortEx ( unsigned char bCaseSensitiveSort, StringFromVariableBlockCallback GetStringFromVariableBlock );
 void LINKED_LIST_CLASS::LongSort ( void );
 void LINKED_LIST_CLASS::LongSortEx ( LongFromVariableBlockCallback Callback );

 unsigned long LINKED_LIST_CLASS::GetUnicodeStringSize ( unsigned short *String );
};
 //experimental
 //Nothing untested currently.

#endif
