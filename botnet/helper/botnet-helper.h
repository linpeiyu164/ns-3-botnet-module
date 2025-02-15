#ifndef BOTNET_HELPER_H
#define BOTNET_HELPER_H

#include "ns3/application-container.h"
#include "ns3/botnet.h"
#include "ns3/brite-topology-helper.h"

namespace ns3
{
enum class BotType
{
    UNINITIALIZED,
    CENTRAL_CONTROLLER,
    BOT,
    BENIGN
};

/* Helps initialize the botnet and launch attacks */
class BotnetHelper
{
  public:
    BotnetHelper();

    /* creates botnet based on topology */
    void CreateBotnet(BriteTopologyHelper* bth,
                      uint32_t maxBotsPerAS,
                      BotnetType type,
                      std::string name);

    /* creates benign nodes, which are the leftover leaf nodes*/
    void CreateBenignNodes(BriteTopologyHelper* bth);

    /*install all cc and bot applications*/
    void InstallApplications();

    /*add an application based on its typeid string*/
    void AddApplication(BotType type, std::string);

    /*get total number of bot nodes including central controller*/
    uint32_t GetNBots();

    /*get total number of benign nodes*/
    uint32_t GetNBenign();

    /*get bot with index num among all bots, does not consider which AS bot belongs to*/
    Ptr<Node> GetBot(uint16_t num);

    /*get benign node with index num among all benign nodes*/
    Ptr<Node> GetBenign(uint16_t num);

    Ptr<Node> GetCC();

    ApplicationContainer ApplicationInstallBot(std::vector<NodeContainer*>& c) const;
    ApplicationContainer ApplicationInstallBenign(std::vector<NodeContainer*>& c) const;
    ApplicationContainer ApplicationInstallCC(NodeContainer c) const;

    ApplicationContainer m_ccAppContainer;
    ApplicationContainer m_botAppContainer;
    ApplicationContainer m_benignAppContainer;

  private:
    Botnet* m_botnet;
    uint32_t m_numAs;
    std::vector<uint32_t> m_numLeafPerAs;
    uint32_t m_maxBotsPerAs;
    std::vector<std::vector<BotType>> m_nodeMap;
    std::vector<ObjectFactory> m_ccApps;
    std::vector<ObjectFactory> m_botApps;
    std::vector<ObjectFactory> m_benignApps;

    /* initializes node map, tracks bot assignment in topology */
    void SetupNodeMap();

    ApplicationContainer ApplicationInstallBot(Ptr<Node> node) const;
    ApplicationContainer InstallPrivBot(Ptr<Node> node) const;

    ApplicationContainer ApplicationInstallCC(Ptr<Node> node) const;
    ApplicationContainer InstallPrivCC(Ptr<Node> node) const;

    ApplicationContainer ApplicationInstallBenign(Ptr<Node> node) const;
    ApplicationContainer InstallPrivBenign(Ptr<Node> node) const;

  public:
    void SetAttributeCC(uint16_t appIndex, std::string name, const AttributeValue& value);
    void SetAttributeBot(uint16_t appIndex, std::string name, const AttributeValue& value);
    void SetAttributeBenign(uint16_t appIndex, std::string name, const AttributeValue& value);
    Ipv4Address GetBotMasterAddress(uint16_t netDeviceIndex);
    uint32_t GetBotMasterNodeId();
};

} // namespace ns3

#endif /* BOTNET_HELPER_H */
