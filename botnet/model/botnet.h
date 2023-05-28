#ifndef BOTNET_H
#define BOTNET_H
#include <string>
#include "ns3/net-device-container.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"

namespace ns3
{

class Botnet{
    public:
        Botnet(int type, std::string name);
        friend class BotnetHelper;
    private:
        std::vector<NodeContainer*> m_botNodes;
        std::vector<NetDeviceContainer*> m_botNetDevices;
        int m_type;
        std::string m_name;
};

}

#endif /* BOTNET_H */
