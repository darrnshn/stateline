//!
//! Contains the functions to send data across ZMQ.
//!
//! \file comms/transport.hpp
//! \author Lachlan McCalman
//! \date 2014
//! \license Lesser General Public License version 3 or later
//! \copyright (c) 2014, NICTA
//!

#pragma once

#include <string>

#include <zmq.hpp>

#include "comms/messages.hpp"

namespace stateline
{
  namespace comms
  {
    //! Read a string form a ZMQ socket. This is a blocking call.
    //!
    //! \param socket The socket to read from.
    //! \return A string containing the data read from the socket.
    //!
    std::string receiveString(zmq::socket_t& socket);

    //! Send a string over a ZMQ socket.
    //!
    //! \param socket The socket to send the string over.
    //! \param string The string data to send.
    //! \return Whether the send was successful.
    //!
    bool sendString(zmq::socket_t& socket, const std::string& string);

    //! Send a string over a ZMQ socket as a chunk of a multipart message.
    //!
    //! \param socket The socket to send the string over.
    //! \param string The string data to send.
    //! \return Whether the send was successful.
    //!
    bool sendStringPart(zmq::socket_t& socket, const std::string& string);

    //! Receive a (possibly multi-part) message.
    //!
    //! \param socket The socket the message is coming into.
    //! \return The Message object received.
    //!
    Message receive(zmq::socket_t& socket);

    //! Send a (possibly multi-part) message.
    //!
    //! \param socket The socket the message is coming into.
    //! \param message The stateline::comms::Message to send.
    //!
    void send(zmq::socket_t& socket, const Message& message);

    //! Send a (possibly multi-part) message.
    //!
    //! \param socket The socket the message is coming into.
    //! \param message The stateline::comms::Message to send.
    //!
    void send(zmq::socket_t& socket, Message&& message);

    //! Computes a random socket ID conforming to zeromq requirements for the
    //! string (not starting with a zero etc, see zeromq doco).
    //!
    //! \return A socket id string.
    //!
    std::string randomSocketID();

    //! Sets the ID of a socket. The socket must be a DEALER or A REQ.
    //!
    //! \param id The socket ID.
    //! \param socket The socket that is being assigned the ID.
    //!
    void setSocketID(const std::string& id, zmq::socket_t& socket); // TODO: the parameters feel more natural in the other order

  } // namespace comms
} // namespace stateline
