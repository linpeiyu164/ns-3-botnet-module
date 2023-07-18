#include "ns3/botnet-helper.h"

#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>

namespace ns3
{
    NS_LOG_COMPONENT_DEFINE("BotnetHelper");

    BotnetHelper::BotnetHelper()
    {
        NS_LOG_FUNCTION(this);
        m_numAs = 0;
        m_numPerAs = 0;
        m_maxBotsPerAs = 0;
    }

    void BotnetHelper::SetupNodeMap()
    {
        NS_LOG_FUNCTION(this);
        if(m_numAs <= 0 || m_numPerAs <= 0){
            exit(1);
        }
        int i;
        for(i = 0; i < m_numAs; i++){
            m_nodeMap.push_back(std::vector<int>(m_numPerAs, 0));
        }
        NS_LOG_DEBUG("m_nodeMap dimension: (" << m_nodeMap.size() << ", " << m_nodeMap[0].size() << ")");
    }

    void BotnetHelper::CreateBotnet(
        // std::vector<NodeContainer*>& nodesByAs,
        BriteTopologyHelper* bth,
        int maxBotsPerAs,
        BotnetType type,
        std::string name)
    {
        NS_LOG_FUNCTION(this);

        m_numAs = bth->GetNAs();
        m_numPerAs = bth->GetNNodesForAs(0);
        m_maxBotsPerAs = maxBotsPerAs;

        NS_LOG_DEBUG("Num AS: " << m_numAs << ", Num per AS: " << m_numPerAs << ", bots per AS: " << maxBotsPerAs);

        SetupNodeMap();

        m_botnet = new Botnet(type, name);

        int asId, nodeId;
        int botId;

        srand(time(NULL));

        for(asId = 0; asId < m_numAs; asId++){
            m_botnet->m_botNodes.push_back(new NodeContainer());
            for(nodeId = 0; nodeId < m_maxBotsPerAs; nodeId++){
                botId = rand() % m_numPerAs;
                if(!m_nodeMap[asId][botId]){
                    m_botnet->m_botNodes[asId]->Add(bth->GetNodeForAs(asId, botId));
                    m_nodeMap[asId][botId] = 1;
                    m_botnet->m_size++;
                }
            }
            NS_LOG_DEBUG("Num nodes in container: " << m_botnet->m_botNodes[asId]->GetN());
        }

        NS_LOG_DEBUG("Total size: " << m_botnet->m_size);

        if(type == BotnetType::CENTRALIZED)
        {
            /* centralized */
            /* choose bot master asId + nodeId */

            do{
                asId = rand() % m_numAs;
                nodeId = rand() % (m_botnet->m_botNodes[asId]->GetN());
            }while(m_nodeMap[asId][nodeId]);

            m_botnet->m_botMaster = m_botnet->m_botNodes[asId]->Get(nodeId);
            m_nodeMap[asId][nodeId] = 2;
            m_botnet->m_size++;
        }

        NS_LOG_DEBUG("Total size: " << m_botnet->m_size);

    }

    ApplicationContainer BotnetHelper::ApplicationInstallBot(std::vector<NodeContainer*>& vc) const
    {
        NS_LOG_FUNCTION(this);
        ApplicationContainer apps;

        std::vector<NodeContainer*>::iterator it;
        for(it = vc.begin(); it != vc.end(); it++){
            for (NodeContainer::Iterator i = (*it)->Begin(); i != (*it)->End(); ++i)
            {
                apps.Add(InstallPrivBot(*i));
            }
        }

        return apps;
    }

    ApplicationContainer BotnetHelper::ApplicationInstallBot(Ptr<Node> node) const
    {
        NS_LOG_FUNCTION(this);
        return ApplicationContainer(InstallPrivBot(node));
    }

    Ptr<Application> BotnetHelper::InstallPrivBot(Ptr<Node> node) const
    {
        NS_LOG_FUNCTION(this);
        Ptr<Application> app = m_botApp.Create<Application>();
        node->AddApplication(app);

        return app;
    }

    ApplicationContainer BotnetHelper::ApplicationInstallCC(NodeContainer c) const
    {
        NS_LOG_FUNCTION(this);
        ApplicationContainer apps;

        for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i)
        {
            apps.Add(InstallPrivCC(*i));
        }

        return apps;
    }

    ApplicationContainer BotnetHelper::ApplicationInstallCC(Ptr<Node> node) const
    {
        NS_LOG_FUNCTION(this);
        return ApplicationContainer(InstallPrivCC(node));
    }

    Ptr<Application> BotnetHelper::InstallPrivCC(Ptr<Node> node) const
    {
        NS_LOG_FUNCTION(this);
        Ptr<Application> app = m_ccApp.Create<Application>();
        node->AddApplication(app);

        return app;
    }

    void BotnetHelper::SetupAttack(std::string ccTypeId, std::string botTypeId)
    {
        NS_LOG_FUNCTION(this << ccTypeId << botTypeId);
        // Application types defined
        m_ccApp.SetTypeId(ccTypeId);
        m_botApp.SetTypeId(botTypeId);


    }

    void BotnetHelper::InstallAttack()
    {
        m_botAppContainer = ApplicationInstallBot(m_botnet->m_botNodes);
        m_ccAppContainer = ApplicationInstallCC(m_botnet->m_botMaster);
    }

    void BotnetHelper::SetAttributeCC(std::string name, const AttributeValue& value)
    {
        NS_LOG_FUNCTION(this << name << &value);
        m_ccApp.Set(name, value);
    }

    void BotnetHelper::SetAttributeBot(std::string name, const AttributeValue& value)
    {
        NS_LOG_FUNCTION(this << name << &value);
        m_botApp.Set(name, value);
    }
}
