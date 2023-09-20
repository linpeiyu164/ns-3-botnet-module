#include "pulsingattackbot.h"

#include "ns3/ipv4-address-helper.h"
#include "ns3/simulator.h"
#include "ns3/tcp-socket-factory.h"
#include <ns3/type-name.h>
#include <ns3/uinteger.h>

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("PulsingAttackBot");
NS_OBJECT_ENSURE_REGISTERED(PulsingAttackBot);

TypeId
PulsingAttackBot::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::PulsingAttackBot")
            .SetParent<Application>()
            .AddConstructor<PulsingAttackBot>()
            .AddAttribute("ReceivePort",
                          "receiving port",
                          UintegerValue(8000),
                          MakeUintegerAccessor(&PulsingAttackBot::m_recv_port),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute("PacketSize",
                          "Packet size of attacking packets",
                          UintegerValue(100),
                          MakeUintegerAccessor(&PulsingAttackBot::m_packet_size),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute("TargetPort",
                          "Target port that will receive the attack packets",
                          UintegerValue(8081),
                          MakeUintegerAccessor(&PulsingAttackBot::m_target_port),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute("TargetAddress",
                          "Target address that will receive the attack packets",
                          Ipv4AddressValue(),
                          MakeIpv4AddressAccessor(&PulsingAttackBot::m_target_address),
                          MakeIpv4AddressChecker())
            .AddAttribute("CCAddress",
                          "CC address",
                          Ipv4AddressValue(),
                          MakeIpv4AddressAccessor(&PulsingAttackBot::m_cc_address),
                          MakeIpv4AddressChecker())
            .AddAttribute("Rounds",
                          "Number of attack rounds",
                          UintegerValue(3),
                          MakeUintegerAccessor(&PulsingAttackBot::m_rounds),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute("AttackInterval",
                          "AttackInterval",
                          TimeValue(Seconds(100.0)),
                          MakeTimeAccessor(&PulsingAttackBot::m_attack_interval),
                          MakeTimeChecker())
            .AddAttribute("CCPort",
                          "CC Port",
                          UintegerValue(8080),
                          MakeUintegerAccessor(&PulsingAttackBot::m_cc_port),
                          MakeUintegerChecker<uint16_t>());
    return tid;
}

TypeId
PulsingAttackBot::GetInstanceTypeId() const
{
    NS_LOG_FUNCTION(this);
    return PulsingAttackBot::GetTypeId();
}

PulsingAttackBot::PulsingAttackBot()
{
    NS_LOG_FUNCTION(this);
}

PulsingAttackBot::~PulsingAttackBot()
{
    NS_LOG_FUNCTION(this);
}

void
PulsingAttackBot::StartApplication()
{
    NS_LOG_FUNCTION(this);
    // create sockets
    m_target_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
    m_recv_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
    Ipv4Address ipv4 = GetNode()->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
    InetSocketAddress addr = InetSocketAddress(ipv4, m_recv_port);

    m_recv_socket->SetAcceptCallback(MakeNullCallback<bool, Ptr<Socket>, const Address&>(),
                                     MakeCallback(&PulsingAttackBot::HandleAccept, this));

    int ret = m_recv_socket->Bind(addr);
    if (ret < 0)
    {
        NS_LOG_DEBUG("Binding was unsuccessful");
    }
    else
    {
        NS_LOG_DEBUG("Successfully bounded recv socket");
    }
    m_recv_socket->Listen();
}

void
PulsingAttackBot::HandleAccept(Ptr<Socket> socket, const Address& address)
{
    NS_LOG_FUNCTION(this << socket << address);
    socket->SetRecvCallback(MakeCallback(&PulsingAttackBot::ReceivePacketCC, this, socket));
}

void
PulsingAttackBot::StopApplication()
{
    NS_LOG_FUNCTION(this);
    m_target_socket->Close();
    m_cc_socket->Close();
}

void
PulsingAttackBot::OpenConnection(Ptr<Socket> socket, Ipv4Address addr, uint16_t port)
{
    NS_LOG_FUNCTION(this);
    int ret = socket->Bind();
    if (ret < 0)
    {
        NS_LOG_ERROR("Error: Failed to bind socket");
    }
    else
    {
        NS_LOG_INFO("Socket bound");
    }

    if (Ipv4Address::IsMatchingType(addr))
    {
        Ipv4Address ipv4 = Ipv4Address::ConvertFrom(addr);
        InetSocketAddress inetSocket = InetSocketAddress(ipv4, port);
        Address address(inetSocket);

        ret = socket->Connect(address);
        if (ret < 0)
        {
            NS_LOG_ERROR("Error: Connection failed");
        }
        else
        {
            NS_LOG_INFO("Bot connected");
        }
    }
    else
    {
        NS_LOG_ERROR("Error: Address incompatible with Ipv4");
    }
}

void
PulsingAttackBot::SendPacketCC(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this);
    Ptr<Packet> packet = Create<Packet>(m_packet_size);
    socket->Send(packet);
}

void
PulsingAttackBot::SendPacketTarget()
{
    NS_LOG_FUNCTION(this);
    Ptr<Packet> packet = Create<Packet>(m_packet_size);
    int ret = m_target_socket->Send(packet);
    if (ret < 0)
    {
        NS_LOG_DEBUG("Error while sending packet to target");
    }
    else
    {
        NS_LOG_DEBUG("Packet sent to target");
    }
    // schedule the next attack round
    m_rounds--;
    if (m_rounds > 0)
    {
        Simulator::Schedule(m_attack_interval, &PulsingAttackBot::SendPacketTarget, this);
    }
}

void
PulsingAttackBot::ReceivePacketCC(Ptr<Socket> socket)
{
    NS_LOG_FUNCTION(this);
    // accept cc command
    Ptr<Packet> packet;
    while ((packet = socket->Recv()))
    {
        if (packet->GetSize() == 0)
        {
            break;
        }
        else
        {
            NS_LOG_INFO("Packet received from CC!");
        }
        // TODO: Schedule SendPacket based on received information
        // Right now it just sends packet immediately
    }

    if (m_rounds > 0)
    {
        int ret = m_target_socket->Bind(InetSocketAddress(Ipv4Address::GetAny(), m_recv_port));
        if (ret < 0)
        {
            NS_LOG_DEBUG("Binding of target socket failed");
        }
        else
        {
            NS_LOG_DEBUG("Binding of target socket successful");
        }

        if (Ipv4Address::IsMatchingType(m_target_address))
        {
            InetSocketAddress addr = InetSocketAddress(m_target_address, m_target_port);
            ret = m_target_socket->Connect(addr);

            if (ret < 0)
            {
                NS_LOG_DEBUG("Connection to target failed");
            }
            else
            {
                NS_LOG_DEBUG("Connection to target succeeded");
            }

            SendPacketTarget();
        }
        else
        {
            NS_LOG_DEBUG("m_target_address does not match ipv4");
        }
    }
}
} // namespace ns3