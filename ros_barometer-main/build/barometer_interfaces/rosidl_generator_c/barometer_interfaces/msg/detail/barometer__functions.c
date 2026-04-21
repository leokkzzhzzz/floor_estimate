// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from barometer_interfaces:msg/Barometer.idl
// generated code does not contain a copyright notice
#include "barometer_interfaces/msg/detail/barometer__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/detail/header__functions.h"

bool
barometer_interfaces__msg__Barometer__init(barometer_interfaces__msg__Barometer * msg)
{
  if (!msg) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__init(&msg->header)) {
    barometer_interfaces__msg__Barometer__fini(msg);
    return false;
  }
  // altitude
  // pressure
  // temperature
  return true;
}

void
barometer_interfaces__msg__Barometer__fini(barometer_interfaces__msg__Barometer * msg)
{
  if (!msg) {
    return;
  }
  // header
  std_msgs__msg__Header__fini(&msg->header);
  // altitude
  // pressure
  // temperature
}

bool
barometer_interfaces__msg__Barometer__are_equal(const barometer_interfaces__msg__Barometer * lhs, const barometer_interfaces__msg__Barometer * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__are_equal(
      &(lhs->header), &(rhs->header)))
  {
    return false;
  }
  // altitude
  if (lhs->altitude != rhs->altitude) {
    return false;
  }
  // pressure
  if (lhs->pressure != rhs->pressure) {
    return false;
  }
  // temperature
  if (lhs->temperature != rhs->temperature) {
    return false;
  }
  return true;
}

bool
barometer_interfaces__msg__Barometer__copy(
  const barometer_interfaces__msg__Barometer * input,
  barometer_interfaces__msg__Barometer * output)
{
  if (!input || !output) {
    return false;
  }
  // header
  if (!std_msgs__msg__Header__copy(
      &(input->header), &(output->header)))
  {
    return false;
  }
  // altitude
  output->altitude = input->altitude;
  // pressure
  output->pressure = input->pressure;
  // temperature
  output->temperature = input->temperature;
  return true;
}

barometer_interfaces__msg__Barometer *
barometer_interfaces__msg__Barometer__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  barometer_interfaces__msg__Barometer * msg = (barometer_interfaces__msg__Barometer *)allocator.allocate(sizeof(barometer_interfaces__msg__Barometer), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(barometer_interfaces__msg__Barometer));
  bool success = barometer_interfaces__msg__Barometer__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
barometer_interfaces__msg__Barometer__destroy(barometer_interfaces__msg__Barometer * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    barometer_interfaces__msg__Barometer__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
barometer_interfaces__msg__Barometer__Sequence__init(barometer_interfaces__msg__Barometer__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  barometer_interfaces__msg__Barometer * data = NULL;

  if (size) {
    data = (barometer_interfaces__msg__Barometer *)allocator.zero_allocate(size, sizeof(barometer_interfaces__msg__Barometer), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = barometer_interfaces__msg__Barometer__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        barometer_interfaces__msg__Barometer__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
barometer_interfaces__msg__Barometer__Sequence__fini(barometer_interfaces__msg__Barometer__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      barometer_interfaces__msg__Barometer__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

barometer_interfaces__msg__Barometer__Sequence *
barometer_interfaces__msg__Barometer__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  barometer_interfaces__msg__Barometer__Sequence * array = (barometer_interfaces__msg__Barometer__Sequence *)allocator.allocate(sizeof(barometer_interfaces__msg__Barometer__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = barometer_interfaces__msg__Barometer__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
barometer_interfaces__msg__Barometer__Sequence__destroy(barometer_interfaces__msg__Barometer__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    barometer_interfaces__msg__Barometer__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
barometer_interfaces__msg__Barometer__Sequence__are_equal(const barometer_interfaces__msg__Barometer__Sequence * lhs, const barometer_interfaces__msg__Barometer__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!barometer_interfaces__msg__Barometer__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
barometer_interfaces__msg__Barometer__Sequence__copy(
  const barometer_interfaces__msg__Barometer__Sequence * input,
  barometer_interfaces__msg__Barometer__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(barometer_interfaces__msg__Barometer);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    barometer_interfaces__msg__Barometer * data =
      (barometer_interfaces__msg__Barometer *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!barometer_interfaces__msg__Barometer__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          barometer_interfaces__msg__Barometer__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!barometer_interfaces__msg__Barometer__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
