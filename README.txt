***********************************
Problem Statement		
***********************************
The goal of this assignment is to implement a simple storage manager - a module that is 
capable of reading blocks from a file on disk into memory and writing blocks from memory
to a file on disk.The storage manager deals with pages (blocks)of fixed size (PAGE_SIZE).
In addition to reading and writing pages from a file,it provides methods for creating, 
opening, and closing files.The storage manager has to maintain several types of 
information for an open file: The number of total pages in the file, the current page
position(for reading and writing),the file name,and a POSIX file descriptor or FILE pointer. 

***********************************
File List
***********************************
1)storage_mgr.c
2)storage_mgr.h
3)dberror.c
4)dberror.h
5)test_helper.h
6)test_assign1_1.c
7)test_assign1_2.c
8)README.txt
9)Makefile


**************************************
Instructions To Run The Code
**************************************
In order to setup the environment to run test cases
----------------------------------------------------
Run : make

With default test cases
--------------------------
Compile : make assign1_1
Run : ./assign1_1


With additional test cases
-----------------------------
Compile : make assign1_2
Run : ./assign1_2


****************************************
Description of the Functions
****************************************

PAGE related functions:
_________________________


FUNCTION :  CreatePageFile
------------------------------------
1)Creates a file with the given filename.
2)Creates 2 pages.
3)The first page will be allocated to store the total number of pages information.The
  second page is the first page where the data will begin to be written to.



FUNCTION :  OpenPageFile
-------------------------------------
1)Checks if the file exists
2)If yes, then the file handle will be initialized with the information about the opened file.
3)If not, returns the FILE_NOT_FOUND error.


FUNCTION  :  ClosePageFile
--------------------------------------
1)Closes the pagefile


FUNCTION : DestroyPageFile
--------------------------------------
1)Removes the pageFile.
2)Checks if the file was removed.
3)If yes, displays the success message.
4)If no, returns the appropriate error message.


READ related functions
___________________________


FUNCTION : readBlockGen
-----------------------------
1)Opens the File.
2)Checks if the file was opened.
3)If yes, returns the success message.
4)If no, returns the error message.
5)Takes the pageNum as the input.
6)Checks if the page exists.
7)If yes, moves the pointer to the specified page  and reads the data.
8) If no, returns the page not found error and file not read error.


FUNCTION : readBlock 
-------------------------------
1)Checks if the file exists.
2)If yes, reads the file.
3)If no, returns File not found error
 


FUNCTION : getBlockPros 
-------------------------------
1)Gets the current position of the page from the currentPagePos of the file handler.

FUNCTION : readFirstBlock 
-------------------------------
1)Reads the first block by giving the pageNum argument as 0 the readBlockGen function.

FUNCTION : readPreviousBlock  
--------------------------------
1)Reads the previous block by giving the pageNum argument as (current position - 1) to the readBlockGen function.

FUNCTION : readCurrentBlock 
--------------------------------
1)Reads the current block by providing the pageNum argument as the current position to the readBlockGen function.

FUNCTION : readNextBlock 
----------------------------------
1)Reads the next block by providing the pageNum argument as the (current position + 1) to the readBlockGen function.

FUNCTION : readLastBlock 
---------------------------------
1)Reads the last block by providing the pageNum argument as the (total number of pages - 1) to the readBlockGen function.


WRITE related Functions
____________________________

FUNCTION : writeBlock
---------------------------
1)Checks if the page number is valid.
2)If yes, checks the initial conditions related to the file.
3)If no, returns the appropriate error message.
4)Seeks the file pointer to the pageNum position.
5)Writes the data addressed by the mem pointer to the file on the disk.

FUNCTION : writeCurrentBlock
------------------------------
1) Writes to the page by providing the current page position as the page number to the writeBlock method.

FUNCTION : appendEmptyBlock
-----------------------------
1)Allocates a memory block equal to PAGE_SIZE and a pointer addressing this block is created.
2)Increments the total number of pages and seeks the file pointer to the newly added page.
3)Writes the data addressed by the memory pointer to the file on the disk.

FUNCTION : ensureCapacity
---------------------------
1)Checks if the file has less than the number of pages required.
2)If yes, calculates the number of pages insufficient to meet the required number of pages.
3)Calls the appendEmptyBlock function and appends the insufficient number of pages to the file on the disk.

***********************************
Additional Features
***********************************

Additional Test Cases 
----------------------
In addition to the default test cases ,we have implemented test cases for all the remaining 
functions in the test_assign1_2c. The instructions to run these test cases are provided 
above.


