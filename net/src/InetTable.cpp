#include"InetTable.h"
#include"../../base/src/commfunc.h"


CInetTable::CInetTable()
{
	// TODO Auto-generated constructor stub
	this->m_iptables.clear();
}

CInetTable::~CInetTable()
{
	// TODO Auto-generated destructor stub
	this->ClearIpAddr();
}

int CInetTable::GetIpAddr(const string &szID, CInetAddr *addrp)
{
	if(m_iptables.find(szID, addrp) != 0)
	{
		return -1;
	}
	return 0;
}

void CInetTable::AddIpAddr(const string &szID,  const CInetAddr *addrp)
{
	CInetAddr *newAddrp = NULL;
	if(m_iptables.find(szID, newAddrp) != 0)
	{
		u_short uPort = addrp->GetPort();
		const char *ip = addrp->GetIp().c_str();
		newAddrp = new CInetAddr(uPort, ip);
	}
	else{
		memcpy(newAddrp, addrp, sizeof(CInetAddr));
	}
	this->m_iptables.set(szID, newAddrp);
}

bool CInetTable::RemoveIpAddr(const string &szID)
{
	CInetAddr *addrp = NULL;
	if(m_iptables.find(szID, addrp) == 0)
	{
		m_iptables.remove(szID);
		delete_memory(addrp);
		return true;
	}
	return false;
}

void CInetTable::ClearIpAddr()
{
	vector<CInetAddr *> vInetAddrs;
	m_iptables.get_all_values(vInetAddrs);
	for(size_t i=0; i < vInetAddrs.size(); ++i)
	{
		CInetAddr *tmp = vInetAddrs[i];
		delete_memory(tmp);
	}
	m_iptables.clear();
}

