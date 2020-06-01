<?php
/*
 Created on 2020-05-31 by Jacob Bethany.
 Purpose: To try to determine all permutations of a sequence as quickly as possible.
 I repeatedly break the original sequence into chunks starting with every item in
 the sequence and including all but the current item. The current item is prefixed to
 the array of arrays returned from the chain of recursive function calls.
 The way to read the permutation output is to follow the diagonals. The left most is
 prepended to the slopes.

 Note: Ive added a function called flatten_permutations to simplify the viewing process.
*/
 //This will be the set to permute.
 $set = array ( 'a', 'b', 'c', 'd' );

 //This will get a series of nested arrays denoting the possible permutations.
 $permutations = get_permutations ( $set );

 //This is a naive view of the permutations.
 //var_dump ( $permutations );

 //This will show the permutations in its minimal logical form: a series of diagonal lines.
 //show_permutations ( $permutations );

 //This will show the permutations as a series of complete "flattened" arrays.
 $flattened_permutations = array (  );
 flatten_permutations ( $permutations, $flattened_permutations );

 //This makes sense, with a "flattened" view of the complete permutations.
 var_dump ( $flattened_permutations );

/*
 This will convert from the diagonal representation to an array of explicit full permutations.
 Example:
 array ( "abc", "acb", "bac", "bca", "cab", "cba" )
*/
 function flatten_permutations (
   &$top_level_permutations,
   &$flattened_permutations,
   $precursor = array (  ),
   $depth = 0
 )
{{
 //echo implode ( ",", $precursor ) . "\n";
 foreach ( $top_level_permutations as $arbitrary_index => $permutations ) {

   foreach ( $permutations as $first_item => $permutation ) {

     //The want to copy what was passed to reset between each of the sets within this recursive call's array segment.
     $tmp_precursor = $precursor;

     //Add the current prefix before making another recursive call.
     $tmp_precursor[] = $first_item;

     //If we need to step into another level of the array, then do that with a recursive call, including
     //the altered precursor array generated from what was passed from the parent call and from the current
     //segment of the permutation array at this level.
     if ( count ( $permutation ) )
          flatten_permutations ( $permutation, $flattened_permutations, $tmp_precursor, $depth+1 );

     //Otherwise, simply assign another flattened permutation to what came before this function call,
     //combined (appended) with the current item at this array nesting level (where there's only one
     //possible permutation).
     else $flattened_permutations[] = array_merge ( $precursor, array ( $first_item ) );
   }

 }
}}

/*
 This will echo N spaces where N is the passed variable, $spaces.
*/
 function indent ( $spaces )
{{
 for ( $i = 0; $i < $spaces; $i ++ )
       echo " ";
}}

/*
 This will show the nested arrays of permutations in y = -x diagonals,
 broken into smaller segments at the points of variation from the root
 item.
 abcd abdc would be broken after a and b, for example.
 a||| < a route
 |b|| < ab route
 ||c| < abc route
 |||d (final result is abcd)
 ||d| < abd route
 |||c (final result is abdc)
 ...etc.

 To get the permutations in their full form as a simple list,
 use flatten_permutations.
*/
 function show_permutations ( &$top_level_permutations, $depth = 0 )
{{
 foreach ( $top_level_permutations as $arbitrary_index => $permutations ) {
   foreach ( $permutations as $first_item => $permutation ) {

     echo "\n";
     indent ( $depth << 1 );
     echo $first_item;
     show_permutations ( $permutation, $depth+1 );
   }

   if ( ! $depth )
        echo "\n";
 }
}}

/*
 This function will take the array passed to it,
 and recursively call itself with the same set
 without one item, for each item in the set.
 The removed item will be the key; and the remaining
 subset will be what is passed to the recursive call.
 What is returned is an array of N arrays of K => subset,
 where N is the number of items in the set passed to the
 function, K is the removed item for that loop iteration,
 and subset is all keys of the passed array other than K.

 This gives something like the following:
 //example abcd
 array (
   0 => array (
     'a' => array (
       'b' => array (
         'c' => array (
           'd' => array () //d relates to an empty set.
         ), //ABC route end.
         'd' => array (
           'c' => array() //c relates to an empty set.
         ) //ABD route end.
       ), //AB route end.
       'c' => array (
         'b' => array (
           'd' => array (  ) //d relates to an empty set.
         ), //ACB route end.
         'd' => array (
           'b' => array () //b relates to an empty set.
         ) //ACD route end.
       ) //AC route end.
     ) //a
   ), //0
   1 => array (
     'b' => array (
       ...
     )
   ), //1
   ...
 )

 See show_permutations to see how this looks.
*/
 function get_permutations ( &$set )
{{
 $permutations = array (  );
 $slice = array (  );
 foreach ( $set as $index => $item ) {

   //Everything but the current item.
   if ( $index )
        $slice = array_merge (
          array_slice ( $set, 0, $index ), //everything before the current.
          array_slice ( $set, $index+1 ) //everything after the current.
        );
   else
        $slice = array_slice ( $set, 1 );

   $permutations[] = array ( $item => get_permutations ( $slice ) );
 }

 return $permutations;
}}

?>
