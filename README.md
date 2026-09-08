# g1-state-reader

A C++ node for the **Unitree G1** humanoid robot. Subscribes to the `rt/lowstate`
DDS topic and prints body orientation, joint angles and motor temperatures to the
terminal. Read-only — it never commands the robot or moves anything.

Useful for diagnostics: you can see which joints are loaded, whether motors are
heating up, whether the robot stands level, and what the arms actually do during
a gesture.

## Example output

```
========== G1 state ==========
Body orientation  roll 0.008  pitch 0.002  yaw 3.119

Left leg:
  hip pitch          -0.162 rad   35 C
  hip roll           -0.003 rad   35 C
  hip yaw             0.037 rad   35 C
  knee                0.281 rad   36 C
  ankle pitch        -0.145 rad   44 C
```

## Requirements

- A Unitree G1 with access to the robot's internal network
- [unitree_sdk2](https://github.com/unitreerobotics/unitree_sdk2) — a build that
  includes the `include/unitree/idl/hg/` headers (humanoid support; older copies
  ship `go2` only)
- CMake 3.10+ and a C++17 compiler

## Build

```bash
git clone https://github.com/MEIR-005/g1-joint-state-reader.git
cd g1-joint-state-reader
mkdir build && cd build
cmake ..
make
```

If the SDK lives somewhere other than `~/unitree_sdk2`:

```bash
cmake .. -DUNITREE_SDK=/path/to/unitree_sdk2
```

## Usage

```bash
./g1_state_reader <interface> [group]
```

| Argument | Meaning |
|---|---|
| `interface` | network interface to the robot, usually `eth0` |
| `group` | `all` (default), `legs`, `arms`, `waist` |

Examples:

```bash
./g1_state_reader eth0          # everything
./g1_state_reader eth0 arms     # arms only
./g1_state_reader eth0 legs     # legs only
```

Press `Ctrl+C` to stop.

## Joint map

Indices follow the `JointIndex` enum from the SDK examples:

| Indices | Body part |
|---|---|
| 0–5 | left leg |
| 6–11 | right leg |
| 12–14 | waist |
| 15–21 | left arm |
| 22–28 | right arm |
| 29+ | unused |

## Troubleshooting

**`eth0: does not match an available interface`**

The interface has no address in the robot's subnet. Check and add one:

```bash
ip -br a | grep eth0
sudo ip addr add 192.168.123.164/24 dev eth0
```

**The node starts but no data arrives**

Check the link to the controller:

```bash
ping -c 1 192.168.123.161
```

**DDS ignores the interface you passed**

If `CYCLONEDDS_URI` is set in the environment, it overrides the command-line
argument. Clear it before running:

```bash
unset CYCLONEDDS_URI
```

## License

MIT
