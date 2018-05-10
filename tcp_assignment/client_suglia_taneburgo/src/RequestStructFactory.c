#include "RequestStructFactory.h"

RequestStruct get_fput_struct(uint64_t size, const char *file_name) {
	RequestStruct rs;
	char relative_path[MAX_STR_LEN];
	rs.op = FPUT;
	rs.file_size = size;
	get_file_name_from_path(file_name, relative_path);
	strcpy(rs.name, relative_path);

	return rs;
}

RequestStruct get_fget_struct(const char *file_name) {
	RequestStruct rs;
	rs.op = FGET;
	rs.file_size = 0;
	strcpy(rs.name, file_name);

	return rs;
}

RequestStruct get_rls_struct() {
	RequestStruct rs;
	rs.op = RLS;
	rs.file_size = 0;

	return rs;
}

RequestStruct get_rmkdir_struct(const char *dir_name) {
	RequestStruct rs;
	rs.op = RMKDIR;
	rs.file_size = 0;
	strcpy(rs.name, dir_name);

	return rs;
}

RequestStruct get_bye_struct() {
	RequestStruct rs;
	rs.op = BYE;
	rs.file_size = 0;

	return rs;
}

