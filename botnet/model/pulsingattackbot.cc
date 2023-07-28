#include "pulsingattackbot.h"
#include "ns3/tcp-socket-factory.h"
#include <ns3/uinteger.h>
#include <ns3/type-name.h>

namespace ns3
{
    NS_LOG_COMPONENT_DEFINE("PulsingAttackBot");
    NS_OBJECT_ENSURE_REGISTERED(PulsingAttackBot);

    TypeId PulsingAttackBot::GetTypeId()
    {
        static TypeId tid = TypeId("ns3::PulsingAttackBot")
                    .SetParent<Application>()
                    .AddConstructor<PulsingAttackBot>()
                    .AddAttribute(
                        "ReceivePort",
                        "receiving port",
                        UintegerValue(8000),
                        MakeUintegerAccessor(&PulsingAttackBot::m_recv_port),
                        MakeUintegerChecker<uint16_t>())
                    .AddAttribute(
                        "PacketSize",
                        "Packet size of attacking packets",
                        UintegerValue(100),
                        MakeUintegerAccessor(&PulsingAttackBot::m_packet_size),
                        MakeUintegerChecker<uint16_t>())
                    .AddAttribute(
                        "TargetPort",
                        "Target port that will receive the attack packets",
                        UintegerValue(8081),
                        MakeUintegerAccessor(&PulsingAttackBot::m_target_port),
                        MakeUintegerChecker<uint16_t>())
                    .AddAttribute(
                        "TargetAddress",
                        "Target address that will receive the attack packets",
                        Ipv4AddressValue(),
                        MakeIpv4AddressAccessor(&PulsingAttackBot::m_target_address),
                        MakeIpv4AddressChecker())
                    .AddAttribute(
                        "CCAddress",
                        "CC address",
                        Ipv4AddressValue(),
                        MakeIpv4AddressAccessor(&PulsingAttackBot::m_cc_address),
                        MakeIpv4AddressChecker()
                    );
        return tid;
    }

    TypeId PulsingAttackBot::GetInstanceTypeId() const
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

    void PulsingAttackBot::StartApplication()
    {
        NS_LOG_FUNCTION(this);
        m_recv_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
        m_recv_socket->SetRecvCallback(MakeCallback(&PulsingAttackBot::ReceivePacketCC, this));
        m_target_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
        m_cc_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
        OpenConnection(m_cc_socket, m_cc_address, m_cc_port);
        // OpenConnection(m_target_socket, m_target_address, m_target_port);
        Ptr<Packet> packet = Create<Packet>(m_packet_size);
        SendPacket(m_cc_socket, packet);
    }

    void PulsingAttackBot::StopApplication()
    {
        NS_LOG_FUNCTION(this);
        m_recv_socket->Close();
        m_target_socket->Close();
        m_cc_socket->Close();
    }

    void PulsingAttackBot::OpenConnection(Ptr<Socket> socket, Ipv4Address addr, uint16_t port)
    {
        NS_LOG_FUNCTION(this);
        int ret = socket->Bind();
        if(ret < 0)
        {
            NS_LOG_ERROR("Error: Failed to bind socket to target");
        }
        else
        {
            NS_LOG_INFO("Socket bound to target");
        }

        if(Ipv4Address::IsMatchingType(addr))
        {
            Ipv4Address ipv4 = Ipv4Address::ConvertFrom(addr);
            InetSocketAddress inetSocket = InetSocketAddress(ipv4, port);
            Address address(inetSocket);

            ret = socket->Connect(address);
            if(ret < 0)
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

    void PulsingAttackBot::SendPacket(Ptr<Socket> socket, Ptr<Packet> packet)
    {
        NS_LOG_FUNCTION(this);
        // Ptr<Packet> packet = Create<Packet>(m_packet_size);
        socket->Send(packet);
        // schedule the next send time
    }

    void PulsingAttackBot::ReceivePacketCC()
    {
        NS_LOG_FUNCTION(this);
        // accept cc command
        Ptr<Packet> packet;
        Address from;
        while((packet = m_recv_socket->RecvFrom(from)))
        {
            if(packet->GetSize() == 0)
            {
                break;
            }
            else
            {
                NS_LOG_INFO("Packet received!");
            }
            // schedule SendPacket based on received information
        }

        Ptr<Packet> targetPacket = Create<Packet>(m_packet_size);

    }
}