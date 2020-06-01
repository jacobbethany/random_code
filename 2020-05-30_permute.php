<?php
/*
 Created on 2020-05-30 by Jacob Bethany.
 Purpose: To create a list of all permutations of a list of any number of unique items.
 The method that I made before worked for all list types (even ones with repeated items),
 but this method doesn't require any recursive function calls and doesn't require any
 guesswork or trial and error. I had a recursion depth variable in it that limited the
 amount of time that could be spent trying to create the list of permutations.

 I create an array the size of the list to be permuted.
 Each number in the list is base N, where N is the number of items in the list to be
 permuted.
 I repeatedly increment the number in the list, as though it were one large base N
 number.
 I use only configurations of this array of base N numbers where 1.) the list contains
 all item associations 0 -> N; and 2.) the list contains only one of each association.
 Each valid configuration is displayed. There should always be !N items.
 This is because the first item has positions 0->N to use. The second item has 0->(N-1),
 since the first item took one; the third item has 0->(N-2), since the first two items
 have each taken one of the N positions. This pattern continues for each subsequent item.

 Note: The same item can be used more than once, in the source sequence. It will just be
 assigned a unique identifier association, internally.
*/

 //Working example:
 //Define a sequence of items to permute.
 $sequence = array ( 'a', 'b', 'c', 'd', 'e', 'f', 'g' );

 //Create a sequence map that holds the permutation associations and descriptions thereof.
 $map = create_sequence_map ( $sequence );

 //For every possible permutation valid or otherwise.
 do {
   //Skip invalid permutations.
   if ( ! is_valid_permutation ( $map ) )
        continue;

   //Otherwise, show the current permutation in the form of the reordered original sequence,
   //rather than an array of numbers describing indices from the original sequence.
   show_array_associations ( $map, $sequence );
 } while ( increment_sequence_map ( $map ) ); //Continuously iterate the fake base N number.

/*
 Check to ensure that each item is referenced exactly once in the current "permutation."
*/
 function is_valid_permutation ( &$map ) : bool
{{
 $existing_keys = array (  );

 foreach ( $map [ 'set' ] as $v ) {
   if ( array_key_exists ( $v, $existing_keys ) )
        $existing_keys [ $v ] ++;
   else $existing_keys [ $v ] = 1;
 }

 //If one or more items to be permuted exists more than once, this isn't a valid permutation.
 foreach ( $existing_keys as $v ) {
   if ( $v > 1 )
        return 0;
 }

 //If not all items to be permuted are represented, this isn't a valid permutation.
 if ( count ( $existing_keys ) < $map [ 'base' ] )
      return 0;

 return 1;
}}

/*
 This will show the current permutation's associations
 within the original sequence array. Essentially, it
 shows the items from the sequence array in the positions
 that are defined in the permutation array.
*/
 function show_array_associations ( &$map, &$sequence )
{{
 foreach ( $map [ 'set' ] as $k => $v ) {

   if ( $k )
        echo ', ';
   echo $sequence [ $v ];

 }

 echo "\n";
}}

//This will increment the base N sequence map
//until the map contains one of each number from
//0 until N-1.
 function increment_sequence_map ( &$map ) : bool
{{
 $i = $map [ 'base' ] - 1;
 $map [ 'set' ] [ $i ] ++; //Increment the value at the end of the base N number set.

 //While the current number in the set is equal to the base,
 //continuously reset the current item to zero and add one to
 //the previous number in the set.
 while ( $map [ 'set' ] [ $i ] == $map [ 'base' ] )
        {
         $map [ 'set' ] [ $i ] = 0;
         $i --;

         //If we'd overflow the base N set of length N, return false.
         if ( $i < 0 )
              return 0;

         $map [ 'set' ] [ $i ] ++;
        }

 //If we didn't overflow, return true.
 return 1;
}}

/*
 This will create an array with information about the sequence to permute.
 It will tell the base to use for each of the numbers in the "base" member,
 and the "set" array member will hold the associations to use from the
 original sequence array passed to the function.
*/
 function create_sequence_map ( array $sequence ) : array
{{
 $length = count ( $sequence );
 $number_set = array (  );

 //The lowest usable number will be 01234...N where N is the length.
 //This will save us from starting at 0000...0 and adding one at a time
 //until we reach a usable number.
 for ( $i = 0; $i < $length; $i ++ )
      {
       $number_set[] = $i;
      }

 $map = array (
   'base' => $length,
   'set' => $number_set
 );

 return $map;
}}

?>
