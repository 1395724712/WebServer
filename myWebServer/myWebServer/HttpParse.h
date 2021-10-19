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
	//�������󷽷�����ʱֻʵ��GET����
	enum METHOD{GET,POST,HEAD,PUT,DELETE,TRACE,OPTIONS,CONNECT,PATH};
	//���Ľ������
	enum HTTPCODE{NO_REQUEST,GET_REQUEST,BAD_REQUEST,NO_RESOURCE,FORBIDDEN_REQUEST,FILE_REQUEST,INTERNAL_ERROR,CLOSED_CONNECTION};
	//��״̬��״̬
	enum CHECKSTATE { CHECK_STATE_REQUESTLINE, CHECK_STATE_HEADER, CHECK_STATE_CONTENT };
	//��״̬��״̬
	enum LINESTATUS{LINE_OK,LINE_BAD,LINE_OPEN};
	//������ɺ�Ļص�����:���ͣ��ر����ӣ�
	typedef std::function<void(HttpParse::HTTPCODE,string)> FinishCallback;

public:
	HttpParse(FinishCallback parsefinishCallback);
	~HttpParse();

	//����������
	void getRequestMsg(string& msg);
	
	///��������
	HTTPCODE praseRequest();
	//����Ӧ����
	bool request();

private:
	
	//��״̬�����������е�����������
	HTTPCODE parseRequestLine(string& line);
	//��״̬�����������е�����ͷ����
	HTTPCODE parseHeaders(string& line);
	//��״̬�����������е���������
	HTTPCODE parseContent(string& line);

	//��״̬����ȡһ�У������Ǳ��ĵ���һ����
	LINESTATUS parseLine(string& line);
	
	//������Ӧ���ĵİ˸�����
	bool addRequest(string& format);
	bool addContent(string& content);
	bool addStatusLine(int status, string& title);
	bool addHeaders(int contentLength);
	bool addContentType();
	bool addContentLength(int contentLength);
	bool addLinger();
	bool addBlankline();
	
private:
	//���ڱ��������ĵ���
	pthread_mutex_t mutex_;
	//������ɺ�Ļص�����:���ͣ��ر����ӣ�
	FinishCallback parsefinishCallback_;
	//��״̬��״̬
	CHECKSTATE mainState_;	
	//���󷽷�
	METHOD method_;
	//���������Դ
	string url_;
	//�汾��
	string version_;
	//������
	string requestMsg_ GUARDED_BY(mutex_);
	//Ӧ������
	string msg_;
	
};

