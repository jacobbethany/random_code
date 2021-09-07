//From a leetcode problem.
int lengthOfLongestSubstring ( char * s ) {
  const char *lpsz_string;
  char sz_visited_characters [ 1024 ] = { 0 };
  int i_longest_substring = 0;
  int i_length = strlen ( s );
  int i_subset_length;
  char *e = sz_visited_characters; //end position of the sz_visited_characters buffer.

  for ( int i_i = 0; i_i < i_length; i_i ++ ) {

    lpsz_string = s + i_i;

    while ( *lpsz_string ) {

      //If the character has already been seen.
      if ( strchr ( sz_visited_characters, *lpsz_string ) ) {
        i_subset_length = e - sz_visited_characters;
        if ( i_longest_substring < i_subset_length ) {
          i_longest_substring = i_subset_length;
        } //if the substring is longer than the previously recorded.

        //Since we've encountered the same character again, start recording, freshly.
        break;
      } //if this character has been seen before.

      *e ++ = *lpsz_string ++;
      *e = 0;
    } //for each of the characters in the string.

    i_subset_length = e - sz_visited_characters;
    if ( i_longest_substring < i_subset_length ) {
      i_longest_substring = i_subset_length;
    }

    e = sz_visited_characters;
    *e = 0;
  }

  return i_longest_substring;
} //length of longest substring
