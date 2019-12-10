/*
 Code written on 2019-12-09 by Jacob Bethany.

 Purpose: This program will find the longest common sequence of characters within two arrays of unsigned characters.
 -The arrays can each be any length greater than zero.
 -They can be different lengths from one another.
 -It doesn't matter if the first or second is the longer of the two, if their sizes differ.
*/
 #include <string.h>
 #include <stdio.h>
 #include <stdlib.h>

/*
 This function assumes that the first array is the same size or larger than the second one.
*/
 unsigned char get_longest_common_sequence (
   unsigned char *lpuc_array1,
   unsigned long ul_array1_length,
   unsigned char *lpuc_array2,
   unsigned long ul_array2_length,
   unsigned char **lpuc_longest_common_sequence_start,
   unsigned char **lpuc_longest_common_sequence_end
 )
{{
 if ( ! lpuc_array1 )
     {
      printf ( "Error: The first of the two passed arrays is a null pointer.\n" );
      return 0;
     }

 if ( ! lpuc_array2 )
     {
      printf ( "Error: The second of the two passed arrays is a null pointer.\n" );
      return 0;
     }

 if ( ! lpuc_longest_common_sequence_start )
     {
      printf ( "Error: A bad reference was passed to the variable that will contain the longest common sequence's start position within the array.\n" );
      return 0;
     }

 if ( ! lpuc_longest_common_sequence_end )
     {
      printf ( "Error: A bad reference was passed to the variable that will contain the longest common sequence's end position within the array.\n" );
      return 0;
     }

 if ( ! ul_array1_length )
     {
      printf ( "Error: The length of the first array is zero!\n" );
      return 0;
     }

 if ( ! ul_array2_length )
     {
      printf ( "Error: The length of the second array is zero!\n" );
      return 0;
     }

 unsigned long ul_longest_run = 0;

 //Ensure that we always check every sequence of the second array from every starting point of the first array by using the longest possible first array.
 if ( ul_array1_length < ul_array2_length )
     {
      unsigned long ul_temp_array_length = ul_array1_length; //Preserve the length of the first of the arrays.
      unsigned char *lpuc_temp_array = lpuc_array1; //Backup the first of the array pointers.
      lpuc_array1 = lpuc_array2; //assign the first to the second, now that it has been preserved.
      lpuc_array2 = lpuc_temp_array; //assign the second to the first.

      ul_array1_length = ul_array2_length; //Set the first of the array lengths to be the same as the second array's length, now that we've preserved the length of the first array.
      ul_array2_length = ul_temp_array_length; //Set the second array's length to be the same as what the first array's length used to be.
     }

 for ( unsigned long array1_iterator; array1_iterator < ul_array1_length; array1_iterator ++ )
      {
       unsigned long ul_current_run_length = 0;
       unsigned char *lpuc_current_run_start = &lpuc_array1 [ array1_iterator ];

       for ( unsigned long array2_iterator = 0; array2_iterator < ul_array2_length; array2_iterator ++ )
            {
             if ( array1_iterator + array2_iterator >= ul_array1_length )
                  break;

             if ( lpuc_array1 [ array1_iterator + array2_iterator] == lpuc_array2 [ array2_iterator ] )
                 {
                  if ( ! ul_current_run_length )
                       lpuc_current_run_start = &lpuc_array1 [ array1_iterator + array2_iterator ];

                  ul_current_run_length ++;
                  printf ( "%c = %c (@ array1 offset %d; @array2 offset %d)\n",
                    lpuc_array1 [ array1_iterator + array2_iterator],
                    lpuc_array2 [ array2_iterator ],
                    array1_iterator,
                    array2_iterator
                  );

                  if ( ul_current_run_length > ul_longest_run )
                      {
                       ul_longest_run = ul_current_run_length;
                       *lpuc_longest_common_sequence_start = lpuc_current_run_start;

                       printf ( "Here's the start of the currently longest match, \"%s\"\n", lpuc_current_run_start );

                       *lpuc_longest_common_sequence_end = &lpuc_current_run_start [ ul_longest_run ];
                       printf ( "Here's the last byte of the sequence: [%c]\n", **lpuc_longest_common_sequence_end );
                      }
                 }
             else
                 {
                  if ( ul_current_run_length )
                       printf ( "The common sequence has ended.\n" );

                  ul_current_run_length = 0;
                 }
            }
      }

 return 1;
}}

 int main ( int argc, char *argv [  ] )
{{
 char array1 [  ] = "This is only a test...y";
 char array2 [  ] = "xTh1s 1s 0n1y 4 test...xx"; //"test..." = 7

 unsigned char *lpuc_longest_common_sequence_start = 0;
 unsigned char *lpuc_longest_common_sequence_end = 0;

 if ( ! get_longest_common_sequence ( (unsigned char *) array1, sizeof(array1), (unsigned char *) array2, sizeof(array2), &lpuc_longest_common_sequence_start, &lpuc_longest_common_sequence_end  ) )
     {
      printf ( "Error: We've failed to obtain the longest common sequence between the two arrays.\n" );
      return 0;
     }

 printf ( "Here's the longest common sequence: [" );
 for ( unsigned char *lpuc_byte = lpuc_longest_common_sequence_start; lpuc_byte < lpuc_longest_common_sequence_end; lpuc_byte ++ )
       printf ( "%c", *lpuc_byte );
 printf ( "]\n" );

 return 0;
}}
