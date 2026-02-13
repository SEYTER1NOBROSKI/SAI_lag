#include <stdio.h>
#include <stdlib.h>
#include "sai.h"

const char* test_profile_get_value(
    _In_ sai_switch_profile_id_t profile_id,
    _In_ const char* variable)
{
    return 0;
}

int test_profile_get_next_value(
    _In_ sai_switch_profile_id_t profile_id,
    _Out_ const char** variable,
    _Out_ const char** value)
{
    return -1;
}

const service_method_table_t test_services = {
    test_profile_get_value,
    test_profile_get_next_value
};

int main()
{
	sai_status_t status;
	sai_lag_api_t* lag_api;
	sai_object_id_t lag_id, lag2_id;
	sai_object_id_t member_ids[4];
	sai_attribute_t attrs[2];
	sai_object_id_t *list_buffer;

	status = sai_api_initialize(0, &test_services);
	if (status != SAI_STATUS_SUCCESS) {
		printf("SAI API initialize failed\n");
		return -1;
	}
	status = sai_api_query(SAI_API_LAG, (void**)&lag_api);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to query LAG API, status=%d\n", status);
		return 1;
	}
	status = lag_api->create_lag(&lag_id, 0, NULL);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to create a LAG, status=%d\n", status);
		return 1;
	}
	attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
	attrs[0].value.oid = lag_id;
	attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
	attrs[1].value.oid = 0x100000000001;
	status = lag_api->create_lag_member(&member_ids[0], 2, attrs);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to create a LAG MEMBER, status=%d\n", status);
		return 1;
	}
	attrs[1].value.oid = 0x100000000002;
	status = lag_api->create_lag_member(&member_ids[1], 2, attrs);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to create a LAG MEMBER, status=%d\n", status);
		return 1;
	}
	attrs[0].id = SAI_LAG_ATTR_PORT_LIST;
	attrs[0].value.objlist.count = 2;
	list_buffer = malloc(sizeof(sai_object_id_t) * attrs[0].value.objlist.count);
	attrs[0].value.objlist.list = list_buffer;
	status = lag_api->get_lag_attribute(lag_id, 1, attrs);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to get LAG attribute, status=%d\n", status);
		return 1;
	}
	free(list_buffer);
	status = lag_api->create_lag(&lag2_id, 0, NULL);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to create a LAG, status=%d\n", status);
		return 1;
	}
	attrs[0].value.oid = lag2_id;
	attrs[1].value.oid = 0x100000000003;
	status = lag_api->create_lag_member(&member_ids[2], 2, attrs);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to create a LAG MEMBER, status=%d\n", status);
		return 1;
	}
	attrs[1].value.oid = 0x100000000004;
	status = lag_api->create_lag_member(&member_ids[3], 2, attrs);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to create a LAG MEMBER, status=%d\n", status);
		return 1;
	}
	//removing LAG with members should fail
	status = lag_api->remove_lag(lag_id);
	
	for (int i = 0; i < 4; i++) {
		status = lag_api->remove_lag_member(member_ids[i]);
		if (status != SAI_STATUS_SUCCESS) {
			printf("Failed to remove LAG MEMBER index %d (OID 0x%lx), status=%d\n", 
					i, member_ids[i], status);
			return 1;
		}
	}
	status = lag_api->remove_lag(lag_id);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to remove a LAG, status=%d\n", status);
		return 1;
	}
	status = lag_api->remove_lag(lag2_id);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to remove a LAG, status=%d\n", status);
		return 1;
	}

	//creating member without LAG ID should fail
	status = lag_api->create_lag_member(&member_ids[0], 2, attrs);

	status = lag_api->create_lag(&lag_id, 0, NULL);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to create a LAG, status=%d\n", status);
		return 1;
	}
	
	sai_object_id_t port_list[1];

	attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
	attrs[0].value.oid = lag_id;
	attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
	attrs[1].value.oid = port_list[0];
	status = lag_api->create_lag_member(&member_ids[0], 2, attrs);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to create a LAG MEMBER, status=%d\n", status);
		return 1;
	}

	//creating member without LAG ID should fail
	attrs[0].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
	attrs[0].value.oid = port_list[0];

	status = lag_api->create_lag_member(&member_ids[0], 1, attrs);

	attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
	status = lag_api->get_lag_member_attribute(member_ids[0], 1, attrs);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to get LAG MEMBER attribute, status=%d\n", status);
		return 1;
	}

	attrs[0].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
	status = lag_api->get_lag_member_attribute(member_ids[0], 1, attrs);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to get LAG MEMBER attribute, status=%d\n", status);
		return 1;
	}

	attrs[0].id = SAI_LAG_ATTR_PORT_LIST;
	attrs[0].value.objlist.count = 1;
	list_buffer = malloc(sizeof(sai_object_id_t) * attrs[0].value.objlist.count);
	attrs[0].value.objlist.list = list_buffer;
	status = lag_api->get_lag_attribute(lag_id, 1, attrs);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed to get LAG attribute, status=%d\n", status);
		return 1;
	}
	free(list_buffer);
}