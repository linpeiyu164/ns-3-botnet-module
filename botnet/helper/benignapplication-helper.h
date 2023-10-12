#ifndef BOTNET_APPLICATION_HELPER_H
#define BOTNET_APPLICATION_HELPER_H

#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"

namespace ns3
{

class BenignApplicationHelper
{
  public:
    BenignApplicationHelper();
    ApplicationContainer Install(NodeContainer c) const;
    ApplicationContainer Install(Ptr<Node> node) const;
    void SetAttribute(std::string name, const AttributeValue& value);

  private:
    Ptr<Application> InstallPriv(Ptr<Node> node) const;
    ObjectFactory m_factory;
};

} // namespace ns3

#endif