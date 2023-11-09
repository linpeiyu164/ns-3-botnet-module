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
    void SetRandomStreamAttributes(Ptr<RandomVariableStream> rv_stream,
                                   std::string rv_type,
                                   double mean,
                                   double stddev);

    // Packet size random variable is lognormal by default
    Ptr<RandomVariableStream> m_packet_size_rv;
    std::string m_packet_size_rv_type;
    double m_packet_size_mean;
    double m_packet_size_stddev;
    uint16_t m_current_packet_size; // records the generated instant packet size

    // Data rate random variable is lognormal by default
    Ptr<RandomVariableStream> m_data_rate_rv;
    std::string m_data_rate_rv_type;
    double m_data_rate_mean;      // attributes
    double m_data_rate_stddev;    // attributes
    DataRate m_instant_data_rate; // records the generated instant data rate

    Ipv4Address m_target_address;
    uint16_t m_target_port;
    Ptr<Socket> m_target_socket;

    EventId m_send_event;
};

} // namespace ns3

#endif