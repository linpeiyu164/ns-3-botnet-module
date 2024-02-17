#include "linkfloodingattack.h"

#include "ns3/config.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/simulator.h"
#include "ns3/tcp-socket-factory.h"
#include <ns3/uinteger.h>

#include <string>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("LinkFloodingAttack");
NS_OBJECT_ENSURE_REGISTERED(LinkFloodingAttack);

LinkFloodingAttack::LinkFloodingAttack()
{
    NS_LOG_FUNCTION(this);
}

LinkFloodingAttack::~LinkFloodingAttack()
{
    NS_LOG_FUNCTION(this);
}

TypeId
LinkFloodingAttack::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::LinkFloodingAttack")
            .SetParent<Application>()
            .AddConstructor<LinkFloodingAttack>()
            .AddAttribute("TargetAddress",
                          "The Ipv4Address of the target link",
                          Ipv4AddressValue(),
                          MakeIpv4AddressAccessor(&LinkFloodingAttack::m_target_address),
                          MakeIpv4AddressChecker())
            .AddAttribute("RemotePort",
                          "Remote port on paired bot",
                          UintegerValue(8000),
                          MakeUintegerAccessor(&LinkFloodingAttack::m_remote_port),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute("PacketSize",
                          "Packet size to send to m_remote_address",
                          UintegerValue(100),
                          MakeUintegerAccessor(&LinkFloodingAttack::m_packet_size),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute("Interval",
                          "Interval between sends",
                          TimeValue(Seconds(1)),
                          MakeTimeAccessor(&LinkFloodingAttack::m_interval),
                          MakeTimeChecker())
            .AddAttribute("WaitForRouteMap",
                          "Time needed to wait for traceroute to populate route map",
                          TimeValue(Seconds(800.0)),
                          MakeTimeAccessor(&LinkFloodingAttack::m_wait_for_traceroute),
                          MakeTimeChecker())
            .AddAttribute("WaitForReceive",
                          "Time needed to wait for traceroute to populate route map",
                          TimeValue(Seconds(1000.0)),
                          MakeTimeAccessor(&LinkFloodingAttack::m_wait_for_receive),
                          MakeTimeChecker())
            .AddAttribute("CCAddress",
                          "The Ipv4Address of the central controller",
                          Ipv4AddressValue(),
                          MakeIpv4AddressAccessor(&LinkFloodingAttack::m_cc_address),
                          MakeIpv4AddressChecker())
            .AddAttribute("CCPort",
                          "Port of central controller",
                          UintegerValue(8080),
                          MakeUintegerAccessor(&LinkFloodingAttack::m_cc_port),
                          MakeUintegerChecker<uint16_t>());
    return tid;
}

TypeId
LinkFloodingAttack::GetInstanceTypeId() const
{
    NS_LOG_FUNCTION(this);
    return LinkFloodingAttack::GetTypeId();
}

void
LinkFloodingAttack::StartApplication()
{
    NS_LOG_FUNCTION(this);

    // Connect to v4traceroute applications
    ConnectToTraceRouteSource();
    m_wait_event = Simulator::Schedule(m_wait_for_traceroute,
                                       &LinkFloodingAttack::CompleteWaitForTraceRoute,
                                       this);
}

void
LinkFloodingAttack::CompleteWaitForTraceRoute()
{
    PrintRouteMap();
    OpenConnectionCC();

    // wait until m_target_address is received, then start attack
    m_wait_event = Simulator::Schedule(m_wait_for_receive, &LinkFloodingAttack::StartAttack, this);
}

void
LinkFloodingAttack::StartAttack()
{
    CalculateRemoteAddress();
    OpenConnections();
    SendPackets();
}

void
LinkFloodingAttack::OpenConnections()
{
    NS_LOG_FUNCTION(this);
    for (auto it = m_remote_addresses.begin(); it != m_remote_addresses.end(); it++)
    {
        Ptr<Socket> new_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
        int ret = new_socket->Bind();
        if (ret < 0)
        {
            NS_LOG_ERROR("Socket bind failed");
            exit(1);
        }

        InetSocketAddress inetSocket = InetSocketAddress(*it, m_remote_port);
        Address address(inetSocket);

        ret = new_socket->Connect(address);
        if (ret < 0)
        {
            NS_LOG_ERROR("Socket connect failed");
            exit(1);
        }
        m_remote_sockets.push_back(new_socket);
        NS_LOG_INFO("Opened socket connections!");
    }
}

void
LinkFloodingAttack::OpenConnectionCC()
{
    NS_LOG_FUNCTION(this);

    m_cc_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
    m_cc_socket->SetRecvCallback(MakeCallback(&LinkFloodingAttack::HandleRead, this));
    int ret = m_cc_socket->Bind();
    if (ret < 0)
    {
        NS_LOG_ERROR("Socket bind failed");
        exit(1);
    }

    InetSocketAddress inetSocket = InetSocketAddress(m_cc_address, m_cc_port);
    Address address(inetSocket);

    ret = m_cc_socket->Connect(address);
    if (ret < 0)
    {
        NS_LOG_ERROR("Socket connect to CC failed");
        exit(1);
    }

    NS_LOG_INFO("Opened socket connection to CC!");
}

void
LinkFloodingAttack::HandleRead(Ptr<Socket> socket)
{
    uint8_t data[32];
    Ptr<Packet> received_pckt = socket->Recv();
    received_pckt->CopyData(data, 32);
    m_target_address.Set(*((uint32_t*)data));
    NS_LOG_INFO("The target address received is: " << m_target_address);
}

void
LinkFloodingAttack::StopApplication()
{
    NS_LOG_FUNCTION(this);
    CancelEvent();
    for (auto it = m_remote_sockets.begin(); it != m_remote_sockets.end(); it++)
    {
        if (*it)
        {
            (*it)->Close();
        }
    }
}

void
LinkFloodingAttack::CancelEvent()
{
    NS_LOG_FUNCTION(this);
    Simulator::Cancel(m_wait_event);
    for (auto it = m_send_events.begin(); it != m_send_events.end(); it++)
    {
        Simulator::Cancel(*it);
    }
}

void
LinkFloodingAttack::CalculateRemoteAddress()
{
    for (auto it = m_routes.begin(); it != m_routes.end(); it++)
    {
        for (auto itt = it->second.begin(); itt != it->second.end(); itt++)
        {
            Ipv4Address address = *itt;
            if (address == m_target_address)
            {
                NS_LOG_INFO("Current Address: "
                            << GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal());
                NS_LOG_INFO("Remote address: " << it->first << " passes through target link: "
                                               << m_target_address);
                m_remote_addresses.push_back(it->first);
                continue;
            }
        }
    }

    if (m_remote_addresses.size() == 0)
    {
        NS_LOG_INFO("Remote address not found for node: " << GetNode()->GetId());
        // when a remote address can't be found, stop the application
        StopApplication();
    }
}

void
LinkFloodingAttack::SendPackets()
{
    for (auto it = m_remote_sockets.begin(); it != m_remote_sockets.end(); it++)
    {
        Ptr<Packet> packet = Create<Packet>(m_packet_size);
        (*it)->Send(packet);
    }
    EventId eventid = Simulator::Schedule(m_interval, &LinkFloodingAttack::SendPackets, this);
    m_send_events.push_back(eventid);
}

uint32_t
LinkFloodingAttack::ContextToApplicationNumber(std::string context)
{
    uint32_t begin = context.find("/ApplicationList");
    begin += 17;
    uint32_t length = context.find("/$ns3::V4TraceRoute") - begin;
    return std::stoi(context.substr(begin, length));
}

void
LinkFloodingAttack::RouteTraceCallback(std::string context, Ipv4Address route)
{
    NS_LOG_FUNCTION(this << context << route);

    uint32_t app_num = ContextToApplicationNumber(context);
    Ipv4Address remote;
    Ipv4AddressValue addressvalue;
    GetNode()->GetApplication(app_num)->GetAttribute("Remote", addressvalue);
    remote = addressvalue.Get();

    // add a new address to the hop vector in our route map
    m_routes[remote].push_back(route);

    NS_LOG_INFO("Source: " << GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal() << ", "
                           << "Dest: " << remote << ", "
                           << "Hop IP: " << route);
}

void
LinkFloodingAttack::ConnectToTraceRouteSource()
{
    uint32_t nodeIndex = GetNode()->GetId();
    std::string path_to_node =
        "/NodeList/" + std::to_string(nodeIndex) + "/ApplicationList/*/$ns3::V4TraceRoute/Route";
    Config::Connect(path_to_node, MakeCallback(&LinkFloodingAttack::RouteTraceCallback, this));
}

void
LinkFloodingAttack::PrintRouteMap()
{
    std::ostringstream map_str;
    for (auto it = m_routes.begin(); it != m_routes.end(); it++)
    {
        map_str << "Remote: " << it->first << "\nRoute: \n";

        for (auto itt = it->second.begin(); itt != it->second.end(); itt++)
        {
            Ipv4Address address = *itt;
            if (itt + 1 != it->second.end())
            {
                map_str << address << ", ";
            }
            else
            {
                map_str << address << "\n";
            }
        }
    }
    NS_LOG_INFO("Table: " << map_str.str());
}
} // namespace ns3