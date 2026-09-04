// State reader node for the Unitree G1 humanoid.
// Subscribes to the rt/lowstate DDS topic and prints body orientation,
// joint angles and motor temperatures. Read-only — never commands the robot.

#include <unitree/robot/channel/channel_subscriber.hpp>
#include <unitree/idl/hg/LowState_.hpp>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace unitree::robot;
using LowState = unitree_hg::msg::dds_::LowState_;

struct Joint {
    int id;
    const char* name;
};

struct Group {
    const char* name;
    std::vector<Joint> joints;
};

// G1 joint map. Indices follow the JointIndex enum from the SDK examples:
// 0-5 left leg, 6-11 right leg, 12-14 waist,
// 15-21 left arm, 22-28 right arm, 29+ unused.
const std::vector<Group> ALL_GROUPS = {
    {"Left leg", {
        {0, "hip pitch"}, {1, "hip roll"}, {2, "hip yaw"},
        {3, "knee"}, {4, "ankle pitch"}, {5, "ankle roll"}}},
    {"Right leg", {
        {6, "hip pitch"}, {7, "hip roll"}, {8, "hip yaw"},
        {9, "knee"}, {10, "ankle pitch"}, {11, "ankle roll"}}},
    {"Waist", {
        {12, "yaw"}, {13, "roll"}, {14, "pitch"}}},
    {"Left arm", {
        {15, "shoulder pitch"}, {16, "shoulder roll"}, {17, "shoulder yaw"},
        {18, "elbow"}, {19, "wrist roll"}, {20, "wrist pitch"},
        {21, "wrist yaw"}}},
    {"Right arm", {
        {22, "shoulder pitch"}, {23, "shoulder roll"}, {24, "shoulder yaw"},
        {25, "elbow"}, {26, "wrist roll"}, {27, "wrist pitch"},
        {28, "wrist yaw"}}},
};

std::string g_filter = "all";

// rt/lowstate publishes at a high rate, so print every Nth message only.
int g_rate = 500;

bool WantGroup(const std::string& name)
{
    if (g_filter == "all")   return true;
    if (g_filter == "legs")  return name.find("leg")   != std::string::npos;
    if (g_filter == "arms")  return name.find("arm")   != std::string::npos;
    if (g_filter == "waist") return name.find("Waist") != std::string::npos;
    return true;
}

void OnMessage(const void* message)
{
    const LowState* msg = static_cast<const LowState*>(message);

    static int counter = 0;
    if (counter++ % g_rate != 0) return;

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "\n========== G1 state ==========" << std::endl;
    std::cout << "Body orientation  roll " << msg->imu_state().rpy()[0]
              << "  pitch " << msg->imu_state().rpy()[1]
              << "  yaw "   << msg->imu_state().rpy()[2] << std::endl;

    for (const auto& group : ALL_GROUPS) {
        if (!WantGroup(group.name)) continue;

        std::cout << "\n" << group.name << ":" << std::endl;
        for (const auto& joint : group.joints) {
            const auto& motor = msg->motor_state()[joint.id];
            std::cout << "  " << std::setw(16) << std::left  << joint.name
                      << std::setw(8)  << std::right << motor.q() << " rad"
                      << "   " << static_cast<int>(motor.temperature()[0]) << " C"
                      << std::endl;
        }
    }
}

void PrintUsage(const char* prog)
{
    std::cout << "Usage: " << prog << " <interface> [group]" << std::endl;
    std::cout << "  interface: network interface to the robot, usually eth0" << std::endl;
    std::cout << "  group:     all (default), legs, arms, waist" << std::endl;
    std::cout << "Example: " << prog << " eth0 arms" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        PrintUsage(argv[0]);
        return -1;
    }
    if (argc >= 3) g_filter = argv[2];

    ChannelFactory::Instance()->Init(0, argv[1]);

    ChannelSubscriberPtr<LowState> subscriber =
        ChannelSubscriberPtr<LowState>(new ChannelSubscriber<LowState>("rt/lowstate"));
    subscriber->InitChannel(OnMessage);

    std::cout << "Reading robot state, group: " << g_filter
              << "  (Ctrl+C to exit)" << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
