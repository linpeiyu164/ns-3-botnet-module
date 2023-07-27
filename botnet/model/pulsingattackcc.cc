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
                    // .AddAttribute(
                    //     "RemoteAddress",
                    //     "Address of the node that will receive the packet",
                    //     Ipv4AddressValue(),
                    //     MakeIpv4AddressAccessor(&PulsingAttackCC::m_remote_address),
                    //     MakeIpv4AddressChecker())
                    .AddAttribute(
                        "RemotePort",
                        "Port of the node that will receive the packet",
                        UintegerValue(8081),
                        MakeUintegerAccessor(&PulsingAttackCC::m_remote_port),
                        MakeUintegerChecker<uint16_t>()
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
        socket->Listen();
    }

    /* Handle connection requests */
    void PulsingAttackCC::HandleAccept(Ptr<Socket> socket, const Address & address)
    {
        NS_LOG_FUNCTION(this << socket << address);
        socket->SetRecvCallback(MakeCallback(&PulsingAttackCC::HandleRead, this));
        m_recv_sockets.push_back(socket); // socket->GetPeerName() will get the remote address
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

    // void PulsingAttackCC::UpdateRtt(Ipv4Address ipv4, double rtt)
    // {
    //     if(rtt > m_maxRtt)
    //     {
    //         m_maxRtt = rtt;
    //     }
    //     m_rtt[ipv4] = rtt;
    // }

    void PulsingAttackCC::StopApplication()
    {
        NS_LOG_FUNCTION(this);
        for(int i = 0; i < m_recv_sockets.size(); i++)
        {
            m_recv_sockets[i]->Close();
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
        // NS_LOG_FUNCTION(context << rtt);
        NS_LOG_INFO("Rtt trace: " << context << " with value of: " << rtt);
        uint32_t nodeId = ContextToNodeId(context);
        Ptr<Node> node = NodeList::GetNode(nodeId);
        Ipv4Address ipv4addr = node->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
        PulsingAttackCC::m_ccRttTable[ipv4addr] = rtt;
        NS_LOG_INFO("IPV4 ADDRESS: " << ipv4addr);
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

    /*
    void PulsingAttackCC::OpenConnection()
    {
        NS_LOG_FUNCTION(this);

        // bind socket
        int ret = m_send_socket->Bind();
        if(ret < 0)
        {
            NS_LOG_ERROR("Error: Binding failed");
        }
        else
        {
            NS_LOG_INFO("Socket bound");
        }

        // connect to remote address
        if(Ipv4Address::IsMatchingType(m_remote_address))
        {
            InetSocketAddress inetSocket = InetSocketAddress(m_remote_address, m_remote_port);
            Address remoteAddress(inetSocket);

            ret = m_send_socket->Connect(remoteAddress);
            if(ret < 0)
            {
                NS_LOG_ERROR("Error: Connection failed");
            }
            else
            {
                NS_LOG_INFO("CC Connected");
            }
        }
        else
        {
            NS_LOG_ERROR("Error: Address incompatible with Ipv4");
        }
    }
    */

    void PulsingAttackCC::SendPacket()
    {
        NS_LOG_FUNCTION(this);

        Ptr<Packet> packet = Create<Packet>(m_packet_size);
        m_send_socket->Send(packet);

        // calculate next_send_time based on received RTT time from bots
        Time send_interval = Seconds(2.0);
        Simulator::Schedule(send_interval, &PulsingAttackCC::SendPacket, this);
    }

    void PulsingAttackCC::ScheduleSend()
    {
        NS_LOG_FUNCTION(this);
    }

}
