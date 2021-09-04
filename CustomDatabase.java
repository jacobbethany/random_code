/*
 Created on 2021-09-04 by Jacob Bethany
 Purpose: To learn how to connect to a MariaDB database with Java.

 Todo:
 [X] Connect to a MariaDB database.
 [X] Select an arbitrary number of rows of data and keep it in a vector of vectors (rows of columns).
 [X] Allow for updating an arbitrary number of rows (with the number of affected rows or zero returned).
 [X*] Allow for doing inserts (with the new row identifier or zero returned).
 (This will return the number of rows inserted at once in one version of the insertion function; and
 it will return either zero or the new row's identifier in the prepared-statement version thereof.
 It's not guaranteed that we'll be able to get the identifier of the newly inserted row,
  depending on which type/version of the JDBC driver you're using. I'd recomend doing a
  select statement to find the row that you've just inserted,  given some unique properties
  thereof so as to obtain the new row's identifier.
 )
 [X] Create a String escaping function to prepare queries, safely.
 (Note: I still need to research Java's regular expression capabilities for matching and replacing.)

 JDBC/Microsoft JDBC/Mysql Connector/J driver required.
 https://dev.mysql.com/downloads/file/?id=506568

 Note: Mysql must be accessable without SSH in order to test this code.

 Example code to reference:
 https://dev.mysql.com/doc/connector-j/8.0/en/connector-j-usagenotes-statements.html

 How to add jar file dependencies to a Java project with VS Code:
 See: https://www.youtube.com/watch?v=3Qm54znQX2E

 (Basically add the .jar file for MySQL Connector/J.)
*/
package customdatabase; ///This must be named after the folder that the java file is in, for some reason.
import java.sql.*;
import java.util.*; //Vector, Hashtable, ...etc.

 public class CustomDatabase {
  private Connection m_connection;
  private Statement m_statement;
  private ResultSet m_resultset;

  //This should be used before allowing the CustomDatabase class to go out of scope,
  //though it's contained objects will likely go out of scope as well and be freed
  //(garbage collected), anyway.
  public Boolean freeDatabaseResources (  ) {
    try {
      if ( m_resultset != null )
           m_resultset .close (  );

      if ( m_statement != null )
           m_statement .close (  );

      if ( m_connection != null )
           m_connection .close (  );
	} catch ( Exception e ) {
      System.out.print ( "Exception while closing the connection to the MariaDB database and freeing resources: \"" + e .getMessage (  ) + "\"\n" );
      return false;
	} catch ( Throwable t ) {
      System.out.print ( "Throwable exception while closing the connection to the MariaDB database and freeing resources: \"" + t .getMessage (  ) + "\"\n" );
      return false;
	}
    return true;
  }

  //Note: Upon instantiation, this object will connect to a dataase using the passed arguments and fill its private members with the connection information.
  public CustomDatabase ( String str_username, String str_password, String str_url, String str_port, String str_default_database ) {

    try {
      //Class .forName ( "com.mysql.cj.jdbc.Driver" ); //This should be unneeded, see: https://github.com/mysql/mysql-connector-j/blob/release/8.0/src/legacy/java/com/mysql/jdbc/Driver.java
      m_connection = DriverManager .getConnection (
        "jdbc:mysql://" + str_url + ":" + str_port + "/" + str_default_database + "?user=" + str_username + "&password=" + str_password
      );
      m_statement = m_connection .createStatement (  );
	} catch ( Exception e ) {
      System.out.print ( "Exception while connecting to MariaDB: \"" + e .getMessage (  ) + "\"\n" );
	} catch ( Throwable t ) {
      System.out.print ( "Throwable exception while connecting to MariaDB: \"" + t .getMessage (  ) + "\"\n" );
	}

  }

  //This will return the number of affected rows.
  //Note: This version of update requirse that you manually escape all user data included within your query via CustomDatabase.escapeString.
  public int update ( String str_query ) {
    try {
      m_statement = m_connection .createStatement (  );
      return m_statement .executeUpdate ( str_query );

      //if ( m_resultset .rowUpdated (  ) || m_resultset .rowDeleted (  ) ) {
      //  return true;
	  //}
	} catch ( Exception e ) {
      System.out.print ( "Exception while running an update query: \"" + e .getMessage (  ) + "\"\n" );
	} catch ( Throwable t ) {
      System.out.print ( "Throwable exception while running an update query: \"" + t .getMessage (  ) + "\"\n" );
	}

    return -1; //return -1 on error.
  }

  //Overload to use with a prepared statement.
  //This will return the number of affected rows; or -1 on error.
  //Note: This version of update will use a prepared statement, so you won't have to escape any user data, manually.
  //The syntax for the prepared statement is normal except that all fields use '?' characters and are replaced by the
  //corresponding offsets of the passed array, ary_str_parameters.
  public int update ( String str_templated_query, String[] ary_str_parameters ) {
    try {
      PreparedStatement prepared_statement = m_connection .prepareStatement ( str_templated_query );
      for ( int i = 0; i < ary_str_parameters .length; i ++ ) {
        //Note: The parameters have one-based indices within the MySQL driver.
        prepared_statement .setString ( i + 1, ary_str_parameters [ i ] );
	  }
      return prepared_statement.executeUpdate (  );

	} catch ( Exception e ) {
      System.out.print ( "Exception while running an update query: \"" + e .getMessage (  ) + "\"\n" );
	} catch ( Throwable t ) {
      System.out.print ( "Throwable exception while running an update query: \"" + t .getMessage (  ) + "\"\n" );
	}

    //On error, return a negative number of affected rows.
    return -1;
  }

  //We can't guarantee that the newest row identifier generated by our insert will be returned, so we'll just return true, if we've managed to insert something.
  //Note: Even with prepared statements, it's not always possible to know, depending on your JDBC driver version.
  //Note #2: This version of insertion requires that you manually escape all user data that you add to the query via CustomDatabase.escapeString.
  public Boolean insert ( String str_query ) {
    try {
      System.out.print ( "Here's the query that we're attempting to run: \"" + str_query + "\"\n" );
      m_statement = m_connection .createStatement (  );
      //System.out.print ( "Here's the statement that we've just created: \"" + m_statement .toString (  ) + "\"\n" );

      System.out.print ( "The statement was created.\n" );

      //It looks like this receives the number of rows inserted (1 is returned on successful insertion of a single row).
      int i_updated_row_count = m_statement .executeUpdate ( str_query );
      //System.out.print ( "integer result of the query: \"" + i_updated_row_count + "\".\n" );
      return i_updated_row_count > 0 ? true : false;
	} catch ( Exception e ) {
      System.out.print ( "Exception while running an update query: \"" + e .getMessage (  ) + "\"\n" );
	} catch ( Throwable t ) {
      System.out.print ( "Throwable exception while running an update query: \"" + t .getMessage (  ) + "\"\n" );
	}

    return false;
  }

  //We can't guarantee that the newest row identifier generated by our insert will be returned, but we'll try.
  //Returns the newly generated key on success;
  //returns 0 on error.
  public long insert ( String str_templated_query, String [] ary_str_parameters ) {
    try {
      PreparedStatement prepared_statement = m_connection .prepareStatement ( str_templated_query, Statement.RETURN_GENERATED_KEYS );
      for ( int i = 0; i < ary_str_parameters .length; i ++ ) {
        //Note: The parameters have one-based indices within the MySQL driver.
        prepared_statement .setString ( i + 1, ary_str_parameters [ i ] );
	  }
      int i_affected_rows =  prepared_statement .executeUpdate (  );
      if ( 0 == i_affected_rows ) {
        return 0;
	  }
      ResultSet rs_generated_keys = prepared_statement .getGeneratedKeys (  );
      if ( rs_generated_keys .next (  ) ) {
        return rs_generated_keys .getLong ( 1 );
	  }
      return 0;

	} catch ( Exception e ) {
      System.out.print ( "Exception while running an update query: \"" + e .getMessage (  ) + "\"\n" );
	} catch ( Throwable t ) {
      System.out.print ( "Throwable exception while running an update query: \"" + t .getMessage (  ) + "\"\n" );
	}

    return 0;
  }

  //This will return a vector of associations between column names and column values.
  public Vector<Map<String, String>> selectMultiple ( String str_query ) {
    try {
      m_resultset = m_statement .executeQuery ( str_query );
	} catch ( Exception e ) {
      System.out.print ( "Exception during call to executeQuery in selectMultiple: \"" + e .getMessage (  ) + "\"\n" );
	} catch ( Throwable t ) {
      System.out.print ( "Throwable exception during call to executeQuery in selectMultiple: \"" + t .getMessage (  ) + "\"\n" );
	}

    Vector<Map<String, String>> rows = new Vector<Map<String, String>> (  );
    try {

       ResultSetMetaData rsmd = m_resultset .getMetaData (  );
       int i_column_count = rsmd .getColumnCount (  );
       while ( m_resultset .next (  ) ) {
         //Create a new (row) vector each loop iteration to add to the vector (rows) of vectors (columns).
         Map<String, String> this_row = new HashMap<String, String> (  );

         for ( int i = 1; i <= i_column_count; i ++ ) {
           this_row .put ( rsmd .getColumnName ( i ), m_resultset .getString ( i ) );
		 }
         rows .add ( this_row );
         //this_row .clear (  ); //Clearing this would actually clear the row that we added, since it adds a reference to this list.

         //fetch the next row and display it.
         System.out.print ( "We're currently on row #" + m_resultset .getRow (  ) + "\n" );
	   }
	} catch ( Exception e ) {
      System.out.print ( "Exception while connecting to MariaDB: \"" + e .getMessage (  ) + "\"\n" );
	} catch ( Throwable t ) {
      System.out.print ( "Throwable exception while connecting to MariaDB: \"" + t .getMessage (  ) + "\"\n" );
	}

    return rows;
  }

  //This function is no longer needed, but remains here for historical purposes.
  public Boolean showQueryResults (  ) {

    Vector<Vector<String>> rows = new Vector<Vector<String>> (  );
    try {

       ResultSetMetaData rsmd = m_resultset .getMetaData (  );
       int i_column_count = rsmd .getColumnCount (  );
       while ( m_resultset .next (  ) ) {
         //Create a new (row) vector each loop iteration to add to the vector (rows) of vectors (columns).
         Vector<String> this_row = new Vector<String> (  );

         for ( int i = 1; i <= i_column_count; i ++ ) {
           this_row .add ( m_resultset .getString ( i ) );
		 }
         rows .add ( this_row );
         //this_row .clear (  ); //Clearing this would actually clear the row that we added, since it adds a reference to this list.

         //fetch the next row and display it.
         System.out.print ( "We're currently on row #" + m_resultset .getRow (  ) + "\n" );
	   }
       int i_row = 1;
       for ( Vector<String> row : rows ) {
         System.out.print ( "Row #" + i_row + ":\n" );
         i_row ++;

         int i_column = 1;
         for ( String column : row ) {
           System.out.print ( "Column #" + i_column + ": \"" + (column) + "\"\n" );
           i_column ++;
		 }

	  }

	} catch ( Exception e ) {
      System.out.print ( "Exception while connecting to MariaDB: \"" + e .getMessage (  ) + "\"\n" );
      return false;
	} catch ( Throwable t ) {
      System.out.print ( "Throwable exception while connecting to MariaDB: \"" + t .getMessage (  ) + "\"\n" );
      return false;
	}

    return true;
  }

  //Usage str_some_string = escape_string ( str_some_string );
  public static String escapeString ( String str_arbitrary_string ) {
    return str_arbitrary_string .replaceAll ( "([\\\\`'\\\"])", "\\\\$1" );
  }
}
