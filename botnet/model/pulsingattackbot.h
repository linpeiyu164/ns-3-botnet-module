#ifndef PULSING_ATTACK_BOT_H
#define PULSINE_ATTACK_BOT_H

#include "ns3/application.h"
#include "ns3/socket.h"

namespace ns3
{
class PulsingAttackBot : public Application
{
  public:
    PulsingAttackBot();
    virtual ~PulsingAttackBot();

    static TypeId GetTypeId();
    virtual TypeId GetInstanceTypeId() const;

    void StartApplication();
    void StopApplication();

    void SendPacketCC(Ptr<Socket> socket);

    void SendPacketTarget();

    void ReceivePacketCC(Ptr<Socket> socket);

    void RttCallback(Time rtt); // callback for pingv4

  private:
    void CancelEvent();
    void OpenConnection(Ptr<Socket> socket, Ipv4Address addr, uint16_t port);

    void HandleAccept(Ptr<Socket> socket, const Address& address);

    Ipv4Address m_target_address;
    uint16_t m_target_port;
    Ptr<Socket> m_target_socket;

    Ptr<Socket> m_recv_socket;
    uint16_t m_recv_port;

    uint16_t m_packet_size;
    Time m_attack_interval;

    Ipv4Address m_cc_address;
    uint16_t m_cc_port;
    Ptr<Socket> m_cc_socket;

    uint16_t m_rounds;

    double m_rtt;

    EventId m_send_event;
};
} // namespace ns3

#endif