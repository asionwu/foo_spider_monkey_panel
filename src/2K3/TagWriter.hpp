#pragma once

class TagWriter
{
public:
	TagWriter(metadb_handle_list_cref handles);

	void from_json_array(JSON& arr);
	void from_json_object(JSON& obj);

private:
	metadb_handle_list m_handles;
};
