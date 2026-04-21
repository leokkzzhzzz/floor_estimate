// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from barometer_interfaces:msg/ZMotion.idl
// generated code does not contain a copyright notice

#ifndef BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__STRUCT_HPP_
#define BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__barometer_interfaces__msg__ZMotion __attribute__((deprecated))
#else
# define DEPRECATED__barometer_interfaces__msg__ZMotion __declspec(deprecated)
#endif

namespace barometer_interfaces
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct ZMotion_
{
  using Type = ZMotion_<ContainerAllocator>;

  explicit ZMotion_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->vspeed = 0.0f;
      this->vacc = 0.0f;
    }
  }

  explicit ZMotion_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : header(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->vspeed = 0.0f;
      this->vacc = 0.0f;
    }
  }

  // field types and members
  using _header_type =
    std_msgs::msg::Header_<ContainerAllocator>;
  _header_type header;
  using _vspeed_type =
    float;
  _vspeed_type vspeed;
  using _vacc_type =
    float;
  _vacc_type vacc;

  // setters for named parameter idiom
  Type & set__header(
    const std_msgs::msg::Header_<ContainerAllocator> & _arg)
  {
    this->header = _arg;
    return *this;
  }
  Type & set__vspeed(
    const float & _arg)
  {
    this->vspeed = _arg;
    return *this;
  }
  Type & set__vacc(
    const float & _arg)
  {
    this->vacc = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    barometer_interfaces::msg::ZMotion_<ContainerAllocator> *;
  using ConstRawPtr =
    const barometer_interfaces::msg::ZMotion_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<barometer_interfaces::msg::ZMotion_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<barometer_interfaces::msg::ZMotion_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      barometer_interfaces::msg::ZMotion_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<barometer_interfaces::msg::ZMotion_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      barometer_interfaces::msg::ZMotion_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<barometer_interfaces::msg::ZMotion_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<barometer_interfaces::msg::ZMotion_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<barometer_interfaces::msg::ZMotion_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__barometer_interfaces__msg__ZMotion
    std::shared_ptr<barometer_interfaces::msg::ZMotion_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__barometer_interfaces__msg__ZMotion
    std::shared_ptr<barometer_interfaces::msg::ZMotion_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const ZMotion_ & other) const
  {
    if (this->header != other.header) {
      return false;
    }
    if (this->vspeed != other.vspeed) {
      return false;
    }
    if (this->vacc != other.vacc) {
      return false;
    }
    return true;
  }
  bool operator!=(const ZMotion_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct ZMotion_

// alias to use template instance with default allocator
using ZMotion =
  barometer_interfaces::msg::ZMotion_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace barometer_interfaces

#endif  // BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__STRUCT_HPP_
