/**
 * @file  SocketManagement.cpp
 * @brief SocketManagement
 *
 * Class implementation for SocketManagement
 *
 * @author     Clay Freeman
 * @date       March 13, 2015
 */

#include <arpa/inet.h>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include "../include/ConnectionManagement.hpp"
#include "../include/FileDescriptorPool.hpp"
#include "../include/Logger.hpp"
#include "../include/Socket.hpp"
#include "../include/SocketManagement.hpp"

std::map<std::string, std::shared_ptr<Socket>> SocketManagement::sockets{};

/**
 * @brief Accept Connections
 *
 * Accepts connections on all sockets
 */
void SocketManagement::acceptConnections() {
  for (auto i : SocketManagement::sockets) {
    try {
      ConnectionManagement::newConnection(i.second->acceptConnection());
    }
    catch (const std::runtime_error&) {}
    catch (const std::exception&) {}
  }
}

/**
 * @brief Close All
 *
 * Closes all Sockets
 */
void SocketManagement::closeAll() {
  SocketManagement::sockets.clear();
}

/**
 * @brief Count
 *
 * Returns the number of Sockets
 *
 * @return # of Sockets
 */
int SocketManagement::count() {
  return SocketManagement::sockets.size();
}

/**
 * @brief Destroy Socket
 *
 * Destroys the Socket with the provided address and port
 *
 * @param addr The address
 * @param port The port
 *
 * @return true if socket exists, false otherwise
 */
bool SocketManagement::destroySocket(const std::string& addr, int port) {
  bool retVal = false;
  std::string key = SocketManagement::getValidIP(addr) + std::to_string(port);
  if (SocketManagement::sockets.count(key) > 0)
    retVal = SocketManagement::sockets.erase(key) > 0;
  return retVal;
}

/**
 * @brief Get Valid IP
 *
 * Sanitizes incoming addresses and provides a clean IP
 *
 * @param addr The address
 *
 * @return A valid IP
 */
std::string SocketManagement::getValidIP(const std::string& addr) {
  char str[INET_ADDRSTRLEN] = {0};
  struct sockaddr_in addr_in;
  inet_pton(AF_INET, addr.c_str(), &(addr_in.sin_addr));
  inet_ntop(AF_INET, &(addr_in.sin_addr), str, INET_ADDRSTRLEN);
  return std::string{str};
}

/**
 * @brief Is Valid IP
 *
 * Checks if the incoming address is a valid IP address
 *
 * @param addr The address
 *
 * @return true if valid, false otherwise
 */
bool SocketManagement::isValidIP(const std::string& addr) {
  struct sockaddr_in addr_in;
  return inet_pton(AF_INET, addr.c_str(), &(addr_in.sin_addr)) == 1;
}

/**
 * @brief New Socket
 *
 * Creates a socket that listens on the provided address and port
 *
 * @param addr The address
 * @param port The port
 *
 * @return true if Socket was created, false otherwise
 */
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

/**
 * @brief Stall
 *
 * Pause program execution until activity occurs on a FileDescriptor
 */
void SocketManagement::stall() {
  // Get the current fd_set
  fd_set rfds = FileDescriptorPool::get();
  // Wait on all sockets
  select(FileDescriptorPool::max(), &rfds, nullptr, nullptr, nullptr);
}
