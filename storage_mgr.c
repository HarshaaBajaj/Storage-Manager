#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage_mgr.h"
#include "dberror.h"
#include <unistd.h>

static int initSM;
SM_FileHandle *fh;
extern RC checkFile(SM_FileHandle *fHandle);
extern RC pageNumbers(int pno);
/*****************************************************************/
/********************Manipulation of Page*************************/
/****************************************************************/

// extern void initStorageManager(void); => Initializing the storage manager
void initStorageManager(void)
{
    initSM = 1;
}

//extern RC createPageFile(char* fileName); =>  Creating a file in File System
RC createPageFile(char* fileName)
{
    printf("Page File Manipulation begins...\n");
    FILE *filePtr;
    if( initSM == 1)
    {
        if((filePtr = fopen(fileName, "w+")) != NULL)
        {
            char *totalPages, *firstPage;
            totalPages = (char *) calloc(PAGE_SIZE, sizeof(char));  /* (the number of elements to be allocated,  size of elements. )allocate "first" page to store total number of pages information */
            firstPage = (char *) calloc(PAGE_SIZE, sizeof(char));   /* considered as actual first page for the data */
            int sizeOfTotalPages = fwrite(totalPages, sizeof(char), PAGE_SIZE, filePtr); //(pointer to the array of elements to be written, size in bytes of each element to be written,the number of elements, each one with a size of size bytes., pointer to a FILE object that specifies an output stream )
            if(sizeOfTotalPages != PAGE_SIZE){printf("ERROR: Failed to write\n"); return RC_WRITE_TO_OUTPUTSTREAM_FAILED;}
            else{printf("Allocated memory for total pages: %p\n", totalPages);}
            int sizeOfFirstPage = fwrite(firstPage, sizeof(char), PAGE_SIZE, filePtr);
            if(sizeOfFirstPage != PAGE_SIZE){printf("ERROR: Failed to write on first page\n"); return RC_WRITE_TO_OUTPUTSTREAM_FAILED;}
            else{printf("allocated memory for first pages: %p\n", firstPage);}
            free(totalPages);
            free(firstPage);
            fclose(filePtr);
            return RC_OK;

        }
        else
        {
            printf("Error: File is not getting created\n");
            return RC_FILE_CREATION_FAILED;
        }

    }
    else
    {
	printf("Error: Storage Manager is not initialized");
        return RC_STORAGE_MANAGER_NOT_INITIALIZED;
    }

}

/* extern RC openPageFile (char *fileName, SM_FileHandle *fHandle); => Open file to read and write
The second parameter is an existing file handle. If opening the file is successful,
then the fields of this file handle should be initialized with the information about the opened file.
*/
RC openPageFile (char *fileName, SM_FileHandle *fHandle){
     if (fHandle == NULL) {
     	return RC_FILE_INFO_UNAVAILABLE;
     }

    FILE *filePtr = fopen(fileName, "r+");
    if(!filePtr){ return RC_FILE_NOT_FOUND;}
    else
    {
        /* get the file name, position and to find the totalNumPages- move pointer to point the end of the file,
        calculate the total bytes of this file and move pointer back */
        double fileLength;
        fHandle -> fileName = fileName;
        fHandle -> curPagePos = 0;
	fHandle -> mgmtInfo = filePtr;
	printf("mgmtInfo: %p\n", filePtr);
        fseek(filePtr,0,SEEK_END);
        fileLength = ((int)ftell(filePtr)/PAGE_SIZE)/2;
	fHandle -> totalNumPages=fileLength;
        printf("Length of file: %lf\n", fileLength);
        printf("total number of pages: %d\n", fHandle->totalNumPages);
        rewind(filePtr);
		fclose(filePtr);
        return RC_OK;

    }

}

//extern RC closePageFile (SM_FileHandle *fHandle); => close an open page file
RC closePageFile(SM_FileHandle *fHandle){

    	fHandle -> fileName = NULL;
    	fHandle -> mgmtInfo = NULL;
	printf("***CLosing file...\n");
	return RC_OK;

}

