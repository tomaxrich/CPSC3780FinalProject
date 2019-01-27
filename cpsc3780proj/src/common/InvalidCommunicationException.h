// SocketException class


#ifndef _INVALID_COMMUNICATION_EXCEPTION_H
#define _INVALID_COMMUNICATION_EXCEPTION_H

#include <string>

class InvalidCommunicationException
{
 public:
  InvalidCommunicationException ( std::string s ) : m_s ( s ) {};
  ~InvalidCommunicationException (){};

  std::string description() { return m_s; }

 private:

  std::string m_s;

};

#endif
