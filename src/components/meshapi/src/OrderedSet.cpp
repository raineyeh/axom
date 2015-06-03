/*
 * OrderedSet.cpp
 *
 *  Created on: Apr 23, 2015
 *      Author: weiss27
 */

#include <stdexcept>
#include <sstream>
#include <iostream>
#include "OrderedSet.hpp"

namespace asctoolkit {
namespace meshapi {


const NullSet OrderedSet::s_nullSet;

bool OrderedSet::isValid(bool verboseOutput) const
{
    bool bValid = true;

    std::stringstream errStr;

//    if( m_size < 0 )
//    {
//        if(verboseOutput)
//        {
//            errStr <<"\n** OrderedSet's size cannot be less than zero"
//                    <<"\n\t Size was " << m_size
//                    ;
//        }
//        bValid = false;
//    }


    if(verboseOutput)
    {
        std::stringstream sstr;

        sstr<<"\n*** Detailed results of isValid on the OrderedSet.\n";
        if(bValid)
        {
            sstr<<"Set was valid."<< std::endl;
        }
        else
        {
            sstr<<"Set was NOT valid.\n"
                     << errStr.str()
                     << std::endl;
        }

        sstr<<"\n** size is " << m_size ;
        std::cout<< sstr.str() << std::endl;
    }

    return bValid;
}

} /* namespace meshapi */
} /* namespace asctoolkit */

