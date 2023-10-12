#include "ns3/botnet-helper.h"

#include <cstdlib>
#include <ctime>
#include <string>
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

void
BotnetHelper::SetupNodeMap()
{
    NS_LOG_FUNCTION(this);
    if (m_numAs <= 0 || m_numPerAs <= 0)
    {
        exit(1);
    }
    int i;
    for (i = 0; i < m_numAs; i++)
    {
        m_nodeMap.push_back(std::vector<BotType>(m_numPerAs, BotType::UNINITIALIZED));
    }
    NS_LOG_DEBUG("m_nodeMap dimension: (" << m_nodeMap.size() << ", " << m_nodeMap[0].size()
                                          << ")");
}

void
BotnetHelper::CreateBotnet(
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

    NS_LOG_DEBUG("Num AS: " << m_numAs << ", Num per AS: " << m_numPerAs
                            << ", bots per AS: " << maxBotsPerAs);

    SetupNodeMap();

    m_botnet = new Botnet(type, name);

    int asId, nodeId;
    int botId;

    srand(time(NULL));

    for (asId = 0; asId < m_numAs; asId++)
    {
        m_botnet->m_botNodes.push_back(new NodeContainer());
        for (nodeId = 0; nodeId < m_maxBotsPerAs; nodeId++)
        {
            botId = rand() % m_numPerAs;
            if (m_nodeMap[asId][botId] == BotType::UNINITIALIZED)
            {
                m_botnet->m_botNodes[asId]->Add(bth->GetNodeForAs(asId, botId));
                m_nodeMap[asId][botId] = BotType::BOT;
                m_botnet->m_size++;
            }
        }
        NS_LOG_DEBUG("Num nodes in container: " << m_botnet->m_botNodes[asId]->GetN());
    }

    NS_LOG_DEBUG("Total size: " << m_botnet->m_size);

    if (type == BotnetType::CENTRALIZED)
    {
        /* centralized */
        /* choose bot master asId + nodeId */

        do
        {
            asId = rand() % m_numAs;
            nodeId = rand() % (m_botnet->m_botNodes[asId]->GetN());
        } while (m_nodeMap[asId][nodeId] == BotType::BOT);

        m_botnet->m_botMaster = m_botnet->m_botNodes[asId]->Get(nodeId);
        m_nodeMap[asId][nodeId] = BotType::CENTRAL_CONTROLLER;
        m_botnet->m_botMasterAsId = asId;
        m_botnet->m_botMasterNodeId = nodeId;
        m_botnet->m_size++;
    }

    /*Add benign nodes*/
    for (asId = 0; asId < m_numAs; asId++)
    {
        m_botnet->m_benignNodes.push_back(new NodeContainer());
        for (nodeId = 0; nodeId < m_numPerAs; nodeId++)
        {
            if (m_nodeMap[asId][nodeId] == BotType::UNINITIALIZED)
            {
                m_nodeMap[asId][nodeId] = BotType::BENIGN;
                m_botnet->m_benignNodes[asId]->Add(bth->GetNodeForAs(asId, nodeId));
            }
        }
        NS_LOG_INFO("Added benign nodes");
    }

    NS_LOG_DEBUG("Size of botnet: " << m_botnet->m_size);
}

void
BotnetHelper::AddApplication(BotType type, std::string typeId)
{
    ObjectFactory app;

    if (type == BotType::BOT)
    {
        m_botApps.push_back(app);
        m_botApps[m_botApps.size() - 1].SetTypeId(typeId);
    }
    else if (type == BotType::CENTRAL_CONTROLLER)
    {
        m_ccApps.push_back(app);
        m_ccApps[m_ccApps.size() - 1].SetTypeId(typeId);
    }
    else if (type == BotType::BENIGN)
    {
        m_benignApps.push_back(app);
        m_benignApps[m_benignApps.size() - 1].SetTypeId(typeId);
    }
    else
    {
        NS_LOG_ERROR("Error: no BotType " << uint16_t(type));
    }
}

ApplicationContainer
BotnetHelper::ApplicationInstallBot(std::vector<NodeContainer*>& vc) const
{
    NS_LOG_FUNCTION(this);
    ApplicationContainer apps;

    std::vector<NodeContainer*>::iterator it;
    for (it = vc.begin(); it != vc.end(); it++)
    {
        for (NodeContainer::Iterator i = (*it)->Begin(); i != (*it)->End(); ++i)
        {
            apps.Add(InstallPrivBot(*i));
        }
    }

    return apps;
}

