/*
 * ros_helpers.h
 *
 *  Created on: Apr 8, 2016
 *      Author: winklera
 */

#ifndef INCLUDE_XPP_MSGS_ROS_HELPERS_H_
#define INCLUDE_XPP_MSGS_ROS_HELPERS_H_

#include <ros/ros.h>

#include <xpp_msgs/StateLin3d.h>
#include <xpp_msgs/Foothold.h>
#include <xpp_msgs/RobotStateTrajectoryCartesian.h>

#include <xpp/utils/geometric_structs.h>
#include <xpp/hyq/foothold.h>
#include <xpp/hyq/hyq_state.h>
#include <xpp/hyq/leg_data_map.h>

namespace xpp {
namespace ros {

/**
 * Ros specific functions that only depend on utils folder, ros messages,
 * ros services.
 */
struct RosHelpers {

typedef Eigen::Vector3d Vector3d;
typedef xpp::utils::Point3d State;
typedef xpp::hyq::Foothold Foothold;
typedef xpp::hyq::LegID LegID;

using FootholdMsg       = xpp_msgs::Foothold;
using StateLin3dMsg     = xpp_msgs::StateLin3d;
using RobotStateTrajMsg = xpp_msgs::RobotStateTrajectoryCartesian;
using RobotStateMsg     = xpp_msgs::RobotStateCartesianStamped;
using BaseStateMsg      = xpp_msgs::BaseState;


static double GetDoubleFromServer(const std::string& ros_param_name) {
  double val;
  if(!::ros::param::get(ros_param_name,val))
    throw ::ros::Exception("GetDoubleFromServer: Couldn't read parameter: " + ros_param_name);
  return val;
}

static double GetBoolFromServer(const std::string& ros_param_name) {
  bool val;
  if(!::ros::param::get(ros_param_name,val))
    throw ::ros::Exception("GetBoolFromServer: Couldn't read parameter: " + ros_param_name);
  return val;
}

static State
RosToXpp(const StateLin3dMsg& ros)
{
  State point;
  point.p.x() = ros.pos.x;
  point.p.y() = ros.pos.y;
  point.p.z() = ros.pos.z;

  point.v.x() = ros.vel.x;
  point.v.y() = ros.vel.y;
  point.v.z() = ros.vel.z;

  point.a.x() = ros.acc.x;
  point.a.y() = ros.acc.y;
  point.a.z() = ros.acc.z;

  return point;
}

static StateLin3dMsg
XppToRos(const State& xpp)
{
  StateLin3dMsg ros;
  ros.pos.x = xpp.p.x();
  ros.pos.y = xpp.p.y();
  ros.pos.z = xpp.p.z();

  ros.vel.x = xpp.v.x();
  ros.vel.y = xpp.v.y();
  ros.vel.z = xpp.v.z();

  ros.acc.x = xpp.a.x();
  ros.acc.y = xpp.a.y();
  ros.acc.z = xpp.a.z();

  return ros;
}

template<typename T>
static Vector3d
RosToXpp(const T& ros)
{
  Vector3d vec;
  vec << ros.x, ros.y, ros.z;
  return vec;
}

template<typename T>
static T
XppToRos(const Vector3d& xpp)
{
  T ros;
  ros.x = xpp.x();
  ros.y = xpp.y();
  ros.z = xpp.z();

  return ros;
}

static Foothold
RosToXpp(const FootholdMsg& ros)
{
  Foothold f;

  assert(0 <= ros.leg && ros.leg < xpp::hyq::_LEGS_COUNT); //integer cannot be mapped to a LegID
  f.leg = static_cast<LegID>(ros.leg);
  f.p   = RosToXpp(ros.p);
  return f;
}

static FootholdMsg
XppToRos(const xpp::hyq::Foothold& xpp)
{
  FootholdMsg ros;
  ros.p.x = xpp.p.x();
  ros.p.y = xpp.p.y();
  ros.p.z = xpp.p.z();
  ros.leg = static_cast<int>(xpp.leg);

  return ros;
}


static std::vector<FootholdMsg>
XppToRos(const std::vector<xpp::hyq::Foothold>& xpp)
{
  int n_footholds = xpp.size();
  std::vector<FootholdMsg> ros_vec(n_footholds);

  for (int i=0; i<n_footholds; ++i) {
    ros_vec.at(i) = XppToRos(xpp.at(i));
  }

  return ros_vec;
}

static std::vector<xpp::hyq::Foothold>
RosToXpp(const std::vector<FootholdMsg>& ros)
{
  std::vector<xpp::hyq::Foothold> xpp_vec(ros.size());

  for (uint i=0; i<ros.size(); ++i) {
    xpp_vec.at(i) = RosToXpp(ros.at(i));
  }

  return xpp_vec;
}

static Eigen::Quaterniond
RosToXpp(const geometry_msgs::Quaternion ros)
{
  Eigen::Quaterniond xpp;
  xpp.w() = ros.w;
  xpp.x() = ros.x;
  xpp.y() = ros.y;
  xpp.z() = ros.z;

  return xpp;
}

static geometry_msgs::Quaternion
XppToRos(const Eigen::Quaterniond xpp)
{
  geometry_msgs::Quaternion ros;
  ros.w = xpp.w();
  ros.x = xpp.x();
  ros.y = xpp.y();
  ros.z = xpp.z();

  return ros;
}

static BaseStateMsg
XppToRos(const xpp::utils::Pose& xpp)
{
  BaseStateMsg msg;

  msg.pose.position = XppToRos<geometry_msgs::Point>(xpp.pos.p);
  msg.twist.linear  = XppToRos<geometry_msgs::Vector3>(xpp.pos.v);
  msg.accel.linear  = XppToRos<geometry_msgs::Vector3>(xpp.pos.a);

  msg.pose.orientation = XppToRos(xpp.ori.q);
  msg.twist.angular    = XppToRos<geometry_msgs::Vector3>(xpp.ori.v);
  msg.accel.angular    = XppToRos<geometry_msgs::Vector3>(xpp.ori.a);

  return msg;
}

static xpp::utils::Pose
RosToXpp(const BaseStateMsg& ros)
{
  xpp::utils::Pose xpp;

  xpp.pos.p = RosToXpp(ros.pose.position);
  xpp.pos.v = RosToXpp(ros.twist.linear);
  xpp.pos.a = RosToXpp(ros.accel.linear);

  xpp.ori.q = RosToXpp(ros.pose.orientation);
  xpp.ori.v = RosToXpp(ros.twist.angular);
  xpp.ori.a = RosToXpp(ros.accel.angular);

  return xpp;
}

static RobotStateMsg
XppToRos(const xpp::hyq::HyqStateStamped& xpp)
{
  RobotStateMsg ros;

  ros.state.base = XppToRos(xpp.base_);
  ros.time       = xpp.t_;

  for (int leg=0; leg<4; ++leg) {
    ros.state.ee_in_contact[leg] = !xpp.swingleg_[leg];
    ros.state.endeffectors[leg]  = XppToRos(xpp.feet_[leg]);
  }

  return ros;
}

static xpp::hyq::HyqStateStamped
RosToXpp(const RobotStateMsg& ros)
{
  xpp::hyq::HyqStateStamped xpp;

  xpp.base_ = RosToXpp(ros.state.base);
  xpp.t_    = ros.time;

  for (int leg=0; leg<4; ++leg) {
    xpp.swingleg_[leg] = !ros.state.ee_in_contact[leg];
    xpp.feet_[leg]     = RosToXpp(ros.state.endeffectors[leg]);
  }

  return xpp;
}

static RobotStateTrajMsg
XppToRos(const std::vector<xpp::hyq::HyqStateStamped>& xpp)
{
  RobotStateTrajMsg msg;

  for (const auto& state : xpp)
    msg.states.push_back(XppToRos(state));

  return msg;
}

static std::vector<xpp::hyq::HyqStateStamped>
RosToXpp(const RobotStateTrajMsg& ros)
{
  std::vector<xpp::hyq::HyqStateStamped> xpp;

  for (const auto& state : ros.states)
    xpp.push_back(RosToXpp(state));

  return xpp;
}

}; // RosHelpers

} // namespace ros
} // namespace xpp

#endif /* INCLUDE_XPP_MSGS_ROS_HELPERS_H_ */
