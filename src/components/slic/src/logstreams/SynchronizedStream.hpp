/*
 * Copyright (c) 2015, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 *
 * All rights reserved.
 *
 * This source code cannot be distributed without permission and further
 * review from Lawrence Livermore National Laboratory.
 */

/*!
 *******************************************************************************
 * \file SynchronizedStream.hpp
 *
 * \date May 7, 2015
 * \author George Zagaris (zagaris2@llnl.gov)
 *
 *******************************************************************************
 */

#ifndef SYNCHRONIZEDSTREAM_HPP_
#define SYNCHRONIZEDSTREAM_HPP_

#include "slic/LogStream.hpp"

#include "common/Types.hpp"

// C/C++ includes
#include <iostream> // for std::ostream

// MPI
#include <mpi.h> // For MPI


namespace asctoolkit {
namespace slic {

/*!
 *******************************************************************************
 * \class SynchronizedStream
 *
 * \brief A concrete instance of LogStream that dumps messages to a C++
 *  std::ostream object.
 *
 * \note The intent of this class is to illustrate how to using the Logging
 *  facility within an MPI distributed environment and provide a utility that
 *  could be useful for debugging problems at small scales.
 *
 * \warning Do not use this for large-scale production runs.
 * \warning The intent of this class is to be used primarily with std::cout,
 *  std::cerr, etc. It is suggested that applications do not use this class
 *  with an std::ofstream object.
 *******************************************************************************
 */
class SynchronizedStream : public LogStream
{
public:
  SynchronizedStream( std::ostream* stream, MPI_Comm comm );
  SynchronizedStream( std::ostream* stream, MPI_Comm comm,
                      std::string& format);

  virtual ~SynchronizedStream();

  /*!
   *****************************************************************************
   * \brief Appends the given message to the stream.
   *
   * \param [in] msgType the type of the message.
   * \param [in] message the user-supplied message.
   * \param [in] fileName the file where this message is appended
   * \param [in] line the line within the file at which the message is appended.
   *
   * \note This method doesn't put anything to the console. Instead the
   *  messages are cached locally to each ranks and are dumped to the console
   *  in rank order when flush is called.
   *****************************************************************************
   */
  virtual void append( message::Level msgLevel,
                       const std::string& message,
                       const std::string& tagName,
                       const std::string& fileName,
                       int line );

  /*!
   *****************************************************************************
   * \brief Dumps the messages to the console in rank-order.
   *****************************************************************************
   */
  virtual void flush();

private:

  /// Forward declarations
  struct MessageCache;

  /// \name Private Members
  /// @{

  MPI_Comm m_comm;
  MessageCache* m_cache;
  std::ostream* m_stream;
  /// @}

  /*!
   *****************************************************************************
   * \brief Default constructor. Made private to prevent applications from
   *  using it. Instead the constructor that passes the underlying MPI comm.
   *  should be used.
   *****************************************************************************
   */
  SynchronizedStream(): m_comm(MPI_COMM_NULL),
                        m_cache( static_cast<MessageCache*>(ATK_NULLPTR) ),
                        m_stream( static_cast<std::ostream*>(ATK_NULLPTR) )
  { };


  /// \name Disabled Methods
  /// @{

  SynchronizedStream( const SynchronizedStream& ); // Not implemented
  SynchronizedStream& operator=( const SynchronizedStream& ); // Not implemented

  /// @}
};

} /* namespace slic */
} /* namespace asctoolkit */

#endif /* SYNCHRONIZEDSTREAM_HPP_ */
