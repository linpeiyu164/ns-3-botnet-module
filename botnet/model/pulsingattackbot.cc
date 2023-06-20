#include "pulsingattackbot.h"
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
                        MakeUintegerChecker<uint16_t>());
        return tid;
    }

    TypeId PulsingAttackBot::GetInstanceTypeId() const
    {
        return PulsingAttackBot::GetTypeId();
    }

    void PulsingAttackBot::SendPacket()
    {
        // send attack packets
    }

    void PulsingAttackBot::ReceivePacket()
    {
        // accept cc command
        /*
        setup
        Ptr<Socket> m_recv_socket;
        uint16_t m_recv_port;
        Ptr<Socket> m_send_socket;
        uint16_t m_send_port;
        uint32_t m_packetSize;
        */
        //
    }

}