# Models

## Overview

-   [Botnet](#botnet)
-   [PulsingAttack Related](#pulsingattack)
    -   [PulsingAttackBot](#pulsingattackbot)
    -   [PulsingAttackCC](#pulsingattackcc)
-   [LinkFloodingAttack Related](#linkfloodingattack-related)
    -   [LinkFloodingAttack](#linkfloodingattack)
    -   [LinkFloodingAttackCC](#linkfloodingattackcc)
-   [Traffic Modeling](#traffic-modeling)
    -   [BenignApplication](#benignapplication)

### Botnet

Botnet contains information of the network nodes. Despite the naming, it includes both infomation regarding the bots and benign nodes. `BotnetHelper` helps initialize the information in `Botnet`. Users do not interact with this model.

## PulsingAttack Related

### PulsingAttackBot

**Attributes**

-   `ReceivePort`: The receiving port. Default value of 8000.
-   `PacketSize`: Packet size of attack packets. Default value of 100.
-   `TargetPort`: The target port that will receive the attack packets. Default value of
    8081\.
-   `TargetAddress`: Target address that will receive the attack packets.
-   `CCAddress`: Central controller address.
-   `Rounds`: The number of attack rounds.
-   `AttackInterval`: The interval between each round of attack.
-   `CCPort`: Central controller port that to communicate with the bot.

### PulsingAttackCC

**Attributes**

-   `ReceivePort`: The receiving port. Default value of 8080.
-   `PacketSize`: Packet size of the command packet from central controller to bots.
-   `RemotePort`: Port of the bot that will receive the command packet.
-   `AttackTime`: The time gap between bot connection and attack. This value is needed
    because we have to wait for all bot connections to be finished.

## LinkFloodingAttack Related

### LinkFloodingAttack

**Attributes**

-   `TargetAddress`: Ipv4 address of target link.
-   `RemotePort`: Remote port on paired bot.
-   `PacketSize`: Packet size to send to the remote address.
-   `Interval`: Interval between sends.
-   `WaitForRouteMap`: Time needed to wait for traceroute to populate route map.
-   `WaitForReceive`: Time needed to wait for traceroute to populate route map.
-   `CCAddress`: The Ipv4 address of the central controller.
-   `CCPort`: Port of central controller.

### LinkFloodingAttackCC

**Attributes**

-   `ListenPort`: Port to listen for bot connections on.
-   `WaitForTraceRoute`: Time to wait for all traceroutes to complete.

## Traffic Modeling

### BenignApplication

**Attributes**

-   `TargetAddress`: Target address of packet sink.
-   `TargetPort`: Target port on packet sink that will receive the attack packets.
-   `PacketSizeRandomVariableType`: Type of random variable for packet size. If the user wants to change the random variable type for the packet size, this value should be changed. Supported types include: `ns3::NormalRandomVariable`, `ns3::LogNormalRandomVariable`, `ConstantRandomVariable`, `UniformRandomVariable`.
-   `PacketSizeRandomVariable`: Packet size random variable stream (bytes).
-   `PacketSizeMean`: Mean value of packet size random variable (bytes).
-   `PacketSizeStdDev`: Standard deviation of packet size random variable (bytes).
-   `DataRateRandomVariableType`: Type of random variable for data rate.
-   `DataRateRandomVariable`: Data rate random variable stream (bit/sec).
-   `DataRateMean`: Mean value of data rate random variable (bit/sec).
-   `DataRateStdDev`: Standard deviation value of data rate random variable (bit/sec).
