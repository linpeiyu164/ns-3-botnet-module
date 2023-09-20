#include "ns3/botnet.h"

#include "string.h"

namespace ns3
{
Botnet::Botnet(BotnetType type, std::string name)
{
    m_type = type;
    m_name = name;
    m_botMaster = NULL;
    m_size = 0;
}
} // namespace ns3
