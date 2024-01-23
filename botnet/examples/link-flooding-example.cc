#include "ns3/applications-module.h"
#include "ns3/botnet-helper.h"
#include "ns3/brite-topology-helper.h"
#include "ns3/core-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/linkfloodingattack.h"
#include "ns3/linkfloodingattackcc.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/v4traceroute-helper.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("LinkFloodingExample");

int
main(int argc, char* argv[])
{
    LogComponentEnable("LinkFloodingExample", LOG_ALL);
    LogComponentEnable("BriteTopologyHelper", LOG_ALL);
    LogComponentEnable("LinkFloodingAttack", LOG_ALL);
    LogComponentEnable("LinkFloodingAttackCC", LOG_ALL);
    LogComponentEnable("PacketSink", LOG_ALL);

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

    NodeContainer targetNetwork;
    targetNetwork.Create(1);
    int numLeaf = bth.GetNLeafNodesForAs(0);
    targetNetwork.Add(bth.GetLeafNodeForAs(0, numLeaf - 1));

    Ptr<Node> targetNode = targetNetwork.Get(0);
    stack.Install(targetNode);

    p2p.SetChannelAttribute("Delay", TimeValue(MilliSeconds(2.0)));
    p2p.SetDeviceAttribute("DataRate", StringValue("10Kbps"));
    NetDeviceContainer p2pTargetDevices = p2p.Install(targetNetwork);

    Ipv4InterfaceContainer targetNetworkInterfaces;
    targetNetworkInterfaces = address.Assign(p2pTargetDevices);
    NS_LOG_DEBUG("Number of devices on target node: " << targetNode->GetNDevices());

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    /* Choose from created nodes to be in botnet */
    BotnetHelper bnh;
    int maxBotsPerAs = 1;
    bnh.CreateBotnet(&bth, maxBotsPerAs, BotnetType::CENTRALIZED, "mybotnet");
    bnh.CreateBenignNodes(&bth);

    uint32_t num_bot_nodes = bnh.GetNBots();

    // add traceroute applications
    NS_LOG_UNCOND("Num bots: " << num_bot_nodes);

    for (uint32_t i = 0; i < num_bot_nodes; i++)
    {
        bnh.AddApplication(BotType::BOT, "ns3::V4TraceRoute");
        Ipv4Address address = bnh.GetBot(i)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
        bnh.SetAttributeBot(i, "Remote", Ipv4AddressValue(address));

        // schedule the traceroutes so they don't congest each other
        // congestion causes duplicate traceroute IP values in the route map
        bnh.SetAttributeBot(i, "StartTime", TimeValue(Seconds(double(10 * i))));
    }

    // add linkfloodingattack applications
    uint32_t lfattack_index = num_bot_nodes;
    bnh.AddApplication(BotType::BOT, "ns3::LinkFloodingAttack");

    // set the targeted address
    Time traceroute_wait_time = Seconds(2000);
    Time target_address_wait_time = Seconds(1000);
    Time stop_time = Seconds(5000);

    bnh.SetAttributeBot(lfattack_index, "WaitForRouteMap", TimeValue(traceroute_wait_time));
    bnh.SetAttributeBot(lfattack_index, "WaitForReceive", TimeValue(target_address_wait_time));
    bnh.SetAttributeBot(lfattack_index, "StopTime", TimeValue(stop_time));
    bnh.SetAttributeBot(lfattack_index, "CCAddress", Ipv4AddressValue(bnh.GetBotMasterAddress(0)));

    // add packetsink applications on bots
    uint32_t packetsink_index = lfattack_index + 1;
    bnh.AddApplication(BotType::BOT, "ns3::PacketSink");
    bnh.SetAttributeBot(packetsink_index, "StartTime", TimeValue(target_address_wait_time));
    bnh.SetAttributeBot(packetsink_index, "StopTime", TimeValue(stop_time));
    bnh.SetAttributeBot(packetsink_index, "Protocol", StringValue("ns3::TcpSocketFactory"));
    bnh.SetAttributeBot(packetsink_index,
                        "Local",
                        AddressValue(InetSocketAddress(Ipv4Address::GetAny(), 8000)));

    // add cc applications on cc
    bnh.AddApplication(BotType::CENTRAL_CONTROLLER, "ns3::LinkFloodingAttackCC");
    bnh.SetAttributeCC(0, "WaitForTraceRoute", TimeValue(traceroute_wait_time));
    bnh.SetAttributeCC(0, "StopTime", TimeValue(stop_time));
    bnh.InstallApplications();

    NS_LOG_INFO("Bot master Address: " << bnh.GetBotMasterAddress(0)
                                       << ", Bot master Node Id: " << bnh.GetBotMasterNodeId());
    AsciiTraceHelper ascii;
    p2p.EnableAsciiAll(ascii.CreateFileStream("link_flooding.tr"));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
