#include "ns3/botnet-helper.h"
#include "ns3/core-module.h"
#include "ns3/brite-topology-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/pulsingattack-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/pulsingattackcc.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("BotnetExample");

/*Callback for packet sink*/
void targetRx(Ptr<const Packet> packet, const Address& address)
{
    NS_LOG_INFO("Target received packet of " << packet->GetSize() << " bytes from " << address);
}

int main(int argc, char* argv[])
{
    LogComponentEnable("BotnetExample", LOG_ALL);
    LogComponentEnable("PulsingAttackCC", LOG_ALL);
    LogComponentEnable("PulsingAttackBot", LOG_ALL);
    LogComponentEnable("PacketSink", LOG_ALL);
    LogComponentEnable("V4Ping", LOG_ALL);
    LogComponentEnable("Config", LOG_ALL);
    LogComponentEnable("BriteTopologyHelper", LOG_ALL);

    std::string confFile = "src/brite/examples/conf_files/GUI_GEN3.conf";

    CommandLine cmd(__FILE__);
    cmd.AddValue("confFile", "BRITE conf file", confFile);
    cmd.Parse(argc, argv);

    BriteTopologyHelper bth(confFile);
    bth.AssignStreams(3);

    PointToPointHelper p2p;

    InternetStackHelper stack;

    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "255.255.255.0");

    bth.BuildBriteTopology(stack);
    bth.AssignIpv4Addresses(address);

    NS_LOG_INFO("Num of AS created " << bth.GetNAs());

    /* Manually create target network, installed under last leaf node of AS 0*/
    /* Install packetSinkApplication on the target node */

    NodeContainer targetNetwork;
    targetNetwork.Create(1);
    int numLeaf = bth.GetNLeafNodesForAs(0);
    targetNetwork.Add(bth.GetLeafNodeForAs(0, numLeaf-1));

    Ptr<Node> targetNode = targetNetwork.Get(0);
    stack.Install(targetNode);

    p2p.SetChannelAttribute("Delay", TimeValue(MilliSeconds(2.0)));
    NetDeviceContainer p2pTargetDevices = p2p.Install(targetNetwork);

    Ipv4InterfaceContainer targetNetworkInterfaces;
    targetNetworkInterfaces = address.Assign(p2pTargetDevices);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    NS_LOG_INFO("Done populating routing tables");

    uint16_t sinkPort = 8081;
    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
    ApplicationContainer sinkApps = packetSinkHelper.Install(targetNode);
    sinkApps.Start(Seconds(0.));

    Ptr<PacketSink> packetSink = sinkApps.Get(0)->GetObject<PacketSink>();
    packetSink->TraceConnectWithoutContext("Rx", MakeCallback(&targetRx));

    /* Choose from created nodes to be in botnet */
    BotnetHelper bnh;
    int maxBotsPerAs = 10;
    bnh.CreateBotnet(&bth, maxBotsPerAs, BotnetType::CENTRALIZED, "mybotnet");

    /*Setup apps for CC*/
    bnh.AddApplication(BotType::CENTRAL_CONTROLLER, "ns3::PulsingAttackCC");

    /*Setup apps for bots*/
    bnh.AddApplication(BotType::BOT, "ns3::V4Ping"); // for pinging botmaster
    bnh.AddApplication(BotType::BOT, "ns3::V4Ping"); // for pinging target
    bnh.AddApplication(BotType::BOT, "ns3::PulsingAttackBot"); // for communicating with cc and attacking target

    /*Set attributes for 1st v4ping*/
    bnh.SetAttributeBot(0, "StartTime", TimeValue(Seconds(0.0)));
    bnh.SetAttributeBot(0, "StopTime", TimeValue(Seconds(3.0)));
    bnh.SetAttributeBot(0, "Remote", Ipv4AddressValue(bnh.GetBotMasterAddress(0)));

    /*Set attributes for 2nd v4ping*/
    bnh.SetAttributeBot(1, "StartTime", TimeValue(Seconds(4.0)));
    bnh.SetAttributeBot(1, "StopTime", TimeValue(Seconds(7.0)));
    bnh.SetAttributeBot(1, "Remote", Ipv4AddressValue(targetNetworkInterfaces.GetAddress(0,0)));

    /*Set attributes for pulsing applications*/
    bnh.SetAttributeBot(2, "StartTime", TimeValue(Seconds(20.0)));
    bnh.SetAttributeBot(2, "TargetAddress", Ipv4AddressValue(targetNetworkInterfaces.GetAddress(0,0)));
    bnh.SetAttributeBot(2, "CCAddress", Ipv4AddressValue(bnh.GetBotMasterAddress(0)));
    bnh.SetAttributeCC(0, "StartTime", TimeValue(Seconds(21.0)));

    bnh.InstallApplications();

    /*Setup trace callback for Rtt*/
    Ptr<PulsingAttackCC> pulsingAttackCC = bnh.m_ccAppContainer.Get(0)->GetObject<PulsingAttackCC>();
    Config::Connect("/NodeList/*/ApplicationList/0/$ns3::V4Ping/Rtt", MakeCallback(&PulsingAttackCC::CCRttTraceCallback));
    Config::Connect("/NodeList/*/ApplicationList/1/$ns3::V4Ping/Rtt", MakeCallback(&PulsingAttackCC::TargetRttTraceCallback));

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
