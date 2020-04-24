// wrapView.h
// This is generated code, do not edit
//
// Copyright (c) 2017-2020, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
/**
 * \file wrapView.h
 * \brief Shroud generated wrapper for View class
 */
// For C users and C++ implementation

#ifndef WRAPVIEW_H
#define WRAPVIEW_H

#include "typesSidre.h"
#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#include "axom/sidre/core/SidreTypes.hpp"
#else
#include <stdbool.h>
#include <stddef.h>
#include "axom/sidre/interface/SidreTypes.h"
#endif

// splicer begin class.View.CXX_declarations
// splicer end class.View.CXX_declarations

#ifdef __cplusplus
extern "C" {
#endif

// splicer begin class.View.C_declarations
// splicer end class.View.C_declarations

SIDRE_IndexType SIDRE_View_get_index(SIDRE_View* self);

const char* SIDRE_View_get_name(const SIDRE_View* self);

void SIDRE_View_get_name_bufferify(const SIDRE_View* self, char* SHF_rv,
                                   int NSHF_rv);

void SIDRE_View_get_path_bufferify(const SIDRE_View* self, char* SHF_rv,
                                   int NSHF_rv);

void SIDRE_View_get_path_name_bufferify(const SIDRE_View* self, char* SHF_rv,
                                        int NSHF_rv);

SIDRE_Group* SIDRE_View_get_owning_group(SIDRE_View* self, SIDRE_Group* SHC_rv);

bool SIDRE_View_has_buffer(const SIDRE_View* self);

SIDRE_Buffer* SIDRE_View_get_buffer(SIDRE_View* self, SIDRE_Buffer* SHC_rv);

bool SIDRE_View_is_external(const SIDRE_View* self);

bool SIDRE_View_is_allocated(SIDRE_View* self);

bool SIDRE_View_is_applied(const SIDRE_View* self);

bool SIDRE_View_is_described(const SIDRE_View* self);

bool SIDRE_View_is_empty(const SIDRE_View* self);

bool SIDRE_View_is_opaque(const SIDRE_View* self);

bool SIDRE_View_is_scalar(const SIDRE_View* self);

bool SIDRE_View_is_string(const SIDRE_View* self);

int SIDRE_View_get_type_id(const SIDRE_View* self);

size_t SIDRE_View_get_total_bytes(const SIDRE_View* self);

size_t SIDRE_View_get_num_elements(const SIDRE_View* self);

size_t SIDRE_View_get_bytes_per_element(const SIDRE_View* self);

size_t SIDRE_View_get_offset(const SIDRE_View* self);

size_t SIDRE_View_get_stride(const SIDRE_View* self);

int SIDRE_View_get_num_dimensions(const SIDRE_View* self);

int SIDRE_View_get_shape(const SIDRE_View* self, int ndims,
                         SIDRE_IndexType* shape);

void SIDRE_View_allocate_simple(SIDRE_View* self);

void SIDRE_View_allocate_from_type(SIDRE_View* self, int type,
                                   SIDRE_IndexType num_elems);

void SIDRE_View_reallocate(SIDRE_View* self, SIDRE_IndexType num_elems);

void SIDRE_View_attach_buffer_only(SIDRE_View* self, SIDRE_Buffer* buff);

void SIDRE_View_attach_buffer_type(SIDRE_View* self, int type,
                                   SIDRE_IndexType num_elems,
                                   SIDRE_Buffer* buff);

void SIDRE_View_attach_buffer_shape(SIDRE_View* self, int type, int ndims,
                                    SIDRE_IndexType* shape, SIDRE_Buffer* buff);

void SIDRE_View_apply_0(SIDRE_View* self);

void SIDRE_View_apply_nelems(SIDRE_View* self, SIDRE_IndexType num_elems);

void SIDRE_View_apply_nelems_offset(SIDRE_View* self, SIDRE_IndexType num_elems,
                                    SIDRE_IndexType offset);

void SIDRE_View_apply_nelems_offset_stride(SIDRE_View* self,
                                           SIDRE_IndexType num_elems,
                                           SIDRE_IndexType offset,
                                           SIDRE_IndexType stride);

void SIDRE_View_apply_type_nelems(SIDRE_View* self, int type,
                                  SIDRE_IndexType num_elems);

void SIDRE_View_apply_type_nelems_offset(SIDRE_View* self, int type,
                                         SIDRE_IndexType num_elems,
                                         SIDRE_IndexType offset);

void SIDRE_View_apply_type_nelems_offset_stride(SIDRE_View* self, int type,
                                                SIDRE_IndexType num_elems,
                                                SIDRE_IndexType offset,
                                                SIDRE_IndexType stride);

void SIDRE_View_apply_type_shape(SIDRE_View* self, int type, int ndims,
                                 SIDRE_IndexType* shape);

void SIDRE_View_set_scalar_int(SIDRE_View* self, int value);

void SIDRE_View_set_scalar_long(SIDRE_View* self, long value);

void SIDRE_View_set_scalar_float(SIDRE_View* self, float value);

void SIDRE_View_set_scalar_double(SIDRE_View* self, double value);

void SIDRE_View_set_string(SIDRE_View* self, const char* value);

void SIDRE_View_set_string_bufferify(SIDRE_View* self, const char* value,
                                     int Lvalue);

void SIDRE_View_set_external_data_ptr_only(SIDRE_View* self,
                                           void* external_ptr);

void SIDRE_View_set_external_data_ptr_type(SIDRE_View* self, int type,
                                           SIDRE_IndexType num_elems,
                                           void* external_ptr);

void SIDRE_View_set_external_data_ptr_shape(SIDRE_View* self, int type,
                                            int ndims, SIDRE_IndexType* shape,
                                            void* external_ptr);

const char* SIDRE_View_get_string(SIDRE_View* self);

void SIDRE_View_get_string_bufferify(SIDRE_View* self, char* name, int Nname);

int SIDRE_View_get_data_int(SIDRE_View* self);

long SIDRE_View_get_data_long(SIDRE_View* self);

float SIDRE_View_get_data_float(SIDRE_View* self);

double SIDRE_View_get_data_double(SIDRE_View* self);

void* SIDRE_View_get_void_ptr(const SIDRE_View* self);

void SIDRE_View_print(const SIDRE_View* self);

bool SIDRE_View_rename(SIDRE_View* self, const char* new_name);

bool SIDRE_View_rename_bufferify(SIDRE_View* self, const char* new_name,
                                 int Lnew_name);

#ifdef __cplusplus
}
#endif

#endif  // WRAPVIEW_H
