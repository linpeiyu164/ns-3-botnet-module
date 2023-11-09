#include "benignapplication.h"

#include "ns3/double.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"
#include "ns3/string.h"
#include "ns3/tcp-socket-factory.h"
#include <ns3/type-name.h>
#include <ns3/uinteger.h>

#include <cmath>

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
            .AddAttribute(
                "PacketSizeRandomVariableType",
                "Type of random variable for packet size. If the user wants to change the random "
                "variable type for the packet size, this value should be changed. Supported types "
                "are: (1) ns3::NormalRandomVariable, (2) ns3::LogNormalRandomVariable, (3) "
                "ConstantRandomVariable, (4) UniformRandomVariable",
                StringValue("ns3::LogNormalRandomVariable"),
                MakeStringAccessor(&BenignApplication::m_packet_size_rv_type),
                MakeStringChecker())
            .AddAttribute("PacketSizeRandomVariable",
                          "Packet size random variable stream (bytes)",
                          StringValue("ns3::LogNormalRandomVariable"),
                          MakePointerAccessor(&BenignApplication::m_packet_size_rv),
                          MakePointerChecker<RandomVariableStream>())
            .AddAttribute("PacketSizeMean",
                          "Mean value of packet size random variable (bytes)",
                          DoubleValue(200.0),
                          MakeDoubleAccessor(&BenignApplication::m_packet_size_mean),
                          MakeDoubleChecker<double>())
            .AddAttribute("PacketSizeStdDev",
                          "Standard deviation of packet size random variable (bytes)",
                          DoubleValue(2.0),
                          MakeDoubleAccessor(&BenignApplication::m_packet_size_stddev),
                          MakeDoubleChecker<double>())
            .AddAttribute(
                "DataRateRandomVariableType",
                "Type of random variable for data rate. If the user wants to change the random "
                "variable type for the data rate, this value should be changed. Supported types "
                "are: (1) ns3::NormalRandomVariable, (2) ns3::LogNormalRandomVariable, (3) "
                "ConstantRandomVariable, (4) UniformRandomVariable",
                StringValue("ns3::LogNormalRandomVariable"),
                MakeStringAccessor(&BenignApplication::m_data_rate_rv_type),
                MakeStringChecker())
            .AddAttribute("DataRateRandomVariable",
                          "Data rate random variable stream (bit/sec)",
                          StringValue("ns3::LogNormalRandomVariable"),
                          MakePointerAccessor(&BenignApplication::m_data_rate_rv),
                          MakePointerChecker<RandomVariableStream>())
            .AddAttribute("DataRateMean",
                          "Mean value of data rate random variable (bit/sec)",
                          DoubleValue(80000.0),
                          MakeDoubleAccessor(&BenignApplication::m_data_rate_mean),
                          MakeDoubleChecker<double>())
            .AddAttribute("DataRateStdDev",
                          "Standard deviation value of data rate random variable (bit/sec)",
                          DoubleValue(800.0),
                          MakeDoubleAccessor(&BenignApplication::m_data_rate_stddev),
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
    NS_LOG_INFO("DataRate RV");
    SetRandomStreamAttributes(m_data_rate_rv,
                              m_data_rate_rv_type,
                              m_data_rate_mean,
                              m_data_rate_stddev);
    NS_LOG_INFO("PacketSize RV");
    SetRandomStreamAttributes(m_packet_size_rv,
                              m_packet_size_rv_type,
                              m_packet_size_mean,
                              m_packet_size_stddev);

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
    Ptr<Packet> packet = Create<Packet>(m_current_packet_size);
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
    m_current_packet_size = m_packet_size_rv->GetInteger();

    uint32_t bits = m_current_packet_size * 8;
    int32_t datarate_gen = m_data_rate_rv->GetInteger();
    NS_LOG_DEBUG("This is the generated value: " << datarate_gen);
    m_instant_data_rate = DataRate(m_data_rate_rv->GetInteger());

    NS_LOG_INFO("Next packet size: (bytes)" << m_current_packet_size);
    NS_LOG_INFO("Instant rate (bit/s): " << m_instant_data_rate);

    Time nextSendingTime = Seconds(bits / (double)m_instant_data_rate.GetBitRate());

    m_send_event = Simulator::Schedule(nextSendingTime, &BenignApplication::SendPacket, this);
    NS_LOG_INFO("Scheduled event after: " << nextSendingTime.GetMilliSeconds() << " milliseconds");
}

void
BenignApplication::CancelEvent()
{
    NS_LOG_FUNCTION(this);
    Simulator::Cancel(m_send_event);
}

void
BenignApplication::StopApplication()
{
    NS_LOG_FUNCTION(this);
    CancelEvent();

    // close target socket
    if (m_target_socket)
    {
        m_target_socket->Close();
    }
}

void
BenignApplication::SetRandomStreamAttributes(Ptr<RandomVariableStream> rv_stream,
                                             std::string rv_type,
                                             double mean,
                                             double stddev)
{
    // Convert the mean and standard deviation to the actual parameters required for the random
    // variables
    NS_LOG_FUNCTION(this << rv_type);
    NS_LOG_INFO("Random variable type: " << rv_type);

    if (!rv_type.compare("ns3::NormalRandomVariable"))
    {
        rv_stream = CreateObject<NormalRandomVariable>();
        rv_stream->SetAttribute("Mean", DoubleValue(mean));
        rv_stream->SetAttribute("Variance", DoubleValue(stddev));
    }
    else if (!rv_type.compare("ns3::ConstantRandomVariable"))
    {
        rv_stream = CreateObject<ConstantRandomVariable>();
        rv_stream->SetAttribute("Constant", DoubleValue(mean));
    }
    else if (!rv_type.compare("ns3::UniformRandomVariable"))
    {
        rv_stream = CreateObject<UniformRandomVariable>();
        double range = stddev * sqrt(12);
        double min = mean - range / 2;
        double max = mean + range / 2;
        NS_LOG_DEBUG("MEAN: " << mean << ", STDDEV: " << stddev);
        NS_LOG_DEBUG("MIN: " << min << ", MAX: " << max);

        rv_stream->SetAttribute("Min", DoubleValue(min));
        rv_stream->SetAttribute("Max", DoubleValue(max));
    }
    else if (!rv_type.compare("ns3::LogNormalRandomVariable"))
    {
        double var = stddev * stddev;
        double mu = std::log(mean) - 0.5 * std::log((var + mean * mean) / (mean * mean));
        double sigma = sqrt(std::log((var + mean * mean) / (mean * mean)));

        rv_stream = CreateObject<LogNormalRandomVariable>();
        rv_stream->SetAttribute("Mu", DoubleValue(mu));
        rv_stream->SetAttribute("Sigma", DoubleValue(sigma));
    }
    else
    {
        NS_LOG_ERROR("The random variable type " << rv_type << " is not yet supported");
        NS_LOG_ERROR("However, it is simple to add it yourself!");
        exit(1);
    }
}

} // namespace ns3