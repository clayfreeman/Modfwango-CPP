/**
 * @file  SocketManagement.cpp
 * @brief SocketManagement
 *
 * Class implementation for SocketManagement
 *
 * @author     Clay Freeman
 * @date       March 13, 2015
 */

#include <algorithm>
#include <arpa/inet.h>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include "../include/ConnectionManagement.h"
#include "../include/Logger.h"
#include "../include/Socket.h"
#include "../include/SocketManagement.h"

std::map<std::string, std::shared_ptr<Socket>> SocketManagement::sockets{};

void SocketManagement::acceptConnections() {
  for (auto i : SocketManagement::sockets) {
    try {
      ConnectionManagement::newConnection(i.second->acceptConnection());
    }
    catch (const std::runtime_error&) {}
    catch (const std::exception&) {}
  }
}

bool SocketManagement::destroySocket(const std::string& addr, int port) {
  bool retVal = false;
  std::string key = SocketManagement::getValidIP(addr) + std::to_string(port);
  if (SocketManagement::sockets.count(key) > 0)
    retVal = SocketManagement::sockets.erase(key) > 0;
  return retVal;
}

std::string SocketManagement::getValidIP(const std::string& addr) {
  char str[INET_ADDRSTRLEN] = {0};
  struct sockaddr_in addr_in;
  inet_pton(AF_INET, addr.c_str(), &(addr_in.sin_addr));
  inet_ntop(AF_INET, &(addr_in.sin_addr), str, INET_ADDRSTRLEN);
  return std::string{str};
}

bool SocketManagement::isValidIP(const std::string& addr) {
  struct sockaddr_in addr_in;
  return inet_pton(AF_INET, addr.c_str(), &(addr_in.sin_addr)) == 1;
}

bool SocketManagement::newSocket(const std::string& addr, int port) {
  bool retVal = false;
  if (SocketManagement::isValidIP(addr)) {
    Socket* s = nullptr;
    try {
      s = new Socket{SocketManagement::getValidIP(addr), port};
    }
    // Catch either bind error
    catch (const std::runtime_error& e) {
      Logger::debug(e.what());
    }

    if (s != nullptr && s->isValid()) {
      std::string key = SocketManagement::getValidIP(addr) +
        std::to_string(port);
      if (SocketManagement::sockets.count(key) == 0) {
        SocketManagement::sockets[key] = std::shared_ptr<Socket>{s};
        retVal = true;
      }
      else if (s != nullptr) delete s;
    }
    else if (s != nullptr) delete s;
  }
  return retVal;
}

void SocketManagement::stall() {
  int max = -1;

  // Prepare file descriptor set
  fd_set rfds;
  FD_ZERO(&rfds);
  for (auto i : SocketManagement::getSockets()) {
    FD_SET(i.second->getSock(), &rfds);
    if (i.second->getSock() > max)
      max = i.second->getSock();
  }

  for (auto i : ConnectionManagement::getConnections()) {
    FD_SET(i->getSock(), &rfds);
    if (i->getSock() > max)
      max = i->getSock();
  }

  // Wait on all sockets
  select(++max, &rfds, nullptr, nullptr, nullptr);
}