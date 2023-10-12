#ifndef BENIGN_APPLICATION_H
#define BENIGN_APPLICATION_H

#include "ns3/application.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"
#include "ns3/socket.h"

namespace ns3
{

class BenignApplication : public Application
{
  public:
    BenignApplication();
    virtual ~BenignApplication();

    static TypeId GetTypeId();
    virtual TypeId GetInstanceTypeId() const;

    void StartApplication();
    void StopApplication();

  private:
    void OpenConnection(Ptr<Socket> socket, Ipv4Address addr, uint16_t port);
    void SendPacket();
    void ScheduleNext();
    void CancelEvent();

    std::string m_model_type;

    Ptr<RandomVariableStream> m_packet_size_rv;
    uint16_t m_mean_packet_size;
    uint16_t m_sigma_packet_size;
    uint16_t m_packet_size;

    Ptr<RandomVariableStream> m_data_rate; // model the datarate from one node as lognormal
    double m_mu;
    double m_sigma;
    double m_mean_rate;
    DataRate m_instant_rate;

    Ipv4Address m_target_address;
    uint16_t m_target_port;
    Ptr<Socket> m_target_socket;

    EventId m_send_event;
};

} // namespace ns3

#endif