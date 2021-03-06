/**
 * @file  Socket.h
 * @brief Socket
 *
 * Class definition for Socket
 *
 * @author     Clay Freeman
 * @date       March 11, 2015
 */

#ifndef _SOCKET_H
#define _SOCKET_H

#include <memory>
#include <string>
#include "Connection.hpp"
#include "FileDescriptor.hpp"

class Socket {
  private:
    std::string                     host   = "0.0.0.0";
    int                             port   = 0;
    std::shared_ptr<FileDescriptor> sockfd = std::shared_ptr<FileDescriptor>{
      new FileDescriptor{}
    };
    // Make sure copying is disallowed
    Socket(const Socket&);
    Socket& operator= (const Socket&);
  public:
    Socket(const std::string& addr, int portno);
    ~Socket();
    std::shared_ptr<Connection>     acceptConnection() const;
    const std::string&              getHost() const;
    int                             getPort() const;
    std::shared_ptr<FileDescriptor> getSock() const;
    bool                            isValid() const;
};

#endif
