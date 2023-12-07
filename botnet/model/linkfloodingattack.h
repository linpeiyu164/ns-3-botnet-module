#ifndef LINK_FLOODING_ATTACK_H
#define LINK_FLOODING_ATTACK_H

#include "ns3/application.h"
#include "ns3/socket.h"

namespace ns3
{
class LinkFloodingAttack : public Application
{
  public:
    LinkFloodingAttack();

    virtual ~LinkFloodingAttack();

    static TypeId GetTypeId();

    virtual TypeId GetInstanceTypeId() const;

    void StartApplication();

    void StopApplication();

    void RouteTraceCallback(std::string context, Ipv4Address route);

  private:
    uint32_t ContextToApplicationNumber(std::string context);

    void SendPackets();

    void CancelEvent();

    void ConnectToTraceRouteSource();

    void PrintRouteMap();

    void CalculateRemoteAddress();

    void OpenConnections();

    void OpenConnectionCC();

    void CompleteWaitForTraceRoute();

    void HandleRead(Ptr<Socket> socket);

    void StartAttack();

    Ipv4Address m_target_address;

    // addresses of paired bots
    std::vector<Ipv4Address> m_remote_addresses;

    // port to paired bots
    uint16_t m_remote_port;

    // central controller address
    Ipv4Address m_cc_address;

    // central controller port
    uint16_t m_cc_port;

    uint16_t m_packet_size;

    // remote sockets connecting to paired bots
    std::vector<Ptr<Socket>> m_remote_sockets;

    // socket connecting to central controller
    Ptr<Socket> m_cc_socket;

    // packet send events
    std::vector<EventId> m_send_events;

    EventId m_wait_event;

    // interval between attack packets
    Time m_interval;

    Time m_wait_for_traceroute;

    Time m_wait_for_receive;

    // maps remote address to traceroute result
    std::unordered_map<Ipv4Address, std::vector<Ipv4Address>, Ipv4AddressHash> m_routes;
};
} // namespace ns3

#endif