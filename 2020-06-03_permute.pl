#
# Created on 2020-06-03 by Jacob Bethany
# Purpose: To refresh my Perl skills by writing a script to show all permutations of a given data set.
# This will work similarly to the most recent PHP permutation script that I wrote.
#
 use strict;
 use warnings;

 my @set = ( 'a', 'b', 'c', 'd', 'e', 'f', 'g' );

 #Get the quickest/smallest representation of the set of permutations.
 my @permutations = get_permutations ( \@set );

 #Show the permutations in its most basic form.
 show_permutations ( \@permutations );

 #Define an array to hold the simplified array of arrays.
 my @permutation_list = (  );

 #Simplify the list of permutations as an array of references to full permutation arrays.
 convert_permutations ( \@permutations, \@permutation_list );

 #Show all of the simplified permutations.
 while ( my ( $k, $ref ) = each ( @permutation_list ) ) {
   if ( $k+1 < 10 ) {
        print "Permutation 0" . ($k+1) . ": { ";
   }
   else {
     print "Permutation " . ($k+1) . ": { ";
   }
   while ( my ( $i, $v ) = each ( @{$ref} ) ) {
     if ( $i ) {
       print ", ";
     }

     print "'" . $v . "'";
   }
   print " } \n";
 }

#The format will be something like this:
#('a', \('b', 'c', 'd')))
#('b', \('c', 'd'))
#('c', \('d'))
#('d', \())
 sub convert_permutations
{{
 my $array_ref = shift || 0;
 my $output_array_ref = shift || 0;
 my $indentation = shift || 0;
 my $previous_ref = shift || 0;

 if ( ! $array_ref ) {
      print "Error: No reference to an array of permutations was passed to convert_permutations to simplify.\n";
      return 0;
 }

 if ( ! $output_array_ref ) {
      print "Error: No output array was referenced in the call to convert_permutations.\n";
      return 0;
 }

 #print "Array size: " . scalar @{$array_ref} . "\n";

 #grab everything that came before what is currently being displayed.
 my @header = (  );
 if ( $previous_ref ) {
      @header = @{$previous_ref};
 }

 while ( my ( $k, $v ) = each ( @{$array_ref} ) ) {

   if ( ref ( $v ) eq 'ARRAY' ) {

        convert_permutations ( $v, $output_array_ref, $indentation + 1, \@header );

   } #ref = array.
   else {
        push ( @header, $v );
        if ( scalar @{$array_ref} == 1 ) {
             push ( @{$output_array_ref}, \@header );
        }
   } #else
 } #while passed array.
}}

 sub show_permutations
{{
 my $array_ref = shift || 0;
 my $indentation = shift || 0;

 if ( ! $array_ref ) {
   return 0;
 }

 while ( my ( $k, $v ) = each ( @{$array_ref} ) ) {
   indent ( $indentation );

   if ( ref ( $v ) eq 'ARRAY' ) {
        show_permutations ( $v, $indentation + 1 );
        print "\n";

   } #ref = array.
   else {
        print "$v\n";
   } #else
 } #while passed array.
}}

 sub indent
{{
 my $spaces = shift || 0;
 for ( my $i = 0; $i < $spaces; $i ++ ) {
   print " ";
 }
}}

#Pass a reference to an array for which you'd like the permutations.
 sub get_permutations
{{
 my $array_ref = shift || 0;
 if ( ! $array_ref ) {
      printf "Error: The call to get_permutations didn't contain a reference to an array to permute.\n";
      return (  ); #return an empty array.
 }

 if ( ref($array_ref) ne 'ARRAY' ) {

     print "Warning: A non-array variable was passed to get_permutations.\n";
     return (  ); #return an empty array.

 }

 my @permutations = (  );
 while ( my ( $k, $v ) = each ( @{$array_ref} ) ) {

   my @branch_head = ( $v ); #this is the only item.
   my @branch_body = (  );
   if ( $k ) {
        @branch_body = @{$array_ref} [ 0 .. $k-1 ];
   }
   my @branch_footer = (  );

   #if we're not on the last element.
   if ( $k < scalar ( @{$array_ref} )-1 ) {
        @branch_footer = @{$array_ref} [ $k+1 .. scalar(@{$array_ref})-1 ];
   }

   @branch_body = ( @branch_body, @branch_footer ); #this will be flattened (array(), array()) = array(....)

   my @permutation = get_permutations ( \@branch_body );
   @permutation = ( @branch_head, @permutation );
   push ( @permutations, \@permutation );

 } #foreach of the array members.

 return @permutations;
}}
