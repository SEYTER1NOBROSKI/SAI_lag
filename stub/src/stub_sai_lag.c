#include "sai.h"
#include "stub_sai.h"
#include "assert.h"

sai_status_t stub_create_lag(
    _Out_ sai_object_id_t* lag_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
	static int32_t next_lag_id = 1;
	sai_status_t status;
	status = stub_create_object(SAI_OBJECT_TYPE_LAG, next_lag_id++, lag_id);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Cannot create a LAG OID\n");
		return status;
	}
	printf("CREATE LAG: 0x%lX\n", *lag_id);
	return SAI_STATUS_SUCCESS;
}

sai_status_t stub_remove_lag(
    _In_ sai_object_id_t  lag_id)
{
	sai_status_t status;
	status = stub_object_to_type(lag_id, SAI_OBJECT_TYPE_LAG, NULL);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Invalid LAG OID 0x%lX\n", lag_id);
		return status;
	}
	printf("REMOVE LAG: 0x%lX\n", lag_id);
	return SAI_STATUS_SUCCESS;
}

sai_status_t stub_set_lag_attribute(
    _In_ sai_object_id_t  lag_id,
    _In_ const sai_attribute_t *attr)
{
    return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t stub_get_lag_attribute(
    _In_ sai_object_id_t lag_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
	// char attr_str[MAX_VALUE_STR_LEN];
	// sai_status_t status;
	// status = stub_object_to_type(lag_id, SAI_OBJECT_TYPE_LAG, NULL);
	// if (status != SAI_STATUS_SUCCESS) {
	// 	printf("Invalid LAG OID 0x%lX\n", lag_id);
	// 	return status;
	// }
	// sai_attr_list_to_str(attr_count, attr_list, NULL, MAX_VALUE_STR_LEN, attr_str);
	// printf("GET LAG: 0x%lX, Attributes: %s\n", lag_id, attr_str);
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_create_lag_member(
    _Out_ sai_object_id_t* lag_member_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
	sai_status_t status;
	sai_object_id_t lag_id = SAI_NULL_OBJECT_ID;
	sai_object_id_t target_port_id = SAI_NULL_OBJECT_ID;
	
	static int32_t next_lag_member_id = 1;
	for (uint32_t i = 0; i < attr_count; i++) {
		if (attr_list[i].id == SAI_LAG_MEMBER_ATTR_LAG_ID) {
			lag_id = attr_list[i].value.oid;
		} else if (attr_list[i].id == SAI_LAG_MEMBER_ATTR_PORT_ID) {
			target_port_id = attr_list[i].value.oid;
		}
	}
	if (lag_id == SAI_NULL_OBJECT_ID) {
		printf("LAG ID attribute is missing\n");
		return SAI_STATUS_INVALID_PARAMETER;
	}
	status = stub_create_object(SAI_OBJECT_TYPE_LAG_MEMBER, next_lag_member_id++, lag_member_id);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Cannot create a LAG MEMBER OID\n");
		return status;
	}
	printf("CREATE LAG MEMBER: 0x%lX linked to LAG: 0x%lX (Port: 0x%lX)\n", 
           *lag_member_id, lag_id, target_port_id);
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_remove_lag_member(
    _In_ sai_object_id_t  lag_member_id)
{
	sai_status_t status;

	status = stub_object_to_type(lag_member_id, SAI_OBJECT_TYPE_LAG_MEMBER, NULL);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Invalid LAG MEMBER OID 0x%lX\n", lag_member_id);
		return status;
	}

	printf("REMOVE LAG MEMBER: 0x%lX\n", lag_member_id);

    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_set_lag_member_attribute(
    _In_ sai_object_id_t  lag_member_id,
    _In_ const sai_attribute_t *attr)
{
    return SAI_STATUS_NOT_IMPLEMENTED;
}

sai_status_t stub_get_lag_member_attribute(
    _In_ sai_object_id_t lag_member_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
	// char attr_str[MAX_VALUE_STR_LEN];
	// sai_status_t status;
	// status = stub_object_to_type(lag_member_id, SAI_OBJECT_TYPE_LAG_MEMBER, NULL);
	// if (status != SAI_STATUS_SUCCESS) {
	// 	printf("Invalid LAG MEMBER OID 0x%lX\n", lag_member_id);
	// 	return status;
	// }
	// sai_attr_list_to_str(attr_count, attr_list, NULL, MAX_VALUE_STR_LEN, attr_str);
	// printf("GET LAG MEMBER: 0x%lX, Attributes: %s\n", lag_member_id, attr_str);
    return SAI_STATUS_SUCCESS;
}

const sai_lag_api_t lag_api = {
    stub_create_lag,
    stub_remove_lag,
    stub_set_lag_attribute,
    stub_get_lag_attribute,
    stub_create_lag_member,
    stub_remove_lag_member,
    stub_set_lag_member_attribute,
    stub_get_lag_member_attribute
};