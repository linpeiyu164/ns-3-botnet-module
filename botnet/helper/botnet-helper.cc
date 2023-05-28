#include "botnet-helper.h"

#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>

namespace ns3
{

BotnetHelper::BotnetHelper(){
    m_numAs = 0;
    m_numPerAs = 0;
    m_maxBotsPerAs = 0;
}

void BotnetHelper::SetupNodeMap(){
    if(m_numAs <= 0 || m_numPerAs <= 0){
        exit(1);
    }
    int i, j;
    for(i = 0; i < m_numAs; i++){
        m_nodeMap.push_back(std::vector<int>(m_numPerAs, 0));
    }
}

void BotnetHelper::CreateBotnet(
    std::vector<NodeContainer*>& nodesByAs,
    int maxBotsPerAs,
    int type,
    std::string name)
{
    m_numAs = nodesByAs.size();
    m_numPerAs = nodesByAs[0]->GetN();
    m_maxBotsPerAs = maxBotsPerAs;

    SetupNodeMap();

    m_botnet = Botnet(type, name);

    int asId, nodeId;
    int botId;

    srand(time(NULL));

    for(asId = 0; asId < m_numAs; asId++){
        for(nodeId = 0; nodeId < m_maxBotsPerAs; nodeId++){
            botId = rand() % m_numPerAs;
            if(!m_nodeMap[asId][botId]){
                m_botnet.m_botNodes[asId]->Add(nodesByAs[asId]->Get(nodeId));
            }
        }
    }
}

}
