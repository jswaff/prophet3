# prophet3
a C based chess engine

Prophet is a console based program, but you can play a game with it using the 
Winboard (Windows) or Xboard (Linux/Mac) graphical user interface.  

http://www.gnu.org/software/xboard/


To build Prophet, you'll need a C++ compiler with SQLite3 development libraries.  On Ubuntu that's libsqlite3-dev.  If you just want binaries, you can download them from http://jamesswafford.com/prophet .

## Opening Book
Prophet has a small opening book but it is not enabled by default.  This is to make life easier for testers.  If you
would like to enable the opening book, you can do it with a command line parameter:

-book=book.db


## Hash Size
The default transposition table size is 128MB, which is really small on most systems.  To increase it you can
use the command line parameter:

-hash=N

where N is the number of MB.





