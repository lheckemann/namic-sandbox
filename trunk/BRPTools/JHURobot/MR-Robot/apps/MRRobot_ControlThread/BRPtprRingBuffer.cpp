#include "BRPtprRingBuffer.h"

#include <stdlib.h> // for "NULL"
#include <assert.h>

#include <string.h>
#define CopyMemory memcpy
#define SetMemory memset


/// Constructor
BRPtprRingBuffer::BRPtprRingBuffer(void)
{
 queue=NULL;
 this->DestroyBuffer(); // resets the variables
}


/// Destructor
BRPtprRingBuffer::~BRPtprRingBuffer(void)
{
 this->DestroyBuffer();
}


/// Free the memory and reset all the variables
void BRPtprRingBuffer::DestroyBuffer(void)
{
 if (queue) {
        delete [] queue;
        queue=NULL;
 }

 record_number = 0;
 record_size = 0;

 read_pointer = 0;
 read_counter = 0;
 
 write_pointer = 0;
 write_counter = 0;
}


/// Creates the buffer
bool BRPtprRingBuffer::Create(unsigned int nRecordNumber, unsigned int nRecordSize)
{
 assert(nRecordNumber);
 assert(nRecordSize);
 
 queue = new char[nRecordNumber*nRecordSize];
 if (!queue)
   return false;
 
  
 record_number = nRecordNumber;
 record_size = nRecordSize;
 SetMemory(queue, 0, record_number*record_size);
 return true;
}
 
 
/// This is how many records are in the queue
int BRPtprRingBuffer::GetAvailabeRecordsNumber(void)
{
 return abs(write_counter - read_counter);
}
 
 
/// This is how many records can we add to the queue
int BRPtprRingBuffer::GetMaxWriteNumber(void)
{
 return record_number-abs(write_counter - read_counter);
}
 
 
/// Adds this record to the end of the queue
bool BRPtprRingBuffer::WriteOneRecord(void *buffer)
{
 if (GetMaxWriteNumber()<1)
   return false;
   
 CopyMemory( &queue[write_pointer*record_size], buffer, record_size );
 write_counter++;
 write_pointer++;
 if (write_pointer>=record_number)
   write_pointer=0; 
 return true;
}
 
 
/// Reads one record from the end of the queue
bool BRPtprRingBuffer::ReadOneRecord(void *buffer)
{
 if (GetAvailabeRecordsNumber()<1) 
   return false;
   
 CopyMemory( buffer, &queue[read_pointer*record_size], record_size );
 read_counter++;
 read_pointer++;
 if (read_pointer>=record_number)
   read_pointer=0;
 return true;
}
