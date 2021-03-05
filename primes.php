<?php
/*
 Created on 2021-02-28 by Jacob Bethany
 Purpose: To find the Nth prime, using the Fibonacci sequence and Euler's
 adjustments thereto when calculating partitions of given integers.

 Note: The adjustments must be made as the Fibonacci subset is generated.
*/

 //works.
 //$unique_partition_array = get_unique_partition_array ( 15 );
 //var_dump ( $unique_partition_array );

 //Note, for any prime, there will be exactly two factors, the prime itself and 1.
 $integer_factor_sum_array = get_integer_factor_sum_array ( 100000 );
 var_dump ( $integer_factor_sum_array );

 $prime_index = 1; //Let's create a one-based list of primes.
 foreach ( $integer_factor_sum_array as $k => $v ) {
   if ( $v - $k == 1 )
        echo "Prime #" . ($prime_index++) . ": " . $k . "\n";
 }

 function get_next_value_with_adjustments ( array &$ary, int $index )
{{
 $adjustment_offset = 0;
 $adjustment_number = 1; //The first + for the final offset is a given.
 $sign = 1;
 $n = end ( $ary ); //same as $ary[count($ary)-1]
 $i = 1;

 //The offsets are cumulative, so we need to track the last one that was used.
 $last_adjustment_offset = 0; //Start offset 0 from the end of the array.
 while ( 1 ) {
   //If this is an odd number.
   if ( $i % 2 )
        $adjustment_offset = ($i + 1) / 2;
   else $adjustment_offset = $i + 1;

   //Move offset units backwards from the previous offset.
   $adjustment_offset += $last_adjustment_offset;
   $last_adjustment_offset = $adjustment_offset;

   //If we'd underflow the buffer, we're not ready to use this offset, yet.
   if ( $adjustment_offset >= count ( $ary ) ) {
        break;
   }

   if ( ++ $adjustment_number > 2 ) {
     $sign = -1;
     if ( $adjustment_number == 4 )
          $adjustment_number = 0;
   }
   else $sign = 1;

   $adjustment = 0;
   $adjustment = bcadd ( $adjustment, bcmul ( $ary [ count ( $ary ) - $adjustment_offset - 1 ], $sign ) );
   $n = bcadd ( $n, $adjustment );

   $i ++;
 }

 return $n;
}}

 function get_unique_partition_array ( int $n ) : array
{{
 $current = 1;

 $fib_array = [ 1 ];

 for ( $i = 1; $i < $n; $i ++ ) {

   $tmp = $current;

   //Show the current subset before showing the adjustments being made.
   //echo "Before adjustments: [" . implode ( ',', $fib_array ) . "]\n";
   $current = get_next_value_with_adjustments ( $fib_array, $i );
   $fib_array [  ] = $current;
   $last = $tmp;
 }
 return $fib_array;
}}

//Each array value relates to the sum of all factors of the key's integer value.
 function get_integer_factor_sum_array ( int $n ) : array
{{
 $current = 1;

 $fib_array = [ 1 ];

 for ( $i = 1; $i < $n; $i ++ ) {

   $tmp = $current;

   //Show the current subset before showing the adjustments being made.
   //echo "Before adjustments: [" . implode ( ',', $fib_array ) . "]\n";
   $current = get_next_value_with_adjustments ( $fib_array, $i );
   $fib_array [  ] = $current;
   $last = $tmp;
   $fib_array [ 0 ] ++;
 }
 return $fib_array;
}}

?>
