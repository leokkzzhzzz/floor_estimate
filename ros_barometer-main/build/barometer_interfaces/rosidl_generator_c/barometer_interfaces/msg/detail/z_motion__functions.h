// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from barometer_interfaces:msg/ZMotion.idl
// generated code does not contain a copyright notice

#ifndef BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__FUNCTIONS_H_
#define BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/visibility_control.h"
#include "barometer_interfaces/msg/rosidl_generator_c__visibility_control.h"

#include "barometer_interfaces/msg/detail/z_motion__struct.h"

/// Initialize msg/ZMotion message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * barometer_interfaces__msg__ZMotion
 * )) before or use
 * barometer_interfaces__msg__ZMotion__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_barometer_interfaces
bool
barometer_interfaces__msg__ZMotion__init(barometer_interfaces__msg__ZMotion * msg);

/// Finalize msg/ZMotion message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_barometer_interfaces
void
barometer_interfaces__msg__ZMotion__fini(barometer_interfaces__msg__ZMotion * msg);

/// Create msg/ZMotion message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * barometer_interfaces__msg__ZMotion__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_barometer_interfaces
barometer_interfaces__msg__ZMotion *
barometer_interfaces__msg__ZMotion__create();

/// Destroy msg/ZMotion message.
/**
 * It calls
 * barometer_interfaces__msg__ZMotion__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_barometer_interfaces
void
barometer_interfaces__msg__ZMotion__destroy(barometer_interfaces__msg__ZMotion * msg);

/// Check for msg/ZMotion message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_barometer_interfaces
bool
barometer_interfaces__msg__ZMotion__are_equal(const barometer_interfaces__msg__ZMotion * lhs, const barometer_interfaces__msg__ZMotion * rhs);

/// Copy a msg/ZMotion message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_barometer_interfaces
bool
barometer_interfaces__msg__ZMotion__copy(
  const barometer_interfaces__msg__ZMotion * input,
  barometer_interfaces__msg__ZMotion * output);

/// Initialize array of msg/ZMotion messages.
/**
 * It allocates the memory for the number of elements and calls
 * barometer_interfaces__msg__ZMotion__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_barometer_interfaces
bool
barometer_interfaces__msg__ZMotion__Sequence__init(barometer_interfaces__msg__ZMotion__Sequence * array, size_t size);

/// Finalize array of msg/ZMotion messages.
/**
 * It calls
 * barometer_interfaces__msg__ZMotion__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_barometer_interfaces
void
barometer_interfaces__msg__ZMotion__Sequence__fini(barometer_interfaces__msg__ZMotion__Sequence * array);

/// Create array of msg/ZMotion messages.
/**
 * It allocates the memory for the array and calls
 * barometer_interfaces__msg__ZMotion__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_barometer_interfaces
barometer_interfaces__msg__ZMotion__Sequence *
barometer_interfaces__msg__ZMotion__Sequence__create(size_t size);

/// Destroy array of msg/ZMotion messages.
/**
 * It calls
 * barometer_interfaces__msg__ZMotion__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_barometer_interfaces
void
barometer_interfaces__msg__ZMotion__Sequence__destroy(barometer_interfaces__msg__ZMotion__Sequence * array);

/// Check for msg/ZMotion message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_barometer_interfaces
bool
barometer_interfaces__msg__ZMotion__Sequence__are_equal(const barometer_interfaces__msg__ZMotion__Sequence * lhs, const barometer_interfaces__msg__ZMotion__Sequence * rhs);

/// Copy an array of msg/ZMotion messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_barometer_interfaces
bool
barometer_interfaces__msg__ZMotion__Sequence__copy(
  const barometer_interfaces__msg__ZMotion__Sequence * input,
  barometer_interfaces__msg__ZMotion__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__FUNCTIONS_H_
