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
    m_numLeafPerAs = std::vector<uint32_t>(m_numAs, 0);
    m_maxBotsPerAs = 0;
}

void
BotnetHelper::SetupNodeMap()
{
    NS_LOG_FUNCTION(this);

    uint32_t asId;
    for (asId = 0; asId < m_numAs; asId++)
    {
        m_nodeMap.push_back(std::vector<BotType>(m_numLeafPerAs[asId], BotType::UNINITIALIZED));
    }
}

void
BotnetHelper::CreateBotnet(BriteTopologyHelper* bth,
                           uint32_t maxBotsPerAs,
                           BotnetType type,
                           std::string name)
{
    NS_LOG_FUNCTION(this);

    uint32_t asId, leafId, count;

    // Get number of ASes
    m_numAs = bth->GetNAs();
    if (m_numAs > 0)
    {
        NS_LOG_INFO("Number of ASes: " << m_numAs);
    }
    else
    {
        NS_LOG_ERROR("Number of ASes is zero");
        exit(1);
    }

    // Get number of leaf nodes in each AS
    for (asId = 0; asId < m_numAs; asId++)
    {
        uint32_t num = bth->GetNLeafNodesForAs(asId);

        NS_LOG_INFO("AS: " << asId << ", Leaf nodes: " << num);
        m_numLeafPerAs.push_back(num);
    }

    m_maxBotsPerAs = maxBotsPerAs;

    SetupNodeMap();

    m_botnet = new Botnet(type, name);

    srand(time(NULL));

    for (asId = 0; asId < m_numAs; asId++)
    {
        m_botnet->m_botNodes.push_back(new NodeContainer());

        // make sure leaf nodes exist in asId, otherwise modular will fail
        if (m_numLeafPerAs[asId] > 0)
        {
            for (count = 0; count < m_maxBotsPerAs; count++)
            {
                leafId = rand() % m_numLeafPerAs[asId];
                if (m_nodeMap[asId][leafId] == BotType::UNINITIALIZED)
                {
                    m_botnet->m_botNodes[asId]->Add(bth->GetLeafNodeForAs(asId, leafId));
                    m_nodeMap[asId][leafId] = BotType::BOT;
                    m_botnet->m_size++;
                }
            }
        }
        NS_LOG_DEBUG("Number of bots in AS " << asId << ": " << m_botnet->m_botNodes[asId]->GetN());
    }

    // BotnetType
    if (type == BotnetType::CENTRALIZED)
    {
        // Add central controller
        // If your code ever gets stuck here, make the topology bigger
        do
        {
            // Choose AS with leaf nodes
            asId = rand() % m_numAs;
            while (m_numLeafPerAs[asId] <= 0)
            {
                asId = rand() % m_numAs;
            }
            // Choose a random leaf node
            leafId = rand() % (m_numLeafPerAs[asId]);

        } while (m_nodeMap[asId][leafId] == BotType::BOT);

        m_botnet->m_botMaster = bth->GetLeafNodeForAs(asId, leafId);
        m_nodeMap[asId][leafId] = BotType::CENTRAL_CONTROLLER;
        m_botnet->m_botMasterAsId = asId;
        m_botnet->m_botMasterLeafId = leafId;
        m_botnet->m_size++;

        NS_LOG_INFO("Central controller: (ASIndex, LeafIndex) = ("
                    << m_botnet->m_botMasterAsId << ", " << m_botnet->m_botMasterLeafId << ")");
        NS_LOG_INFO("Botnet size: " << m_botnet->m_size);
    }
    else
    {
        NS_LOG_ERROR("BotnetType" << uint16_t(type) << " not yet supported");
    }
}

void
BotnetHelper::CreateBenignNodes(BriteTopologyHelper* bth)
{
    // Add benign nodes
    uint32_t asId, leafId;
    for (asId = 0; asId < m_numAs; asId++)
    {
        m_botnet->m_benignNodes.push_back(new NodeContainer());
        for (leafId = 0; leafId < m_numLeafPerAs[asId]; leafId++)
        {
            if (m_nodeMap[asId][leafId] == BotType::UNINITIALIZED)
            {
                m_nodeMap[asId][leafId] = BotType::BENIGN;
                m_botnet->m_benignNodes[asId]->Add(bth->GetLeafNodeForAs(asId, leafId));
            }
        }
        NS_LOG_INFO("AS " << asId << ", Benign nodes: " << m_botnet->m_benignNodes[asId]->GetN());
    }
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
        NS_LOG_ERROR("BotType " << uint16_t(type) << " not supported");
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
