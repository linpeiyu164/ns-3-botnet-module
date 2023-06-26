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

            void StartApplication();
            void StopApplication();

            void OpenConnection();
            void SendPacket();
            void ReceivePacket();
        private:
            Address m_remote_address;
            uint16_t m_remote_port;

            Ptr<Socket> m_send_socket;
            uint16_t m_send_port;

            Ptr<Socket> m_recv_socket;
            uint16_t m_recv_port;

            int16_t m_packet_size;
            int16_t attack_interval;
    };
}