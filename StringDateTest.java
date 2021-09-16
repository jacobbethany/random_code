/*
 Created on 2021-09-15 by Jacob Bethany
 Purpose: To check to see which of two user-defined string dates was earlier.

 Note: I made improvements to the code afterwards to learn more about Date objects in Java.
 Setting leniency seems to be important or more dates will get through than should.
*/
import java.text.ParseException;
import java.text.SimpleDateFormat;
//import java.time.Instant;
import java.util.Date;

public class StringDateTest {

//Accepts a string date in this format "2021-09-15"
//Returns a Date object that represents this date on success;
//returns a reference to null on failure.
//Usage if ( (someDate = parseyyyymmddStringDate ( "2021-09-15" )) == null ) {
// System.out.print ( "Date parsing failed.\n" );
// return false;
//}
//... Continue processing with the new valid date object.
 public static Date parseyyyymmddStringDate ( String strDate ) {

   SimpleDateFormat simple_date_yyyymmdd_formater = new SimpleDateFormat ( "yyyy-mm-dd" );
   simple_date_yyyymmdd_formater .setLenient ( false ); //Otherwise "12345-123-123" will parse with no exception.

   try {

     return simple_date_yyyymmdd_formater .parse ( strDate );

   } catch ( ParseException e ) {

     //We could default to a date that is guaranteed to be before the first date, to force a failure.
     //System.out.print ( "Parsing error: " + e .getMessage (  ) + ".\n" );
     //return Date .from ( Instant.parse ( "0001-01-01T00:00:00.000Z" ) );

     //I think that returning a null reference to a Date object is better, though.
     return null;

   }
 }

 public static void main ( String[] args ) {

   String startDate = "2021-02-03";
   String[] endDates = new String[] {
     "2021-02-04", //Success test.
     "2021-02-01", //Premature ending date fail test.
     "12345-12-12", //Parse fail test. (This actually succeeds.)
     "12345-123-12", //Parse fail test #2. (passes without leniency set to false)
     "12345-123-123", //Parse fail test #3. (passes leniency set to false)
     "12345/123/123" //Parse fail test #4. (fails because of - vs / as the delimiting token.)
   };

   Date date_start = parseyyyymmddStringDate ( startDate );
   for ( String endDate : endDates ) {
     System.out.print ( "Ensuring that the date, \"" + endDate + "\" is after \"" + startDate + "\"\n" );
     Date date_end = parseyyyymmddStringDate ( endDate );

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
