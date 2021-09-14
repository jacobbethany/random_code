/*
 Created on 2021-09-13 by Jacob Bethany
 Purpose: To create my own unit testing class for Java, since I couldn't figure out how
 to configure JSUnit or any of its subsidiary branches to work with VS Code.
*/
import Java.io;

//We can also throw our own custom exception, if we want. I'm not sure what the advantage would be, however.
class CustomException extends Exception {
  CustomException ( String str_exception_message ) {
    Super .Exception ( str_exception_message );
  }
}

//Make a class with static methods to handle unit testing.
class FakeUnitTest {
  FakeUnitTest (  ) {
		}

  //This will return a string to prepend to the assertion error displayed in assertEquals upon failure.
  private static String  getCurrentLineNumber (  ) {
    //return "Line #" + Thread .currentThread (  ) .getStackTrace (  ) [ 1 ] .getLineNumber (  ) + ": "; //This method.
    //return "Line #" + Thread .currentThread (  ) .getStackTrace (  ) [ 2 ] .getLineNumber (  ) + ": "; //assertEquals method.
    return "Line #" + Thread .currentThread (  ) .getStackTrace (  ) [ 3 ] .getLineNumber (  ) + ": "; //The function that called the assertEquals method.
		}
  public static void  assertEquals ( Object a, Object b ) throws Exception {
    if ( a == null) {
      throw new Exception ( getCurrentLineNumber (  ) + "Error: assertEquals called upon a null a term.\n" );
				}

    if (  b == null ) {
      throw new Exception ( getCurrentLineNumber (  ) + "Error: assertEquals called upon a null a term.\n" );
				}

    if ( ! a .equals ( b ) ) {
      throw new Exception ( getCurrentLineNumber (  ) + "Error: Objects are not equal; the assertion has failed.\n" );
				}
		}
} //end of FakeUnitTest class.

//Define a class that manages a set (unordered array filled with unique entities)
//This takes any arbitrary type(s) and can add/remove/search for them.
class CustomSet {

 Object[] m_objects;
 int m_i_next_index;

  //Initialize the array to contain ten nodes;
  //and set the insertion point at the first of those nodes (0).
  CustomSet (  ) {
    m_objects = new Object [ 10 ];
    m_i_next_index = 0;
		}

  //Inform the caller if the set is empty.
  Boolean isEmpty (  ) {
    return m_i_next_index == 0;
	 }

 //Add a node to the set, expanding the array, if necessary.
 Boolean addNode ( Object obj ) {
   if ( containsNode ( obj ) ) {
     return false;
			}

   if ( m_i_next_index == m_objects .length ) {

     System.out.print ( "DEBUG: Doubling the size of the array from \"" + m_objects .length + "\" to \"" + (m_objects .length << 1) + "\"\n" );

     //Create a new array that is twice the size of the old array.
     Object[] obj_extended_objects = new Object [ m_i_next_index << 1 ];
     //Essentially memcpy the elements from the source array to the destination array.
     System .arraycopy (
       m_objects, //Copy from...
       0, //starting on offset...
       obj_extended_objects, //copy to...
       0, //starting at insertion offset...
       m_i_next_index //for this many elements...
     );

     //Let the garbage collect the old object as we reference the new array instead.
     m_objects = obj_extended_objects;

			}

   //Now that we're sure that we have room, add the object and keep track of the next insertion point.
   m_objects [ m_i_next_index ++ ] = obj;

   return true;
	}

 //Try to find the index of a node.
 //Returns the index of the node on success.
 //returns -1 (invalid offset) on error.
 int findNode ( Object obj ) {
   if ( m_i_next_index == 0 ) {
     return -1;
			}

   for ( int i = 0; i < m_i_next_index; i ++ ) {
     if ( obj.equals ( m_objects [ i ] ) ) {
       return i;
					} //if objects are equal.
			} //for
   return -1;
	}

 //Checks to see if a node with a given value exists within the set.
 //Returns true, if the node exists;
 //returns false, if the node does not exist.
 Boolean containsNode ( Object obj ) {
   return findNode ( obj ) != -1;
	}

 //Removes a node from the set by its value, if it exists.
 //Returns true, if the node was found;
 //returns false, otherwise.
 Boolean removeNode ( Object obj ) {

   //Try to find an index for the node to remove.
   int i_index = findNode ( obj );

   //If the object couldn't be found just return false.
   if ( i_index == -1 ) {
     return false;
			}

  //Otherwise, move the last node of the list to this position,
   m_objects [ i_index ] = m_objects [ m_i_next_index - 1 ];

   //then null the last node, pre-decrementing the insertion index in the process.
   m_objects [ -- m_i_next_index ] = null;

   return true;
	}

 //Returns the integer index where the next node would be inserted.
 //This is equal to the current number of nodes within the list.
 int getNodeCount (  ) {
   //Since the insertion point is always the next index, it will also be equal to the count.
   return m_i_next_index;
	}

 //Show all nodes contained within the set in stdout.
 void showNodes (  ) {
   for ( int i = 0; i < m_i_next_index; i ++ ) {
     System.out.print ( "This is the current object: \"" + m_objects [ i ] + "\"\n" );
			}
	}

}

//Define our main class for this Java application.
class UnitTest {

 //Program entry point.
  public static void main ( String[] argv ) {

    try {
      CustomSet set = new CustomSet (  );
      System.out.print ( "Checking to see whether the set is empty.\n" );
      System.out.print ( "Is the set empty? [" + set .isEmpty (  ) + "]\n" );
      FakeUnitTest .assertEquals (
        true, //should be true.
        set .isEmpty (  )
      );

      System.out.print ( "Adding a node to the set.\n" );
      set .addNode ( "This is only a test..." );
      System.out.print ( "Is the set empty? [" + set .isEmpty (  ) + "]\n" );
      FakeUnitTest.assertEquals (
        false,
        set .isEmpty (  )
      );

      System.out.print ( "Attemping to add the same node to the set, again.\n" );
      set .addNode ( "This is only a test..." );
      System.out.print ( "There are currently " + set .getNodeCount (  ) + " nodes in the set and we expect 1.\n" );
      FakeUnitTest .assertEquals (
        1,
        set .getNodeCount (  )
      );

      //Add the words, "This", "is", ..., "test" as individual nodes of the set.
      for ( String str : new String[] { "This", "is", "only", "a", "test" } ) {
        set .addNode ( str );
						}
      FakeUnitTest .assertEquals (
        6, //5 to force an exception.
        set .getNodeCount (  )
      );

      set.showNodes (  );

      //Setting i to 155 will trigger the DEBUG: Doubling the size of the array from __ to __ message one last time.
      for ( int i = 0; i < 154; i ++ ) {
        set .addNode ( i );
						}

      FakeUnitTest .assertEquals (
        160, //The number of nodes in the list should be 160, which means that the highest index is currently 159.
        set .getNodeCount (  )
      );

			 } catch ( Exception e ) {
      System.out.print ( "Here's the error: \"" + e .getMessage (  ) + "\"\n" );
			 } catch ( Throwable t ) {
      System.out.print ( "Here's the error: \"" + t .getMessage (  ) + "\"\n" );
			 }

	 }
}
