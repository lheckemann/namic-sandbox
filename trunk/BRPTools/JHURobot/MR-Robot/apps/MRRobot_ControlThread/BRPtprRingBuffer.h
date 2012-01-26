/** @file
  \brief Thread-safe Ring Buffer implementation (Csaba Csoma)
  
  If variable message size needed, look at Bip Buffer:
  http://www.codeproject.com/internet/bipbuffer.asp 
*/ 


#ifndef _BRPtprRingBuffer_h_
#define _BRPtprRingBuffer_h_

class BRPtprRingBuffer {
public:
 BRPtprRingBuffer(void);
 ~BRPtprRingBuffer(void);
 
 bool Create(unsigned int nRecordNumber, unsigned int nRecordSize=1);
 void DestroyBuffer(void);
 
 int GetAvailabeRecordsNumber(void);
 int GetMaxWriteNumber(void);
 
 bool WriteOneRecord(void *buffer);
 bool ReadOneRecord(void *buffer);
 
 // Other? Search record?
 
protected:
 char *queue;  ///< This will hold the data

 unsigned int record_number;  ///< Max number of records in buffer
 unsigned int record_size;    ///< The size of one record

 unsigned int read_pointer; ///< We'll read from here
 unsigned int read_counter; ///< Incremented with each read (will wrap over)
 
 unsigned int write_pointer; ///< We'll write here
 unsigned int write_counter; ///< Incremented with each write (will wrap over)
};


#endif // _BRPtprRingBuffer_h_
