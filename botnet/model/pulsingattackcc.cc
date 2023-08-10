#include "pulsingattackcc.h"
#include "ns3/simulator.h"
#include <ns3/uinteger.h>
#include <ns3/type-name.h>
#include <ns3/socket.h>
#include <ns3/tcp-socket-factory.h>
#include "ns3/internet-module.h"
#include "ns3/ipv4-address-helper.h"

namespace ns3
{
    NS_LOG_COMPONENT_DEFINE("PulsingAttackCC");
    NS_OBJECT_ENSURE_REGISTERED(PulsingAttackCC);

    TypeId PulsingAttackCC::GetTypeId()
    {
        static TypeId tid = TypeId("ns3::PulsingAttackCC")
                    .SetParent<Application>()
                    .AddConstructor<PulsingAttackCC>()
                    .AddAttribute(
                        "ReceivePort",
                        "Receiving port",
                        UintegerValue(8080),
                        MakeUintegerAccessor(&PulsingAttackCC::m_recv_port),
                        MakeUintegerChecker<uint16_t>())
                    .AddAttribute(
                        "PacketSize",
                        "Packet size",
                        UintegerValue(100),
                        MakeUintegerAccessor(&PulsingAttackCC::m_packet_size),
                        MakeUintegerChecker<uint16_t>())
                    .AddAttribute(
                        "RemotePort",
                        "Port of the node that will receive the packet",
                        UintegerValue(8081),
                        MakeUintegerAccessor(&PulsingAttackCC::m_remote_port),
                        MakeUintegerChecker<uint16_t>())
                    .AddAttribute(
                        "AttackTime",
                        "Time between bot connection and attack",
                        TimeValue(Seconds(20.0)),
                        MakeTimeAccessor(&PulsingAttackCC::m_attack_time),
                        MakeTimeChecker()
                    );

        return tid;
    }

    TypeId PulsingAttackCC::GetInstanceTypeId() const
    {
        NS_LOG_FUNCTION(this);
        return PulsingAttackCC::GetTypeId();
    }

    PulsingAttackCC::PulsingAttackCC()
    {
        m_maxDelay = Seconds(0.0);
        NS_LOG_FUNCTION(this);
    }

    PulsingAttackCC::~PulsingAttackCC()
    {
        NS_LOG_FUNCTION(this);
    }

    void PulsingAttackCC::StartApplication()
    {
        NS_LOG_FUNCTION(this);
        Ptr<Socket> socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
        socket->SetAcceptCallback(
            MakeNullCallback<bool, Ptr<Socket>, const Address &>(),
            MakeCallback(&PulsingAttackCC::HandleAccept, this));
        if(socket->Bind(InetSocketAddress(Ipv4Address::GetAny(), m_recv_port)) == -1)
        {
            NS_LOG_ERROR("Socket bind failed");
        }
        else
        {
            socket->Listen();
            Simulator::Schedule(Seconds(100), &PulsingAttackCC::ScheduleBots, this);
        }
    }

    /* Handle connection requests */
    void PulsingAttackCC::HandleAccept(Ptr<Socket> socket, const Address & address)
    {
        NS_LOG_FUNCTION(this << socket << address);
        socket->SetRecvCallback(MakeCallback(&PulsingAttackCC::HandleRead, this));
        Ipv4Address ipv4 = InetSocketAddress::ConvertFrom(address).GetIpv4();
        NS_LOG_DEBUG("Accepted Ipv4 address: " << ipv4);
        m_recv_sockets[ipv4] = socket;
        // socket->GetPeerName() will get the remote address
    }

    /* Handle packet reads */
    void PulsingAttackCC::HandleRead(Ptr<Socket> socket)
    {
        NS_LOG_FUNCTION(this << socket);
        Ptr<Packet> packet;
        while((packet = socket->Recv()))
        {
            NS_LOG_DEBUG("packet received");
            // get the rtt packet tag
            // Address addr = socket->GetPeerName();
            // UpdateRtt(addr, rtt);
            // check if ScheduleSend should be called
        }
    }

    void PulsingAttackCC::StopApplication()
    {
        NS_LOG_FUNCTION(this);
        for(auto it = m_recv_sockets.begin(); it != m_recv_sockets.end(); it++)
        {
            it->second->Close();
        }
        m_send_socket->Close();
    }

    uint32_t PulsingAttackCC::ContextToNodeId(std::string context)
    {
        std::string sub = context.substr(10);
        uint32_t pos = sub.find("/ApplicationList");
        return std::stoi(sub.substr(0, pos));
    }

    void PulsingAttackCC::CCRttTraceCallback(std::string context, Time rtt)
    {
        NS_LOG_INFO("Rtt trace: " << context << " with value of: " << rtt);
        uint32_t nodeId = ContextToNodeId(context);
        Ptr<Node> node = NodeList::GetNode(nodeId);
        Ipv4Address ipv4addr = node->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
        PulsingAttackCC::m_ccRttTable[ipv4addr] = rtt;
        NS_LOG_INFO("IPV4 ADDRESS: " << ipv4addr);
    }

    void PulsingAttackCC::ScheduleBots()
    {
        NS_LOG_FUNCTION(this);
        for(auto it = PulsingAttackCC::m_ccRttTable.begin(); it != PulsingAttackCC::m_ccRttTable.end(); it++)
        {
            Ipv4Address ipv4 = it->first;
            it->second = (it->second + m_targetRttTable[ipv4])/2;
            NS_LOG_DEBUG("Delay: " << it->second);
            if(it->second > m_maxDelay)
            {
                m_maxDelay = it->second;
            }
        }

        for(auto it = PulsingAttackCC::m_ccRttTable.begin(); it != PulsingAttackCC::m_ccRttTable.end(); it++)
        {
            if(it == NULL)
            {
                NS_LOG_ERROR("Null iterator");
            }
            else
            {
                Time commandTime = m_attack_time + m_maxDelay - it -> second;
                Simulator::Schedule(commandTime, &PulsingAttackCC::SendCommand, this, it->first);
            }

        }
    }

    void PulsingAttackCC::TargetRttTraceCallback(std::string context, Time rtt)
    {
        NS_LOG_INFO("Rtt trace: " << context << " with a value of: " << rtt);
        uint32_t nodeId = ContextToNodeId(context);
        Ptr<Node> node = NodeList::GetNode(nodeId);
        Ipv4Address ipv4addr = node->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
        PulsingAttackCC::m_targetRttTable[ipv4addr] = rtt;
        NS_LOG_INFO("IPV4 ADDRESS: " << ipv4addr);
    }

    void PulsingAttackCC::SendCommand(Ipv4Address ipv4)
    {
        NS_LOG_FUNCTION(this);

        Ptr<Packet> packet = Create<Packet>(m_packet_size);
        Ptr<Socket> socket = m_recv_sockets[ipv4];
        socket->Send(packet);
    }

}
