#include "ns3/pulsingattack-helper.h"

#include "ns3/inet-socket-address.h"

namespace ns3
{

/* Pulsing Attack CC Helper */
PulsingAttackCCHelper::PulsingAttackCCHelper()
{
    m_factory.SetTypeId("ns3::PulsingAttackCC");
}

void
PulsingAttackCCHelper::SetAttribute(std::string name, const AttributeValue& value)
{
    m_factory.Set(name, value);
}

ApplicationContainer
PulsingAttackCCHelper::Install(NodeContainer c) const
{
    ApplicationContainer apps;
    for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    {
        apps.Add(InstallPriv(*i));
    }

    return apps;
}

ApplicationContainer
PulsingAttackCCHelper::Install(Ptr<Node> node) const
{
    return ApplicationContainer(InstallPriv(node));
}

Ptr<Application>
PulsingAttackCCHelper::InstallPriv(Ptr<Node> node) const
{
    Ptr<Application> app = m_factory.Create<Application>();
    node->AddApplication(app);

    return app;
}

/* Pulsing Attack Bot Helper */
PulsingAttackBotHelper::PulsingAttackBotHelper()
{
    m_factory.SetTypeId("ns3::PulsingAttackBot");
}

void
PulsingAttackBotHelper::SetAttribute(std::string name, const AttributeValue& value)
{
    m_factory.Set(name, value);
}

ApplicationContainer
PulsingAttackBotHelper::Install(NodeContainer c) const
{
    ApplicationContainer apps;
    for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    {
        apps.Add(InstallPriv(*i));
    }

    return apps;
}

ApplicationContainer
PulsingAttackBotHelper::Install(Ptr<Node> node) const
{
    return ApplicationContainer(InstallPriv(node));
}

Ptr<Application>
PulsingAttackBotHelper::InstallPriv(Ptr<Node> node) const
{
    Ptr<Application> app = m_factory.Create<Application>();
    node->AddApplication(app);

    return app;
}
} // namespace ns3
