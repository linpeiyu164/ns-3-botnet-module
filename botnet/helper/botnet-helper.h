#ifndef BOTNET_HELPER_H
#define BOTNET_HELPER_H

#include "ns3/botnet.h"
#include "ns3/brite-topology-helper.h"
#include "ns3/application-container.h"

namespace ns3
{
    enum class BotType
    {
        CENTRAL_CONTROLLER,
        BOT
    };

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

            // void SetupAttack(std::string ccTypeId, std::string botTypeId);

            // add app to the object factory vector
            void SetupApplicationBot(std::string typeId);

            // add app to object factory vector
            void SetupApplicationCC(std::string typeId);

            // install all cc and bot applications
            void InstallApplications();

            void AddApplication(BotType type, std::string);

            ApplicationContainer ApplicationInstallBot(std::vector<NodeContainer*>& c) const;
            ApplicationContainer ApplicationInstallCC(NodeContainer c) const;

        private:
            Botnet *m_botnet;
            int m_numAs;
            int m_numPerAs;
            int m_maxBotsPerAs;
            std::vector<std::vector<int>> m_nodeMap;
            std::vector<ObjectFactory> m_ccApps;
            std::vector<ObjectFactory> m_botApps;
            ApplicationContainer m_ccAppContainer;
            ApplicationContainer m_botAppContainer;

            /* initializes node map, tracks bot assignment in topology */
            void SetupNodeMap();

            ApplicationContainer ApplicationInstallBot(Ptr<Node> node) const;
            ApplicationContainer InstallPrivBot(Ptr<Node> node) const;

            ApplicationContainer ApplicationInstallCC(Ptr<Node> node) const;
            ApplicationContainer InstallPrivCC(Ptr<Node> node) const;

        public:
            void SetAttributeCC(uint16_t appIndex, std::string name, const AttributeValue& value);
            void SetAttributeBot(uint16_t appIndex, std::string name, const AttributeValue& value);

    };

}

#endif /* BOTNET_HELPER_H */
