#ifndef PULSING_ATTACK_CC_H
#define PULSING_ATTACK_CC_H
#include "ns3/application.h"
#include "ns3/socket.h"

#include <unordered_map>

namespace ns3
{
class PulsingAttackCC : public Application
{
  public:
    PulsingAttackCC();
    virtual ~PulsingAttackCC();

    static TypeId GetTypeId();
    virtual TypeId GetInstanceTypeId() const;

    void StartApplication(); // inherited once when app starts
    void StopApplication();

    void CCRttTraceCallback(std::string context, Time rtt);
    void TargetRttTraceCallback(std::string context, Time rtt);

    void ConnectToRttSource(uint32_t appIndexCC, uint32_t appIndexTarget);

    static uint32_t ContextToNodeId(std::string context);

    inline static std::unordered_map<uint32_t, Time> m_ccRttTable;
    inline static std::unordered_map<uint32_t, Time> m_targetRttTable;
    std::unordered_map<uint32_t, Ptr<Socket>> m_socketMap;

  private:
    /* Schedule send based on RTT*/
    void ScheduleSend();

    /* Handle packet reads */
    void HandleRead(Ptr<Socket> socket);

    /* Update rtt table*/
    void UpdateRtt(Ipv4Address ipv4, double rtt);

    /* Handle connection requests */
    void HandleAccept(Ptr<Socket> socket, const Address& address);

    /*Schedule bots based on RTT*/
    void ScheduleBots();

    void SendCommand(uint32_t nodeId);

    void CancelEvent();

    // Ptr<Socket> m_recv_socket;
    uint16_t m_recv_port;

    Ptr<Socket> m_send_socket;

    // Ipv4Address m_remote_address;
    uint16_t m_remote_port;

    uint32_t m_packet_size;

    Time m_maxDelay;

    Time m_attack_time;

    std::vector<EventId> m_send_events;

    EventId m_schedule_bots_event;

    uint32_t m_app_index_cc;

    uint32_t m_app_index_target;
};
} // namespace ns3
#endif /* ATTACK_H */