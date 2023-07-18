#ifndef BOTNET_HELPER_H
#define BOTNET_HELPER_H

#include "ns3/botnet.h"
#include "ns3/brite-topology-helper.h"
#include "ns3/application-container.h"

namespace ns3
{
    /* Helps initialize the botnet and launch attacks */
    class BotnetHelper{
        public:
            BotnetHelper();

            /* creates botnet based on topology */
            void CreateBotnet(
                BriteTopologyHelper* bth,
                int maxBotsPerAS,
                BotnetType type,
                std::string name);

            void SetupAttack(std::string ccTypeId, std::string botTypeId);
            void InstallAttack();
            void LaunchAttack();

        private:
            Botnet *m_botnet;
            int m_numAs;
            int m_numPerAs;
            int m_maxBotsPerAs;
            std::vector<std::vector<int>> m_nodeMap;
            ObjectFactory m_ccApp;
            ObjectFactory m_botApp;
            ApplicationContainer m_ccAppContainer;
            ApplicationContainer m_botAppContainer;

            /* initializes node map, tracks bot assignment in topology */
            void SetupNodeMap();

            ApplicationContainer ApplicationInstallBot(std::vector<NodeContainer*>& c) const;
            ApplicationContainer ApplicationInstallBot(Ptr<Node> node) const;
            Ptr<Application> InstallPrivBot(Ptr<Node> node) const;

            ApplicationContainer ApplicationInstallCC(NodeContainer c) const;
            ApplicationContainer ApplicationInstallCC(Ptr<Node> node) const;
            Ptr<Application> InstallPrivCC(Ptr<Node> node) const;
        public:
            void SetAttributeCC(std::string name, const AttributeValue& value);
            void SetAttributeBot(std::string name, const AttributeValue& value);

    };

}

#endif /* BOTNET_HELPER_H */
