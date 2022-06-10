import jsonschema
import json


def validate_json_schema(schema, json_data):
    if schema is None:
        return False, "No json schema provided."
    
    if json_data is None:
        return False, "No json data provided."
    
    try:
        jsonschema.validate(instance=json_data, schema=schema)
    except jsonschema.ValidationError as err:
        return False, err.message
    return True, None


def validate_json_structure(data):
    try:
        json.loads(data)
    except json.JSONDecodeError as error:
        return False, str(error)
    return True, None