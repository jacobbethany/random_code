/*
 Created on 2021-09-08 by Jacob Bethany
 Purpose: To create a simple method of generating
 graphs so that I can learn and try out breadth-first
 and depth-first searches.

 Note:
 -I haven't, yet, come up with a situation where there
 are multiple viable paths to the destination, since
 the shortest path always branches out first and
 disallows further nodes to reach the destination
 owing to the already-visited check. I wonder if
 that's always true.
*/
 #include "stdio.h"
 #include "stdlib.h"
 #include "string.h"
 #include "stdint.h"
 #include <vector>
 #include <algorithm>

//The graph will be connectable in all 2D directions.
typedef struct NODE {
  NODE *left;
  NODE *top;
  NODE *right;
  NODE *bottom;

  uint64_t ui64_index;
} NODE;

//This will return a pointer to a new graph node on success.
//It returns a pointer to 0 on failure.
 NODE *new_node (
   NODE *left,
   NODE *top,
   NODE *right,
   NODE *bottom,
   uint64_t ui64_index
 )
{{
 NODE *node = (NODE *) calloc ( sizeof(NODE), 1 );
 node ->left = left;
 node ->top = top;
 node ->right = right;
 node ->bottom = bottom;
 node ->ui64_index = ui64_index;

 return node;
}}

//Attacher's left node will be attached to the attachee's right node.
 unsigned char attach_node_left ( NODE *attachee, NODE *attacher )
{{
 if ( ! attachee || ! attacher ) {
   return 0;
 }

 attachee ->left = attacher;
 attacher ->right = attachee;

 return 1;
}}

//Attacher's top node will be attached to the attachee's bottom node.
 unsigned char attach_node_top ( NODE *attachee, NODE *attacher )
{{
 if ( ! attachee || ! attacher ) {
   return 0;
 }

 attachee ->top = attacher;
 attacher ->bottom = attachee;

 return 1;
}}

//Attacher's right node will be attached to the attachee's left node.
 unsigned char attach_node_right ( NODE *attachee, NODE *attacher )
{{
 if ( ! attachee || ! attacher ) {
   return 0;
 }

 attachee ->right = attacher;
 attacher ->left = attachee;

 return 1;
}}

//Attacher's bottom node will be attached to the attachee's top node.
 unsigned char attach_node_bottom ( NODE *attachee, NODE *attacher )
{{
 if ( ! attachee || ! attacher ) {
   return 0;
 }

 attachee ->bottom = attacher;
 attacher ->top = attachee;

 return 1;
}}

//If the node is non-zero and hasn't been visited, it will
//be added to vec_visited and the function will return 1(true).
//Otherwise, the function will leave vec_visited untouched and
//returned 0 (false).
 unsigned char should_visit_graph_node (
   NODE *node,
   std::vector<NODE *> &vec_visited
 )
{{
 if ( node ) {
   //If this node hasn't been visited, yet.
   if ( std::find (
          vec_visited .begin (  ),
          vec_visited .end (  ),
          node
        ) == vec_visited .end (  )
      ) {
     //Note that we have visited it, preemptively.
     vec_visited .push_back ( node );

     //Let the caller know to go ahead with the recusive call.
     return 1;
   }
 }

 //If the node doesn't exist or if it has already been searched, don't bother.
 return 0;
}}

//This function will simply collect all vistable nodes
//within a vector to be freed by the other overload.
 unsigned char free_graph (
   NODE *node,
   std::vector<NODE *> &vec_visited
 )
{{
 //If there are no nodes to free, bail.
 if ( ! node ) {
   return 0;
 }

 if ( should_visit_graph_node ( node ->left, vec_visited ) ) {
   free_graph ( node ->left, vec_visited );
 }

 if ( should_visit_graph_node ( node ->top, vec_visited ) ) {
   free_graph ( node ->top, vec_visited );
 }

 if ( should_visit_graph_node ( node ->right, vec_visited ) ) {
   free_graph ( node ->right, vec_visited );
 }

 if ( should_visit_graph_node ( node ->bottom, vec_visited ) ) {
   free_graph ( node ->bottom, vec_visited );
 }

 return 1;
}}

 //Note: This assumes that the graph is not disjointed.
 unsigned char free_graph ( NODE **root )
{{
 //If there's no list to free.
 if ( ! root ) {
   return 1;
 }
 std::vector<NODE *> vec_visited;
 unsigned char b_success = free_graph ( *root, vec_visited );
 printf (
   "Attempting to free a total of #%llu nodes.\n",
   vec_visited .size (  )
 );
 for ( std::vector<NODE *>::iterator it = vec_visited .begin (  );
       it != vec_visited .end (  );
       it ++
     ) {
   printf (
     "Freeing node with identifier: #%llu\n",
     (*it) ->ui64_index
   );
   free ( *it );
 }
 *root = 0;
 return b_success;
}}

 void find_shortest_path (
   NODE *start,
   NODE *end,
   std::vector<NODE *> vec_visited, //unique per route.
   std::vector<std::vector<NODE *>> &vec_valid_paths //paths that worked (visited when node = end)
 )
{{
 //If we've found the end node, then add the visited nodes to the collection of
 //valid paths so that we can see whether or not this particular route is optimal.
 if ( start == end ) {
   vec_valid_paths .push_back ( vec_visited );

   return ;
 }

 //Note that each of these recursive calls must copy the visited vector so that they
 //can diverge from each other's paths.
 if ( should_visit_graph_node ( start ->left, vec_visited ) ) {
   find_shortest_path ( start ->left, end, vec_visited, vec_valid_paths );
 }
 if ( should_visit_graph_node ( start ->top, vec_visited ) ) {
   find_shortest_path ( start ->top, end, vec_visited, vec_valid_paths );
 }
 if ( should_visit_graph_node ( start ->right, vec_visited ) ) {
   find_shortest_path ( start ->right, end, vec_visited, vec_valid_paths );
 }
 if ( should_visit_graph_node ( start ->bottom, vec_visited ) ) {
   find_shortest_path ( start ->bottom, end, vec_visited, vec_valid_paths );
 }
}}

