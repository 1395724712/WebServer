#pragma once
#include<functional>
#include<string>
#include<pthread.h>

#if defined(__clang__) && (!defined(SWIG))
#define THREAD_ANNOTATION_ATTRIBUTE__(x) __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x) // no-op
#endif

#define GUARDED_BY(x) \
    THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

namespace detail
{
	void* prase(void* HttpParseObj);
}

using namespace std;
class HttpParse
{
public:
	//报文请求方法，暂时只实现GET方法
	enum METHOD{GET,POST,HEAD,PUT,DELETE,TRACE,OPTIONS,CONNECT,PATH};
	//报文解析结果
	enum HTTPCODE{NO_REQUEST,GET_REQUEST,BAD_REQUEST,NO_RESOURCE,FORBIDDEN_REQUEST,FILE_REQUEST,INTERNAL_ERROR,CLOSED_CONNECTION};
	//主状态机状态
	enum CHECKSTATE { CHECK_STATE_REQUESTLINE, CHECK_STATE_HEADER, CHECK_STATE_CONTENT };
	//从状态机状态
	enum LINESTATUS{LINE_OK,LINE_BAD,LINE_OPEN};
	//解析完成后的回调函数:发送？关闭连接？
	typedef std::function<void(HttpParse::HTTPCODE,string)> FinishCallback;

public:
	HttpParse(FinishCallback parsefinishCallback);
	~HttpParse();

	//读入请求报文
	void getRequestMsg(string& msg);
	
	///解析报文
	HTTPCODE praseRequest();
	//产生应答报文
	bool request();

private:
	
	//主状态机解析报文中的请求行数据
	HTTPCODE parseRequestLine(string& line);
	//主状态机解析报文中的请求头数据
	HTTPCODE parseHeaders(string& line);
	//主状态机解析报文中的请求内容
	HTTPCODE parseContent(string& line);

	//从状态机读取一行，解析是报文的那一部分
	LINESTATUS parseLine(string& line);
	
	//生成响应报文的八个部分
	bool addRequest(string& format);
	bool addContent(string& content);
	bool addStatusLine(int status, string& title);
	bool addHeaders(int contentLength);
	bool addContentType();
	bool addContentLength(int contentLength);
	bool addLinger();
	bool addBlankline();
	
private:
	//用于保护请求报文的锁
	pthread_mutex_t mutex_;
	//解析完成后的回调函数:发送？关闭连接？
	FinishCallback parsefinishCallback_;
	//主状态机状态
	CHECKSTATE mainState_;	
	//请求方法
	METHOD method_;
	//所请求的资源
	string url_;
	//版本号
	string version_;
	//请求报文
	string requestMsg_ GUARDED_BY(mutex_);
	//应答数据
	string msg_;
	
};