//extern RC destroyPageFile (char *fileName); => destroy a page file
RC destroyPageFile(char *fileName){
	int result = remove(fileName);
	if (result != 0){ printf("file not destroyed at end\n"); return RC_FILE_DESTROY_FAILED;}
	else{ printf("file removed successfully!\n");return RC_OK;}
}

/*****************************************************************/
/********************Read file code form here*********************/
/****************************************************************/

/*Below is a generic function to read a specified block of page*/

/* RC readBlockGen(int pageNum, SM_FileHandle *fHandle,
		SM_PageHandle memPage) {

	//Check if page requested does exist
	if (fHandle->totalNumPages < (pageNum + 1))
		THROW(RC_READ_NON_EXISTING_PAGE, "Attempt to read non-existing page");

	fHandle->curPagePos = pageNum;

	FILE *fp = (FILE*) fHandle->mgmtInfo;

	fflush(fp);

	long int currentPos = ftell(fp);
	long int newPos = ((pageNum) * PAGE_SIZE) + 1;

	if (newPos != currentPos)
		fseek(fp, (newPos - currentPos), SEEK_CUR);

	char buf[PAGE_SIZE];
	if ((fread(buf, 1, PAGE_SIZE, fp)) != PAGE_SIZE) {
		printf("Unable to read from specified block");
	}

	memcpy(memPage, buf, PAGE_SIZE);

	return RC_OK;
}

*/
RC readBlockGen(int pageNum, SM_FileHandle * fHandle, SM_PageHandle memPage) {

	FILE * fptr;
//	= NULL;
	FILE *fp = (FILE*) fHandle->mgmtInfo;
	//char buf[PAGE_SIZE];
	int seekVal;
	fptr=fopen(fHandle -> fileName,"r+"); 
	printf("checking fptr before calculating current pos %p\n",fptr);
	//printf(strerror);
	long int currentPos = ftell(fptr);
	printf("current position is %d\n", currentPos);
	long int newPos = ((pageNum) * PAGE_SIZE) + 1;
	
	printf("new position is %d\n", newPos);

	printf("the file name is: %p\n", fHandle->fileName);
 	if(!fptr){
   		printf("ERROR: File not found for reading\n"); 
   		fclose(fptr);
   		return RC_FILE_NOT_FOUND;
  	}
 	printf("Page position is %d\n", pageNum);
	printf("Page numbers in total is %d\n", fHandle -> totalNumPages);
	//  
  	if (pageNum < 0 || pageNum > fHandle -> totalNumPages) {
    	return RC_READ_NON_EXISTING_PAGE;
  	} 
	else {
	printf("Pageno is valid..\n");

    	printf("mgmtInfo: %p\n", fp);
		if (newPos != currentPos){
		 seekVal= fseek(fptr, (newPos - currentPos), SEEK_CUR);
		
    	
	printf("Seek Val: %d\n",seekVal);
		}
		
		
    	if (seekVal != 0) {
      		return RC_PAGE_SPECIFIED_NOT_FOUND;
    	} 
		else {
				
			  	int fr = fread(memPage, sizeof(char), PAGE_SIZE, fptr);
		printf("page size is %d\n", PAGE_SIZE);
		printf("Read Val: %d\n",fr);
		//&& fr != PAGE_SIZE+1
			if (fr != PAGE_SIZE   ) {
		printf("Page not read successfully...\n");
        	return RC_FILE_NOT_READ_COMPLETELY;
      	} else {
        	fHandle -> curPagePos = pageNum + 1;
        	printf("Curr position is %d\n", fHandle -> curPagePos);
        	return RC_OK;
      	}
    }
  }
}


/*Below functions read current/First/Previous/Next/Last block of pages
getBlockPos gives the current pointer of the file handler*/

RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {

	printf("Starting read function..\n");
  	if (fHandle == NULL || memPage == NULL) {
    	return RC_FILE_INFO_UNAVAILABLE;
  	}
  	readBlockGen(pageNum, fHandle, memPage);
}

int getBlockPos(SM_FileHandle * fHandle) {
  return fHandle -> curPagePos;
}

RC readFirstBlock(SM_FileHandle * fHandle, SM_PageHandle memPage) {

  if (fHandle == NULL || memPage == NULL) {
    return RC_FILE_INFO_UNAVAILABLE;
  }
  return readBlockGen(0, fHandle, memPage);
}

RC readPreviousBlock(SM_FileHandle * fHandle, SM_PageHandle memPage) {
  if (fHandle == NULL || memPage == NULL) {
    return RC_FILE_INFO_UNAVAILABLE;
  }
  if (fHandle -> curPagePos == 0) {
    return RC_PAGE_NOT_AVAILABLE;
  }
  return readBlockGen(fHandle -> curPagePos - 1, fHandle, memPage);
}

RC readCurrentBlock(SM_FileHandle * fHandle, SM_PageHandle memPage) {
  if (fHandle == NULL || memPage == NULL) {
    printf("File/Page info unavailable!\n");
    return RC_FILE_INFO_UNAVAILABLE;
  }
  return readBlockGen(fHandle -> curPagePos, fHandle, memPage);
}

RC readNextBlock(SM_FileHandle * fHandle, SM_PageHandle memPage) {
  if (fHandle == NULL || memPage == NULL) {
    return RC_FILE_INFO_UNAVAILABLE;
  }
  if (fHandle -> curPagePos + 1 == fHandle -> totalNumPages) {
    return RC_PAGE_NOT_AVAILABLE;
  }

  return readBlockGen(fHandle -> curPagePos + 1, fHandle, memPage);
}

RC readLastBlock(SM_FileHandle * fHandle, SM_PageHandle memPage) {
  if (fHandle == NULL || memPage == NULL) {
    return RC_FILE_INFO_UNAVAILABLE;
  }

  return readBlockGen(fHandle -> totalNumPages - 1, fHandle, memPage);
}


/************************************************************/
/*Write block from here*/

/***************************************************/

/***Writing a page to disk using the absolute position
  input paramerters:   
	pageNum => the page number in the file on the disk where the data has to be written
	fHandle => pointer to SM_FileHandle which holds information about the file like file name, file pointer, number of pages and current page location 
    memPag  => location in memory from where the data has to be written
  return values:
  	RC  => values defined in dberror.h as per the operational status of the function
***/
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPag) 
{
	int curPos = -1;
    FILE *fptr = NULL;
    if (fHandle->fileName == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    } else {
        fptr = fHandle->mgmtInfo;
        if (fptr == NULL) {
            return RC_FILE_INFO_UNAVAILABLE;
        } else {
            //Seeking to the given block 
            if ((fseek(fptr, (pageNum + 1) * PAGE_SIZE, SEEK_SET)) < 0) {
                return RC_FILE_SEEK_ERROR;
            } else {
                if ((fwrite(memPag, PAGE_SIZE, 1, fptr)) == -1) {
                    return RC_WRITE_FAILED;
                } else {
                    curPos = (ftell(fptr) / PAGE_SIZE) - 1;
                    fHandle->curPagePos = curPos;
                    fHandle->totalNumPages = fHandle->totalNumPages + 1;
                }
            }
        }
    }
    return RC_OK;
	/*FILE *fptr = (FILE*) fHandle->mgmtInfo;

	RC numval = pageNumbers(pageNum);	//check in the page number is valid
	if (numval != RC_OK) 
		return numval;
	RC filestat = checkFile(fHandle);	//checking intial contidions related to file
	if (filestat != RC_OK) 
		return filestat;
	printf(" Conditions checked.\n Writing block to page number %d\n", pageNum);
	int i= fseek(fptr, (PAGE_SIZE * pageNum)+1, SEEK_SET);
printf("after seeking %d\n", i);

int j =	fwrite(memPag, sizeof(char),PAGE_SIZE, fptr);
printf("after wrtng: %d\n", j);
	//fclose(fptr);
	//printf("after closing file \n");
	return RC_OK;
*/
}

