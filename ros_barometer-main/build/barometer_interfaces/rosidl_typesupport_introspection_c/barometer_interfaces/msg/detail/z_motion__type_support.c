// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from barometer_interfaces:msg/ZMotion.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "barometer_interfaces/msg/detail/z_motion__rosidl_typesupport_introspection_c.h"
#include "barometer_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "barometer_interfaces/msg/detail/z_motion__functions.h"
#include "barometer_interfaces/msg/detail/z_motion__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void barometer_interfaces__msg__ZMotion__rosidl_typesupport_introspection_c__ZMotion_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  barometer_interfaces__msg__ZMotion__init(message_memory);
}

void barometer_interfaces__msg__ZMotion__rosidl_typesupport_introspection_c__ZMotion_fini_function(void * message_memory)
{
  barometer_interfaces__msg__ZMotion__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember barometer_interfaces__msg__ZMotion__rosidl_typesupport_introspection_c__ZMotion_message_member_array[3] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(barometer_interfaces__msg__ZMotion, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "vspeed",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(barometer_interfaces__msg__ZMotion, vspeed),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "vacc",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(barometer_interfaces__msg__ZMotion, vacc),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers barometer_interfaces__msg__ZMotion__rosidl_typesupport_introspection_c__ZMotion_message_members = {
  "barometer_interfaces__msg",  // message namespace
  "ZMotion",  // message name
  3,  // number of fields
  sizeof(barometer_interfaces__msg__ZMotion),
  barometer_interfaces__msg__ZMotion__rosidl_typesupport_introspection_c__ZMotion_message_member_array,  // message members
  barometer_interfaces__msg__ZMotion__rosidl_typesupport_introspection_c__ZMotion_init_function,  // function to initialize message memory (memory has to be allocated)
  barometer_interfaces__msg__ZMotion__rosidl_typesupport_introspection_c__ZMotion_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t barometer_interfaces__msg__ZMotion__rosidl_typesupport_introspection_c__ZMotion_message_type_support_handle = {
  0,
  &barometer_interfaces__msg__ZMotion__rosidl_typesupport_introspection_c__ZMotion_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_barometer_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, barometer_interfaces, msg, ZMotion)() {
  barometer_interfaces__msg__ZMotion__rosidl_typesupport_introspection_c__ZMotion_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  if (!barometer_interfaces__msg__ZMotion__rosidl_typesupport_introspection_c__ZMotion_message_type_support_handle.typesupport_identifier) {
    barometer_interfaces__msg__ZMotion__rosidl_typesupport_introspection_c__ZMotion_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &barometer_interfaces__msg__ZMotion__rosidl_typesupport_introspection_c__ZMotion_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
