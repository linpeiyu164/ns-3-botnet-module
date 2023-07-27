#ifndef PULSING_ATTACK_BOT_H
#define PULSINE_ATTACK_BOT_H

#include "ns3/application.h"
#include "ns3/socket.h"

namespace ns3
{
    class PulsingAttackBot: public Application
    {
        public:
            PulsingAttackBot();
            virtual ~PulsingAttackBot();

            static TypeId GetTypeId();
            virtual TypeId GetInstanceTypeId() const;

            void StartApplication();
            void StopApplication();

            void SendPacket();
            void ReceivePacket();

            void RttCallback(Time rtt); // callback for pingv4
        private:
            void OpenConnection();

            Ipv4Address m_remote_address;
            uint16_t m_remote_port;

            Ptr<Socket> m_send_socket;

            Ptr<Socket> m_recv_socket;
            uint16_t m_recv_port;

            uint16_t m_packet_size;
            uint16_t m_attack_interval;

            double m_rtt;
    };
}

#endif