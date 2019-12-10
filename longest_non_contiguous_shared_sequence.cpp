/*
 This code was written on 2019-12-09 by Jacob Bethany.
 Purpose: To find and display the longest of all common non-contiguous subsequences between two arrays and its length.
 I do this by masking the shorter of the two arrays with each possible combination of values to use for finding similarly
 ordered but non-contiguous sequences.
*/
 #include "string.h"
 #include "stdio.h"
 #include "stdlib.h"

/*
 This will treat an array of variable length as a series of bits (it has boolean 1 or 0 values for each element).
 Each call to this function will increment the "value" of the array by one by starting at its last element and
 flipping  the value of each of the elements (byte flags emulating bit flags) until a zero is reached.
*/
 unsigned char increment_array_mask ( unsigned char *lpuc_array_mask, unsigned long ul_mask_length )
{{
 for ( unsigned long ul_i = ul_mask_length; ul_i > 0; ul_i -- )
      {
       if ( lpuc_array_mask [ ul_i - 1 ] == 0 )
           {
            lpuc_array_mask [ ul_i - 1 ] = 1;
            break;
           }
       lpuc_array_mask [ ul_i - 1 ] = ! lpuc_array_mask [ ul_i - 1 ];
      }
}}

/*
 This displays the array mask as a binary value of variable size (array length bits).
*/
 unsigned char show_array_mask ( unsigned char *lpuc_array_mask, unsigned long ul_mask_length )
{{
 for ( unsigned long ul_i = 0; ul_i < ul_mask_length; ul_i ++ )
      {
       printf ( "%d", (lpuc_array_mask [ ul_i ] ? 1 : 0) );
      }
 printf ( "\n" );
}}