/*
*/
 std::vector<NODE *> find_shortest_path ( NODE *start, NODE *end )
{{
 //By default, we've already visited the initial node.
 std::vector<NODE *> vec_visited;
 vec_visited .push_back ( start );

 //Define a variable to hold all valid paths.
 std::vector<std::vector<NODE *>> vec_valid_paths;
 find_shortest_path (
   start,
   end,
   vec_visited,
   vec_valid_paths
 );

 //If there are no valid paths.
 if ( ! vec_valid_paths .size (  ) ) {
   return std::vector<NODE *> (  );
 }

 uint32_t ui32_shortest_path = vec_valid_paths [ 0 ] .size (  );
 uint32_t ui32_shortest_path_index = 0;
 for ( std::vector<std::vector<NODE *>>::const_iterator it = vec_valid_paths .cbegin (  );
       it != vec_valid_paths .cend (  );
       it ++
     ) {

   printf ( "Potential path:\n" );
   for ( std::vector<NODE *>::const_iterator jt = it ->cbegin (  );
         jt != it ->cend (  );
         jt ++
       ) {
     printf (
       "Potential path node: %llu.\n",
       (*jt) ->ui64_index
     );
   }

   if ( ui32_shortest_path > it ->size (  ) ) {
     ui32_shortest_path = it ->size (  );
     ui32_shortest_path_index = it - vec_valid_paths .cbegin (  );
   }
 }

 //Return the vector that holds the shortest path.
 return vec_valid_paths [ ui32_shortest_path_index ];
}}

 int main ( int argc, char **argv )
{{
 uint64_t ui64_index = 0;

 printf ( "Creating the initial node.\n" );
 fflush ( stdout );
 NODE *graph = new_node ( 0, 0, 0, 0, ui64_index ++ );
/*
 O
*/

 printf ( "Creating a second node and attaching it to the first.\n" );
 fflush ( stdout );

 //Attach a left, top, right, and bottom node from the first node in the graph.
 attach_node_left (
   graph,
   new_node (
     0, 0, 0, 0, ui64_index ++
   )
 );
/*
 -O
*/

 attach_node_top (
   graph,
   new_node (
     0, 0, 0, 0, ui64_index ++
   )
 );
/*
  |
 -O
*/

 attach_node_right (
   graph,
   new_node (
     0, 0, 0, 0, ui64_index ++
   )
 );

/*
  |
 -O-
*/
 attach_node_bottom (
   graph,
   new_node (
     0, 0, 0, 0, ui64_index ++
   )
 );
/*
  |
 -O-
  |
*/

//two nodes to the left.
 attach_node_left (
   graph ->left,
   new_node (
     0, 0, 0, 0, ui64_index ++
   )
 );
/*
   |
 --O-
   |
*/


 const char *lpsz_ary_node_ordering [  ] = {
   "middle",
   "left",
   "top",
   "right",
   "bottom",
   "left-left"
 };
 for ( uint32_t ui32_i = 0;
       ui32_i < sizeof(lpsz_ary_node_ordering)/sizeof(const char*);
       ui32_i ++
     ) {
   printf ( "Node #%ld : \"%s\"\n", ui32_i, lpsz_ary_node_ordering [ ui32_i ] );
 }

/*
 Make this:
  -o  //toptop, toptopleft, toptopleftbottom
  ||
 --O-
   |
*/
 attach_node_top ( //^^
   graph ->top, //Attach it to this node's bottom.
   new_node (
     0, 0, 0, 0, ui64_index ++
   )
 );
/*
   |
   |
 --O-
   |
*/

 attach_node_left ( //^^<
   graph ->top ->top,
   new_node (
     0, 0, 0, 0, ui64_index ++
   )
 );
/*
  ||
   |
 --O-
   |
*/

 //^^>V = <
 attach_node_bottom (
   graph ->left, ///this will have an attachment to...
   graph ->top ->top ->left
 );
/*
 //Note that this is the same:
 attach_node_bottom (
   graph ->top ->top ->left,
   graph ->left
 );
*/

/*
  (|)<<|
   V   |
 -(-)>>O-
   |
*/

 std::vector<NODE *> vec_shortest_path = find_shortest_path ( graph, graph ->left );
 for ( std::vector<NODE *>::const_iterator it = vec_shortest_path .cbegin (  );
       it != vec_shortest_path .cend (  );
       it ++
     ) {
   printf ( "Node #%llu.\n", (*it) ->ui64_index );
 }

 //todo: show the graph.

 printf ( "Freeing the graph.\n" );
 fflush ( stdout );

 free_graph ( &graph );

 printf ( "Exiting...\n" );
 fflush ( stdout );
}}
