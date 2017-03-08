// wrapExClass2.h
// This is generated code, do not edit
// blah blah
// yada yada
//
/**
 * \file wrapExClass2.h
 * \brief Shroud generated wrapper for ExClass2 class
 */
// For C users and C++ implementation

#ifndef WRAPEXCLASS2_H
#define WRAPEXCLASS2_H

#include "sidre/SidreTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// declaration of wrapped types
struct s_AA_exclass1;
typedef struct s_AA_exclass1 AA_exclass1;
struct s_AA_exclass2;
typedef struct s_AA_exclass2 AA_exclass2;

// splicer begin class.ExClass2.C_definition
// splicer end class.ExClass2.C_definition

AA_exclass2 * AA_exclass2_ex_class2(const char * name);

AA_exclass2 * AA_exclass2_ex_class2_bufferify(const char * name, int Lname);

void AA_exclass2_delete(AA_exclass2 * self);

const char * AA_exclass2_get_name(const AA_exclass2 * self);

void AA_exclass2_get_name_bufferify(const AA_exclass2 * self, char * SH_F_rv, int LSH_F_rv);

const char * AA_exclass2_get_name2(AA_exclass2 * self);

void AA_exclass2_get_name2_bufferify(AA_exclass2 * self, char * SH_F_rv, int LSH_F_rv);

char * AA_exclass2_get_name3(const AA_exclass2 * self);

void AA_exclass2_get_name3_bufferify(const AA_exclass2 * self, char * SH_F_rv, int LSH_F_rv);

char * AA_exclass2_get_name4(AA_exclass2 * self);

void AA_exclass2_get_name4_bufferify(AA_exclass2 * self, char * SH_F_rv, int LSH_F_rv);

const int AA_exclass2_get_name_length(AA_exclass2 * self);

AA_exclass1 * AA_exclass2_get_class1(AA_exclass2 * self, const AA_exclass1 * in);

void AA_exclass2_declare_0(AA_exclass2 * self, int type);

void AA_exclass2_declare_1(AA_exclass2 * self, int type, SIDRE_SidreLength len);

void AA_exclass2_destroyall(AA_exclass2 * self);

int AA_exclass2_get_type_id(const AA_exclass2 * self);

void AA_exclass2_set_value_int(AA_exclass2 * self, int value);

void AA_exclass2_set_value_long(AA_exclass2 * self, long value);

void AA_exclass2_set_value_float(AA_exclass2 * self, float value);

void AA_exclass2_set_value_double(AA_exclass2 * self, double value);

int AA_exclass2_get_value_int(AA_exclass2 * self);

double AA_exclass2_get_value_double(AA_exclass2 * self);

#ifdef __cplusplus
}
#endif

#endif  // WRAPEXCLASS2_H
