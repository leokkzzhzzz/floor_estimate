// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from barometer_interfaces:msg/ZMotion.idl
// generated code does not contain a copyright notice

#ifndef BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__STRUCT_H_
#define BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.h"

/// Struct defined in msg/ZMotion in the package barometer_interfaces.
typedef struct barometer_interfaces__msg__ZMotion
{
  std_msgs__msg__Header header;
  /// m/s
  float vspeed;
  /// m/s^2
  float vacc;
} barometer_interfaces__msg__ZMotion;

// Struct for a sequence of barometer_interfaces__msg__ZMotion.
typedef struct barometer_interfaces__msg__ZMotion__Sequence
{
  barometer_interfaces__msg__ZMotion * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} barometer_interfaces__msg__ZMotion__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__STRUCT_H_
