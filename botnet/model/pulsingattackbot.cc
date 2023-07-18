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
                        "RemotePort",
                        "Remote port that will receive the attack packets",
                        UintegerValue(8081),
                        MakeUintegerAccessor(&PulsingAttackBot::m_remote_port),
                        MakeUintegerChecker<uint16_t>())
                    .AddAttribute(
                        "RemoteAddress",
                        "Remote address that will receive the attack packets",
                        Ipv4AddressValue(),
                        MakeIpv4AddressAccessor(&PulsingAttackBot::m_remote_address),
                        MakeIpv4AddressChecker());
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
        m_recv_socket->SetRecvCallback(MakeCallback(&PulsingAttackBot::ReceivePacket, this));
        m_send_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
        OpenConnection();
        SendPacket();
    }

    void PulsingAttackBot::StopApplication()
    {
        NS_LOG_FUNCTION(this);
        m_recv_socket->Close();
        m_send_socket->Close();
    }

    void PulsingAttackBot::OpenConnection()
    {
        NS_LOG_FUNCTION(this);
        int ret = m_send_socket->Bind();
        if(ret < 0)
        {
            NS_LOG_ERROR("failed to bind socket");
        }
        else
        {
            NS_LOG_INFO("socket bound");
        }

        Ipv4Address ipv4 = Ipv4Address::ConvertFrom(m_remote_address);
        InetSocketAddress inetSocket = InetSocketAddress(ipv4, m_remote_port);
        Address remoteAddress(inetSocket);

        // NS_LOG_DEBUG("Remote address passed in: " << m_remote_address);
        // NS_LOG_DEBUG("Convert to Ipv4 address: " << ipv4);
        // NS_LOG_DEBUG("Opened connection to: " << inetSocket);
        // NS_LOG_DEBUG("Remote address: " << remoteAddress);

        ret = m_send_socket->Connect(remoteAddress);
        if(ret < 0)
        {
            NS_LOG_ERROR("Error: Bot: Connection failed");
        }
        else
        {
            NS_LOG_INFO("Bot connected");
        }
    }

    void PulsingAttackBot::SendPacket()
    {
        NS_LOG_FUNCTION(this);
        char buf[m_packet_size-1] = {'a'};
        Ptr<Packet> packet = Create<Packet>((const uint8_t*)buf, m_packet_size);
        m_send_socket->Send(packet);
        // schedule the next send time
    }

    void PulsingAttackBot::ReceivePacket()
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
    }
}