/***Writing a page to disk using the relative position
  input paramerters:   
	fHandle => pointer to SM_FileHandle which holds information about the file like file name, file pointer, number of pages and current page location 
    memPag  => location in memory from where the data has to be written
  return values:
  	RC => values defined in dberror.h as per the operational status of the function
***/
RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) 
{
	int curPos = -1;
    FILE *fptr = NULL;
    if (fHandle->fileName == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    } else {
        fptr = fHandle->mgmtInfo;
        if (fptr == NULL) {
            return RC_FILE_INFO_UNAVAILABLE;
        } else {
            curPos = fHandle->curPagePos;
            if (curPos == -1) {
                return RC_INVALID_POSITION;
				} else {
                //Seeking to the current block in the file
                if ((fseek(fptr, (curPos + 1) * PAGE_SIZE, SEEK_SET)) < 0) {
                    return RC_FILE_SEEK_ERROR;
                } else {
                    if ((fwrite(memPage, PAGE_SIZE, 1, fptr)) == -1) {
                        return RC_WRITE_FAILED;
                    } else {
                        curPos = (ftell(fptr) / PAGE_SIZE) - 1;
                        fHandle->curPagePos = curPos;
                        fHandle->totalNumPages = fHandle->totalNumPages + 1;
                    }
                }
            }
        }
    }
    return RC_OK;
	/*FILE *fptr = (FILE*) fHandle->mgmtInfo;

	RC filestat = checkFile(fHandle);	//checking intial contidions related to file
	if (filestat!= RC_OK) 
		return filestat;
	printf("Writing current block to %s", fHandle->fileName);
	int pos= fHandle->curPagePos;		// get the current page position
	pos = (pos) * PAGE_SIZE;
	if ( pos < 0) 						// if position value is negative return error
		return RC_INVALID_POSITION;
	if (-1 == (fwrite(memPage, 1, PAGE_SIZE, fptr))) 		//if the write function fails
		return RC_WRITE_FAILED; 
	else 
	{
		fclose(fptr); 		//close if the write was successful 
		return RC_OK;
	}*/
	
}

/***This function adds a zero bytes empty block at the end and increases the number of pages by one.
 input paramerters:   
	fHandle => pointer to SM_FileHandle which holds information about the file like file name, file pointer, number of pages and current page location 
 return values:
  	RC => values defined in dberror.h as per the operational status of the function
***/
RC appendEmptyBlock(SM_FileHandle *fHandle) 
{
	    char buf[PAGE_SIZE];
    FILE *fptr = NULL;
    fptr = fHandle->mgmtInfo;
    if (fptr == NULL) {
        return RC_FILE_NOT_OPENED;
    } else {
        int totalNumberPages = fHandle->totalNumPages;
        if (totalNumberPages == 0) {
            return RC_NO_PAGES_ERROR;
        } else {
            if ((fseek(fptr, 0L, SEEK_END)) < 0) {
                return RC_FILE_SEEK_ERROR;
            } else {
                if ((fwrite(buf, PAGE_SIZE, 1, fptr)) == -1) {
                    return RC_WRITE_FAILED;
                } else {
                    int curPos = (ftell(fptr) / PAGE_SIZE);
                    fHandle->curPagePos = curPos;
                    fHandle->totalNumPages = fHandle->totalNumPages + 1;
                }
            }
        }
    }
    return RC_OK;

	/*FILE *fptr = (FILE*) fHandle->mgmtInfo;

	RC filestat = checkFile(fHandle);   		//checking intial contidions related to file
	if (filestat != RC_OK)      
		return filestat;
	printf("Adding an empty block to file %s", fHandle->fileName);
	//totalPages = (char *) calloc(PAGE_SIZE, sizeof(char));  /* (the number of elements to be allocated,  size of elements. )allocate "first" page to store total number of pages information */
       /*     SM_PageHandle temp;
	fseek(fptr, (fHandle->totalNumPages * PAGE_SIZE), SEEK_END);
	temp = (char *)calloc(PAGE_SIZE, sizeof(char));
	fwrite(temp, sizeof(char), PAGE_SIZE, fptr); 		//append empty char to the page
	fHandle->totalNumPages = fHandle->totalNumPages + 1; 
	free(temp);
	fclose(fptr);				//close after adding empty page
	return RC_OK;
	*/ 
}

