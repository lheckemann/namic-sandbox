#include "igtlMessage.h"

igtlMessage::igtlMessage()
    : body_length_(0)
  {
          initialize_header();
  }



  void igtlMessage::body_length(size_t length)
  {
    body_length_ = length;
    if (body_length_ > max_body_length)
      body_length_ = max_body_length;
  }

  bool igtlMessage::decode_header()
  {
        std::memcpy(&decoded_header, data_, header_length);
        igtl_header_convert_byte_order(&decoded_header);
        body_length_ = (size_t)decoded_header.body_size; /// ATTENTION 64bit -> 32bit conversion!
        if (body_length_ > max_body_length)
    {
      body_length_ = 0;
      return false;
    }
    return true;
  }

  void igtlMessage::encode_header()
  {
        decoded_header.body_size=body_length_;
        igtl_header_convert_byte_order(&decoded_header);
        memcpy(data_, &decoded_header, header_length);
        igtl_header_convert_byte_order(&decoded_header);
  }


  void igtlMessage::initialize_header(void)
  {
          decoded_header.version = IGTL_HEADER_VERSION;
          decoded_header.timestamp = 0;
          decoded_header.crc =  0;
          std::memset(decoded_header.device_name, 0, IGTL_HEADER_DEVSIZE);
          std::memset(decoded_header.name, 0, IGTL_HEADER_NAMESIZE);
          decoded_header.body_size = 0;
  }

