#include "sai.h"
#include "stub_sai.h"
#include "assert.h"
#include <sys/types.h>

#define MAX_NUMBER_OF_LAG_MEMBERS 16
#define MAX_NUMBER_OF_LAGS 5

typedef struct _lag_member_db_entry_t {
    bool            is_used;
    sai_object_id_t port_oid;
    sai_object_id_t lag_oid;
} lag_member_db_entry_t;

typedef struct _lag_db_entry_t {
    bool            is_used;
    sai_object_id_t members_ids[MAX_NUMBER_OF_LAG_MEMBERS];
} lag_db_entry_t;

struct lag_db_t {
    lag_db_entry_t        lags[MAX_NUMBER_OF_LAGS];
    lag_member_db_entry_t members[MAX_NUMBER_OF_LAG_MEMBERS];
} lag_db;

sai_status_t get_lag_member_attribute(_In_ const sai_object_key_t   *key,
                                      _Inout_ sai_attribute_value_t *value,
                                      _In_ uint32_t                  attr_index,
                                      _Inout_ vendor_cache_t        *cache,
                                      void                          *arg)
{
    sai_status_t status;
    uint32_t     db_index;

    assert((SAI_LAG_MEMBER_ATTR_LAG_ID == (int64_t)arg) || (SAI_LAG_MEMBER_ATTR_PORT_ID == (int64_t)arg) || (SAI_OBJECT_TYPE_LAG == (int64_t)arg));

	if ((int64_t)arg != SAI_OBJECT_TYPE_LAG) {
		status = stub_object_to_type(key->object_id, SAI_OBJECT_TYPE_LAG_MEMBER, &db_index);
		if (status != SAI_STATUS_SUCCESS) {
			printf("Cannot get LAG DB index.\n");
			return status;
		}
	} else {
		status = stub_object_to_type(key->object_id, SAI_OBJECT_TYPE_LAG, &db_index);
		if (status != SAI_STATUS_SUCCESS) {
			printf("Cannot get LAG DB index.\n");
			return status;
		}
	}

    switch ((int64_t)arg) {
    case SAI_LAG_MEMBER_ATTR_LAG_ID:
        value->oid = lag_db.members[db_index].lag_oid;
		printf("Get LAG MEMBER attribute LAG_ID: 0x%lX\n", value->oid);
        break;
	case SAI_LAG_MEMBER_ATTR_PORT_ID:
		value->oid = lag_db.members[db_index].port_oid;
		printf("Get LAG MEMBER attribute PORT_ID: 0x%lX\n", value->oid);
		break;
	case SAI_OBJECT_TYPE_LAG:
		uint32_t real_count = 0;
		for (uint32_t i = 0; i < MAX_NUMBER_OF_LAG_MEMBERS; i++) {
			if (lag_db.lags[db_index].members_ids[i] != SAI_NULL_OBJECT_ID) {
				value->objlist.list[value->objlist.count] = lag_db.lags[db_index].members_ids[i];
				value->objlist.count++;
				real_count++;
			}
		}
		printf("Get LAG attribute PORT_LIST: count %d\n", real_count);
		break;
    default:
        printf("Got unexpected attribute ID\n");
        return SAI_STATUS_FAILURE;
    }

    return SAI_STATUS_SUCCESS;
}

static const sai_attribute_entry_t lag_attribs[] = {
    { SAI_LAG_ATTR_PORT_LIST, false, false, false, true,
      "List of ports in LAG", SAI_ATTR_VAL_TYPE_OBJLIST },
    { END_FUNCTIONALITY_ATTRIBS_ID, false, false, false, false,
      "", SAI_ATTR_VAL_TYPE_UNDETERMINED }
};

static const sai_attribute_entry_t lag_member_attribs[] = {
    { SAI_LAG_MEMBER_ATTR_LAG_ID, true, true, false, true,
      "LAG ID", SAI_ATTR_VAL_TYPE_OID }, // 0
    { SAI_LAG_MEMBER_ATTR_PORT_ID, true, true, false, true,
      "PORT ID", SAI_ATTR_VAL_TYPE_OID }, // 1
    { END_FUNCTIONALITY_ATTRIBS_ID, false, false, false, false,
      "", SAI_ATTR_VAL_TYPE_UNDETERMINED }
};