/*
 This will find the longest non-contiguous shared sequence between the two arrays.
 A reference to mask used to find this sequence will be returned in out_lpuc_longest_mask.
 The length of the longest sequence (as denoted by the returned mask) will be returned when the function terminates.
*/
 unsigned long find_longest_common_noncontiguous_sequence (
   unsigned char *lpuc_array1,
   unsigned long ul_array1_length,
   unsigned char *lpuc_array2,
   unsigned long ul_array2_length,
   unsigned char **out_lpuc_longest_mask
 )
{{
 //Ensure that a bad pointer wasn't passed for the first of the two arrays.
 if ( ! lpuc_array1 )
     {
      printf ( "Error: The pointer to the first array was invalid.\n" );
      return 0;
     }

 //Ensure that a bad pointer wasn't passed for the second of the two arrays.
 if ( ! lpuc_array2 )
     {
      printf ( "Error: The pointer to the second array was invalid.\n" );
      return 0;
     }

 //Ensure that the first of the two arrays actually has a length.
 if ( ! ul_array1_length )
     {
      printf ( "Error: The length of the first array was zero.\n" );
      return 0;
     }

 //Ensure that the second of the two arrays actually has a length.
 if ( ! ul_array2_length )
     {
      printf ( "Error: The length of the second array was zero.\n" );
      return 0;
     }

 //Ensure that the variable passed to hold the reference to the mask that we will have used to find the longest
 //of the shared non-contiguous sequences is valid.
 if ( ! out_lpuc_longest_mask )
     {
      printf ( "Error: The passed reference to a variable to receive a pointer to the mask used to find the longest common non-contiguous sequence was invalid.\n" );
      return 0;
     }

 //Notes on the method that this function will employ:
 //Check the masked members of the shorter array against the longer array.
 //We'll only look forwardly for the next masked character of the shorter array until we find it.
 //Eventually, we'll be unable to find one of the items of the mask.
 //We'll want to return the number of matched items within the returned mask so that we know which
 //items of the mask matched and how many of them there were.

 //Force array1 to be the shorter of the two arrays, always.
 if ( ul_array1_length > ul_array2_length )
     {
      unsigned long ul_temp_length = ul_array1_length;
      ul_array1_length = ul_array2_length;
      ul_array2_length = ul_temp_length;

      unsigned char *lpuc_temp_array;
      lpuc_array2 = lpuc_array1;
      lpuc_array1 = lpuc_temp_array;
     }

 //Allocate a buffer to use for masking the shorter of the two arrays by incrementing it between searches within the larger of the two arrays
 //for all masked items within the shorter.
 unsigned char *lpuc_mask = (unsigned char *) calloc ( ul_array1_length, 1 ); //Allocate some pre-zeroed memory.
 if ( ! lpuc_mask )
     {
      printf ( "Error: We were unable to allocate the memory required to search for non-contiguous subsequences.\n" );
      return 0;
     }

 //Allocate a buffer to hold the mask that matched the longest non-contiguous sequence.
 //This needs to be stored, or we need to store the number of times the mask was incremented and rebuild it by zeroing it and incrementing it N times.
 //This would save some memory at the cost of processing time.
 unsigned char *lpuc_longest_mask = (unsigned char *) calloc ( ul_array1_length, 1 ); //Allocate some pre-zeroed memory.
 if ( ! lpuc_longest_mask )
     {
      printf ( "Error: We were unable to allocate the memory required to store the longest of the non-contiguous subsequences.\n" );
      return 0;
     }

 //Start with a longest non-contiguous subsequence length of zero.
 unsigned long ul_longest_subsequence_length = 0;

 //For every possible combination of mask values of the shorter array.
 for ( unsigned long ul_mask_iterator = 0; ul_mask_iterator < 1<<ul_array1_length; ul_mask_iterator ++ )
      {
       printf ( "We're incrementing the mask (%d) before we continue checking anything.\n", ul_mask_iterator );

       increment_array_mask ( lpuc_mask, ul_array1_length );
       unsigned long ul_current_subsequence_length = 0;

       //We need to start farther in the array each time, so we need to know where we last found something.
       unsigned long ul_previous_match_position = 0;
       unsigned char uc_matched_something_already = 0;

       //For every item in the mask that we've just built (which is the same size as the mask).
       for ( unsigned long ul_shorter_array_iterator = 0; ul_shorter_array_iterator < ul_array1_length; ul_shorter_array_iterator ++ )
            {
             //show_array_mask ( lpuc_mask, ul_array1_length );

             //If we don't need to match this element of the shorter (masked) array, just skip it.
             if ( ! lpuc_mask [ ul_shorter_array_iterator ] )
                  continue;

             printf ( "Persuant to the aformentioned array mask, we're going to attempt to match the current associated character of the shorter of the arrays, [%c].\n",
                      lpuc_array1 [ ul_shorter_array_iterator ]
             );

             //For each member after the previous match (if there was one) of the longer array, try to match the short array's current masked element.
             for ( unsigned long ul_longer_array_iterator = uc_matched_something_already ? ul_previous_match_position+1 : 0; ul_longer_array_iterator < ul_array2_length; ul_longer_array_iterator ++ )
                  {
                   if ( ul_mask_iterator == 45 ) //the pattern that would match it.
                        printf ( "[%c] =?= [%c]\n", lpuc_array1 [ ul_shorter_array_iterator ], lpuc_array2 [ ul_longer_array_iterator ] );

                   //If we've managed to match the maked element of the shorter array.
                   if ( lpuc_array2 [ ul_longer_array_iterator ] == lpuc_array1 [ ul_shorter_array_iterator ] )
                       {
                        uc_matched_something_already = 1;
                        ul_current_subsequence_length ++; //Track how long this masked sequence is.
                        ul_previous_match_position = ul_longer_array_iterator; //Track where we found this element of the masked array within the larger array so that we don't use it again or start before it to find something else.

                        if ( ul_longest_subsequence_length < ul_current_subsequence_length )
                            {
                             ul_longest_subsequence_length = ul_current_subsequence_length;
                             memcpy ( lpuc_longest_mask, lpuc_mask, ul_array1_length );
                            }

                        break; //We've found what we were looking for, so there's no need to continue iterating through the longer array.
                       }
                  }
            }
      }

 //We no longer need the search mask.
 free ( lpuc_mask );

 //Return the longest mask's pointer in the passed reference variable.
 *out_lpuc_longest_mask = lpuc_longest_mask;

 return ul_longest_subsequence_length;
}}

 int main ( int argc, char *argv [  ] )
{{
 char array1 [  ] = "AGGTAB";
 char array2 [  ] = "GXTXAYB";
 unsigned long ul_shortest_array_length = sizeof(array1) <= sizeof(array2) ? sizeof(array1) : sizeof (array2);
 char *lpuc_shortest_array = sizeof(array1) < sizeof(array2) ? array1 : array2;

 unsigned char *lpuc_mask = 0;

 unsigned long ul_longest_common_non_contiguous_sequence_length = find_longest_common_noncontiguous_sequence (
   (unsigned char *) array1,
   sizeof ( array1 ),
   (unsigned char *) array2,
   sizeof ( array2 ),
   &lpuc_mask
 );

 if ( ul_longest_common_non_contiguous_sequence_length )
     {
      printf ( "The longest non-contiguous sequence length was %d.\n", ul_longest_common_non_contiguous_sequence_length );

      printf ( "Here is the longest non-contiguous common sequence: [" );
      for ( unsigned long ul_i = 0; ul_i < ul_shortest_array_length; ul_i ++ )
           {
            if ( lpuc_mask [ ul_i ] )
                 printf ( "%c", lpuc_shortest_array [ ul_i ] );
           }
      printf ( "]\n" );
     }
 else printf ( "No common non-contiguous sequence length was returned.\n" );

 if ( lpuc_mask )
     {
      show_array_mask ( lpuc_mask, ul_shortest_array_length );
      printf ( "We're now freeing the returned mask.\n" );
      free ( lpuc_mask );
     }

/*
 //This is a test to see what it looks like if we
 unsigned char uc_array_mask [ 20 ] = { 0 };
 for ( unsigned long ul_i = 0; ul_i < 1 << 20; ul_i ++ )
      {
       increment_array_mask ( uc_array_mask, 20 );
       show_array_mask ( uc_array_mask, 20 );
      }
*/

 return 0;
}}
