/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
****************************************************************************/

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

namespace OpenVDS
{

class Exception : public std::exception
{
public:
  const char* what() const noexcept override { return GetErrorMessage(); }
  virtual const char *GetErrorMessage() const noexcept = 0;
};

template<int BUFFERSIZE>
class MessageBufferException : public Exception
{
public:
  enum
  {
    MESSAGE_BUFFER_SIZE = BUFFERSIZE
  };
private:
  char          m_messageBuffer[MESSAGE_BUFFER_SIZE];
  int           m_usedSize;
protected:
  MessageBufferException() : m_messageBuffer(), m_usedSize(0) {}

  const char *AddToBuffer(const char *message)
  {
    if(m_usedSize == MESSAGE_BUFFER_SIZE || !message)
    {
      return "";
    }

    const char *start = &m_messageBuffer[m_usedSize];

    while(*message && m_usedSize < MESSAGE_BUFFER_SIZE - 1)
    {
      m_messageBuffer[m_usedSize++] = *message++;
    }
    m_messageBuffer[m_usedSize++] = '\0';

    return start;
  }
};

class FatalException : public MessageBufferException<16384>
{
  const char *m_errorMessage;
public:
  FatalException(const char* errorMessage) : m_errorMessage(AddToBuffer(errorMessage)) {}

  const char *GetErrorMessage() const noexcept override { return m_errorMessage; }
};

class InvalidOperation : public MessageBufferException<512>
{
  const char *m_errorMessage;
public:
  InvalidOperation(const char *errorMessage) : m_errorMessage(AddToBuffer(errorMessage)) {}

  const char *GetErrorMessage() const noexcept override { return m_errorMessage; }
};

class InvalidArgument : public MessageBufferException<512>
{
  const char *m_errorMessage;
  const char *m_parameterName;

public:
  InvalidArgument(const char* errorMessage, const char *parameterName) : m_errorMessage(AddToBuffer(errorMessage)), m_parameterName(AddToBuffer(parameterName)) {}

  const char *GetErrorMessage() const noexcept override { return m_errorMessage; }
  const char *GetParameterName() const noexcept { return m_parameterName; }
};

class IndexOutOfRangeException : public Exception
{
public:
  const char *GetErrorMessage() const noexcept override { return "IndexOutOfRange"; }
};

struct ReadErrorException : public MessageBufferException<512>
{
public:
  //keping member public for api compatibility
  const char *message;
  int errorCode;
  ReadErrorException(const char* errorMessage, int errorCode) : message(AddToBuffer(errorMessage)) , errorCode(errorCode) {}

  const char *GetErrorMessage() const noexcept override { return message; }
  int GetErrorCode() const noexcept { return errorCode;  }
};
} /* namespace OpenVDS */

#endif //EXCEPTIONS_H
