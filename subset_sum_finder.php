<?php
/*
 Code written on 2019_06_06 by Jacob Bethany.
 Purpose: To find which array subsets can combine for a requested sum.
*/
 require_once "permutator.php";

//This will break it.
// $whole_array = array ( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 );
 $whole_array = array ( 1, 2, 3, 4, 5 );



 //echo is_permutation ( array ( 1, 2, 3 ), array ( 3, 1, 2 ) ) . "\n";

 $sum_subsets = find_sum_in_permutations ( $whole_array, 12 );
 var_dump ( $sum_subsets );

 function find_sum_in_permutations ( $ary, $sum )
{{
 $permutations = calculate_permutations ( $ary, $permutations, 0 );
// var_dump ( $permutations );
//return ;

 $sum_subsets = array (  );
 $length = count ( $permutations [ 0 ] ); //they'll all be the same length, since they're just re-orderings.
 foreach ( $permutations as $permutation )
          {
           $total = 0;
           for ( $i = 0; $i < $length; $i ++ )
                {
                 $total += $permutation [ $i ];
                 if ( $total == $sum )
                     {
                      $sub_array = get_array_segment ( $permutation, 0, $i );
                      sort ( $sub_array ); //so that the key will be unique AND only belong to one permutation.
                      $sum_subsets [ implode ( ',', $sub_array ) ] = 1;
                      break;
                     }
                }
          }

 //var_dump ( $sum_subsets );
 $final_subsets = array (  );
 foreach ( $sum_subsets as $k => $v )
          {
           $final_subsets [] = explode ( ",", $k );
          }


// echo count ( $permutations ) . "\n\n";
//return array();
 return $final_subsets;
}}

 function get_array_segment ( $whole_ary, $start_index, $end_index )
{{
 $ary = array (  );
 foreach ( $whole_ary as $k => $v )
          {
           if ( $k < $start_index )
                continue;

           if ( $k > $end_index )
                break;

           $ary [] = $v;
          }
 return $ary;
}}

?>
