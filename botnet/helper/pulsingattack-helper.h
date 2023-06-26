#ifndef PULSING_ATTACK_CC_HELPER_H
#define PULSING_ATTACK_CC_HELPER_H
#include "ns3/application-container.h"
// #include "ns3/ipv4-address.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"


namespace ns3
{
    class PulsingAttackCCHelper
    {
        public:
            PulsingAttackCCHelper();
            ApplicationContainer Install(NodeContainer c) const;
            ApplicationContainer Install(Ptr<Node> node) const;
            void SetAttribute(std::string name, const AttributeValue& value);
        private:
            Ptr<Application> InstallPriv(Ptr<Node> node) const;
            ObjectFactory m_factory;
    };

    class PulsingAttackBotHelper
    {
        public:
            PulsingAttackBotHelper();
            ApplicationContainer Install(NodeContainer c) const;
            ApplicationContainer Install(Ptr<Node> node) const;
            void SetAttribute(std::string name, const AttributeValue& value);
        private:
            Ptr<Application> InstallPriv(Ptr<Node> node) const;
            ObjectFactory m_factory;
    };
}
#endif