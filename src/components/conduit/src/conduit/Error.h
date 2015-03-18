//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2014, Lawrence Livermore National Security, LLC.
// 
// Produced at the Lawrence Livermore National Laboratory
// 
// LLNL-CODE-666778
// 
// All rights reserved.
// 
// This file is part of Conduit. 
// 
// For details, see https://lc.llnl.gov/conduit/.
// 
// Please also read conduit/LICENSE
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the disclaimer below.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the disclaimer (as noted below) in the
//   documentation and/or other materials provided with the distribution.
// 
// * Neither the name of the LLNS/LLNL nor the names of its contributors may
//   be used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
// LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//-----------------------------------------------------------------------------
///
/// file: Error.h
///
//-----------------------------------------------------------------------------

#ifndef __CONDUIT_ERROR_H
#define __CONDUIT_ERROR_H

//-----------------------------------------------------------------------------
// -- conduit library includes -- 
//-----------------------------------------------------------------------------
#include "Core.h"

//-----------------------------------------------------------------------------
// -- standard lib includes -- 
//-----------------------------------------------------------------------------
#include <string>
#include <sstream>

//-----------------------------------------------------------------------------
//
/// The THROW_ERROR macro is the primary mechanism used to capture errors  
/// in conduit. It currently throws a c++ exception, in the form of a
/// conduit::Error class. If we decide to not use exceptions in the future
/// we can simply change the behavior of this macro.
///
//-----------------------------------------------------------------------------
#define THROW_ERROR( msg )                                          \
{                                                                   \
    std::ostringstream oss;                                         \
    oss << msg;                                                     \
    throw conduit::Error( oss.str(), __FILE__, __LINE__);           \
}                                                                   \

//-----------------------------------------------------------------------------
// -- begin conduit:: --
//-----------------------------------------------------------------------------
namespace conduit
{

//-----------------------------------------------------------------------------
// -- begin conduit::Error --
//-----------------------------------------------------------------------------
///
/// class: conduit::Error
///
/// description:
///  Class used to record a runtime error in conduit.
///
//-----------------------------------------------------------------------------
class CONDUIT_API Error : public std::exception
{
public:    
//-----------------------------------------------------------------------------
//
// -- conduit::Error public members --
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Construction and Destruction
//-----------------------------------------------------------------------------
    /// Default constructor
    Error();
    /// Copy constructor
    Error(const Error &err);
    /// Main constructor, used by THROW_ERROR macro
    Error(const std::string &msg, 
          const std::string &file,
          index_t line);
    /// Destructor
    ~Error() throw();
    
//-----------------------------------------------------------------------------
// Methods used to access and display Error as a human readable string. 
//-----------------------------------------------------------------------------
    /// Print info about this error to stdout.
    void                print()   const;
    /// Return a human readable string that describes this error.
    std::string         message() const;
    /// Writes a human readable string that describes this error to the 
    /// passed output stream.
    void                message(std::ostringstream &oss) const;
    /// part of std::exception interface
    virtual const char* what() const throw() 
                        { return message().c_str(); }

private:
//-----------------------------------------------------------------------------
//
// -- conduit::Error private data members --
//
//-----------------------------------------------------------------------------
    /// holds the error message
    std::string m_msg;
    /// holds the source file name where the error was raised
    std::string m_file;
    /// holds the line number in the source file where the error was raised.
    index_t     m_line;
    
};
//-----------------------------------------------------------------------------
// -- end conduit::Error --
//-----------------------------------------------------------------------------

}
//-----------------------------------------------------------------------------
// -- end conduit:: --
//-----------------------------------------------------------------------------


#endif
