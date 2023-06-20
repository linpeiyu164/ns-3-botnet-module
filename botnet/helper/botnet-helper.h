#ifndef BOTNET_HELPER_H
#define BOTNET_HELPER_H

#include "botnet.h"
#include "ns3/brite-module.h"

namespace ns3
{

/* Helps initialize the botnet and launch attacks */
class BotnetHelper{
    public:
        BotnetHelper();

        /* creates botnet based on topology */
        void CreateBotnet(
            Ptr<BriteTopologyHelper> bth,
            int maxBotsPerAS,
            BotnetType type,
            std::string name);

        void SetupAttack();

        void LaunchAttack();

    private:
        Botnet *m_botnet;
        int m_numAs;
        int m_numPerAs;
        int m_maxBotsPerAs;
        std::vector<std::vector<int>> m_nodeMap;

        /* initializes node map, tracks bot assignment in topology */
        void SetupNodeMap();
};

}

#endif /* BOTNET_HELPER_H */
