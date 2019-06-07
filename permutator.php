<?php
/*
 Code written on 2019_06_06 by Jacob Bethany.
 Purpose: To try to create every possible permutation of an array's values.

 Method:
 Step1:
 I rotate the array to the left until it reaches its starting point.

 Step2:
 I swap i, and i+gap_size, starting at every point in the array.
 I increase the gap size for later; I spawn a new recursive function call with the new array as the "starting point"; then I repeat step1 (in the main function -- though the recursive call does it, too).

 Step3:
 When the gap size exceeds the length of the array, I break out of the main loop and display the keys of the associative array of permutations.
 Note: The keys of the associative array are used to ensure that all members are unique.

 This has been tested with array lengths of 3 and 4, so far.

 Note: The depth requirement was solved by checking the count and increasing it until it was equal to arraylength factorial.
*/

 //$ary = array ( 'a', 'b', 'c', 'd', 'e' );
/*
 $ary = array ( 1, 2, 3, 4, 5, 6, 7, 8 );
 $permutations = array (  );
 $permutations = calculate_permutations ( $ary, $permutations, 0 );
 echo "Total permutations: " . count ( $permutations ) . ".\n";
*/
/*
 $ary = array ( 1, 2, 3, 4, 5 );
 $permutations = array (  );
 $permutations = calculate_permutations ( $ary, $permutations, 0 );
 echo "Total permutations: " . count ( $permutations ) . ".\n";
*/

//Usage:
// $permutations = array (  ); //just start with an empty array.
// $permutations = calculate_permutations ( array ( 1, 2, 3, 4, 5, ...etc. ), $permutations, 0 );
 function calculate_permutations ( $ary, &$current_permutations, $depth )
{{
 $required_depth = array ( 0 => 1,
                           1 => 1,
                           2 => 1,
                           3 => 1,
                           4 => 1,
                           5 => 1,
                           6 => 2,
                           7 => 3,
                           8 => 5
                         );
 $array_length = count ( $ary );
 if ( empty ( $required_depth [ $array_length ] ) )
      $depth_limit = 5; //just arbitrarily pick 5, since it's already getting slow.
 else $depth_limit = $required_depth [ $array_length ];

 //It'd be good to figure out a way to determine what the depth needs to be, programmatically.
 if ( $depth > $depth_limit ) //1 works for 5; 2 works for 6; 3 works for 7; 5 for 8.
      return;

 $length = count ( $ary );
 $current_width = 1;
 $start_array = $ary;
 $swap_start_position = 0;
 $iterations = 0;

 $fail_safe = 0;

 while ( 1 )
      {
       //if ( $depth )
       //     var_dump ( $ary );

       $current_permutations [ implode(',', $ary) ] = 1;
       //echo $swap_start_position . ", " . $current_width . "\n";

       if ( $iterations == $length ) //if we've rotated the lengthof the array times, then we're back where we started.
           {
            if ( $current_width + $swap_start_position < $length ) //<=length-1 is the same.
                 swap_with_width_and_position ( $ary, $swap_start_position, $current_width );
            $swap_start_position ++;

            //Now that we've swapped two members of the array, use this as a new starting point and do all of the same swapping mutations.
            calculate_permutations ( $ary, $current_permutations, $depth+1 ); //recurse so that we handle all swaps of all gap sizes for all rotations.

            //If we've reached the end of the array with our swaps, start at the begining, again, but with a new gap size for the swaps.
            if ( $swap_start_position > $length - 1 )
                {
                 //echo "We've started at the end.\n";

                 $swap_start_position = 0;
                 $current_width ++;
                }

            //If we've already swapped with the first and last elements of the array, there are no more larger gaps to use between rotations.
            if ( $current_width > $length )
                 break;

            //echo "Arrays are the same at all points.\n\n\n\n\n";
            $iterations = 0; //start over with the newly swapped array.
           }
       else
           {
            rotate_array ( $ary );
            $iterations ++;

            $fail_safe ++;
            if ( $fail_safe > 100 )
                 break;
           }
      }

 //Only show the results, if this is the first function call that was made.
 if ( ! $depth )
     {
      $permutations = array (  );
      foreach ( $current_permutations as $permutation_string => $v )
                $permutations[] = explode ( ",", $permutation_string );
      //return array_keys ( $current_permutations );
      //var_dump ( array_keys ( $current_permutations ) );
      return $permutations;
     }

}}

 function rotate_array ( &$ary )
{{
 $value = $ary [ 0 ];
 unset ( $ary [ 0 ] );
 $ary = array_merge ( $ary, array ( $value ) );
}}

 function swap_with_width_and_position ( &$ary, $swap_start, $swap_gap )
{{
 $v = $ary [ $swap_start ];
 $ary [ $swap_start ] = $ary [ $swap_start + $swap_gap ];
 $ary [ $swap_start + $swap_gap ] = $v;
}}

?>
