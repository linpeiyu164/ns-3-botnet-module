#include "benignapplication.h"

#include "ns3/double.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"
#include "ns3/string.h"
#include "ns3/tcp-socket-factory.h"
#include <ns3/type-name.h>
#include <ns3/uinteger.h>

namespace ns3
{
NS_LOG_COMPONENT_DEFINE("BenignApplication");
NS_OBJECT_ENSURE_REGISTERED(BenignApplication);

TypeId
BenignApplication::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::BenignApplication")
            .SetParent<Application>()
            .AddConstructor<BenignApplication>()
            .AddAttribute("TargetAddress",
                          "Target address of packet sink",
                          Ipv4AddressValue(),
                          MakeIpv4AddressAccessor(&BenignApplication::m_target_address),
                          MakeIpv4AddressChecker())
            .AddAttribute("TargetPort",
                          "Target port that will receive the attack packets",
                          UintegerValue(8081),
                          MakeUintegerAccessor(&BenignApplication::m_target_port),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute("PacketSize",
                          "Packet size of attacking packets",
                          StringValue("ns3::LogNormalRandomVariable"),
                          MakePointerAccessor(&BenignApplication::m_packet_size_rv),
                          MakePointerChecker<RandomVariableStream>())
            .AddAttribute("MeanPacketSize",
                          "Mean packet size of attacking packets",
                          UintegerValue(200),
                          MakeUintegerAccessor(&BenignApplication::m_mean_packet_size),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute("VariancePacketSize",
                          "Variation of packet size",
                          UintegerValue(2),
                          MakeUintegerAccessor(&BenignApplication::m_sigma_packet_size),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute("SendingRate",
                          "Sending data rate, is a lognormal random variable",
                          StringValue("ns3::LogNormalRandomVariable"),
                          MakePointerAccessor(&BenignApplication::m_data_rate),
                          MakePointerChecker<RandomVariableStream>())
            .AddAttribute("MeanSendingRate",
                          "Mean sending rate",
                          DoubleValue(8000.0),
                          MakeDoubleAccessor(&BenignApplication::m_mean_rate),
                          MakeDoubleChecker<double>())
            .AddAttribute("Sigma",
                          "Sigma value of DataRate lognormal variable",
                          DoubleValue(2.0),
                          MakeDoubleAccessor(&BenignApplication::m_sigma),
                          MakeDoubleChecker<double>());
    return tid;
}

TypeId
BenignApplication::GetInstanceTypeId() const
{
    NS_LOG_FUNCTION(this);
    return BenignApplication::GetTypeId();
}

BenignApplication::BenignApplication()
{
    NS_LOG_FUNCTION(this);
}

BenignApplication::~BenignApplication()
{
    NS_LOG_FUNCTION(this);
}

void
BenignApplication::StartApplication()
{
    NS_LOG_FUNCTION(this);

    // initiate socket
    m_target_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());

    // initiate datarate random variable
    m_data_rate = CreateObject<LogNormalRandomVariable>();

    // calculate mu from mean and sigma, since mean is more accessible toward the user
    // reference: https://brilliant.org/wiki/log-normal-distribution/
    m_mu = (double)std::log(m_mean_rate) - 0.5 * m_sigma * m_sigma;
    m_data_rate->SetAttribute("Mu", DoubleValue(m_mu));
    m_data_rate->SetAttribute("Sigma", DoubleValue(m_sigma));

<<<<<<< HEAD
=======
    m_packet_size_rv = CreateObject<LogNormalRandomVariable>();
    double mu =
        (double)std::log(m_mean_packet_size) - 0.5 * m_sigma_packet_size * m_sigma_packet_size;
    m_packet_size_rv->SetAttribute("Mu", DoubleValue(mu));
    m_packet_size_rv->SetAttribute("Sigma", DoubleValue(m_sigma_packet_size));

>>>>>>> 7908269 (add benign traffic model with common destination)
    OpenConnection(m_target_socket, m_target_address, m_target_port);
    ScheduleNext();
}

void
BenignApplication::OpenConnection(Ptr<Socket> socket, Ipv4Address addr, uint16_t port)
{
    NS_LOG_FUNCTION(this);
    int ret = socket->Bind();
    if (ret < 0)
    {
        NS_LOG_ERROR("Failed to bind socket");
    }
    else
    {
        NS_LOG_INFO("Socket bound");
    }

    if (Ipv4Address::IsMatchingType(addr))
    {
        Ipv4Address ipv4 = Ipv4Address::ConvertFrom(addr);
        InetSocketAddress inetSocket = InetSocketAddress(ipv4, port);
        Address address(inetSocket);

        ret = socket->Connect(address);
        if (ret < 0)
        {
            NS_LOG_ERROR("Error: Connection failed");
        }
        else
        {
            NS_LOG_INFO("Connected to target of benign traffic");
        }
    }
    else
    {
        NS_LOG_ERROR("Error: Address incompatible with Ipv4");
    }
}

void
BenignApplication::SendPacket()
{
    NS_LOG_FUNCTION(this);
<<<<<<< HEAD
=======
    // NS_LOG_INFO("m_packet_size: " << m_packet_size);
>>>>>>> 7908269 (add benign traffic model with common destination)
    Ptr<Packet> packet = Create<Packet>(m_packet_size);
    int ret = m_target_socket->Send(packet);
    if (ret == -1)
    {
        NS_LOG_ERROR("Packet send failed");
    }
    else
    {
        ScheduleNext();
    }
}

void
BenignApplication::ScheduleNext()
{
    NS_LOG_FUNCTION(this);
<<<<<<< HEAD
    uint32_t bits = m_packet_size * 8;
    m_instant_rate = DataRate(m_data_rate->GetInteger());
    Time nextSendingTime = Seconds(bits / (double)m_instant_rate.GetBitRate());

    EventId id = Simulator::Schedule(nextSendingTime, &BenignApplication::SendPacket, this);
    NS_LOG_INFO("Scheduled event after: " << nextSendingTime.GetMilliSeconds() << "milliseconds");
=======
    m_packet_size = m_packet_size_rv->GetInteger();

    uint32_t bits = m_packet_size * 8;
    m_instant_rate = DataRate(m_data_rate->GetInteger());
    NS_LOG_INFO("Next packet size: (bit)" << m_packet_size);
    NS_LOG_INFO("Instant rate (bit/s): " << m_instant_rate);
    Time nextSendingTime = Seconds(bits / (double)m_instant_rate.GetBitRate());

    m_send_event = Simulator::Schedule(nextSendingTime, &BenignApplication::SendPacket, this);
    NS_LOG_INFO("Scheduled event after: " << nextSendingTime.GetMilliSeconds() << " milliseconds");
}

void
BenignApplication::CancelEvent()
{
    NS_LOG_FUNCTION(this);
    Simulator::Cancel(m_send_event);
>>>>>>> 7908269 (add benign traffic model with common destination)
}

void
BenignApplication::StopApplication()
{
    NS_LOG_FUNCTION(this);
<<<<<<< HEAD
=======
    CancelEvent();

>>>>>>> 7908269 (add benign traffic model with common destination)
    // close target socket
    if (m_target_socket)
    {
        m_target_socket->Close();
    }
}

} // namespace ns3