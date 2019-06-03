# prophet3

a C++ as a better C based chess engine

## Introduction 

Prophet is a console based program, but you can play a game with it using the 
Winboard (Windows) or Xboard (Linux/Mac) graphical user interface.  

http://www.gnu.org/software/xboard/

You can read more about Prophet at http://jamesswafford.com/prophet .


## Installing

To build Prophet, you'll need a C++ compiler with SQLite3 development libraries.  On Ubuntu that's libsqlite3-dev.  If you just want binaries, you can download them from http://jamesswafford.com/prophet .

If you have the proper build tools, just do:

```
make
```


## Opening Book
Prophet has a small opening book but it is not enabled by default.  This is to make life easier for testers.  If you
would like to enable the opening book, you can do it with a command line parameter:

```-book=book.db```


## Memory Usage

Normally you wouldn't need to worry about memory usage, but if you want to tweak
Prophet here is some important information.

Prophet currently employs two transposition tables.  One is used in the main 
search, and one in the pawn evaluation.  Each of those tables has a default size of 128 MB, which is
pretty conservative.
 
You can specify the maximum memory allocated to each table via command line
parameters, but you would really only want to do this if you were running the program 
directly from the command line, and not using a Winboard compatible GUI or test harness. 
(I do this when running test suites but that's about it.)  

```
-hash=512 -phash=256
``` 

The above arguments would allocate 512 MB to the table used in the main search,
and 256 MB to the pawn hash table, for a total of 512 MB + 256 MB = 768 MB.  
 
Winboard / XBoard has an option to specify the maximum memory usage, and Prophet does
respect that.  The allocation strategy is to give each table equal share of the total.

## Running Test Suites

You can run EPD formatted test suites with Prophet using the 'suite' command line argument.  The
default time per problem is 5 seconds, but that can also be changed with the 'time'
argument.

```
prophet -suite=wac.epd  -time=10
```

The command above would start Prophet to process the Win At Chess (WAC) test suite,
giving it 10 seconds per problem.  (A few test suites can be found in the 'suites' folder.)


