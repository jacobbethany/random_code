<?php
/*
 Code written on 2019_06_06 by Jacob Bethany.
 Purpose: I wanted to take my bit mask idea from yesterday and allow it to be used for arrays of any size.
 So, I made an "array mask" of the original array.
 Each element of the mask has the same relative index
 (it only works for numeric arrays, though copying keys when generating the mask and using foreach loops
 could work for string based keys, too) and has a value of either true or false.
 These true and false values from the right most element (length-1) become true, just like incrementing
 in binary. This, therefore, covers every "combination" (not permutation) of array subsets for addition.

 Since arrays are dynamic in size, this mask can work for an array of any length. When using a bit mask,
 I had to worry about the max size of an integer in PHP. I'd assume that it probably wouldn't be able
 to exceed 2^64-1 in value (or 64 bits --and, therefore, arrays of length 64 or less).
*/
 $some_array = array ( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 );
 $some_mask = create_array_mask ( $some_array );

 $required_value = 12;

 //Loop until we wrap around to all zeros.
 do
   {
    if ( check_masked_sum ( $some_array, $some_mask, $required_value ) )
         show_masked_array_values ( $some_array, $some_mask );

    increment_array_mask ( $some_mask );
   } while ( ! array_mask_is_full ( $some_mask, 0 ) );

 function show_masked_array_values ( $ary, $ary_mask )
{{
 $b_first = 1;
 foreach ( $ary_mask as $k => $v )
          {
           if ( $v )
               {
                if ( $b_first )
                     $b_first = 0;
                else echo ", ";
                echo $ary [ $k ];
               }
          }
 echo "\n";
}}

 function check_masked_sum ( $ary, $ary_mask, $desired_value )
{{
 $total = 0;
 foreach ( $ary_mask as $k => $v )
          {
           if ( $v )
               {
                $total += $ary [ $k ];

                if ( $total > $desired_value )
                     return false;
               }
          }

 //We have to let the loop finish because partial array subsets from the overall mask could match, otherwise.
 if ( $total == $desired_value )
      return true;

 return false;
}}

 function create_array_mask ( $ary )
{{
 $ary_mask = array (  );
 $length = count ( $ary );
 for ( $i = 0; $i < $length; $i ++ )
       $ary_mask [ $i ] = false;

 return $ary_mask;
}}

/*
 This starts at the right-most bit mask and toggles it
 from 0 to 1 or 1 0 and keeps doing so until  it finds
 a bit that is zero (inclusive).
*/
 function increment_array_mask ( &$ary_mask )
{{
 $length = count ( $ary_mask );
 $i = $length - 1;
 while ( $i >= 0 )
        {
         if ( $ary_mask [ $i ] )
             {
              $ary_mask [ $i ] = ! $ary_mask [ $i ];
              $i --;
              continue;
             }

         $ary_mask [ $i ] = ! $ary_mask [ $i ];
         break;
        }
}}

/*
 This checks to see if all "bit flags" are already 1.
 This would case an overflow error and it would wrap
 around to being all 0s, again, if we were to use
 increment_array_mask().
*/
 function array_mask_is_full ( $ary_mask, $b_true )
{{
 foreach ( $ary_mask as $v )
          {
           if ( ! ($v == $b_true ) )
               {
                return 0;
               }
          }

 //If we didn't find any 0s, it's full.
 return 1;
}}

?>
