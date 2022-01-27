USAGE  
make verify  
./verify  
This creates 'verify_out.txt', a listing of different file parser's information for each test file
and the syntax highliter language choosen.  
  
make tall  
./tall  
This makes use of libpfile.a to make sure read files are utf-8 buffers, to read into a gtktextview.
This is where encodings are used. Also explains why I choose 'tp' as param passing. It also tests
for icon listings for each mime type.  
  
NOTE  
Examples show how to alter certain mimes to make use of gtksourceview language manager.
