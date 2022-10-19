/*
 Created on 2022-10-19 by Jacob Bethany
 Purpose: To show the pitfalls of passing &/ storing std::shared_ptr<T>
 &/ std::unique_ptr<T> by reference.

 Test #3 is the fail case with undefined behavior (allowing a std::shared_ptr<some_class>&)
 to reference a locally defined std::shared_ptr<some_class> object (automatic storage duration)
 that has gone out of scope and been destructed and using a function of the wrapped some_class
 object.)

 All other tests (1, 2, & 4) should pass.
*/
 #include <memory>
 #include <iostream>

 class some_class {
   private:
     char *m_lpsz_buffer = 0;
   public:
     some_class ( const char *lpsz_string );
     ~some_class (  );
     void show ( void );
 };
 some_class::some_class ( const char *lpsz_string ) {
   const char *s = lpsz_string;
   for ( ; *s; s ++ );
   m_lpsz_buffer = new char [ s - lpsz_string + 1 ];

   char *d = m_lpsz_buffer;
   s = lpsz_string;
   while ( *d ++ = *s ++ );
 }
 some_class::~some_class (  ) {
   delete[] m_lpsz_buffer;
   std::cout << "destructor called." << std::endl;
 }
 void some_class::show ( void ) {
   std::cout << m_lpsz_buffer << std::endl;
 }


 class some_other_class {
   private:
     std::shared_ptr<some_class> &m_sptr_some_class;
   public:
     some_other_class ( std::shared_ptr<some_class> &sptr_some_class );
     ~some_other_class (  );
    void show ( void );
 };
 some_other_class::some_other_class ( std::shared_ptr<some_class> &sptr_some_class ) :
   m_sptr_some_class ( sptr_some_class ) {
 }
 some_other_class::~some_other_class (  ) {
 }
 void some_other_class::show ( void ) {
   //This check won't save us, if the underlying std::shared_ptr<some_class> that we're referencing (&) has gone out of scope/been destructed, in general.
   //See: test3 & foo2.
   if ( ! m_sptr_some_class ) {
     std::cout << "some_other_class::show: Error: m_sptr_some_class (std::shared_ptr<some_class>&) holds an invalid reference." << std::endl;
     return ;
   }
   m_sptr_some_class ->show (  );
 }


 class some_final_class {
   private:
     std::shared_ptr<some_class> m_sptr_some_class; //not a reference, but a copy of the shared pointer object.
   public:
     some_final_class ( std::shared_ptr<some_class> &sptr_some_class );
     ~some_final_class (  );
     void show ( void );
 };
 some_final_class::some_final_class ( std::shared_ptr<some_class> &sptr_some_class ) {
   m_sptr_some_class = sptr_some_class;
 }
 some_final_class::~some_final_class (  ) {
 }
 void some_final_class::show ( void ) {
   if ( ! m_sptr_some_class ) {
     std::cout << "some_final_class::show: shared pointer contains no valid reference to some_class object." << std::endl;
     return ;
   }

   m_sptr_some_class ->show (  );
   std::cout << "some_final_class: reference count: " << m_sptr_some_class .use_count (  ) << std::endl;
 }




 void bar ( std::shared_ptr<some_class> &c ) {
   std::cout << "bar: " << c .use_count (  ) << "." << std::endl;
 }

 std::shared_ptr<some_class> foo ( void ) {
   std::shared_ptr<some_class> c = std::make_shared<some_class> ( "This is only a test..." );
   std::cout << "foo: " << c .use_count (  ) << "." << std::endl;
   bar ( c );
   return c;
 }

 void test1 ( void ) {
   auto c = foo (  );
   c .get (  ) ->show (  );
   std::cout << "main: " << c .use_count (  ) << "." << std::endl;
 }

 //Create an instance of a class that shares the shared pointer object
 //to let it go out of scope and see whether that invalidates the shared
 //pointer for the original owner.
 void bar2 ( std::shared_ptr<some_class> &sptr_some_class ) {
   std::cout << "bar2: Original use count: " << sptr_some_class .use_count (  ) << "." << std::endl;
   some_other_class soc ( sptr_some_class );
   soc .show (  );
   std::cout << "bar2 New use count: " << sptr_some_class .use_count (  ) << "." << std::endl;
 }
 void test2 ( void ) {

   //We have a valid shared pointer.
   std::shared_ptr<some_class> c = std::make_shared<some_class> ( "This is a second test." );

   //Show that the shared pointer is valid.
   c ->show (  );

   std::cout << "test2: Original use count: " << c .use_count (  ) << "." << std::endl;

   //Share the shared pointer with a class that holds it by reference (std::shared_ptr<T> &)
   //in a new function so that it can go out of scope.
   bar2 ( c );

   std::cout << "test2: New use count: " << c .use_count (  ) << "." << std::endl;

   //See if allowing the shared pointer to be shared by reference with another object that
   //has gone out of scope has caused the shared pointer that we own to become invalid.
   c ->show (  );
 }

 //This should fail because the local sptr_some_class variable should go out of scope and
 //invalidate the shared pointer stored by reference within the some_other_class object.
 some_other_class foo2 ( void ) {
   std::shared_ptr<some_class> sptr_some_class = std::make_shared<some_class> ( "This is a final test." );
   std::cout << "foo2: initial reference count: " << sptr_some_class .use_count (  ) << std::endl;
   some_other_class soc ( sptr_some_class );
   std::cout << "foo2: new reference count: " << sptr_some_class .use_count (  ) << std::endl;
   return soc;
 }
 //Test what happens when we share a reference to a shared pointer and let the original
 //owner of the shared pointer go out of scope and invalidate the reference.
 void test3 (  ) {
   //This object should hold an invalid reference to a some_class object because the
   //shared object that it stores has gone out of scope and had its destructor called.
   some_other_class soc = foo2 (  );
   std::cout << "test3: Trying to use some_other_class::show when it holds an invalid reference." << std::endl;
   soc .show (  );
 }



 //This should fail because the local sptr_some_class variable should go out of scope and
 //invalidate the shared pointer stored by reference within the some_other_class object.
 some_final_class foo3 ( void ) {
   std::shared_ptr<some_class> sptr_some_class = std::make_shared<some_class> ( "This is a final test." );
   std::cout << "foo2: initial reference count: " << sptr_some_class .use_count (  ) << std::endl;
   some_final_class soc ( sptr_some_class );
   std::cout << "foo2: new reference count: " << sptr_some_class .use_count (  ) << std::endl;
   return soc;
 }
 //Test what happens when we share a reference to a shared pointer and let the original
 //owner of the shared pointer go out of scope and invalidate the reference.
 void test4 (  ) {
   //This object should hold an invalid reference to a some_class object because the
   //shared object that it stores has gone out of scope and had its destructor called.
   some_final_class soc = foo3 (  );
   std::cout << "test3: Trying to use some_other_class::show, which should still hold a valid reference." << std::endl;
   soc .show (  );
 }


 int main ( int argc, char **argv ) {

   std::cout << "main: before test1." << std::endl;
   test1 (  );

   std::cout << "main: before test2." << std::endl;
   //Thankfully, allowing for a shared pointer to be passed and stored by reference in a class doesn't cause that class's destructor to free and invalidate the shared pointer.
   //In fact, in general, any class object that is a reference (T * or T &) will not have its destructor called via delete or delete[] during the parent object's destructor.
   //This can, however, be done manually, if desired.
   test2 (  );

   std::cout << "main: before test3." << std::endl;
   test3 (  );

   std::cout << "main: before test4." << std::endl;
   test4 (  );

   std::cout << "main: done." << std::endl;

   return 0;
 }
