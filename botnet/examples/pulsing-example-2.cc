#include "ns3/applications-module.h"
#include "ns3/botnet-helper.h"
#include "ns3/brite-topology-helper.h"
#include "ns3/core-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/pulsingattack-helper.h"
#include "ns3/pulsingattackcc.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("PulsingExample2");

/*Callback for packet sink*/
void
targetRx(Ptr<const Packet> packet, const Address& address)
{
    NS_LOG_INFO("Target received packet of " << packet->GetSize() << " bytes from " << address);
}

int
main(int argc, char* argv[])
{
    LogComponentEnable("PulsingExample2", LOG_ALL);
    LogComponentEnable("PulsingAttackCC", LOG_ALL);
    LogComponentEnable("PulsingAttackBot", LOG_ALL);
    LogComponentEnable("PacketSink", LOG_ALL);
    LogComponentEnable("V4Ping", LOG_ALL);
    LogComponentEnable("Config", LOG_ALL);
    LogComponentEnable("BriteTopologyHelper", LOG_ALL);

    std::string confFile = "contrib/botnet/examples/conf_files/GUI_GEN3.conf";

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

    /* Manually create target network, installed under AS 0 */
    /* Install packetSinkApplication on the target node */

    NodeContainer targetNetwork;
    targetNetwork.Create(12);
    stack.Install(targetNetwork);

    int numLeaf = bth.GetNLeafNodesForAs(0);
    NS_LOG_INFO("Number of leaf nodes in AS 0: " << numLeaf);

    /*
    Network:
    - switch 2nd ring: index = [0, 9]
    - switch 1st ring: index = 10
    - target node: index = 11
    */
    p2p.SetDeviceAttribute("DataRate", StringValue("10Kbps"));
    p2p.SetChannelAttribute("Delay", TimeValue(MilliSeconds(2.0)));

    NodeContainer pairs[21];
    NetDeviceContainer pairContainers[21];
    Ipv4InterfaceContainer interfaceContainers[21];

    if (numLeaf >= 10)
    {
        for (int leafNodeId = 0; leafNodeId < 10; leafNodeId++)
        {
            pairs[leafNodeId] = NodeContainer();
            pairs[leafNodeId].Add(targetNetwork.Get(leafNodeId),
                                  bth.GetLeafNodeForAs(0, leafNodeId));
            pairContainers[leafNodeId] = p2p.Install(pairs[leafNodeId]);
            interfaceContainers[leafNodeId] = address.Assign(pairContainers[leafNodeId]);

            pairs[10 + leafNodeId] = NodeContainer();
            pairs[10 + leafNodeId].Add(targetNetwork.Get(leafNodeId), targetNetwork.Get(10));
            pairContainers[10 + leafNodeId] = p2p.Install(pairs[10 + leafNodeId]);
            interfaceContainers[10 + leafNodeId] = address.Assign(pairContainers[10 + leafNodeId]);
        }
        pairs[20] = NodeContainer();
        pairs[20].Add(targetNetwork.Get(10), targetNetwork.Get(11));
        pairContainers[20] = p2p.Install(pairs[20]);
        interfaceContainers[20] = address.Assign(pairContainers[20]);
    }
    else
    {
        NS_LOG_INFO("Too few leaf nodes, try again");
    }

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    NS_LOG_INFO("Done populating routing tables");

    Ptr<Node> targetNode = targetNetwork.Get(11);
    uint16_t sinkPort = 8081;
    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",
                                      InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
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
    bnh.AddApplication(BotType::BOT,
                       "ns3::PulsingAttackBot"); // for communicating with cc and attacking target

    /*Set attributes for 1st v4ping*/
    bnh.SetAttributeBot(0, "StartTime", TimeValue(Seconds(0.0)));
    bnh.SetAttributeBot(0, "StopTime", TimeValue(Seconds(3.0)));
    bnh.SetAttributeBot(0, "Remote", Ipv4AddressValue(bnh.GetBotMasterAddress(0)));

    uint32_t numInterfaces = targetNetwork.Get(11)->GetObject<Ipv4>()->GetNInterfaces();
    NS_LOG_DEBUG("Number of interfaces on target node: " << numInterfaces);
    Ipv4Address targetAddress =
        targetNetwork.Get(11)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();

    /*Set attributes for 2nd v4ping*/
    bnh.SetAttributeBot(1, "StartTime", TimeValue(Seconds(4.0)));
    bnh.SetAttributeBot(1, "StopTime", TimeValue(Seconds(7.0)));
    bnh.SetAttributeBot(1, "Remote", Ipv4AddressValue(targetAddress));

    /*Set attributes for pulsing applications*/
    bnh.SetAttributeBot(2, "StartTime", TimeValue(Seconds(20.0)));
    bnh.SetAttributeBot(2, "TargetAddress", Ipv4AddressValue(targetAddress));
    bnh.SetAttributeBot(2, "CCAddress", Ipv4AddressValue(bnh.GetBotMasterAddress(0)));

    bnh.SetAttributeCC(0, "StartTime", TimeValue(Seconds(0.0)));

    bnh.InstallApplications();

    p2p.EnablePcap("botnet-example-2", targetNetwork);

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
