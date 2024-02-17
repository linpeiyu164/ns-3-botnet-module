#include "linkfloodingattackcc.h"

#include "ns3/config.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/node-list.h"
#include "ns3/point-to-point-module.h"
#include "ns3/simulator.h"
#include "ns3/tcp-socket-factory.h"
#include <ns3/uinteger.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("LinkFloodingAttackCC");
NS_OBJECT_ENSURE_REGISTERED(LinkFloodingAttackCC);

LinkFloodingAttackCC::LinkFloodingAttackCC()
{
    NS_LOG_FUNCTION(this);
    m_max_hops = 0;
    m_critical_node_id = -1;
}

LinkFloodingAttackCC::~LinkFloodingAttackCC()
{
    NS_LOG_FUNCTION(this);
}

TypeId
LinkFloodingAttackCC::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::LinkFloodingAttackCC")
            .SetParent<Application>()
            .AddConstructor<LinkFloodingAttackCC>()
            .AddAttribute("ListenPort",
                          "Port to listen for bot connections on",
                          UintegerValue(8080),
                          MakeUintegerAccessor(&LinkFloodingAttackCC::m_listen_port),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute("WaitForTraceRoute",
                          "Time to wait for all traceroutes to complete",
                          TimeValue(Seconds(1000.0)),
                          MakeTimeAccessor(&LinkFloodingAttackCC::m_wait_for_traceroute),
                          MakeTimeChecker());
    return tid;
}

TypeId
LinkFloodingAttackCC::GetInstanceTypeId() const
{
    NS_LOG_FUNCTION(this);
    return LinkFloodingAttackCC::GetTypeId();
}

void
LinkFloodingAttackCC::StartApplication()
{
    NS_LOG_FUNCTION(this);
    StartListening();
    ConnectToTraceRouteSource();
}

void
LinkFloodingAttackCC::StopApplication()
{
    NS_LOG_FUNCTION(this);
}

void
LinkFloodingAttackCC::RouteTraceCallback(std::string config, Ipv4Address addr)
{
    NS_LOG_FUNCTION(this << addr);
    m_routers[addr] += 1;

    if (m_routers[addr] > m_max_hops)
    {
        NS_LOG_DEBUG("Updates MX address: " << addr << ", MX count: " << m_routers[addr]);
        m_max_hops = m_routers[addr];
        m_max_address = addr;
    }
}

void
LinkFloodingAttackCC::ConnectToTraceRouteSource()
{
    NS_LOG_FUNCTION(this);
    std::string path_to_node = "/NodeList/*/ApplicationList/*/$ns3::V4TraceRoute/Route";
    Config::Connect(path_to_node, MakeCallback(&LinkFloodingAttackCC::RouteTraceCallback, this));
}

Ipv4Address
LinkFloodingAttackCC::GetMaxAddress()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("Max address: " << m_max_address);
    return m_max_address;
}

int
LinkFloodingAttackCC::GetMaxCount()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("Max hop count: " << m_max_hops);
    return m_max_hops;
}

int32_t
LinkFloodingAttackCC::GetCriticalNodeIndex()
{
    Ipv4Address maxaddress = GetMaxAddress();
    for (NodeList::Iterator it = NodeList::Begin(); it != NodeList::End(); it++)
    {
        Ptr<Node> node = *it;
        Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();

        int32_t interface = ipv4->GetInterfaceForAddress(maxaddress);
        if (interface == -1)
        {
            // max address is not found on this node
            continue;
        }
        else
        {
            m_critical_node_id = (int32_t)node->GetId();
            NS_LOG_INFO("Max address is found on node: " << node->GetId());
            // max address is found on this node
        }
    }
    NS_LOG_INFO("Critical Link NodeId: " << m_critical_node_id);
    return m_critical_node_id;
}

void
LinkFloodingAttackCC::StartListening()
{
    NS_LOG_FUNCTION(this);
    m_listen_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
    m_listen_socket->SetAcceptCallback(MakeNullCallback<bool, Ptr<Socket>, const Address&>(),
                                       MakeCallback(&LinkFloodingAttackCC::HandleAccept, this));
    if (m_listen_socket->Bind(InetSocketAddress(Ipv4Address::GetAny(), m_listen_port)) == -1)
    {
        NS_LOG_ERROR("Socket bind failed");
    }
    else
    {
        m_listen_socket->Listen();
    }
}

/* Handle connection requests */
void
LinkFloodingAttackCC::HandleAccept(Ptr<Socket> socket, const Address& address)
{
    NS_LOG_FUNCTION(this << socket << address);
    Ipv4Address ipv4 = InetSocketAddress::ConvertFrom(address).GetIpv4();
    NS_LOG_INFO("Accepted connection from address: " << ipv4);

    // send out the target link address to the connected bots

    Ipv4Address target_address = GetMaxAddress();

    while (m_critical_node_id == -1)
    {
        target_address = GetMaxAddress();
        GetCriticalNodeIndex();
    }

    EnablePcapForCriticalNode();

    uint32_t addr = target_address.Get();
    uint8_t* buffer = (uint8_t*)&addr;
    Ptr<Packet> packet = Create<Packet>(buffer, sizeof(buffer));
    socket->Send(packet);
}

void
LinkFloodingAttackCC::EnablePcapForCriticalNode()
{
    for (NodeList::Iterator it = NodeList::Begin(); it != NodeList::End(); it++)
    {
        Ptr<Node> node = *it;
        if ((int32_t)node->GetId() == m_critical_node_id)
        {
            PointToPointHelper p2p;
            p2p.EnablePcap("link-flooding", m_critical_node_id, 1);
            break;
        }
    }
}

} // namespace ns3
