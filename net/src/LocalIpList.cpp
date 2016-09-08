#include"LocalIpList.h"
#include<unistd.h>
#include<netdb.h>
#include<string.h>
#include<arpa/inet.h>

#include<algorithm>
using namespace std;

CLocalIpList::CLocalIpList()
{
	// TODO Auto-generated constructor stub

}

CLocalIpList::~CLocalIpList()
{
	// TODO Auto-generated destructor stub
}

int CLocalIpList::GetLocalIpList()
{
	this->m_iplist.clear();
	char name[128];
	if(-1 == gethostname(name, sizeof(name)))
	{
		return -1;
	}

	struct hostent *phost = NULL;
	phost = gethostbyname(name);
	if(NULL == phost)
	{
		return -2;
	}

	char **p = phost->h_addr_list;
	for(; *p!=NULL; ++p)
	{
		string strip = inet_ntoa(*(in_addr *)(*p));
		this->m_iplist.push_back(strip);
	}

	return (int)m_iplist.size();
}

int CLocalIpList::GetLocalIpList(vector<string> &ips)
{
	this->GetLocalIpList();
	ips = this->m_iplist;
	return (int)ips.size();
}

bool CLocalIpList::islocalIp(const char *ip)
{
	if(memcmp(ip, "127.0.0.1", sizeof("127.0.0.1")) == 0)
		return true;
	vector<string>::iterator it = find(this->m_iplist.begin(), this->m_iplist.end(), ip);
	if(it != this->m_iplist.end())
		return true;
	return false;
}
