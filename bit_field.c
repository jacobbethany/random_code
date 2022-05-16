/*
 Created on 2022-05-16 by Jacob Bethany
 Purpose: To see how space/alignment works with bit fields in C.
 Results: They are padded in a way that is unpredictable without
 some run-time tests. I think that I'll stick to uint8_t[N] instead
 of using bit fields. The only use case that I can find for this would
 be to load in data from a file so many bits at a time to read headers,
 but this is still wasteful, since the bits aren't packed next to one
 another with shifts, like I would do with a bit array.
 Note that even doing this:
 struct name { ... } __attribute__ ((packed));
 does not force two consecutive bit fields to share any common
 bytes in memory (on MSVC -- only with GCC), if one ends before a
 multiple of 8 bits. It seems like this won't ever help to
 read/write files that have bit fields in their headers. The amount
 of save space saved in memory also seems questionable.

 Note #2 (GCC specific): It seems as though bit fields are padded
 at the end and always directly before a non-bit field member of
 a structure is defined.
*/
#ifdef _WIN32
 #include <windows.h>
#else
 #include <string.h>
#endif
 #include <stdio.h>
 #include <stdint.h>

 #define PACK_STRUCTURE(N) __attribute__ ((packed, aligned(N)))

struct date {
  //After testing, I now know that this is padded to 1 full byte, since
  //modifying exactly 8 bits of data at the top of the structure affects
  //only this member.
  uint8_t d : 5; //8-4-sizeof(m)

  //This is padded to be 3 bytes to fit to x64 (8 byte) alignment.
  uint8_t m : 4; //8-4-sizeof(d)

  uint32_t y; //4 bytes
};

#ifdef _WIN32
#pragma pack(1) //pack with one-byte alignment.
#endif
struct packed_date {
  //This is padded to be 1 byte on MSVC but is partially shared with packed_date.m on GCC.
  uint8_t d : 5; //6-4-sizeof(m)

  //This is padded to be 1 byte on MSVC but is partially shared with d and padded on GCC.
  uint8_t m : 4; //6-4-sizeof(d)

  uint32_t y; //4 bytes
}
#ifndef _WIN32
  PACK_STRUCTURE(1); //(Adding this property makes the structure one-byte aligned and reduces the structure size to 6 bytes.)
#else
  ; //the pragma pack directive already occurred, in this case.
#endif
#ifdef _WIN32
#pragma pack() //resume default packing
#endif
//} __attribute__ ((packed)); //(Adding this property makes the structure one-byte aligned and reduces the structure size to 6 bytes.)

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
