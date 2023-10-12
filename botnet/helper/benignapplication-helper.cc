#include "benignapplication-helper.h"

namespace ns3
{

BenignApplicationHelper::BenignApplicationHelper()
{
    m_factory.SetTypeId("ns3::BenignApplciation");
}

ApplicationContainer
BenignApplicationHelper::Install(NodeContainer c) const
{
    ApplicationContainer apps;
    for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    {
        apps.Add(InstallPriv(*i));
    }

    return apps;
}

ApplicationContainer
BenignApplicationHelper::Install(Ptr<Node> node) const
{
    return ApplicationContainer(InstallPriv(node));
}

void
BenignApplicationHelper::SetAttribute(std::string name, const AttributeValue& value)
{
    m_factory.Set(name, value);
}

Ptr<Application>
BenignApplicationHelper::InstallPriv(Ptr<Node> node) const
{
    Ptr<Application> app = m_factory.Create<Application>();
    node->AddApplication(app);

    return app;
}

} // namespace ns3