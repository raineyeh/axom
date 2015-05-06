/*
 * Copyright (c) 2015, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 */

/*!
 *******************************************************************************
 * \file logging_example.cc
 *
 * \date May 7, 2015
 * \author George Zagaris (zagaris2@llnl.gov)
 *
 *******************************************************************************
 */

// C/C++ includes
#include <cstdlib> // for rand()

// Logging includes
#include "logapi/Logger.hpp"
#include "logapi/Console.hpp"

using namespace asctoolkit;

#define N 10

int getRandomEvent( const int start, const int end )
{
  return( std::rand() % (end-start) + start );
}

//------------------------------------------------------------------------------
int main( int argc, char** argv )
{
  // STEP 0: initialize logging environment
  logapi::Logger::initialize();

  // STEP 1: Setup the log stream
  logapi::Console* cls = new logapi::Console();

  // STEP 2: enable logging of all messages
  for ( int mtype=0; mtype < logapi::Num_Msg_Types; ++mtype ) {

    logapi::Logger::getInstance()->enable( mtype );
    logapi::Logger::getInstance()->setLogStream( mtype, cls);

  } // END for all message types

  // STEP 3: loop N times and generate a random logging event
  for ( int i=0; i < N; ++i ) {

    logapi::Logger::getInstance()->logMessage(
            getRandomEvent(0,logapi::Num_Msg_Types),
            "a random message", __FILE__,  __LINE__  );

  }

  // STEP 4: shutdown logging environment
  logapi::Logger::finalize();

  delete cls;

  return 0;
}


