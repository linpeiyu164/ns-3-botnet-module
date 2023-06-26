#include "pulsingattackbot.h"
#include "ns3/tcp-socket-factory.h"
#include <ns3/uinteger.h>
#include <ns3/type-name.h>

namespace ns3
{
    TypeId PulsingAttackBot::GetTypeId()
    {
        static TypeId tid = TypeId("ns3:PulsingAttackBot")
                    .SetParent<Application>()
                    .AddConstructor<PulsingAttackBot>()
                    .AddAttribute(
                        "ReceivePort",
                        "receiving port",
                        UintegerValue(8000),
                        MakeUintegerAccessor(&PulsingAttackBot::m_recv_port),
                        MakeUintegerChecker<uint16_t>())
                    .AddAttribute(
                        "SendPort",
                        "Sending port",
                        UintegerValue(8080),
                        MakeUintegerAccessor(&PulsingAttackBot::m_send_port),
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
                        UintegerValue(8000),
                        MakeUintegerAccessor(&PulsingAttackBot::m_remote_port),
                        MakeUintegerChecker<uint16_t>());
        return tid;
    }

    TypeId PulsingAttackBot::GetInstanceTypeId() const
    {
        return PulsingAttackBot::GetTypeId();
    }

    void PulsingAttackBot::StartApplication()
    {
        m_recv_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
        m_recv_socket->SetRecvCallback(MakeCallback(&PulsingAttackBot::ReceivePacket, this));
        m_send_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
        OpenConnection();
    }

    void PulsingAttackBot::StopApplication()
    {
        m_recv_socket->Close();
        m_send_socket->Close();
    }

    void PulsingAttackBot::OpenConnection()
    {
        int ret = m_send_socket -> Bind();
        Ipv4Address ipv4 = Ipv4Address::ConvertFrom(m_remote_address);
        InetSocketAddress inetSocket = InetSocketAddress(ipv4, m_remote_port);
        ret = m_send_socket->Connect(inetSocket);
    }

    void PulsingAttackBot::SendPacket()
    {
        Ptr<Packet> packet = Create<Packet>(m_packet_size);
        m_send_socket->Send(packet);
        // schedule the next send time
    }

    void PulsingAttackBot::ReceivePacket()
    {
        // accept cc command
        Ptr<Packet> packet;
        Address from;
        while((packet = m_recv_socket->RecvFrom(from)))
        {
            if(packet->GetSize() == 0)
            {
                break;
            }
            // schedule SendPacket based on received information
        }
    }
}