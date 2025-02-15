#ifndef LINK_FLOODING_ATTACK_CC_H
#define LINK_FLOODING_ATTACK_CC_H

#include "ns3/application.h"
#include "ns3/socket.h"
#include "ns3/point-to-point-module.h"

namespace ns3
{
class LinkFloodingAttackCC : public Application
{
  public:
    LinkFloodingAttackCC();

    virtual ~LinkFloodingAttackCC();

    static TypeId GetTypeId();

    virtual TypeId GetInstanceTypeId() const;

    void StartApplication();

    void StopApplication();

    void RouteTraceCallback(std::string config, Ipv4Address route);

    void ConnectToTraceRouteSource();

    Ipv4Address GetMaxAddress();

    int GetMaxCount();

    int32_t GetCriticalNodeIndex();

    void PrintCriticalNodeStatistics();

  private:
    void CompleteWaitForTraceRoute();

    void EnablePcapForCriticalNode();

    /* Handle connection requests */
    void HandleAccept(Ptr<Socket> socket, const Address& address);

    void StartListening();

    std::unordered_map<Ipv4Address, uint16_t, Ipv4AddressHash> m_routers;

    uint32_t m_max_hops;

    Ipv4Address m_max_address;

    Time m_wait_for_traceroute;

    Ptr<Socket> m_listen_socket;

    uint16_t m_listen_port;

    int32_t m_critical_node_id;

};
} // namespace ns3
#endif