static const sai_vendor_attribute_entry_t lag_member_vendor_attribs[] = {
    { SAI_LAG_MEMBER_ATTR_LAG_ID,
      { true, false, false, true },
      { true, false, false, true },
	  get_lag_member_attribute, (void*) SAI_LAG_MEMBER_ATTR_LAG_ID,
      NULL, NULL },
    { SAI_LAG_MEMBER_ATTR_PORT_ID,
      { true, false, false, true },
      { true, false, false, true },
      get_lag_member_attribute, (void*) SAI_LAG_MEMBER_ATTR_PORT_ID,
      NULL, NULL }
};

static const sai_vendor_attribute_entry_t lag_vendor_attribs[] = {
    { SAI_LAG_ATTR_PORT_LIST,
      { false, false, false, true },
      { false, false, false, true },
      get_lag_member_attribute, (void*) SAI_OBJECT_TYPE_LAG,
      NULL, NULL }
};

sai_status_t stub_create_lag(
    _Out_ sai_object_id_t* lag_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{	
	for (int i = 0; lag_attribs[i].id != END_FUNCTIONALITY_ATTRIBS_ID; i++) {
		if (lag_attribs[i].mandatory_on_create) {
			bool found = false;
			for (uint32_t j = 0; j < attr_count; j++) {
				if (attr_list[j].id == lag_attribs[i].id) {
					found = true;
					break;
				}
			}
			if (!found) {
				printf("Mandatory attribute %d is missing\n", lag_attribs[i].id);
				return SAI_STATUS_INVALID_PARAMETER;
			}
		}
	}
	sai_status_t status;
	status = check_attribs_metadata(attr_count, attr_list, lag_attribs, lag_vendor_attribs, SAI_OPERATION_CREATE);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed attributes check\n");
		return status;
	}
    uint32_t ii = 0;
    for (; ii < MAX_NUMBER_OF_LAGS; ii++) {
        if (!lag_db.lags[ii].is_used) {
        break;
        }
    }
    if (ii == MAX_NUMBER_OF_LAGS) {
        printf("Cannot create LAG: limit is reached\n");
        return SAI_STATUS_FAILURE;
    }

	uint32_t lag_db_id = ii;
	lag_db.lags[lag_db_id].is_used = true;
	status = stub_create_object(SAI_OBJECT_TYPE_LAG, lag_db_id, lag_id);
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
	uint32_t     lag_db_id;
	status = stub_object_to_type(lag_id, SAI_OBJECT_TYPE_LAG, &lag_db_id);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Cannot get LAG DB ID.\n");
		return status;
	}
	if (lag_db.members[lag_db_id].is_used) {
		printf("LAG with ID 0x%lX cannot be removed: it has members.\n", lag_id);
		return SAI_STATUS_OBJECT_IN_USE;
	} else {
		lag_db.lags[lag_db_id].is_used = false;
		memset(lag_db.lags[lag_db_id].members_ids, 0, sizeof(lag_db.lags[lag_db_id].members_ids));
		printf("REMOVE LAG: 0x%lX\n", lag_id);
		return SAI_STATUS_SUCCESS;
	}
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
	const sai_object_key_t key = { .object_id = lag_id };
	return sai_get_attributes(&key, NULL, lag_attribs, lag_vendor_attribs, attr_count, attr_list);
}

