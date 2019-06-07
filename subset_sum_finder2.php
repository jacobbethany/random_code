<?php
/*
 Code written by Jacob Bethany on 2019_06_06.
 This is a more efficient way that I wrote to find subsets of an array that match a desired sum.
 This uses a bitmask to choose what indices of the array to combine to make the sum.
 This increments the mask one at a time so that every combination of indices is used.
*/
 $ary = array ( 1, 2, 3, 4, 5, 6 );
 $matching_combinations = find_sum_in_subset ( $ary, 6 );
 foreach ( $matching_combinations as $index )
          {
           var_dump ( $matching_combinations );
          }

 //00001
 //00010
 //00011
 //00100
 //00101
 //00110
 //00111
 //01000
 //00001
 //..etc.
 //Each one of these should relate to a position in the array and each combination should be added together to see if it equals the proper sum.

 function find_sum_in_subset ( $ary, $sum )
{{
 $successful_combinations = array (  );
 $mask = 0;
 $length = count ( $ary );
 for ( $mask = 0; $mask < 1 << $length; $mask ++ )
      {
       $indices = get_indices_from_mask ( $mask );
       //$mask & ??
       $total = 0;
       foreach ( $indices as $index )
                 $total += $ary [ $index ];
       if ( $total == $sum )
           {
            $combination = array (  );
            foreach ( $indices as $index )
                      $combination[] = $ary [ $index ];

            $successful_combinations[] = $combination;
           }
      }

 return $successful_combinations;
}}

 function get_indices_from_mask ( $mask )
{{
 $indices = array (  );
 for ( $i = 0; $i < 64; $i ++ ) //I don't think php supports numbers above 8 bytse long.
      {
       if ( $mask & (1<<$i) )
            $indices[] = $i;
      }
 return $indices;
}}

?>