/***This function is to check and update the number of pages available.
 input paramerters:   
    numberOfPages => the number of pages we want the file to have
	fHandle => pointer to SM_FileHandle which holds information about the file like file name, file pointer, number of pages and current page location 
 return values:
  	RC => values defined in dberror.h as per the operational status of the function
***/
RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle) 
{    char buf[PAGE_SIZE];
    FILE *fptr = NULL;
    int i;
    fptr = fHandle->mgmtInfo;
    if (fptr == NULL) {
        return RC_FILE_NOT_OPENED;
    } else {
        int totalNumberPages = fHandle->totalNumPages;
        if (totalNumberPages < numberOfPages) {
            fseek(fptr, 0L, SEEK_END);
            for (i = 0; i < (numberOfPages - totalNumberPages); i++) {
                fwrite(buf, PAGE_SIZE, 1, fptr);
            }
            fHandle->totalNumPages = numberOfPages;
        }
    }
    return RC_OK;

	/*FILE *fptr = (FILE*) fHandle->mgmtInfo;

	RC isval = pageNumbers(numberOfPages);    // check if the input number of pages is valid
	if (isval != RC_OK) 
		return isval;
	RC filestat = checkFile(fHandle);     	//checking intial contidions related to file
	if (filestat != RC_OK) 
		return filestat;
	if (fHandle->totalNumPages < numberOfPages)     // check if total number of pages is less than the requirement
	{
	int deficit = numberOfPages - fHandle->totalNumPages;   //calculate the deficit
 	int i = 0 ;
	SM_PageHandle ph;
	    for (i = 0; i < deficit; i++){
		ph = (char*)malloc(PAGE_SIZE*sizeof(char));
	        memset(ph,0,PAGE_SIZE*sizeof(char));
		int d = fwrite(ph, sizeof(char),PAGE_SIZE,fptr);
		if(d!=PAGE_SIZE){return RC_WRITE_FAILED;}
		fHandle->totalNumPages= fHandle->totalNumPages + 1;
		rewind(fptr);
	     }

	fHandle->curPagePos = fHandle->totalNumPages - 1 ;
	}
	
	return RC_OK;*/
}

/***This function is for checking intial contidions related to file.
 input paramerters:   
	fHandle => pointer to SM_FileHandle which holds information about the file like file name, file pointer, number of pages and current page location 
 return values:
  	RC => values defined in dberror.h as per the operational status of the function
***/
RC checkFile(SM_FileHandle *fHandle) 
{
	FILE *fptr = (FILE*) fHandle->mgmtInfo;

	if (fHandle == NULL || fHandle->fileName == NULL|| fptr== NULL || fHandle->totalNumPages < 0 || fHandle->curPagePos < 0) 		
		return RC_FILE_HANDLE_NOT_INITIATED;
	else 
		printf("file proper\n");
return RC_OK;
}

/***This function is for checking if the page number is valid.
 input paramerters:   
	pno => page number
 return values:
  	RC => values defined in dberror.h as per the operational status of the function
***/
RC pageNumbers(int pno) 
{
	if (pno >= 0) 	// page number can't be negative
		return RC_OK;
	else 
		return RC_PAGENUM_INVALID;
}

