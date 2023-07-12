#include "pulsingattackcc.h"
#include "ns3/simulator.h"
#include <ns3/uinteger.h>
#include <ns3/type-name.h>
#include <ns3/socket.h>
#include <ns3/tcp-socket-factory.h>

namespace ns3
{
    NS_LOG_COMPONENT_DEFINE("PulsingAttackCC");
    NS_OBJECT_ENSURE_REGISTERED(PulsingAttackCC);

    TypeId PulsingAttackCC::GetTypeId()
    {
        static TypeId tid = TypeId("ns3::PulsingAttackCC")
                    .SetParent<Application>()
                    .AddConstructor<PulsingAttackCC>()
                    .AddAttribute(
                        "SendPort",
                        "Sending port",
                        UintegerValue(8000),
                        MakeUintegerAccessor(&PulsingAttackCC::m_send_port),
                        MakeUintegerChecker<uint16_t>())
                    .AddAttribute(
                        "ReceivePort",
                        "Receiving port",
                        UintegerValue(8080),
                        MakeUintegerAccessor(&PulsingAttackCC::m_recv_port),
                        MakeUintegerChecker<uint16_t>())
                    .AddAttribute(
                        "PacketSize",
                        "Packet size",
                        UintegerValue(100),
                        MakeUintegerAccessor(&PulsingAttackCC::m_packet_size),
                        MakeUintegerChecker<uint16_t>())
                    .AddAttribute(
                        "RemoteAddress",
                        "Address of the node that will receive the packet",
                        AddressValue(),
                        MakeAddressAccessor(&PulsingAttackCC::m_remote_address),
                        MakeAddressChecker())
                    .AddAttribute(
                        "RemotePort",
                        "Port of the node that will receive the packet",
                        UintegerValue(8080),
                        MakeUintegerAccessor(&PulsingAttackCC::m_remote_port),
                        MakeUintegerChecker<uint16_t>()
                    );

        return tid;
    }

    TypeId PulsingAttackCC::GetInstanceTypeId() const
    {
        NS_LOG_FUNCTION(this);
        return PulsingAttackCC::GetTypeId();
    }

    PulsingAttackCC::PulsingAttackCC()
    {
        NS_LOG_FUNCTION(this);
    }

    PulsingAttackCC::~PulsingAttackCC()
    {
        NS_LOG_FUNCTION(this);
    }

    void PulsingAttackCC::StartApplication()
    {
        NS_LOG_FUNCTION(this);
        m_recv_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
        // m_recv_socket->SetRecvCallback(MakeCallback(&PulsingAttackCC::ReceivePacket, this));
        m_send_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
        OpenConnection();
    }

    void PulsingAttackCC::StopApplication()
    {
        NS_LOG_FUNCTION(this);
        m_recv_socket->Close();
        m_send_socket->Close();
    }

    void PulsingAttackCC::OpenConnection()
    {
        NS_LOG_FUNCTION(this);
        int ret = m_send_socket -> Bind();
        Ipv4Address ipv4 = Ipv4Address::ConvertFrom(m_remote_address);
        InetSocketAddress inetSocket = InetSocketAddress(ipv4, m_remote_port);
        ret = m_send_socket->Connect(inetSocket);
    }

    void PulsingAttackCC::SendPacket()
    {
        NS_LOG_FUNCTION(this);
        // send current packet
        Ptr<Packet> packet = Create<Packet>(m_packet_size);
        m_send_socket->Send(packet);

        // calculate next_send_time based on received RTT time from bots
        int send_interval = 2;
        Time next_time(Seconds(Simulator::Now().GetSeconds() + send_interval));
        Simulator::Schedule(next_time, &PulsingAttackCC::SendPacket, this);
    }

}
