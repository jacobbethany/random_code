/*
 Created on 2022-05-16 by Jacob Bethany
 Purpose: To see how space/alignment works with bit fields in C.
 Results: They are padded in a way that is unpredictable without
 some run-time tests. I think that I'll stick to uint8_t[N] instead
 of using bit fields. The only use case that I can find for this would
 be to load in data from a file so many bits at a time to read headers,
 but this is still wasteful, since the bits aren't packed next to one
 another with shifts, like I would do with a bit array.

 Note: Bit fields can be useful with this method:
 struct name { ... } __attribute__ ((packed));
 By packing the structure to one byte alignment, the bit fields are
 crammed together, which allows for reading from and writing to files
 in a way that actually saves space.

 Note #2: It seems as though bit fields are padded at the end and always
 directly before a non-bit field member of a structure is defined.
*/
 #include <stdio.h>
 #include <stdint.h>

struct date {
  //Either this is padded to be two bytes or it is 1 byte and
  //the next variable, m, is padded to be 3 bytes.
  uint8_t d : 5; //8-4-sizeof(m)

  //I've checked by overwriting bytes from the top of the structure.
  //This variable is padded to three bytes and the variable above is
  //padded to be exactly one byte. This is probably because, combined
  //with the 4 bytes for the variable, y, this structure is exactly
  //8 bytes in length which properly aligns for pointers in the x64
  //architecture.
  uint8_t m : 4; //8-4-sizeof(d)

  uint32_t y; //4 bytes
};

struct packed_date {
  //These two variables are crammed into parts of the same byte and
  //m extends into the next full byte. The end of the second byte
  //containing m is padding.
  //y exists in its own four-byte space.
  uint8_t d : 5; //6-4-sizeof(m)
  uint8_t m : 4; //6-4-sizeof(d)
  uint32_t y; //4 bytes
} __attribute__ ((packed, aligned(1))); //(Adding this property makes the structure one-byte aligned and reduces the structure size to 6 bytes.)
//} __attribute__ ((packed)); //(Adding this property makes the structure one-byte aligned and reduces the structure size to 6 bytes.)
//Note: With this property, the bits of d and m are packed directly
//against one another, making the bitfields useful for file packing.

 void test_date ( void ) {
   printf (
     "Size of date is %lu bytes\n",
     sizeof(struct date)
   );
   struct date dt = { 1, 1, 1 };
   printf ( "Date(d/m/y) is %02d/%02d/%04d\n\n", dt.d, dt.m, dt.y );

/*
   //It is illegal to call sizeof on a bitfield.
   printf (
     "sizeof(d): %d\nsizeof(m): %d\nsizeof(y): %d\n",
     sizeof(dt.d), sizeof(dt.m), sizeof(dt.y)
   );
*/

/*
   //It is illegal to take the address of a bitfield.
   printf (
     "&dt: %p\n&dt.d: %p\&dt.m: %p\n&dt.y: %p\n",
     &dt, &dt.d, &dt.m, &dt.y
   );
*/

   printf (
     "sizeof(date): %d\n&dt: %p\n&dt.y: %p\ndelta: %d\n",
     sizeof(struct date),
     &dt, &dt.y,
     (uint8_t*) &dt.y - (uint8_t *) &dt
   );

   //Defined d:5, m:4, y:32
   //uint8_t *lp_day = (uint8_t *) &dt.d; //It is illegal to reference the bit field.

   //Since we can't reference the bit fields directly, let's reference
   //the top of the structure and manipulate its bytes one at a time,
   //while noting any differences in stdout so we can see where the
   //bit fields exist relative to the top of the structure and to other
   //members therein.
   for ( int i = 0; i < sizeof(struct date); i ++ ) {

     uint8_t *lpui8_p = ((uint8_t *) &dt) + i;

     printf (
       "Writing %d bytes to address %p (delta: %d).\n",
       sizeof(uint8_t),
       lpui8_p,
       lpui8_p - (uint8_t *) &dt
     );

     *lpui8_p = 0xff; //By setting all bits to 1, we can see which variables are affected by each 8 bit manipulation to get a clearer indication of what is stored at each address.

     //Show the date, now that the current date has been overridden.
     printf ( "Date(d/m/y) is %02d/%02d/%04d\n", dt.d, dt.m, dt.y );
   }
   printf ( "\n\n" );

 }

 void test_packed_date ( void ) {
   printf (
     "Size of packed_date is %lu bytes\n",
     sizeof(struct packed_date)
   );
   struct packed_date dt = { 1, 1, 1 };
   printf ( "packed_date(d/m/y) is %02d/%02d/%04d\n\n", dt.d, dt.m, dt.y );

   printf (
     "sizeof(packed_date): %d\n&dt: %p\n&dt.y: %p\ndelta: %d\n",
     sizeof(struct packed_date),
     &dt, &dt.y,
     (uint8_t*) &dt.y - (uint8_t *) &dt
   );

   //Since we can't reference the bit fields directly, let's reference
   //the top of the structure and manipulate its bytes one at a time,
   //while noting any differences in stdout so we can see where the
   //bit fields exist relative to the top of the structure and to other
   //members therein.
   for ( int i = 0; i < sizeof(struct packed_date); i ++ ) {

     uint8_t *lpui8_p = ((uint8_t *) &dt) + i;

     printf (
       "Writing %d bytes to address %p (delta: %d).\n",
       sizeof(uint8_t),
       lpui8_p,
       lpui8_p - (uint8_t *) &dt
     );

     *lpui8_p = 0xff; //By setting all bits to 1, we can see which variables are affected by each 8 bit manipulation to get a clearer indication of what is stored at each address.

     //Show the date, now that the current date has been overridden.
     printf ( "packed_date(d/m/y) is %02d/%02d/%04d\n", dt.d, dt.m, dt.y );
   }
   printf ( "\n\n" );
 }

 int main ( int argc, char **argv ) {

   test_date (  );
   test_packed_date (  );

   return 0;

 }
