/**
 * \file Map.h
 *
 * \brief Basic API for a map from each element of a set to some domain
 *
 */

#ifndef MESHAPI_MAP_HPP_
#define MESHAPI_MAP_HPP_

#include <vector>
#include <sstream>
#include <iostream>

#include "common/Types.hpp"
#include "common/Utilities.hpp"
#include "meshapi/Set.hpp"
#include "meshapi/NullSet.hpp"

namespace asctoolkit {
namespace meshapi    {

    class NullSet;

    template<typename DataType>
    class Map
    {
    public:
        typedef Set::SetIndex                                          SetIndex;
        typedef Set::size_type                                         size_type;

        typedef std::vector<DataType>                                  OrderedMap;

        static NullSet const s_nullSet;

    public:
        Map(Set const* theSet = &s_nullSet) : m_set(theSet)
        {
            m_data.resize( m_set->size());
        }

        Map(Set const* theSet, DataType defaultValue) : m_set(theSet)
        {
            m_data.resize( m_set->size(), defaultValue );
        }

        ~Map(){}

        DataType const& operator[](SetIndex setIndex) const
        {
            verifyIndex(setIndex);
            return m_data[setIndex];
        }

        DataType & operator[](SetIndex setIndex)
        {
            verifyIndex(setIndex);
            return m_data[setIndex];
        }


        Set const* set() const { return m_set; }


        //* Placeholder for function that returns the (pointer to) underlying data **/
        OrderedMap      & data()        { return m_data; }
        //* Placeholder for function that returns the (const pointer to) underlying data **/
        OrderedMap const& data() const  { return m_data; }


        size_type size() const { return m_set->size(); }

        bool isValid(bool verboseOutput = false) const;

    private:
        inline void  verifyIndex(SetIndex setIndex)       const { ATK_ASSERT( setIndex < m_set->size() ); }

    private:
        Set const*  m_set;
        OrderedMap         m_data;
    };



    /**
     * \brief Definition of static instance of nullSet for all maps
     * \note Should this be a singleton or a global object?  Should the scope be public?
     */
    template<typename DataType>
    NullSet const Map<DataType>::s_nullSet;

    template<typename DataType>
    bool Map<DataType>::isValid(bool verboseOutput) const
    {
        bool bValid = true;

        std::stringstream errStr;

        if(*m_set == s_nullSet)
        {
            if(! m_data.empty() )
            {
                if(verboseOutput)
                {
                    errStr << "\n\t* the underlying set was never provided, but its associated data is not empty"
                        <<" , data has size " << m_data.size();
                }

                bValid = false;
            }
        }
        else
        {
            if( m_data.size() != m_set->size())
            {
                if(verboseOutput)
                {
                    errStr << "\n\t* the underlying set and its associated mapped data have different sizes"
                        <<" , underlying set has size " << m_set->size()
                        <<" , data has size " << m_data.size();
                    ;
                }

                bValid = false;
            }
        }


        if(verboseOutput)
        {
            std::stringstream sstr;

            sstr<<"\n*** Detailed results of isValid on the map.\n";
            if(bValid)
            {
                sstr<<"Map was valid."<< std::endl;
            }
            else
            {
                sstr<<"Map was NOT valid.\n"
                         << sstr.str()
                         << std::endl;
            }

            if(!m_set)
            {
                sstr<<"\n** map is empty.";
            }
            else
            {
                sstr<< "\n** underlying set has size " << m_set->size() <<": ";

                sstr<< "\n** Mapped data:";
                for(SetIndex idx = 0; idx < this->size(); ++idx)
                {
                    sstr<<"\n\telt[" << idx << "]:\t" << (*this)[idx];
                }
            }
            std::cout<< sstr.str() << std::endl;
        }

        return bValid;
    }




} // end namespace meshapi
} // end namespace asctoolkit



#endif // MESHAPI_MAP_HPP_
