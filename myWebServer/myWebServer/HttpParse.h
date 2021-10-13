#pragma once
#include<functional>
#include<string>
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
	//todo:��Ȼ�ǰ��н��������ҿ����ڽ������ǰ�ȼ�顰\n��,���û�оͲ��������
	enum LINESTATUS{LINE_OK,LINE_BAD,LINE_OPEN};
	//������ɺ�Ļص�����:���ͣ��ر����ӣ�
	typedef std::function<void()> FinishCallback;

public:
	HttpParse(FinishCallback parsefinishCallback);
	~HttpParse();
	
	//��������
	void prase(string& line);
	//����Ӧ����
	string request();

private:
	//��״̬�����������е�����������
	HTTPCODE parseRequestLine(string& line);
	//��״̬�����������е�����ͷ����
	HTTPCODE parseHeaders(string& line);
	//��״̬�����������е���������
	HTTPCODE parseContent(string& line);

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
	//������ɺ�Ļص�����:���ͣ��ر����ӣ�
	FinishCallback parsefinishCallback_;
	//��״̬��״̬
	CHECKSTATE mainState_;
	//���󷽷�
	METHOD method_;
	//Ӧ������
	string msg_;
	
};

