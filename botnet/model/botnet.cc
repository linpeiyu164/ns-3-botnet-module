#include "botnet.h"
#include "string.h"

namespace ns3
{

/* ... */
Botnet::Botnet(int type, std::string name){
    m_type = type;
    m_name = name;
}

}
