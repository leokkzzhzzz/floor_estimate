// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from barometer_interfaces:msg/ZMotion.idl
// generated code does not contain a copyright notice

#ifndef BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__TRAITS_HPP_
#define BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "barometer_interfaces/msg/detail/z_motion__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'header'
#include "std_msgs/msg/detail/header__traits.hpp"

namespace barometer_interfaces
{

namespace msg
{

inline void to_flow_style_yaml(
  const ZMotion & msg,
  std::ostream & out)
{
  out << "{";
  // member: header
  {
    out << "header: ";
    to_flow_style_yaml(msg.header, out);
    out << ", ";
  }

  // member: vspeed
  {
    out << "vspeed: ";
    rosidl_generator_traits::value_to_yaml(msg.vspeed, out);
    out << ", ";
  }

  // member: vacc
  {
    out << "vacc: ";
    rosidl_generator_traits::value_to_yaml(msg.vacc, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const ZMotion & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: header
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "header:\n";
    to_block_style_yaml(msg.header, out, indentation + 2);
  }

  // member: vspeed
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "vspeed: ";
    rosidl_generator_traits::value_to_yaml(msg.vspeed, out);
    out << "\n";
  }

  // member: vacc
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "vacc: ";
    rosidl_generator_traits::value_to_yaml(msg.vacc, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const ZMotion & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace barometer_interfaces

namespace rosidl_generator_traits
{

[[deprecated("use barometer_interfaces::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const barometer_interfaces::msg::ZMotion & msg,
  std::ostream & out, size_t indentation = 0)
{
  barometer_interfaces::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use barometer_interfaces::msg::to_yaml() instead")]]
inline std::string to_yaml(const barometer_interfaces::msg::ZMotion & msg)
{
  return barometer_interfaces::msg::to_yaml(msg);
}

template<>
inline const char * data_type<barometer_interfaces::msg::ZMotion>()
{
  return "barometer_interfaces::msg::ZMotion";
}

template<>
inline const char * name<barometer_interfaces::msg::ZMotion>()
{
  return "barometer_interfaces/msg/ZMotion";
}

template<>
struct has_fixed_size<barometer_interfaces::msg::ZMotion>
  : std::integral_constant<bool, has_fixed_size<std_msgs::msg::Header>::value> {};

template<>
struct has_bounded_size<barometer_interfaces::msg::ZMotion>
  : std::integral_constant<bool, has_bounded_size<std_msgs::msg::Header>::value> {};

template<>
struct is_message<barometer_interfaces::msg::ZMotion>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // BAROMETER_INTERFACES__MSG__DETAIL__Z_MOTION__TRAITS_HPP_
