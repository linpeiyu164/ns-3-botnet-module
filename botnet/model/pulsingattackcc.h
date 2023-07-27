#ifndef PULSING_ATTACK_CC_H
#define PULSING_ATTACK_CC_H
#include "ns3/application.h"
#include "ns3/socket.h"
#include <unordered_map>

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

            static void CCRttTraceCallback(std::string context, Time rtt);
            static void TargetRttTraceCallback(std::string context, Time rtt);
            static uint32_t ContextToNodeId(std::string context);

            inline static std::unordered_map<Ipv4Address, Time, Ipv4AddressHash> m_ccRttTable;
            inline static std::unordered_map<Ipv4Address, Time, Ipv4AddressHash> m_targetRttTable;

        private:

            // void InitRtt(uint16_t numAs, uint16_t numNodePerAs);

            // void UpdateRtt(uint16_t asId, uint16_t NodeId, double rtt);

            /* open the connection to remote address */
            // void OpenConnection();

            /* Schedule send based on RTT*/
            void ScheduleSend();

            /* Schedule send based on RTT*/
            void ScheduleSend();

            /* send packet and schedule the next packet send */
            void SendPacket();

            /* Handle packet reads */
            void HandleRead(Ptr<Socket> socket);

            /* Update rtt table*/
            void UpdateRtt(Ipv4Address ipv4, double rtt);

            /* Handle connection requests */
            void HandleAccept(Ptr<Socket> socket, const Address & address);

            // Ptr<Socket> m_recv_socket;
            uint16_t m_recv_port;

            Ptr<Socket> m_send_socket;

            // Ipv4Address m_remote_address;
            uint16_t m_remote_port;

            uint32_t m_packet_size;

            // m_rtt[botUid][rtt_total]
            // rtt_total = bot_target_rtt + master_bot_rtt
            // std::vector<std::vector<double>> m_rtt;
            // std::unordered_map<Ipv4Address, double, Ipv4AddressHash> m_rtt;


            double m_maxRtt;

            std::vector<Ptr<Socket>> m_recv_sockets;
    };
}
#endif /* ATTACK_H */