ApplicationContainer
BotnetHelper::ApplicationInstallBot(Ptr<Node> node) const
{
    NS_LOG_FUNCTION(this);
    return ApplicationContainer(InstallPrivBot(node));
}

ApplicationContainer
BotnetHelper::InstallPrivBot(Ptr<Node> node) const
{
    NS_LOG_FUNCTION(this);
    ApplicationContainer apps;

    for (auto it = m_botApps.begin(); it != m_botApps.end(); it++)
    {
        Ptr<Application> app = it->Create<Application>();
        node->AddApplication(app);
        apps.Add(app);
    }

    return apps;
}

ApplicationContainer
BotnetHelper::ApplicationInstallCC(NodeContainer c) const
{
    NS_LOG_FUNCTION(this);
    ApplicationContainer apps;

    for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    {
        apps.Add(InstallPrivCC(*i));
    }

    return apps;
}

ApplicationContainer
BotnetHelper::ApplicationInstallCC(Ptr<Node> node) const
{
    NS_LOG_FUNCTION(this);
    return ApplicationContainer(InstallPrivCC(node));
}

ApplicationContainer
BotnetHelper::InstallPrivCC(Ptr<Node> node) const
{
    NS_LOG_FUNCTION(this);
    ApplicationContainer apps;
    for (auto it = m_ccApps.begin(); it != m_ccApps.end(); it++)
    {
        Ptr<Application> app = it->Create<Application>();
        node->AddApplication(app);
        apps.Add(app);
    }

    return apps;
}

void
BotnetHelper::InstallApplications()
{
    m_botAppContainer = ApplicationInstallBot(m_botnet->m_botNodes);
    m_ccAppContainer = ApplicationInstallCC(m_botnet->m_botMaster);
    m_benignAppContainer = ApplicationInstallBenign(m_botnet->m_benignNodes);
}

void
BotnetHelper::SetAttributeCC(uint16_t appIndex, std::string name, const AttributeValue& value)
{
    // appIndex: the index of the application in m_ccApps
    NS_LOG_FUNCTION(this << appIndex << name << &value);
    m_ccApps[appIndex].Set(name, value);
}

void
BotnetHelper::SetAttributeBot(uint16_t appIndex, std::string name, const AttributeValue& value)
{
    // appIndex: the index of the application in m_ccApps
    NS_LOG_FUNCTION(this << appIndex << name << &value);
    m_botApps[appIndex].Set(name, value);
}

void
BotnetHelper::SetAttributeBenign(uint16_t appIndex, std::string name, const AttributeValue& value)
{
    NS_LOG_FUNCTION(this << appIndex << name << &value);
    m_benignApps[appIndex].Set(name, value);
}

Ipv4Address
BotnetHelper::GetBotMasterAddress(uint16_t netDeviceIndex)
{
    return Ipv4Address::ConvertFrom(
        m_botnet->m_botMaster->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal());
}

ApplicationContainer
BotnetHelper::ApplicationInstallBenign(std::vector<NodeContainer*>& vc) const
{
    NS_LOG_FUNCTION(this);
    ApplicationContainer apps;

    std::vector<NodeContainer*>::iterator it;
    for (it = vc.begin(); it != vc.end(); it++)
    {
        for (NodeContainer::Iterator i = (*it)->Begin(); i != (*it)->End(); ++i)
        {
            apps.Add(InstallPrivBenign(*i));
        }
    }

    return apps;
}

ApplicationContainer
BotnetHelper::ApplicationInstallBenign(Ptr<Node> node) const
{
    NS_LOG_FUNCTION(this);
    return ApplicationContainer(InstallPrivBenign(node));
}

ApplicationContainer
BotnetHelper::InstallPrivBenign(Ptr<Node> node) const
{
    NS_LOG_FUNCTION(this);
    ApplicationContainer apps;

    for (auto it = m_benignApps.begin(); it != m_benignApps.end(); it++)
    {
        Ptr<Application> app = it->Create<Application>();
        node->AddApplication(app);
        apps.Add(app);
    }

    return apps;
}

} // namespace ns3
