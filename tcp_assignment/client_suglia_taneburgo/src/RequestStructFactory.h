#ifndef REQUESTSTRUCTFACTORY_H_
#define REQUESTSTRUCTFACTORY_H_

#include "CommonDefinitions.h"
#include "UtilityFunctions.h"

RequestStruct get_fput_struct(uint64_t, const char*);
RequestStruct get_fget_struct(const char*);
RequestStruct get_rls_struct();
RequestStruct get_rmkdir_struct(const char*);
RequestStruct get_bye_struct();

#endif /* REQUESTSTRUCTFACTORY_H_ */
