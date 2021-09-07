 /*
  My solution to a leetcode challenge.
  The challenge was to take two linked lists of digits (in reverse order) and add the numbers one digit at a time.
  The resultant values should be returned in another linked list (reversed order digits).
  See the prototype function given by leetcode, addTwoNumbers(struct ListNode *, struct ListNode *) for usage.
 */
 #include "stdio.h"
 #include "stdlib.h"
 #include "string.h"
 #include "stdint.h"

//Definition for singly-linked list.
struct ListNode {
 int val;
 struct ListNode *next;
};

 void free_nodes ( struct ListNode **root )
{{
 struct ListNode *to_free, *next = *root;
 while ( next ) {
   to_free = next;
   next = next ->next;
   free ( to_free );
 }
 *root = 0;
}}

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
uint64_t custom_pow ( uint64_t ui64_base, uint64_t ui64_power )
{
  uint64_t ui64_val = 1;
  for ( uint64_t ui64_i = 0; ui64_i < ui64_power; ui64_i ++ ) {
    ui64_val *= ui64_base;
  }
  return ui64_val;
}

uint64_t list_to_int ( struct ListNode *some_list, unsigned char *b_too_long )
{
  if ( ! some_list ) {
    return 0;
  }

  uint64_t ui64_total = 0;
  uint64_t ui64_index = 0;
  for ( struct ListNode *node = some_list; node; node = node ->next ) {
    ui64_total += custom_pow ( 10, ui64_index ++ ) * ((uint64_t) node ->val);
  }
  //log10(18,446,744,073,709,551,615) = 19.2659197225
  if ( ui64_index >= 19 ) {
    *b_too_long = 1;
  }
  else {
    *b_too_long = 0;
  }
  return ui64_total;
}

struct ListNode *new_node ( uint64_t ui64_digit )
{
  struct ListNode *node = (struct ListNode *) calloc ( sizeof(struct ListNode), 1 );
  node ->val = ui64_digit;
  node ->next = 0;
  return node;
}

void add_node ( struct ListNode **root, uint64_t ui64_digit )
{
 if ( ! *root ) {
   *root = new_node ( ui64_digit );
   return ;
 }
  struct ListNode *node = *root;
  while ( node ->next ) {
    node = node ->next;
  }
  node ->next = new_node ( ui64_digit );
}

struct ListNode *int_to_list ( uint64_t ui64_digit )
{
  struct ListNode *node = 0;

   do {
     uint64_t ui64_remainder = ui64_digit % 10;
     ui64_digit /= 10;
     add_node ( &node, ui64_remainder );
   } while ( ui64_digit );
  return node;
}

struct ListNode *add_lists_jank ( struct ListNode *l1, struct ListNode *l2 )
{
  struct ListNode *res = 0;
  struct ListNode *b = l2;

  

  uint64_t ui64_digit_overflow = 0, ui64_digit;
  struct ListNode *a = l1;
  while ( 1 ) {

    //The lists of digits can be differently sized.
    if ( b ) {
      //The current digit should be the sum of the two aligned digits from the
      //two lists plus the overflow from the previous digit, if any exists.
      ui64_digit = a ->val + b ->val + ui64_digit_overflow;
      printf ( "The sum of the digits and the carry is %llu.\n", ui64_digit );

      ui64_digit_overflow = ui64_digit > 9 ? ui64_digit / 10 : 0;
      printf ( "The carry for the next digit is %llu.\n", ui64_digit_overflow );

      //add_node ( &res, ui64_digit%10 );
      add_node ( &res, ui64_digit > 9 ? ui64_digit % 10 : ui64_digit );

      b = b ->next;
    }
    else {
      ui64_digit = a ->val + ui64_digit_overflow;
      printf ( "The sum of the digits and the carry is %llu.\n", ui64_digit );

      ui64_digit_overflow = ui64_digit > 9 ? ui64_digit / 10 : 0;
      printf ( "The carry for the next digit is %llu.\n", ui64_digit_overflow );
      add_node ( &res, ui64_digit > 9 ? ui64_digit % 10 : ui64_digit );
    }

    a = a ->next;
    //If the main list ends,
    if ( ! a ) {
      //Then if the other list has members left, switch to it.
      if ( b ) {
        a = b;
        b = 0; //invalidate the other list, since it's already referenced by a, now.
      }
      //Otherwise, there are no nodes to use to continue in either of the lists.
      else {
        break;
      }
    }
  }
  if ( ui64_digit_overflow ) {
    add_node ( &res, ui64_digit_overflow );
  }
  return res;
}

//Prototype given by leetcode problem.
struct ListNode *addTwoNumbers (
  struct ListNode *l1,
  struct ListNode *l2
) {
 unsigned char b_too_long;

 //Try to parse the first list as an unsigned 64 bit integer.
 uint64_t ui64_a = list_to_int ( l1, &b_too_long );

 //If the list has too many nodes (doesn't start with zero unless equal to zero), then it has a value that we can't hold.
 if ( b_too_long ) {
   return add_lists_jank ( l1, l2 );
 }

 //Try to parse the second list as an unsigned 64 bit integer.
 uint64_t ui64_b = list_to_int ( l2, &b_too_long );

 //If the list has too many nodes...see above.
 if ( b_too_long ) {
   return add_lists_jank ( l1, l2 );
 }

 return int_to_list ( ui64_a + ui64_b );
}

//Added entry point to make the tester able to be compiled.
//Note that this has no output, currently.
 int main ( int argc, char **argv )
{{
 struct ListNode *a = new_node ( 0 ), *b = new_node ( 0 );
 struct ListNode *result_list = addTwoNumbers ( 0, 0 );
 free_nodes ( &a );
 free_nodes ( &b );
 free_nodes ( &result_list );

 return 0;
}}
