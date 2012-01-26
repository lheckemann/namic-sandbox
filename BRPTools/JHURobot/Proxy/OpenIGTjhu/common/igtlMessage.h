#ifndef _IGTL_MESSAGE_H_
#define _IGTL_MESSAGE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include "igtl_header.h"
#include "crc32.h"

class igtlMessage;
typedef std::deque<igtlMessage> igtlMessage_queue;


// No virtual functions!!

class igtlMessage
{
public:
  enum { header_length = IGTL_HEADER_SIZE };
  enum { max_body_length = 454 };

  igtlMessage(void);
  
  void Invalidate(void) {body_length_=0xfffffffe; } // max. 4TB packet /// TODO - check CRC!
  bool IsValid(void) { return body_length_ != 0xfffffffe; }


  const char* data() const
  {
    return data_;
  }

  char* data()
  {
    return data_;
  }

  size_t length() const
  {
    return header_length + body_length_;
  }

  const char* body() const
  {
    return data_ + header_length;
  }

  char* body()
  {
    return data_ + header_length;
  }

  size_t body_length() const
  {
    return body_length_;
  }

  void body_length(size_t length);


  bool decode_header();

  void encode_header();

  igtl_header *get_header(void)
  {
          return &decoded_header;
  }

  void initialize_header(void);


private:
  char data_[header_length + max_body_length];
  size_t body_length_;
  igtl_header decoded_header;
};


#endif

