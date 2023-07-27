#ifndef BOTNET_H
#define BOTNET_H
#include <string>
#include "ns3/net-device-container.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/node.h"

namespace ns3
{

enum class BotnetType
{
    CENTRALIZED,
    P2P
};

class Botnet{
    public:
        Botnet(BotnetType type, std::string name);
        friend class BotnetHelper;
        uint32_t m_size;
    private:
        std::vector<NodeContainer*> m_botNodes;
        BotnetType m_type;
        std::string m_name;
        Ptr<Node> m_botMaster;
        uint16_t m_botMasterAsId;
        uint16_t m_botMasterNodeId;
};

}

#endif /* BOTNET_H */
