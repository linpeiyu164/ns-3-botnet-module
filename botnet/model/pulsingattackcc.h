#ifndef PULSING_ATTACK_CC_H
#define PULSING_ATTACK_CC_H
#include "ns3/application.h"
#include "ns3/socket.h"

namespace ns3
{
    class PulsingAttackCC: public Application
    {
        public:
            PulsingAttackCC();
            virtual ~PulsingAttackCC();

            static TypeId GetTypeId();
            virtual TypeId GetInstanceTypeId() const;

            void StartApplication(); // inherited once when app starts
            void StopApplication();

        private:
            /* open the connection to remote address */
            void OpenConnection();

            /* send packet and schedule the next packet send */
            void SendPacket();

            Ptr<Socket> m_recv_socket;
            uint16_t m_recv_port;

            Ptr<Socket> m_send_socket;
            uint16_t m_send_port;

            Address m_remote_address;
            uint16_t m_remote_port;

            uint32_t m_packet_size;
    };
}
#endif /* ATTACK_H */