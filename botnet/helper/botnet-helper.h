#ifndef BOTNET_HELPER_H
#define BOTNET_HELPER_H

#include "botnet.h"

namespace ns3
{

/* ... */
class BotnetHelper{
    public:
        BotnetHelper();

        /* initializes node map */
        void SetupNodeMap();

        /* creates botnet based on topology */
        void CreateBotnet(
            std::vector<NodeContainer*>& nodesByAs,
            int maxBotsPerAS,
            int type,
            std::string name);

    private:
        Botnet m_botnet;
        int m_numAs;
        int m_numPerAs;
        int m_maxBotsPerAs;
        std::vector<std::vector<int>> m_nodeMap;
};

}

#endif /* BOTNET_HELPER_H */