sai_status_t stub_create_lag_member(
    _Out_ sai_object_id_t* lag_member_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
	for (int i = 0; lag_member_attribs[i].id != END_FUNCTIONALITY_ATTRIBS_ID; i++) {
		if (lag_member_attribs[i].mandatory_on_create) {
			bool found = false;
			for (uint32_t j = 0; j < attr_count; j++) {
				if (attr_list[j].id == lag_member_attribs[i].id) {
					found = true;
					break;
				}
			}
			if (!found) {
				printf("Mandatory attribute %d is missing\n", lag_member_attribs[i].id);
				return SAI_STATUS_INVALID_PARAMETER;
			}
		}
	}
	sai_status_t status;
	status = check_attribs_metadata(attr_count, attr_list, lag_member_attribs, lag_member_vendor_attribs, SAI_OPERATION_CREATE);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Failed attributes check\n");
		return status;
	}
	uint32_t ii = 0;
	for (; ii < MAX_NUMBER_OF_LAG_MEMBERS; ii++) {
		if (!lag_db.members[ii].is_used) {
			break;
		}
	}
	if (ii == MAX_NUMBER_OF_LAG_MEMBERS) {
		printf("Cannot create LAG MEMBER: limit is reached\n");
		return SAI_STATUS_FAILURE;
	}

	const sai_attribute_value_t *lag_id;
	uint32_t lag_id_idx;
	status = find_attrib_in_list(attr_count, attr_list, SAI_LAG_MEMBER_ATTR_LAG_ID, &lag_id, &lag_id_idx);
	if (status != SAI_STATUS_SUCCESS) {
		printf("LAG_ID attribute not found.\n");
		return status;
	}

	uint32_t port_id_idx;
	const sai_attribute_value_t *port_id;
	status = find_attrib_in_list(attr_count, attr_list, SAI_LAG_MEMBER_ATTR_PORT_ID, &port_id, &port_id_idx);
	if (status != SAI_STATUS_SUCCESS) {
		printf("PORT_ID attribute not found.\n");
		return status;
	}

	uint32_t member_db_id = ii;
	lag_db.members[member_db_id].is_used = true;
	status = stub_create_object(SAI_OBJECT_TYPE_LAG_MEMBER, member_db_id, lag_member_id);
	if (status != SAI_STATUS_SUCCESS) {
		printf("Cannot create a LAG MEMBER OID\n");
		return status;
	}

	lag_db.members[member_db_id].lag_oid = lag_id->oid;
	lag_db.members[member_db_id].port_oid = port_id->oid;

	uint32_t real_id = lag_id->oid;

	if (real_id >= MAX_NUMBER_OF_LAGS || !lag_db.lags[real_id].is_used) {
		printf("Invalid LAG OID 0x%X or there is no LAG with this ID\n", real_id);
		return SAI_STATUS_INVALID_PARAMETER;
	}

	bool added_to_lag = false;
	for (uint32_t i = 0; i < MAX_NUMBER_OF_LAG_MEMBERS; i++) {
		if (lag_db.lags[real_id].members_ids[i] == SAI_NULL_OBJECT_ID) {
			lag_db.lags[real_id].members_ids[i] = *lag_member_id;
			added_to_lag = true;
			break;
		}
	}

	if (!added_to_lag) {
		printf("LAG 0x%X cannot accept more members\n", real_id);
		lag_db.members[member_db_id].is_used = false;
		memset(&lag_db.members[member_db_id], 0, sizeof(lag_db.members[member_db_id]));
		return SAI_STATUS_TABLE_FULL;
	}

	printf("CREATE LAG MEMBER: 0x%lX, Linked to LAG: %lx (Port: %lx)\n", *lag_member_id, lag_id->oid, port_id->oid);
	return SAI_STATUS_SUCCESS;
}

sai_status_t stub_remove_lag_member(
    _In_ sai_object_id_t  lag_member_id)
{
	sai_status_t status;
	uint32_t     member_db_id;
	status = stub_object_to_type(lag_member_id, SAI_OBJECT_TYPE_LAG_MEMBER, &member_db_id);
	if (status != SAI_STATUS_SUCCESS) {
	 	printf("Invalid LAG MEMBER OID 0x%lX\n", lag_member_id);
	 	return status;
	}

	lag_db.members[member_db_id].is_used = false;
	memset(&lag_db.members[member_db_id], 0, sizeof(lag_db.members[member_db_id]));
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
	const sai_object_key_t key = { .object_id = lag_member_id };
	return sai_get_attributes(&key, NULL, lag_member_attribs, lag_member_vendor_attribs, attr_count, attr_list);
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