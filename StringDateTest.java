/*
 Created on 2021-09-15 by Jacob Bethany
 Purpose: To check to see which of two user-defined string dates was earlier.

 Note: I made improvements to the code afterwards to learn more about Date objects in Java.
 Setting leniency seems to be important or more dates will get through than should.
*/
import java.text.ParseException;
import java.text.SimpleDateFormat;
//import java.time.Instant; //For if we wanted to return a particular default date, rather than a null reference on date parsing errors.
import java.util.Date;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
public class StringDateTest {

//Accepts a string date in this format "2021-09-15"
//Returns a Date object that represents this date on success;
//returns a reference to null on failure.
//Usage if ( (someDate = parseyyyymmddStringDate ( "2021-09-15" )) == null ) {
// System.out.print ( "Date parsing failed.\n" );
// return false;
//}
//... Continue processing with the new valid date object.
 public static Date parseyyyymmddStringDate ( String str_date ) {

   //Create a regular expression to ensure that the date is in the proper format, since the leniency flag manipulation is still not enough.
   //It allows dates like "2021-01-1" to be used.
   Pattern pat_valid_date  = Pattern .compile ( "^[0-9]{4}\\-[0-9]{2}\\-[0-9]{2}$" );

   //Try to match the regular expression against the string.
   Matcher match = pat_valid_date .matcher ( str_date );

   //If the regular expression didn't match, then the date is malformed.
   if ( ! match.find (  ) ) {
     //Debugging output to ensure that the regular expression is the failure point, rather than the SimpleDateFormat object.
     //System.out.print ( "The date is malformed.\n" );
     return null;
   }

   SimpleDateFormat simple_date_yyyymmdd_formatter = new SimpleDateFormat ( "yyyy-mm-dd" );
   simple_date_yyyymmdd_formatter .setLenient ( false ); //Otherwise "12345-123-123" will parse with no exception.

   try {

     return simple_date_yyyymmdd_formatter .parse ( str_date );

   } catch ( ParseException e ) {

     //Debugging output to show where the failure point was (regular expression vs this).
     //System.out.print ( "The date could not be parsed by simple_date_yyyymmdd_formatter.\n" );

     //We could default to a date that is guaranteed to be before the first date, to force a failure.
     //System.out.print ( "Parsing error: " + e .getMessage (  ) + ".\n" );
     //return Date .from ( Instant.parse ( "0001-01-01T00:00:00.000Z" ) );

     //I think that returning a null reference to a Date object is better, though.
     return null;

   }
 }

 public static void main ( String[] args ) {

   String str_start_date = "2021-02-03";
   String[] ary_end_date_strings = new String[] {
     "2021-02-04",    //Success test.
     "2021-02-01",    //Premature ending date fail test.
     "12345-12-12",   //Parse fail test. (This actually succeeds.)
     "12345-123-12",  //Parse fail test #2. (passes without leniency set to false)
     "12345-123-123", //Parse fail test #3. (passes leniency set to false)
     "12345/123/123", //Parse fail test #4. (fails because of - vs / as the delimiting token.)
     "1234-12-1"      //Parse passes even with leniency flag manipulation but fails with my regular expression checking!
   };

   Date date_start = parseyyyymmddStringDate ( str_start_date );
   for ( String str_end_date : ary_end_date_strings ) {
     System.out.print ( "Ensuring that the date, \"" + str_end_date + "\" is after \"" + str_start_date + "\"\n" );
     Date date_end = parseyyyymmddStringDate ( str_end_date );

     if ( date_end == null ) {
       System.out.print ( "Error: The ending date could not be parsed.\n" );
       continue;
     }

     //If the ending date isn't after the starting date, note that fact in stdout.
     if ( ! date_end .after ( date_start ) ) {
       System.out.print ( "The date is improper.\n" );
     }
     //Otherwise, the ending date comes after the starting date as it should be.
     else {
       System.out.print ( "The date is proper.\n" );
     }
   }
 } //main method end.

} //StringDateTest class end.
