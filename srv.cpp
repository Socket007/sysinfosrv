#include "httplib.h"
#include "InitSock.h"
#include <vector>
#include <fstream>
#include <exception>
#include "tlhelp32.h"
using namespace httplib;
std::vector<std::string> vec;

static const std::string PROCESS_NAME = "sysinfosrv.exe";
//TCHAR PROCESS_NAME[] = L"sysinfosrv.exe";
std::string TCHAR2STRING(TCHAR *STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	delete chRtn;
	return str;
	
}
	
bool processExists()
{
	HANDLE handle; //定义CreateToolhelp32Snapshot系统快照句柄
	HANDLE handle1; //定义要结束进程句柄
	handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//获得系统快照句柄
	PROCESSENTRY32 *info; //定义PROCESSENTRY32结构字指
	//PROCESSENTRY32 结构的 dwSize 成员设置成 sizeof(PROCESSENTRY32)
	info = new PROCESSENTRY32;
	info->dwSize = sizeof(PROCESSENTRY32);
	//调用一次 Process32First 函数，从快照中获取进程列表
	Process32First(handle, info);
	std::string strExeFile = TCHAR2STRING(info->szExeFile);
	if (strExeFile.find(PROCESS_NAME) != -1){
		return true;
	}
	//if (wcscmp(PROCESS_NAME, info->szExeFile) == 0){
	//	return true;
	//}
	//重复调用 Process32Next，直到函数返回 FALSE 为止
	while (Process32Next(handle, info) != FALSE)
	{
		std::string strExeFile = TCHAR2STRING(info->szExeFile);
		if (strExeFile.find(PROCESS_NAME) != -1){
			return true;
		}
		//strcmp字符串比较函数同要结束相同
		//if (wcscmp(PROCESS_NAME, info->szExeFile) == 0){
		//	return true;
		//}
	}

	return false;
}

void GetAllIpAddress(std::vector<std::string>& vec)
{
	std::ofstream out("log.log");
	try{
		vec.clear();
		char cHost[256] = { 0 };
		::gethostname(cHost, 256);  //取得本地主机名
		hostent* pHost = ::gethostbyname(cHost);    //通过主机名得到地址信息
	
		//打印出所有Ip地址
		in_addr addr;
		for (int i = 0;; i++)
		{
			char*p = pHost->h_addr_list[i];
			if (p == NULL)
			{ 
				break;
			}
			memcpy(&addr.S_un.S_addr, p, pHost->h_length);
			char*cIp = ::inet_ntoa(addr);
			std::string ipAddr = cIp;
			std::cout << "IP:" << ipAddr << std::endl;
			vec.push_back(ipAddr);
		}
	}
	catch(std::exception e){
		out << "exception: " << e.what() << std::endl;
		out.flush();
		
	}
	out.close();

}


std::string dump_headers(const Headers &headers) {
  std::string s;
  char buf[BUFSIZ];

  for (auto it = headers.begin(); it != headers.end(); ++it) {
    const auto &x = *it;
    snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
    s += buf;
  }

  return s;
}

std::string log(const Request &req, const Response &res) {
  std::string s;
  char buf[BUFSIZ];

  s += "================================\n";

  snprintf(buf, sizeof(buf), "%s %s %s", req.method.c_str(),
           req.version.c_str(), req.path.c_str());
  s += buf;

  std::string query;
  for (auto it = req.params.begin(); it != req.params.end(); ++it) {
    const auto &x = *it;
    snprintf(buf, sizeof(buf), "%c%s=%s",
             (it == req.params.begin()) ? '?' : '&', x.first.c_str(),
             x.second.c_str());
    query += buf;
  }
  snprintf(buf, sizeof(buf), "%s\n", query.c_str());
  s += buf;

  s += dump_headers(req.headers);

  s += "--------------------------------\n";

  snprintf(buf, sizeof(buf), "%d %s\n", res.status, res.version.c_str());
  s += buf;
  s += dump_headers(res.headers);
  s += "\n";

  if (!res.body.empty()) { s += res.body; }

  s += "\n";

  return s;
}


int main(void)
{


  GetAllIpAddress(::vec);
  
  Server svr;

  svr.Get("/hi", [](const Request& req, Response& res) {
    res.set_content("Hello World!", "text/plain");
  });

  svr.Get(R"(/numbers/(\d+))", [&](const Request& req, Response& res) {
    auto numbers = req.matches[1];
    res.set_content(numbers, "text/plain");
  });


  svr.Get("/stop", [&](const Request& req, Response& res) {
    svr.stop();
  });
  
  //svr.Get(R"(/getip/(.*?))", [](const Request& req, Response& res) {
  svr.Get("/getip", [](const Request& req, Response& res) {
	std::cout << "### ip address size : " << ::vec.size() <<  std::endl;
	std::string callback = req.get_param_value("callback");
	std::cout << "callback:" << callback << std::endl;
	std::string resp = "";
	//auto jsonp = req.matches[1];
	auto jsonp = "xxx";
	//std::cout << "callback:" << callback << std::endl;
	
	if (::vec.size() == 0){
		resp = "None";
	}
	else{
		resp = ::vec[0];
		for (int i = 1; i < ::vec.size(); ++i){
			 resp += ",";
			 resp += ::vec[i];
		}
	}
	
	resp = callback + "('" + resp + "')";
	
	std::cout << "response: " << resp << std::endl;
	res.set_content(resp, "text/plain"); 
	//res.set_header("Content-type", "application/json; charset=UTF-8");
	//if (req.has_header("Origin") || req.has_header("origin")){
		
		//std::string http_origin = req.get_header_value("Origin");
		//std::cout << "Origin:" << req.has_header("Origin") << std::endl;
		//std::cout << "Access-Control-Allow-Origin:" << "*" << std::endl;
		//res.set_header("Access-Control-Allow-Origin", "*");
		//res.set_header("Access-Control-Allow-Credentials", "true");
        //res.set_header("Access-Control-Allow-Methods", "*");
        //res.set_header("Access-Control-Allow-Headers", "Content-Type,Accept,Authorization");
        //res.set_header("Access-Control-Max-Age", "86400");
	//}
	

  });
  
  svr.set_logger([](const Request &req, const Response &res) {
    printf("%s", ::log(req, res).c_str());
  });

  svr.listen("localhost", 9527);
}