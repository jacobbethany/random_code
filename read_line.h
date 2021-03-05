#ifndef READ_LINE_HEADER
#define READ_LINE_HEADER 1
 char string_compare ( const char *lpsz_haystack, const char *lpsz_needle );
 const char *find_string ( const char *lpsz_haystack, const char *sz_needle );
 const char *find_character ( const char *lpsz_haystack, char c_needle );
 void string_concat ( char *lpsz_destination_string, const char *lpsz_to_concatenate );
 void memory_move ( char *lp_destination, char *lp_source, unsigned long ul_byte_count );
 unsigned long string_length ( const char *lpsz_string );
 char trim_string ( char *sz_string );
 char concatenate_string ( char **lplpsz_string, char *lpsz_to_concatenate );
 char *read_line ( void );
#endif
