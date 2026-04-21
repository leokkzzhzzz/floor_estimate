// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from barometer_interfaces:msg/Barometer.idl
// generated code does not contain a copyright notice

#ifndef BAROMETER_INTERFACES__MSG__DETAIL__BAROMETER__BUILDER_HPP_
#define BAROMETER_INTERFACES__MSG__DETAIL__BAROMETER__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "barometer_interfaces/msg/detail/barometer__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace barometer_interfaces
{

namespace msg
{

namespace builder
{

class Init_Barometer_temperature
{
public:
  explicit Init_Barometer_temperature(::barometer_interfaces::msg::Barometer & msg)
  : msg_(msg)
  {}
  ::barometer_interfaces::msg::Barometer temperature(::barometer_interfaces::msg::Barometer::_temperature_type arg)
  {
    msg_.temperature = std::move(arg);
    return std::move(msg_);
  }

private:
  ::barometer_interfaces::msg::Barometer msg_;
};

class Init_Barometer_pressure
{
public:
  explicit Init_Barometer_pressure(::barometer_interfaces::msg::Barometer & msg)
  : msg_(msg)
  {}
  Init_Barometer_temperature pressure(::barometer_interfaces::msg::Barometer::_pressure_type arg)
  {
    msg_.pressure = std::move(arg);
    return Init_Barometer_temperature(msg_);
  }

private:
  ::barometer_interfaces::msg::Barometer msg_;
};

class Init_Barometer_altitude
{
public:
  explicit Init_Barometer_altitude(::barometer_interfaces::msg::Barometer & msg)
  : msg_(msg)
  {}
  Init_Barometer_pressure altitude(::barometer_interfaces::msg::Barometer::_altitude_type arg)
  {
    msg_.altitude = std::move(arg);
    return Init_Barometer_pressure(msg_);
  }

private:
  ::barometer_interfaces::msg::Barometer msg_;
};

class Init_Barometer_header
{
public:
  Init_Barometer_header()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Barometer_altitude header(::barometer_interfaces::msg::Barometer::_header_type arg)
  {
    msg_.header = std::move(arg);
    return Init_Barometer_altitude(msg_);
  }

private:
  ::barometer_interfaces::msg::Barometer msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::barometer_interfaces::msg::Barometer>()
{
  return barometer_interfaces::msg::builder::Init_Barometer_header();
}

}  // namespace barometer_interfaces

#endif  // BAROMETER_INTERFACES__MSG__DETAIL__BAROMETER__BUILDER_HPP_
