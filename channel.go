/*
 This code was created by Jacob Bethany on 2019-05-29 @ 10:05 AM.
 To run: "go run channel.go"

 Purpose:
 To understand the <- operator.

 Consequently, I've learned the following things:
 The Go keyword starts a new thread with the given function (it can be created on the fly like "go func(){ do stuff here... }()"
 Channels can be read-only, if you define them in the parameter of a function with the <- operator.

 The syntax for assigning an array of items is:

 var [variable name] = [array item count][type] { ..., ..., ... }
 or
 [variable name] := [array item count][type] { ... }

 Note the position of the equation symbol (between the variable name and the type -- a bizarre construct).

 While loops don't exist in Golang and must be emulated like this:
 for dummy := 0; true; dummy++ {
     ..do somethingg here...
 }



 Expectations:
 I thought that the output would be random elements from { "This", "is", "only", "a", "test", "..." } by having two
 threads out of sync with the addition of a Sleep command in the spawned worker thread.
 I was thinking that it was likely that we'd only see "This" 60 times, since the main thread would complete so much more quickly.

 Results:
 Reading from a channel breaks concurrency, and the main thread locks itself to wait for something to exist in the pipe.
 This is a timeout of some duration, I think, because I had the main loop try to read more than what existed and the compiled
 application  eventually complained about all threads being deadlocked.
*/
package main
import "fmt"; //for displaying text in stdout.
//import "regexp";
import "time"; //for sleep
import "strings"; //for strings.Compare (works like strcmp)

// [variable name] <- chan [type]
//Somehow, this is a one-way channel and can't be used here.
//func some_thread ( messages <- chan string ) {

//This way allows the channel to be mutable.
func some_thread ( messages chan string ) {

 //string_list []string := { "This", "is", "only", "a", "test", "..." }

 //or we can declare it explicitly.
 var string_list = [6]string {  "This", "is", "only", "a", "test", "..." }
 //Note the position of the equals sign.
 //var [variable name] = [brackets to denote array][type] {[list of values]}
 //string_list := [3]string{"Japan", "Australia", "Germany"}

  for i := 0; i < 10; i ++ {
      for j := 0; j < 6; j ++ {
          messages <- string_list [ j ]

          time.Sleep ( 1 * time.Second ) //This pause doesn't effect the main thread, but reading from the channel will, since it waits for data to exist before continuing its execution.
      }
  }


 messages <- "done"

} //end of the test worker thread.

func main ( ) {

 //messages := make ( chan string, 100 ) //we'll have 100 of these strings in the channel.
 messages := make ( chan string ) //see if setting the channel to the same string multiple times concatenates or crashes.

 //Using the Go keyword here will run this function concurrently by starting a new thread and continuing the execution of the current function in this current thread.
 go some_thread ( messages )

 //Display the current result of the value in the channel by piping it out in a loop.
 for i := 0; i < 10*6; i ++ {

     //This breaks the concurrency of the two threads. The main thread is esentially locked until there's something to read from the channel.
     some_string := <- messages //get the current total string from the pipe.
     fmt.Printf ( "%d -- Here's the string: [%s]\n", i, some_string );


     //fmt.Printf ( "Is the sleep call locking this thread, if we don't use the channel data?\n" ); //This whole loop happens instantly, if I use this instead of the above two lines.

     //time.Sleep ( 1 * time.Second ) //Having this here will give the same results as having the sleep in the thread that we spawn, since the channel, apparently, queues all data passed to it. Each read pops something off of the queue.
 }

 //Simulate a while loop, since it dosen't seem to exist in Golang.
 for i := 0; true; i ++ {
     if ( 0 == strings.Compare ( "done", <- messages ) ) {
          break;
     }
 }

} //end of main function.
