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
                        Ipv4AddressValue(),
                        MakeIpv4AddressAccessor(&PulsingAttackCC::m_remote_address),
                        MakeIpv4AddressChecker())
                    .AddAttribute(
                        "RemotePort",
                        "Port of the node that will receive the packet",
                        UintegerValue(8081),
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
        SendPacket();
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

        // bind socket
        int ret = m_send_socket->Bind();
        if(ret < 0)
        {
            NS_LOG_ERROR("Error: Binding failed");
        }
        else
        {
            NS_LOG_INFO("Socket bound");
        }

        // connect to remote address
        if(Ipv4Address::IsMatchingType(m_remote_address))
        {
            InetSocketAddress inetSocket = InetSocketAddress(m_remote_address, m_remote_port);
            Address remoteAddress(inetSocket);

            // NS_LOG_DEBUG("Remote address passed in: " << m_remote_address);
            // NS_LOG_DEBUG("Convert to Ipv4 address: " << ipv4);
            // NS_LOG_DEBUG("INET SOCKET: " << inetSocket);
            // NS_LOG_DEBUG("Opened connection to: " << remoteAddress);

            ret = m_send_socket->Connect(remoteAddress);
            if(ret < 0)
            {
                NS_LOG_ERROR("Error: Connection failed");
            }
            else
            {
                NS_LOG_INFO("CC Connected");
            }
        }
        else
        {
            NS_LOG_ERROR("Error: Address incompatible with Ipv4");
        }
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
        // Simulator::Schedule(next_time, &PulsingAttackCC::SendPacket, this);
    }

}
