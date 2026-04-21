// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from barometer_interfaces:msg/ZMotion.idl
// generated code does not contain a copyright notice

#ifndef BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__BUILDER_HPP_
#define BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "barometer_interfaces/msg/detail/z_motion__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace barometer_interfaces
{

namespace msg
{

namespace builder
{

class Init_ZMotion_vacc
{
public:
  explicit Init_ZMotion_vacc(::barometer_interfaces::msg::ZMotion & msg)
  : msg_(msg)
  {}
  ::barometer_interfaces::msg::ZMotion vacc(::barometer_interfaces::msg::ZMotion::_vacc_type arg)
  {
    msg_.vacc = std::move(arg);
    return std::move(msg_);
  }

private:
  ::barometer_interfaces::msg::ZMotion msg_;
};

class Init_ZMotion_vspeed
{
public:
  explicit Init_ZMotion_vspeed(::barometer_interfaces::msg::ZMotion & msg)
  : msg_(msg)
  {}
  Init_ZMotion_vacc vspeed(::barometer_interfaces::msg::ZMotion::_vspeed_type arg)
  {
    msg_.vspeed = std::move(arg);
    return Init_ZMotion_vacc(msg_);
  }

private:
  ::barometer_interfaces::msg::ZMotion msg_;
};

class Init_ZMotion_header
{
public:
  Init_ZMotion_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_ZMotion_vspeed header(::barometer_interfaces::msg::ZMotion::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_ZMotion_vspeed(msg_);
  }

private:
  ::barometer_interfaces::msg::ZMotion msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::barometer_interfaces::msg::ZMotion>()
{
  return barometer_interfaces::msg::builder::Init_ZMotion_header();
}

}  // namespace barometer_interfaces

#endif  // BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__BUILDER_HPP_
