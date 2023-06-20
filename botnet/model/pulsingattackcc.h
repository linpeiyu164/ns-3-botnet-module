#ifndef ATTACK_H
#define ATTACK_H
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
            void SetupReceiveSocket(Ptr<Socket>, uint16_t port);
            /* schedule the next packet send */
            void ScheduleTx();
            /* send packet */
            void SendPacket();
            /* handle packet receive */
            bool HandleReceive(Ptr<Socket> socket);

            Ptr<Socket> m_recv_socket;
            uint16_t m_recv_port;
            Ptr<Socket> m_send_socket;
            uint16_t m_send_port;
            uint32_t m_packet_size;
    };
}
#endif /* ATTACK_H */