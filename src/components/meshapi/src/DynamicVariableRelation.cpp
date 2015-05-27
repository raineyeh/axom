/**
 * \file DynamicVariableRelation.cpp
 *
 *  Created on: May 11, 2015
 *  Author: weiss27
 */

#include "DynamicVariableRelation.hpp"

#include <sstream>
#include <iostream>
#include <iterator>

namespace asctoolkit {
namespace meshapi {

    DynamicVariableRelation::DynamicVariableRelation (Set* fromSet, Set* toSet)
    : m_fromSet(fromSet), m_toSet(toSet)
    {
        if(m_fromSet)
        {
            m_relationsVec.resize( m_fromSet->size() );
        }
    }



    bool DynamicVariableRelation::isValid(bool verboseOutput) const
    {
        bool bValid = true;

        std::stringstream sstr;

        if( m_fromSet == NULL || m_toSet == NULL)
        {
            if(!m_relationsVec.empty())
            {
                if(verboseOutput)
                {
                    sstr << "\n\t* relations vector was not empty "
                        <<" -- fromSet was " << (m_fromSet == NULL ? "" : " not ") << "null"
                        <<" , toSet was " << (m_toSet == NULL ? "" : " not ") << "null";
                }

                bValid = false;
            }
        }
        else
        {
            if(verboseOutput)
                sstr << "\n\t* Neither set was null";

            // Check that the the relations vector has the right size (should be same as fromSet's size() )
            if( m_relationsVec.size() != m_fromSet->size() )
            {
                if(verboseOutput)
                {
                    sstr << "\n\t* relations vector has the wrong size."
                         << "\n\t-- from set size is: " << m_fromSet->size()
                         << "\n\t-- expected relation size: " << m_fromSet->size()
                         << "\n\t-- actual size: " << m_relationsVec.size()
                         ;
                }
                bValid = false;
            }

            // Check that all elements of the relations vector point to valid set elements in the toSet
            for(SetIndex fromIdx=0; fromIdx < m_fromSet->size(); ++fromIdx)
            {
                SetIndex idx = (*m_fromSet)[fromIdx];
                for(RelationVecConstIterator rIt = begin(idx), rEnd = end(idx); rIt < rEnd; ++rIt)
                {
                    if( *rIt >= m_toSet->size() )
                    {
                        if(verboseOutput)
                        {
                            sstr << "\n\t* relation for element " << fromIdx << " of fromSet had an out-of-range element."
                                    << " -- value of element " << std::distance( begin(idx), rIt) << " was " << *rIt
                                    << ". Max possible value should be " << m_toSet->size() <<"." ;
                        }
                        bValid = false;
                    }
                }
            }
        }


        if(verboseOutput)
        {
            std::cout<<"\n*** Detailed results of isValid on the relation.\n";
            if(bValid)
            {
                std::cout<<"(dynamic,variable) Relation was valid."<< std::endl;
            }
            else
            {
                std::cout<<"Relation was NOT valid.\n"
                         << sstr.str()
                         << std::endl;
            }

            if(m_fromSet)
                std::cout<< "\n** fromSet has size " << m_fromSet->size() <<": ";
            if(m_toSet)
                std::cout<< "\n** toSet has size " << m_toSet->size() <<": ";

            if(m_relationsVec.empty())
            {
                std::cout<< "\n** relations vec is empty:";
            }
            else
            {
                size_type overallCount = 0;
                std::cout<< "\n** relations vec elements:";

                for(SetIndex fromIdx=0; fromIdx < m_fromSet->size(); ++fromIdx)
                {
                    SetIndex idx = (*m_fromSet)[fromIdx];
                    std::cout<<"\n\telt[" << fromIdx << "] (" <<size(idx)  << "):\t";
                    std::copy(begin(idx), end(idx), std::ostream_iterator<SetIndex>(std::cout, " "));
                    overallCount += size(idx);
                }
                std::cout<< "\n\n\tOverall size of relation" << overallCount << std::endl;
            }
        }

        return bValid;
    }

} // namespace meshapi
} // namespace asctoolkit
