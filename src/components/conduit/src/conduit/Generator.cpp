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
/// file: Generator.cpp
///
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// -- conduit library includes -- 
//-----------------------------------------------------------------------------
#include "Generator.h"
#include "Error.h"
#include "Utils.h"

//-----------------------------------------------------------------------------
// -- rapidjson includes -- 
//-----------------------------------------------------------------------------
#include "rapidjson/document.h"

//-----------------------------------------------------------------------------
// -- standard lib includes -- 
//-----------------------------------------------------------------------------
#include <stdio.h>

//-----------------------------------------------------------------------------
// -- begin conduit:: --
//-----------------------------------------------------------------------------
namespace conduit
{

//-----------------------------------------------------------------------------
// -- stand alone methods for parsing via rapidjson -- 
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// we want to isolate the conduit API from the rapidjson headers
// so any methods using rapidjson types are defined in the cpp imp.
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------//
index_t 
json_to_numeric_dtype(const rapidjson::Value &jvalue)
{
    index_t res = DataType::EMPTY_T; 
    if(jvalue.IsNumber())
    {
        // TODO: We could have better logic for dealing with int vs uint
        if(jvalue.IsUint64() || 
           jvalue.IsInt64()  || 
           jvalue.IsUint()   ||
           jvalue.IsInt())
        {
            res  = DataType::INT64_T; // for int
        }
        else if(jvalue.IsDouble())
        {
            res  = DataType::FLOAT64_T; // for float
        } 
        // else -- value already inite to EMPTY_T
    }
    
    return res;
}

//---------------------------------------------------------------------------//
index_t
check_homogenous_json_array(const rapidjson::Value &jvalue)
{
    // check for homogenous array of ints or floats
    // promote to float64 as the most wide type
    // (this is a heuristic decision)

    if(jvalue.Size() == 0)
        return DataType::EMPTY_T;

    index_t val_type = json_to_numeric_dtype(jvalue[(rapidjson::SizeType)0]); 
    bool homogenous  = (val_type != DataType::EMPTY_T);

    for (rapidjson::SizeType i = 1; i < jvalue.Size() && homogenous; i++)
    {
        index_t curr_val_type = json_to_numeric_dtype(jvalue[i]);
        if((val_type == DataType::INT64_T || val_type == DataType::INT64_T) &&
           curr_val_type ==  DataType::FLOAT64_T)
        {
            // promote to a double (may lose prec in some cases)
            val_type = DataType::FLOAT64_T;
        }
        else if(curr_val_type == DataType::EMPTY_T)
        {
            // non hmg inline
            homogenous = false;
            val_type = DataType::EMPTY_T;
        }
    }

    return val_type;
}

//---------------------------------------------------------------------------// 
void
parse_json_int64_array(const rapidjson::Value &jvalue,
                        std::vector<int64> &res)
{
   res.resize(jvalue.Size(),0);
   for (rapidjson::SizeType i = 0; i < jvalue.Size(); i++)
   {
       res[i] = jvalue[i].GetInt64();
   }
}

//---------------------------------------------------------------------------//
void
parse_json_int64_array(const rapidjson::Value &jvalue,
                       Node &node)
{
    // TODO: we can make this more efficent 
    std::vector<int64> vals;
    parse_json_int64_array(jvalue,vals);
    
    switch(node.dtype().id())
    {
        case DataType::INT8_T:   
            node.as_int8_array().set(vals);
            break;
        case DataType::INT16_T: 
            node.as_int16_array().set(vals);
            break;
        case DataType::INT32_T:
            node.as_int32_array().set(vals);
            break;
        case DataType::INT64_T:
            node.as_int64_array().set(vals);
            break;
        // unsigned ints
        case DataType::UINT8_T:
            node.as_uint8_array().set(vals);
            break;
        case DataType::UINT16_T:
            node.as_uint16_array().set(vals);
            break;
        case DataType::UINT32_T:
            node.as_uint32_array().set(vals);
            break;
        case DataType::UINT64_T:
            node.as_uint64_array().set(vals);
            break;  
        //floats
        case DataType::FLOAT32_T:
            node.as_float32_array().set(vals);
            break;
        case DataType::FLOAT64_T:
            node.as_float64_array().set(vals);
            break;
    }
}

//---------------------------------------------------------------------------//
void
parse_json_uint64_array(const rapidjson::Value &jvalue,
                         std::vector<uint64> &res)
{
    res.resize(jvalue.Size(),0);
    for (rapidjson::SizeType i = 0; i < jvalue.Size(); i++)
    {
        res[i] = jvalue[i].GetUint64();
    }
}

//---------------------------------------------------------------------------//
void
parse_json_uint64_array(const rapidjson::Value &jvalue,
                        Node &node)
{
    // TODO: we can make this more efficent 
    std::vector<uint64> vals;
    parse_json_uint64_array(jvalue,vals);
    
    switch(node.dtype().id())
    {
        case DataType::INT8_T:   
            node.as_int8_array().set(vals);
            break;
        case DataType::INT16_T: 
            node.as_int16_array().set(vals);
            break;
        case DataType::INT32_T:
            node.as_int32_array().set(vals);
            break;
        case DataType::INT64_T:
            node.as_int64_array().set(vals);
            break;
        // unsigned ints
        case DataType::UINT8_T:
            node.as_uint8_array().set(vals);
            break;
        case DataType::UINT16_T:
            node.as_uint16_array().set(vals);
            break;
        case DataType::UINT32_T:
            node.as_uint32_array().set(vals);
            break;
        case DataType::UINT64_T:
            node.as_uint64_array().set(vals);
            break;  
        //floats
        case DataType::FLOAT32_T:
            node.as_float32_array().set(vals);
            break;
        case DataType::FLOAT64_T:
            node.as_float64_array().set(vals);
            break;
    }
}

//---------------------------------------------------------------------------//
void
parse_json_float64_array(const rapidjson::Value &jvalue,
                         std::vector<float64> &res)
{
    res.resize(jvalue.Size(),0);
    for (rapidjson::SizeType i = 0; i < jvalue.Size(); i++)
    {
        res[i] = jvalue[i].GetDouble();
    }
}

//---------------------------------------------------------------------------//
void
parse_json_float64_array(const rapidjson::Value &jvalue,
                         Node &node)
{
    // TODO: we can make this more efficent 
    std::vector<float64> vals;
    parse_json_float64_array(jvalue,vals);
    
    switch(node.dtype().id())
    {
        case DataType::INT8_T:   
            node.as_int8_array().set(vals);
            break;
        case DataType::INT16_T: 
            node.as_int16_array().set(vals);
            break;
        case DataType::INT32_T:
            node.as_int32_array().set(vals);
            break;
        case DataType::INT64_T:
            node.as_int64_array().set(vals);
            break;
        // unsigned ints
        case DataType::UINT8_T:
            node.as_uint8_array().set(vals);
            break;
        case DataType::UINT16_T:
            node.as_uint16_array().set(vals);
            break;
        case DataType::UINT32_T:
            node.as_uint32_array().set(vals);
            break;
        case DataType::UINT64_T:
            node.as_uint64_array().set(vals);
            break;  
        //floats
        case DataType::FLOAT32_T:
            node.as_float32_array().set(vals);
            break;
        case DataType::FLOAT64_T:
            node.as_float64_array().set(vals);
            break;
    }
}

//---------------------------------------------------------------------------//
void
parse_leaf_dtype(const rapidjson::Value &jvalue, index_t offset, DataType &dtype_res)
{
    
    if(jvalue.IsString())
    {
        std::string dtype_name(jvalue.GetString());
        index_t dtype_id = DataType::name_to_id(dtype_name);
        index_t ele_size = DataType::default_bytes(dtype_id);
        dtype_res.set(dtype_id,
                      1,
                      offset,
                      ele_size,
                      ele_size,
                      Endianness::DEFAULT_T);
    }
    else if(jvalue.IsObject())
    {
        std::string dtype_name(jvalue["dtype"].GetString());
        index_t length=0;
        if(jvalue.HasMember("length"))
        {
            length = jvalue["length"].GetUint64();
        }
        index_t dtype_id  = DataType::name_to_id(dtype_name);
        index_t ele_size  = DataType::default_bytes(dtype_id);
        index_t stride    = ele_size;
    
        //  parse offset (override offset if passed)
        if(jvalue.HasMember("offset") && jvalue["offset"].IsNumber())
        {
            offset = jvalue["offset"].GetUint64();
        }


        // parse stride
        if(jvalue.HasMember("stride") && jvalue["stride"].IsNumber())
        {
            stride = jvalue["stride"].GetUint64();
        }

        // TODO: parse element_size
    
        // parse endianness
        index_t endianness = Endianness::DEFAULT_T;
        if(jvalue.HasMember("endianess") && jvalue["endianness"].IsString())
        {
            std::string end_val(jvalue["endianness"].GetString());
            if(end_val == "big")
            {
                endianness = Endianness::BIG_T;
            }
            else
            {
                endianness = Endianness::LITTLE_T;
            }
        
        }
    
        if(length == 0)
        {
            if(jvalue.HasMember("value") &&
               jvalue["value"].IsArray())
            {
                length = jvalue["value"].Size();
            }
            else if(!jvalue.HasMember("length")) // support explicit length 0 in a schema
            {
                length = 1;
            }
        }
    
        dtype_res.set(dtype_id,
                      length,
                      offset,
                      stride, 
                      ele_size,
                      endianness);
    }
    else
    {
        /// TODO: Error
    }
}

//---------------------------------------------------------------------------//
void
parse_inline_leaf(const rapidjson::Value &jvalue,
                  Node &node)
{
    if(jvalue.IsString())
    {
        if(node.dtype().id() == DataType::CHAR8_STR_T)
        {
            std::string sval(jvalue.GetString());
            node.set(sval);
        }
        else
        {
             /// TODO: ERROR
             // type incompat with char8_str
             // only allow strings to be assigned to a char8_str type
             // throw parsing error if our inline values
             // don't match what we expected
        }
    }
    else if(jvalue.IsBool())
    {
        //
        if(node.dtype().id() == DataType::UINT8_T)
        {
            node.set((uint8)jvalue.GetBool());
        }
        else
        {
             /// TODO: ERROR
             // type incompat with uint8
             // only allow json bools to be assigned to a uint8 type
             // throw parsing error if our inline values
             // don't match what we expected
        }
    }
    else if(jvalue.IsNumber())
    {
        switch(node.dtype().id())
        {
            // signed ints
            case DataType::INT8_T:   
                node.set((int8)jvalue.GetInt64());
                break;
            case DataType::INT16_T: 
                node.set((int16)jvalue.GetInt64());
                break;
            case DataType::INT32_T:
                node.set((int32)jvalue.GetInt64());
                break;
            case DataType::INT64_T:
                node.set((int64)jvalue.GetInt64());
                break;
            // unsigned ints
            case DataType::UINT8_T:
                node.set((uint8)jvalue.GetUint64());
                break;
            case DataType::UINT16_T:
                node.set((uint16)jvalue.GetUint64());
                break;
            case DataType::UINT32_T:
                node.set((uint32)jvalue.GetUint64());
                break;
            case DataType::UINT64_T:
                node.set((uint64)jvalue.GetUint64());
                break;  
            //floats
            case DataType::FLOAT32_T:
                node.set((float32)jvalue.GetDouble());
                break;
            case DataType::FLOAT64_T:
                node.set((float64)jvalue.GetDouble());
                break;
            // case default:
            //     /// TODO: ERROR
            //     // type incompat with numeric
            //     // only allow numeric to be assigned to a numeric type
            //     // throw parsing error if our inline values
            //     // don't match what we expected
            //     ;
            //     break;
        }
    }
}

//---------------------------------------------------------------------------//
void
parse_inline_value(const rapidjson::Value &jvalue,
                   Node &node)
{
    if(jvalue.IsArray())
    {
        // we assume a "value" is a leaf or list of compatiable leafs
        index_t hval_type = check_homogenous_json_array(jvalue);
        
        if(node.dtype().number_of_elements() < jvalue.Size())
        {
            std::cout << "ERROR" << std::endl;
            // TODO: error
        }
        
        if(hval_type == DataType::INT64_T)
        {
            if(node.dtype().is_unsigned_integer())
            {
                parse_json_uint64_array(jvalue,node);                
            }
            else
            {
                parse_json_int64_array(jvalue,node);
            }
        }
        else if(hval_type == DataType::FLOAT64_T)
        {
            parse_json_float64_array(jvalue,node);
        }
        else // error
        {
            //TODO: Parsing Error, not hmg
        }
    }
    else
    {
        parse_inline_leaf(jvalue,node);
    }
}


//---------------------------------------------------------------------------//
void 
walk_json_schema(Schema *schema,
                 const   rapidjson::Value &jvalue,
                 index_t curr_offset)
{
    // object cases
    if(jvalue.IsObject())
    {
        if (jvalue.HasMember("dtype"))
        {
            // if dtype is an object, we have a "list_of" case
            const rapidjson::Value &dt_value = jvalue["dtype"];
            if(dt_value.IsObject())
            {
                int length =1;
                if(jvalue.HasMember("length"))
                {
                    // TODO: Handle reference 
                    if(jvalue["length"].IsObject() && 
                       jvalue["length"].HasMember("reference"))
                    {
                        // in some cases we shouldn't get here ...
                        // TODO ref without "data" could be a problem
                    }
                    else
                    {
                        length = jvalue["length"].GetInt();
                    }
                }
                // we will create `length' # of objects of obj des by dt_value
                 
                // TODO: we only need to parse this once, not leng # of times
                // but this is the easiest way to start.
                for(int i=0;i< length;i++)
                {
                    Schema curr_schema(DataType::Objects::list());
                    walk_json_schema(&curr_schema,dt_value, curr_offset);
                    schema->append(curr_schema);
                    curr_offset += curr_schema.total_bytes();
                }
            }
            else
            {
                // handle leaf node with explicit props
                DataType dtype;
                parse_leaf_dtype(jvalue,curr_offset,dtype);
                schema->set(dtype);
            }
        }
        else
        {
            // loop over all entries
            for (rapidjson::Value::ConstMemberIterator itr =
                 jvalue.MemberBegin(); 
                 itr != jvalue.MemberEnd(); ++itr)
            {
                std::string entry_name(itr->name.GetString());
                Schema &curr_schema = schema->fetch(entry_name);
                curr_schema.set(DataType::Objects::object());
                walk_json_schema(&curr_schema,itr->value, curr_offset);
                curr_offset += curr_schema.total_bytes();
            }
        }
    }
    // List case 
    else if (jvalue.IsArray()) 
    {
        for (rapidjson::SizeType i = 0; i < jvalue.Size(); i++)
        {
            Schema curr_schema(DataType::Objects::list());
            walk_json_schema(&curr_schema,jvalue[i], curr_offset);
            curr_offset += curr_schema.total_bytes();
            // this will coerce to a list
            schema->append(curr_schema);
        }
    }
    // Simplest case, handles "uint32", "float64", etc
    else if(jvalue.IsString())
    {
        DataType dtype;
        parse_leaf_dtype(jvalue,curr_offset,dtype);
        schema->set(dtype);
    }
}

//---------------------------------------------------------------------------//
void 
walk_pure_json_schema(Node  *node,
                     Schema *schema,
                     const rapidjson::Value &jvalue)
{
    // object cases
    if(jvalue.IsObject())
    {
        // loop over all entries
        for (rapidjson::Value::ConstMemberIterator itr = jvalue.MemberBegin(); 
             itr != jvalue.MemberEnd(); ++itr)
        {
            std::string entry_name(itr->name.GetString());
            Schema *curr_schema = schema->fetch_pointer(entry_name);
            Node *curr_node  = new Node();
            curr_node->set_schema_pointer(curr_schema);
            curr_node->set_parent(node);
            walk_pure_json_schema(curr_node,curr_schema,itr->value);
            node->append_node_pointer(curr_node);

        }
    }
    // List case 
    else if (jvalue.IsArray()) 
    {
        index_t hval_type = check_homogenous_json_array(jvalue);
        if(hval_type == DataType::INT64_T)
        {
            std::vector<int64> res;
            parse_json_int64_array(jvalue,res);
            node->set(res);
        }
        else if(hval_type == DataType::FLOAT64_T)
        {
            std::vector<float64> res;
            parse_json_float64_array(jvalue,res);
            node->set(res);            
        }
        else // not numeric array
        {
            for (rapidjson::SizeType i = 0; i < jvalue.Size(); i++)
            {
                schema->append();
                Schema *curr_schema = schema->child_pointer(i);
                Node * curr_node = new Node();
                curr_node->set_schema_pointer(curr_schema);
                curr_node->set_parent(node);
                walk_pure_json_schema(curr_node,curr_schema,jvalue[i]);
                node->append_node_pointer(curr_node);
            }
        }
    }
    // Simplest case, handles "uint32", "float64", with extended type info
    else if(jvalue.IsString()) // bytestr case
    {
        std::string sval(jvalue.GetString());
        node->set(sval);
    }
    else if(jvalue.IsNull())
    {
        node->reset();
    }
    else if(jvalue.IsBool())
    {
        // we store bools as uint8s
        if(jvalue.IsTrue())
        {
            node->set((uint8)1);
        }
        else
        {
            node->set((uint8)0);
        }
    }
    else if(jvalue.IsNumber())
    {
        // use 64bit types by default ... 
        if(jvalue.IsInt() || jvalue.IsInt64())
        {
            node->set((int64)jvalue.GetInt64());
        }
        else if(jvalue.IsUint() || jvalue.IsUint64())
        {
            node->set((uint64)jvalue.GetUint64());
        }
        else  // double case
        {
            node->set((float64)jvalue.GetDouble());
        }
    }
}


//---------------------------------------------------------------------------//
void 
walk_json_schema(Node   *node,
                 Schema *schema,
                 void   *data,
                 const rapidjson::Value &jvalue,
                 index_t curr_offset)
{
    // object cases
    if(jvalue.IsObject())
    {

        if (jvalue.HasMember("dtype"))
        {
            // if dtype is an object, we have a "list_of" case
            const rapidjson::Value &dt_value = jvalue["dtype"];
            if(dt_value.IsObject())
            {
                index_t length =1;
                if(jvalue.HasMember("length"))
                {
                    if(jvalue["length"].IsNumber())
                    {
                        length = jvalue["length"].GetInt();
                    }
                    else if(jvalue["length"].IsObject() && 
                            jvalue["length"].HasMember("reference"))
                    {
                        std::string ref_path = 
                          jvalue["length"]["reference"].GetString();
                        length = node->fetch(ref_path).to_index_t();
                    }
                    
                }
                // we will create `length' # of objects of obj des by dt_value
                 
                // TODO: we only need to parse this once, not leng # of times
                // but this is the easiest way to start.                             
                for(index_t i=0;i< length;i++)
                {
                    schema->append();
                    Schema *curr_schema = schema->child_pointer(i);
                    Node *curr_node = new Node();
                    curr_node->set_schema_pointer(curr_schema);
                    curr_node->set_parent(node);
                    walk_json_schema(curr_node,
                                     curr_schema,
                                     data,
                                     dt_value,
                                     curr_offset);
                    // auto offset only makes sense when we have data
                    if(data != NULL)
                        curr_offset += curr_schema->total_bytes();
                    node->append_node_pointer(curr_node);
                }
                
            }
            else
            {
                // handle leaf node with explicit props
                DataType dtype;
                
                parse_leaf_dtype(jvalue,curr_offset,dtype);
   
                if(data != NULL)
                {
                    // node needs to link schema ptr
                    schema->set(dtype);
                    node->set_schema_pointer(schema);
                    node->set_data_pointer(data);
                }
                else
                {
                    node->set_schema_pointer(schema); // properly links back to schema tree
                    // we need to dynamically alloc
                    node->set(dtype);  // causes an init
                }

                // check for inline json values
                if(jvalue.HasMember("value"))
                {
    
                    parse_inline_value(jvalue["value"],*node);
                }
            }
        }
        else
        {
            // standard object case - loop over all entries
            for (rapidjson::Value::ConstMemberIterator itr = 
                 jvalue.MemberBegin(); 
                 itr != jvalue.MemberEnd(); ++itr)
            {
                std::string entry_name(itr->name.GetString());
                Schema *curr_schema = schema->fetch_pointer(entry_name);
                Node *curr_node = new Node();
                curr_node->set_schema_pointer(curr_schema);
                curr_node->set_parent(node);
                walk_json_schema(curr_node,
                                 curr_schema,
                                 data,
                                 itr->value,
                                 curr_offset);
                // auto offset only makes sense when we have data
                if(data != NULL)
                    curr_offset += curr_schema->total_bytes();
                node->append_node_pointer(curr_node);                
            }
            
        }
    }
    // List case 
    else if (jvalue.IsArray()) 
    {
        for (rapidjson::SizeType i = 0; i < jvalue.Size(); i++)
        {
            schema->append();
            Schema *curr_schema = schema->child_pointer(i);
            Node *curr_node = new Node();
            curr_node->set_schema_pointer(curr_schema);
            curr_node->set_parent(node);
            walk_json_schema(curr_node,
                             curr_schema,
                             data,
                             jvalue[i],
                             curr_offset);
            // auto offset only makes sense when we have data
            if(data != NULL)
                curr_offset += curr_schema->total_bytes();
            node->append_node_pointer(curr_node);
        }
        
    }
    // Simplest case, handles "uint32", "float64", with extended type info
    else if(jvalue.IsString())
    {
        DataType dtype;
        parse_leaf_dtype(jvalue,curr_offset,dtype);
        schema->set(dtype);
        
        if(data != NULL)
        {
             // node needs to link schema ptr 
             node->set_schema_pointer(schema);
             node->set_data_pointer(data);
             
        }
        else
        {
             // sets the pointer
             node->set_schema_pointer(schema); // properly links back to schema tree
             // we need to dynamically alloc
             node->set(dtype);  // causes an init
        }
    }
}


//-----------------------------------------------------------------------------
// -- begin conduit::Generator --
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Generator Construction and Destruction
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------//
Generator::Generator(const std::string &json_schema)
:m_json_schema(json_schema),
 m_protocol("conduit"),
 m_data(NULL)
{}


//---------------------------------------------------------------------------//
Generator::Generator(const std::string &json_schema,
                     void *data)
:m_json_schema(json_schema),
 m_protocol("conduit"),
 m_data(data)
{}

//---------------------------------------------------------------------------//
Generator::Generator(const std::string &json_schema,
                     const std::string &protocol,
                     void *data)
:m_json_schema(json_schema),
 m_protocol(protocol),
 m_data(data)
{}

//-----------------------------------------------------------------------------
// JSON Parsing interface
//-----------------------------------------------------------------------------s

//---------------------------------------------------------------------------//
void 
Generator::walk(Schema &schema) const
{
    schema.reset();
    rapidjson::Document document;
    std::string res = utils::json_sanitize(m_json_schema);
    if(document.Parse<0>(res.c_str()).HasParseError())
    {
        THROW_ERROR("rapidjson parse error");
        /// TODO: better parse error msg
    }
    index_t curr_offset = 0;
    conduit::walk_json_schema(&schema,document,curr_offset);
}

//---------------------------------------------------------------------------//
void 
Generator::walk(Node &node) const
{
    /// TODO: THis is an inefficient code path, need better solution?
    Node n;
    walk_external(n);
    n.compact_to(node);
}

//---------------------------------------------------------------------------//
void 
Generator::walk_external(Node &node) const
{
    node.reset();
    // if data is null, we can parse the schema via the other 'walk' method
    if(m_protocol == "json")
    {
        rapidjson::Document document;
        std::string res = utils::json_sanitize(m_json_schema);
        if(document.Parse<0>(res.c_str()).HasParseError())
        {
            THROW_ERROR("rapidjson parse error");
            /// TODO: better parse error msg
        }
        conduit::walk_pure_json_schema(&node,
                                       node.schema_pointer(),
                                       document);
    }
    else
    {
        rapidjson::Document document;
        std::string res = utils::json_sanitize(m_json_schema);
        if(document.Parse<0>(res.c_str()).HasParseError())
        {
            THROW_ERROR("rapidjson parse error");
            /// TODO: better parse error msg
        }
        index_t curr_offset = 0;
        conduit::walk_json_schema(&node,
                                  node.schema_pointer(),
                                  m_data,
                                  document,
                                  curr_offset);
    }
}


};
//-----------------------------------------------------------------------------
// -- end conduit:: --
//-----------------------------------------------------------------------------

