#ifndef BOTNET_H
#define BOTNET_H
#include "ns3/net-device-container.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/node.h"

#include <string>

namespace ns3
{

enum class BotnetType
{
    CENTRALIZED,
    P2P
};

class Botnet
{
  public:
    Botnet(BotnetType type, std::string name);
    friend class BotnetHelper;
    // number of bots (exclude central controller)
    uint32_t m_botNum;
    // number of benign nodes
    uint32_t m_benignNum;

  private:
    std::vector<NodeContainer*> m_botNodes;
    std::vector<NodeContainer*> m_benignNodes;

    NodeContainer* m_botNodesAll;
    NodeContainer* m_benignNodesAll;

    BotnetType m_type;
    std::string m_name;
    Ptr<Node> m_botMaster;
    uint32_t m_botMasterAsId;
    uint32_t m_botMasterLeafId;
};

} // namespace ns3

#endif /* BOTNET_H */
