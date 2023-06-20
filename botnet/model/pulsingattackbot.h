#include "ns3/application.h"
#include "ns3/socket.h"

namespace ns3
{
    class PulsingAttackBot: public Application{
        public:
            PulsingAttackBot();
            virtual ~PulsingAttackBot();
            static TypeId GetTypeId();
            virtual TypeId GetInstanceTypeId() const;
            void SendPacket();
            void ReceivePacket();
        private:
            Ipv4Address m_dest_address;
            uint16_t m_dest_port;
            Ptr<Socket> m_send_socket;
            Ptr<Socket> m_recv_socket;
            uint16_t m_send_port;
            uint16_t m_recv_port;
            int16_t m_packet_size;
            int attack_interval;
    };
}