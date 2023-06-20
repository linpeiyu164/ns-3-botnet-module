#include "pulsingattackcc.h"
#include "ns3/simulator.h"
#include <ns3/uinteger.h>
#include <ns3/type-name.h>
#include <ns3/socket.h>
#include <ns3/tcp-socket-factory.h>



namespace ns3
{
    TypeId PulsingAttackCC::GetTypeId()
    {
        static TypeId tid = TypeId("ns3:PulsingAttackCC")
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
                        MakeUintegerChecker<uint16_t>());

        return tid;
    }

    TypeId PulsingAttackCC::GetInstanceTypeId() const
    {
        return PulsingAttackCC::GetTypeId();
    }

    PulsingAttackCC::PulsingAttackCC(){}

    PulsingAttackCC::~PulsingAttackCC(){}

    void PulsingAttackCC::StartApplication()
    {
        m_recv_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
        m_recv_socket->SetRecvCallback(MakeCallback(&PulsingAttackCC::HandleReceive, this));
        m_send_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
    }

    // bool PulsingAttackCC::HandleReceive(Ptr<Socket> socket)
    // {
    //     Ptr<Packet> packet;
    //     Address from;
    //     Address localAddress;
    //     while((packet = socket->RecvFrom(from)))
    //     {
    //         // logic...
    //     }
    // }
    void PulsingAttackCC::OpenConnection()
    {
        ret = m_send_socket -> Bind();

    }
    void PulsingAttackCC::SendPacket()
    {
        Ptr<Packet> packet = Create<Packet>(m_packet_size); // packet size
        // calculate next_send_time based on received RTT time from bots
        m_send_socket -> Send(packet);
        int send_interval = 2;
        Time next_time(Seconds(Simulator::Now().GetSeconds() + send_interval));
        Simulator::Schedule(next_time, &PulsingAttackCC::SendPacket, this);
        // Simulator.Schedule(next_send_time, &PulsingAttackCC::SendPacket, this);
    }


}
