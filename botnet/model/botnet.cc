#include "ns3/botnet.h"

#include "string.h"

namespace ns3
{
Botnet::Botnet(BotnetType type, std::string name)
{
    m_type = type;
    m_name = name;
    m_botMaster = NULL;
    m_botNum = 0;
    m_benignNum = 0;
    m_botNodesAll = new NodeContainer();
    m_benignNodesAll = new NodeContainer();
}
} // namespace ns3
