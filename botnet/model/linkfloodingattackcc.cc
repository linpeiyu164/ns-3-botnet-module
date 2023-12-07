#include "linkfloodingattackcc.h"

#include "ns3/config.h"
#include "ns3/ipv4-address-helper.h"
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
LinkFloodingAttackCC::RouteTraceCallback(std::string config, Ipv4Address route)
{
    NS_LOG_FUNCTION(this << route);
    m_routers[route] += 1;

    if (m_routers[route] > m_max_hops)
    {
        NS_LOG_DEBUG("Updates MX address: " << route << ", MX count: " << m_routers[route]);
        m_max_hops = m_routers[route];
        m_max_address = route;
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
    uint32_t addr = target_address.Get();
    uint8_t* buffer = (uint8_t*)&addr;
    Ptr<Packet> packet = Create<Packet>(buffer, sizeof(buffer));
    socket->Send(packet);
}

} // namespace ns3
