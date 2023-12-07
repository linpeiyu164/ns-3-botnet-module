#include "ns3/applications-module.h"
#include "ns3/benignapplication.h"
#include "ns3/botnet-helper.h"
#include "ns3/brite-topology-helper.h"
#include "ns3/core-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/point-to-point-module.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("BenignExample");

/*Callback for packet sink*/
void
targetRx(Ptr<const Packet> packet, const Address& address)
{
    NS_LOG_INFO("Target received packet of " << packet->GetSize() << " bytes from " << address);
}

int
main(int argc, char* argv[])
{
    LogComponentEnable("BenignExample", LOG_ALL);
    LogComponentEnable("BenignApplication", LOG_ALL);
    LogComponentEnable("PacketSink", LOG_ALL);
    LogComponentEnable("Config", LOG_ALL);
    LogComponentEnable("BotnetHelper", LOG_ALL);
    LogComponentEnable("BriteTopologyHelper", LOG_ALL);

    std::string confFile = "contrib/botnet/examples/conf_files/GUI_GEN_small.conf";

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
    targetNetwork.Add(bth.GetLeafNodeForAs(0, numLeaf - 1));

    Ptr<Node> targetNode = targetNetwork.Get(0);
    stack.Install(targetNode);

    p2p.SetChannelAttribute("Delay", TimeValue(MilliSeconds(2.0)));
    p2p.SetDeviceAttribute("DataRate", StringValue("50Mbps"));

    NetDeviceContainer p2pTargetDevices = p2p.Install(targetNetwork);

    Ipv4InterfaceContainer targetNetworkInterfaces;
    targetNetworkInterfaces = address.Assign(p2pTargetDevices);
    NS_LOG_DEBUG("Number of devices on target node: " << targetNode->GetNDevices());

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    NS_LOG_INFO("Done populating routing tables");

    uint16_t sinkPort = 8081;
    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",
                                      InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
    ApplicationContainer sinkApps = packetSinkHelper.Install(targetNode);
    sinkApps.Start(Seconds(0.));
    sinkApps.Stop(Seconds(1200.0));

    Ptr<PacketSink> packetSink = sinkApps.Get(0)->GetObject<PacketSink>();
    packetSink->TraceConnectWithoutContext("Rx", MakeCallback(&targetRx));

    /*Choose from created nodes to be in botnet*/
    BotnetHelper bnh;
    int maxBotsPerAs = 2;
    bnh.CreateBotnet(&bth, maxBotsPerAs, BotnetType::CENTRALIZED, "botnet");

    // Initialize benign nodes
    bnh.CreateBenignNodes(&bth);

    /*Setup apps for nodes*/
    bnh.AddApplication(BotType::BENIGN, "ns3::PacketSink");
    bnh.AddApplication(BotType::BENIGN, "ns3::BenignApplication");

    bnh.SetAttributeBenign(0, "StartTime", TimeValue(Seconds(0.0)));
    bnh.SetAttributeBenign(0, "StopTime", TimeValue(Seconds(1200.0)));
    bnh.SetAttributeBenign(0, "Protocol", StringValue("ns3::TcpSocketFactory"));
    bnh.SetAttributeBenign(0,
                           "Local",
                           AddressValue(InetSocketAddress(Ipv4Address::GetAny(), sinkPort)));

    /*Set attributes for benign application*/
    bnh.SetAttributeBenign(1, "StartTime", TimeValue(Seconds(0.0)));
    bnh.SetAttributeBenign(1, "StopTime", TimeValue(Seconds(1200.0)));
    bnh.SetAttributeBenign(1,
                           "TargetAddress",
                           Ipv4AddressValue(targetNetworkInterfaces.GetAddress(0, 0)));
    bnh.SetAttributeBenign(1, "TargetPort", UintegerValue(sinkPort));
    bnh.SetAttributeBenign(1,
                           "DataRateRandomVariableType",
                           StringValue("ns3::LogNormalRandomVariable"));
    bnh.SetAttributeBenign(1,
                           "PacketSizeRandomVariableType",
                           StringValue("ns3::ConstantRandomVariable"));

    bnh.InstallApplications();

    p2p.EnablePcap("benign-example", targetNetwork.Get(0)->GetId(), 1);

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}