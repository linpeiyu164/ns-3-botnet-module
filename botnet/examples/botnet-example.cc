#include "ns3/botnet-helper.h"
#include "ns3/core-module.h"
#include "ns3/brite-topology-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/pulsingattack-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/ipv4-address-helper.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("BotnetExample");

int
main(int argc, char* argv[])
{
    LogComponentEnable("BotnetHelper", LOG_ALL);
    // LogComponentEnable("Ipv4AddressHelper", LOG_ALL);
    LogComponentEnable("BotnetExample", LOG_ALL);
    LogComponentEnable("Ipv4Address", LOG_ALL);
    // LogComponentEnable("Simulator", LOG_ALL);
    LogComponentEnable("DefaultSimulatorImpl", LOG_INFO);
    LogComponentEnable("PulsingAttackCC", LOG_ALL);
    LogComponentEnable("PulsingAttackBot", LOG_ALL);

    std::string confFile = "src/brite/examples/conf_files/GUI_GEN3.conf";

    CommandLine cmd(__FILE__);
    cmd.AddValue("confFile", "BRITE conf file", confFile);
    cmd.Parse(argc, argv);

    BriteTopologyHelper bth(confFile);
    bth.AssignStreams(3);

    PointToPointHelper p2p;

    InternetStackHelper stack;

    Ipv4AddressHelper address;
    address.SetBase("10.0.0.0", "255.255.252.0");

    bth.BuildBriteTopology(stack);
    bth.AssignIpv4Addresses(address);

    NS_LOG_INFO("Num of AS created " << bth.GetNAs());

    /* Manually create target network, installed under last leaf node of AS 0*/
    /* Install packetSinkApplication on the target node */

    NodeContainer targetNetwork;
    targetNetwork.Create(1);
    stack.Install(targetNetwork);

    int numLeaf = bth.GetNLeafNodesForAs(0);
    targetNetwork.Add(bth.GetLeafNodeForAs(0, numLeaf-1));

    NetDeviceContainer p2pTargetDevices = p2p.Install(targetNetwork);

    address.SetBase("11.0.0.0", "255.255.252.0");
    Ipv4InterfaceContainer targetNetworkInterfaces;
    targetNetworkInterfaces = address.Assign(p2pTargetDevices);

    uint16_t sinkPort = 8080;
    Address sinkAddress(InetSocketAddress(targetNetworkInterfaces.GetAddress(0), sinkPort));
    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", sinkAddress);
    ApplicationContainer sinkApps = packetSinkHelper.Install(targetNetwork.Get(0));
    sinkApps.Start(Seconds(0));
    sinkApps.Stop(Seconds(10));

    /* Choose from created nodes to be in botnet */
    BotnetHelper bnh;
    int maxBotsPerAs = 10;
    bnh.CreateBotnet(&bth, maxBotsPerAs, BotnetType::CENTRALIZED, "mybotnet");
    bnh.SetupAttack("ns3::PulsingAttackCC", "ns3::PulsingAttackBot");
    bnh.SetAttributeCC("RemoteAddress", AddressValue(targetNetwork.Get(0)->GetDevice(0)->GetAddress()));
    bnh.SetAttributeBot("RemoteAddress", AddressValue(targetNetwork.Get(0)->GetDevice(0)->GetAddress()));

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
