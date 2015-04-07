//!
//! Contains the functions to send data across ZMQ.
//!
//! \file comms/transport.cpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Lesser General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#include "comms/transport.hpp"

#include <iomanip>
#include <random>
#include <sstream>

#include <glog/logging.h>

#include "comms/serial.hpp"

namespace stateline
{
  namespace comms
  {
    std::string receiveString(zmq::socket_t& socket)
    {
      std::string result;
      try
      {
        zmq::message_t message;
        socket.recv(&message);
        result = std::string(static_cast<char*>(message.data()), message.size());
      }
      catch(const zmq::error_t& e)
      {
        VLOG(1) << "ZMQ receive has thrown with type " << e.what();
        throw;
      }
      return result;
    }

    bool sendString(zmq::socket_t& socket, const std::string& string)
    {
      // Taken from zhelpers.hpp
      zmq::message_t message{string.size()};
      memcpy(message.data(), string.data(), string.size());

      return socket.send(message);
    }

    bool sendStringPart(zmq::socket_t& socket, const std::string& string)
    {
      // Taken from zhelpers.hpp
      zmq::message_t message{string.size()};
      memcpy(message.data(), string.data(), string.size());

      return socket.send(message, ZMQ_SNDMORE);
    }

    Message receive(zmq::socket_t& socket)
    {
      Address address;

      // Read address frames until we read the "" delimiter.
      for (auto frame = receiveString(socket); frame != ""; frame = receiveString(socket))
        address.push_back(std::move(frame));

      // address is a stack, so reverse it to get the right way around
      std::reverse(std::begin(address), std::end(address));

      // We've just read the delimiter, so now get subject
      auto subjectString = receiveString(socket);

      // The underlying representation is (explicitly) an int so fairly safe
      auto subject = (Subject)detail::unserialise<std::uint32_t>(subjectString);

      // Now read the data
      std::vector<std::string> data;

      std::int64_t isMore = 0;
      std::size_t moreSize = sizeof(isMore);
      while (socket.getsockopt(ZMQ_RCVMORE, &isMore, &moreSize), isMore)
        data.push_back(receiveString(socket));

      return {address, subject, data};
    }

    void send(zmq::socket_t& socket, const Message& message)
    {
      // Remember we're using the vector as a stack, so iterate through the
      // address in reverse.
      std::for_each(message.address.rbegin(), message.address.rend(),
        [&](const std::string& s) { sendStringPart(socket, s); });

      // Send delimiter
      sendStringPart(socket, "");

      // Send subject, then data if there is any
      auto subjectString = detail::serialise<std::uint32_t>(message.subject);
      auto dataSize = message.data.size();
      if (dataSize > 0)
      {
        // The subject
        sendStringPart(socket, subjectString);

        // The data -- multipart
        std::for_each(std::begin(message.data), std::prev(std::end(message.data)),
            [&](const std::string& s) { return sendStringPart(socket, s); });

        // Final or only part
        sendString(socket, message.data.back());
      }
      else
      {
        // The subject
        sendString(socket, subjectString);
      }
    }

    // TODO: reduce code duplication between this and the lvalue reference version
    // (maybe just have a single send function which takes by copy, and just move)
    void send(zmq::socket_t& socket, Message&& message)
    {
      // Remember we're using the vector as a stack, so iterate through the
      // address in reverse.
      for (auto it = message.address.rbegin(); it != message.address.rend(); ++it)
      {
        sendStringPart(socket, std::move(*it));
      }

      // Send delimiter
      sendStringPart(socket, "");

      // Send subject, then data if there is any
      auto subjectString = detail::serialise<std::uint32_t>(message.subject);
      uint dataSize = message.data.size();
      if (dataSize > 0)
      {
        // The subject
        sendStringPart(socket, std::move(subjectString));

        // The data -- multipart
        for (auto it = message.data.begin(); it != std::prev(message.data.end()); ++it)
        {
          sendStringPart(socket, std::move(*it));
        }

        // final or only part
        sendString(socket, std::move(message.data.back()));
      }
      else
      {
        // The subject
        sendString(socket, std::move(subjectString));
      }
    }

    std::string randomSocketID()
    {
      // Inspired by zhelpers.hpp
      static std::mt19937 gen{std::random_device{}()};
      static std::uniform_int_distribution<> dis{0, 0x10000};
      std::stringstream ss;
      ss << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << dis(gen) << "-" << std::setw(4) << std::setfill('0')
        << dis(gen);
      return ss.str();
    }

    void setSocketID(const std::string& id, zmq::socket_t& socket)
    {
      socket.setsockopt(ZMQ_IDENTITY, id.c_str(), id.length());
    }

  } // namespace comms
} // namespace stateline
