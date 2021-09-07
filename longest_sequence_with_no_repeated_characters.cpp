//C++ version of the challenge by the same name in this repository.
class Solution {
private:

 int get_subset_length ( const char *s, int i_current_length, std::vector<char> &vec_visited )
 {
   //If we've reached the null terminator, just return the current length.
   if ( ! *s ) {
        return i_current_length;
   }

   //If we've already seen this character, just return the current length.
   if ( std::find ( vec_visited.begin(), vec_visited.end(), *s) != vec_visited.end() ) {
        return i_current_length;
   }

   //Otherwise, add this caracter to the list of found characters, and recurse to find the proper return value.
   vec_visited.push_back ( *s );
   return get_subset_length ( s+1, i_current_length+1, vec_visited );
 }

public:
    int lengthOfLongestSubstring ( string s ) {
      int i_longest_subset_length = 0; //Track the longest subset length that we've found so far.
      std::vector<char> v; //Define the vector outside of the for loop and .reset each iteration.
      const char *p = s .c_str (  ); //Store this in a local variable to avoid extra function calls each loop iteration.
      for ( int i = 0; i < s.length(); i++ ) {
        int i_subset_length = get_subset_length ( p+i, 0, v );
      
        if ( i_longest_subset_length < i_subset_length ) {
          i_longest_subset_length = i_subset_length;
        }
        v .clear (  );
      }

      return i_longest_subset_length;
    } //length of longest substring
};
