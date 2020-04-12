# Created on 2020-04-12 by Jacob Bethany
# Purpose: To refresh/improve my knowledge of Perl by working with data types, generically.
# This code contains functions to dump constants, variables by copy, and variables by reference,
# regardless of whether they are scalar, string, array, or hash in nature, and regardless of
# whether they include nested hashes or arrays within them.
 use strict;
 use warnings;

 my %g_some_hash = (
   'a' => {

     'nested_value' => 1,

     'nested_hash' => {
       'x' => 'ten',
       'y' => 'twenty'
     }, #end of 'nested_hash'

     #Using ( gets flattened and causes it to act like a series of key and value pairs of the hash, 'a'.
     #Using \( does reference an array, but it is treated as a scalar value rather than an array reference and can't be used generically.
     #Using [ avoids flattening and stores the fact that this is an ARRAY reference within the meta data of the variable, thereby allowing
     #the variable to be dumped, generically.
     'nested_array' => [ #( \( ##we MUST use [] for our purposes!
      1, 2, 3, "four", "5", 6, "seven"
     ], #end of 'nested_array'

     #same as above, owing to the facts outlined in the previous block of comments.
     'nested_anonymous_array' => [
       "1", "two", "three"
     ] #end of 'nested_anonymous_array'

   }, #end of 'a'

   'some_value' => 8
 ); #end of %g_some_hash definition.

 my @g_some_array = (

   'some_array_value', #0

   1, #1

   [ # nested array #2
     7, 6, 5
   ],

   { #nested hash #3
     'a' => 1,
     'b' => 2,
     'c' => 3,
     '.' => '..',
     'z' => 26
   }

 ); #end of @g_some_array.

 print "Dumping g_some_hash:\n";
 dump_variable ( \%g_some_hash );

 print "\nDumping g_some_array:\n";
 dump_variable ( \@g_some_array );

 print "\nDumping a constant number:\n";
 dump_variable ( 1 );

 print "\nDumping a constant string:\n";
 dump_variable ( "two" );

 my $a = 3;
 my $b = "four";

 print "\nDumping a variable holding an integer by copy:\n";
 dump_variable ( $a );

 print "\nDumping a variable holding a string by copy:\n";
 dump_variable ( $b );

 print "\nDumping a variable holding a integer by reference:\n";
 dump_variable ( \$a );

 print "\nDumping a variable holding a string by reference:\n";
 dump_variable ( \$b );

 sub indent
{{
 my $indentation = shift || 0;
 $indentation = $indentation >= 0 ? $indentation : 0;

 for ( my $i = 0; $i < $indentation; $i ++ ) {
   print " ";
 }
}}

#Usage: dump_variable ( \[@%$]some_variable );
#Note: The indentation argument is automatically adjusted with recursive calls and need not be passed.
 sub dump_variable
{{
 my $indentation = $_ [ 1 ] || 1;

 #!! Note: This is just for debugging.
 ##print "Someone passed us a \"" . ref($_[0]) . "\"\n";

 if ( ref ( $_ [ 0 ] ) eq 'HASH' ) {

   my %hash = %{$_ [ 0 ]}; #This is a dereferenced argument 0.
   my $hash_length = keys %hash; #the number of keys in the hash is the length.

   while ( my ( $key, $value ) = each ( %hash ) ) {

     $hash_length --;
     indent ( $indentation );

     #If this is a reference to an array, we need to make a recursive call.
     if ( ref($value) eq 'ARRAY' ) {
       print "\"" . $key . "\" => [\n";
       dump_variable ( $value, $indentation + 2 );
       indent ( $indentation );
       print "]";
     }
     #If this is reference to a hash, we need to make a recursive call.
     elsif ( ref($value) eq 'HASH' ) {
       print "\"" . $key . "\" => {\n";
       dump_variable ( $value, $indentation + 2 );
       indent ( $indentation );
       print "}";
     }
     #If this is a reference to a reference to anything else (scalar, string, ...etc.)
     elsif ( ref($value) eq 'REF' ) {
       print "\"" . $key . "\" => \"" . $value . "\"";
     }
     #If this is anything else (scalar, string, ...etc.)
     else {
       print "\"" .$key . "\" => \"" . $value . "\"";
     }

     if ( $hash_length ) {
       print ",";
     }
     print "\n";

   } #end of while

 } #end if hash condition.
 elsif ( ref ( $_[0] ) eq 'ARRAY' ) {

   my @array = @{$_ [ 0 ]}; #This is a dereferenced argument 0.

   while ( my ( $key, $value ) = each ( @array ) ) {

     indent ( $indentation );

     #If this is a reference to an array, we need to make a recursive call.
     if ( ref($value) eq 'ARRAY' ) {
       print "[\n";
       dump_variable ( $value, $indentation + 2 );
       indent ( $indentation );
       print "]";
     }
     #If this is reference to a hash, we need to make a recursive call.
     elsif ( ref($value) eq 'HASH' ) {
       print "{\n";
       dump_variable ( $value, $indentation + 2 );
       indent ( $indentation );
       print "}";
     }
     #If this is a reference to a reference to anything else (scalar, string, ...etc.)
     elsif ( ref($value) eq 'REF' ) {
       print "\"" . $key . "\" => \"" . $value . "\"\n";
     }
     #If this is anything else (scalar, string, ...etc.)
     elsif ( ref($value) eq '' ) {
       print $value;
     }
     else {
       print "ref(value) = \"" . ref($value) . "\"\n";
     }

     if ( $key != scalar @array - 1 ) {
       print ","
     }
     print "\n";

   } #end of while

 }
 #If this is a generic reference or a referene to a scalar, show it.
 elsif ( ref($_[0]) eq 'REF' || ref ($_[0]) eq 'SCALAR' ) {

   print "This is a reference to an unknown variable type: \"" . ${$_ [ 0 ]} . "\"\n";

 }
 #Otherwise, this is probably not a reference at all.
 #Just display whatever it is without any dereferencing.
 else {

   print "This is something else ( \"" . ref($_[0]) . "\" ), \"" . $_ [ 0 ] . "\".\n";

 }
}}
