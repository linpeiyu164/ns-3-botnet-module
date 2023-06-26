#include "botnet-helper.h"
#include "ns3/brite-module.h"
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>

namespace ns3
{
    BotnetHelper::BotnetHelper()
    {
        m_numAs = 0;
        m_numPerAs = 0;
        m_maxBotsPerAs = 0;
    }

    void BotnetHelper::SetupNodeMap()
    {
        if(m_numAs <= 0 || m_numPerAs <= 0){
            exit(1);
        }
        int i, j;
        for(i = 0; i < m_numAs; i++){
            m_nodeMap.push_back(std::vector<int>(m_numPerAs, 0));
        }
    }

    void BotnetHelper::CreateBotnet(
        // std::vector<NodeContainer*>& nodesByAs,
        Ptr<BriteTopologyHelper> bth,
        int maxBotsPerAs,
        BotnetType type,
        std::string name)
    {
        m_numAs = bth->GetNAs();
        m_numPerAs = bth->GetNNodesForAs(0);
        m_maxBotsPerAs = maxBotsPerAs;

        SetupNodeMap();

        m_botnet = new Botnet(type, name);

        int asId, nodeId;
        int botId;

        srand(time(NULL));

        for(asId = 0; asId < m_numAs; asId++){
            NodeContainer c;
            m_botnet->m_botNodes.push_back(&c);
            for(nodeId = 0; nodeId < m_maxBotsPerAs; nodeId++){
                botId = rand() % m_numPerAs;
                if(!m_nodeMap[asId][botId]){
                    m_botnet->m_botNodes[asId]->Add(bth->GetNodeForAs(asId, nodeId));
                }
            }
        }

        if(type == BotnetType::CENTRALIZED)
        {
            /* centralized */
            /* choose bot master asId + nodeId */
            asId = rand() % m_numAs;
            nodeId = rand() % (m_botnet->m_botNodes[asId]->GetN());
            m_botnet->m_botMaster = mbotnet->m_botNodes[asId]->Get(nodeId);
        }
    }

    ApplicationContainer BotnetHelper::ApplicationInstallBot(NodeContainer c) const
    {
        ApplicationContainer apps;
        for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i)
        {
            apps.Add(InstallPrivBot(*i));
        }

        return apps;
    }

    ApplicationContainer BotnetHelper::ApplicationInstallBot(Ptr<Node> node) const
    {
        return ApplicationContainer(InstallPrivBot(node));
    }

    Ptr<Application> BotnetHelper::InstallPrivBot(Ptr<Node> node) const
    {
        Ptr<Application> app = m_botApp.Create<Application>();
        node->AddApplication(app);

        return app;
    }

    ApplicationContainer BotnetHelper::ApplicationInstallCC(NodeContainer c) const
    {
        ApplicationContainer apps;
        for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i)
        {
            apps.Add(InstallPrivCC(*i));
        }

        return apps;
    }

    ApplicationContainer BotnetHelper::ApplicationInstallCC(Ptr<Node> node) const
    {
        return ApplicationContainer(InstallPrivCC(node));
    }

    Ptr<Application> BotnetHelper::InstallPrivCC(Ptr<Node> node) const
    {
        Ptr<Application> app = m_ccApp.Create<Application>();
        node->AddApplication(app);

        return app;
    }

    void BotnetHelper::SetupAttack(std::string ccTypeId, std::string botTypeId)
    {
        // Application types defined
        m_ccApp.SetTypeId(ccTypeId);
        m_botApp.SetTypeId(botTypeId);

        m_botAppContainer = ApplicationInstallBot(m_botnet->m_botNodes);
        m_ccAppContainer = ApplicationInstallCC(m_botnet->m_botMaster);
    }

    void BotnetHelper::SetAttributeCC(std::string name, const AttributeValue& value)
    {
        m_ccApp.Set(name, value);
    }

    void BotnetHelper::SetAttributeBot(std::string name, const AttributeValue& value)
    {
        m_botApp.Set(name, value);
    }
}
