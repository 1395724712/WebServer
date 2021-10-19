#include "HttpParse.h"
#include<assert.h>
#include<string.h>
#include<sstream>
HttpParse::HttpParse(FinishCallback parsefinishCallback) :
	parsefinishCallback_(parsefinishCallback),
	mainState_(CHECK_STATE_REQUESTLINE)
{
	pthread_mutex_init(&mutex_, nullptr);
}

HttpParse::~HttpParse()
{
	pthread_mutex_destroy(&mutex_);
}

void HttpParse::getRequestMsg(string & msg)
{
	pthread_mutex_lock(&mutex_);
	requestMsg_ += msg;
	pthread_mutex_unlock(&mutex_);

	pthread_t threadId;
	int rtnVal = pthread_create(&threadId, nullptr, &detail::prase, (void*)this);
}

HttpParse::HTTPCODE HttpParse::praseRequest()
{
	string line;
	LINESTATUS lineStatus = parseLine(line);

	while(lineStatus == LINE_OK)
	{
		HTTPCODE rtnVal;
		switch(mainState_)
		{
		case CHECK_STATE_REQUESTLINE:
			rtnVal = parseHeaders(line);
			if(rtnVal==BAD_REQUEST)
			{
				parsefinishCallback_(BAD_REQUEST, "");
				return BAD_REQUEST;
			}
			break;
		case CHECK_STATE_HEADER:
			rtnVal = parseHeaders(line);
			if (rtnVal == BAD_REQUEST)
			{
				parsefinishCallback_(BAD_REQUEST, "");
				return BAD_REQUEST;
			}
			else if(rtnVal = GET_REQUEST)
			{
				request();
				parsefinishCallback_(GET_REQUEST, msg_);
			}
			break;
		case CHECK_STATE_CONTENT:
			parsefinishCallback_(BAD_REQUEST, "");
			break;
		default:
			parsefinishCallback_(BAD_REQUEST, "");
			break;
		}

		line = "";
		lineStatus = parseLine(line);
	}
	
	return NO_REQUEST;
}

HttpParse::HTTPCODE HttpParse::parseRequestLine(string & line)
{
	int begIdx = 0;
	int endIdx = 0;

	//������method
	while(line[begIdx]==' ')
	{
		begIdx++;
		endIdx++;
	}

	while (line[endIdx] != ' ')
		endIdx++;

	string tmpStr = line.substr(begIdx, endIdx - begIdx);
	if (tmpStr == "GET")
		method_ = GET;
	else if (tmpStr == "POST")
		method_ = POST;
	else
		return BAD_REQUEST;

	//������url
	url_ ="/";
	begIdx = endIdx;
	while(line[begIdx]==' ')
	{
		begIdx++;
		endIdx++;
	}
	while (line[endIdx] != ' ')
		endIdx++;
	if (line.substr(begIdx, begIdx + 7) == "http://")
		url_ += line.substr(begIdx + 7, endIdx - begIdx - 7);
	else if (line.substr(begIdx, begIdx + 8) == "https://")
		url_ += line.substr(begIdx + 8, endIdx - begIdx - 7);
	else
		return BAD_REQUEST;

	//����ǰ汾��
	begIdx = endIdx;
	while(line[begIdx]==' ')
	{
		begIdx++;
		endIdx++;
	}
	while (line[endIdx] != ' ')
		endIdx++;
	version_ = line.substr(begIdx, endIdx - begIdx);
	if (version_ != "HTTP/1.1")
		return BAD_REQUEST;

	mainState_ = CHECK_STATE_HEADER;
	return NO_REQUEST;
}

HttpParse::HTTPCODE HttpParse::parseHeaders(string & line)
{
	//todo:��������ʱֻ����GET����
	mainState_ = CHECK_STATE_CONTENT;
	if (line == "")
	{
		return GET_REQUEST;
	}

	return NO_REQUEST;
}

HttpParse::HTTPCODE HttpParse::parseContent(string & line)
{
	return BAD_REQUEST;
}

HttpParse::LINESTATUS HttpParse::parseLine(string & line)
{
	//todo:�������������ʱ�任�ռ�
	//���Ҫ�ĳ��ÿռ任ʱ�䣺
	//��¼ÿ�ο�ʼ��λ�ã��ӿ�ʼ��λ�ÿ�ʼɨ�裬�Ͳ�Ҫÿ�ζ�ɨ�������ַ����ˡ�
	//requestMsg_Ҳ�����ͷ���
	assert(line == "");
	pthread_mutex_lock(&mutex_);
	string tmpStr = requestMsg_;
	pthread_mutex_unlock(&mutex_);
	for(string::iterator itr=tmpStr.begin();itr<tmpStr.end();itr++)
	{
		if(*itr=='\r')
		{
			if (itr + 1 == tmpStr.end())
			{
				return LINE_OPEN;
			}
			else if (*(itr + 1) == '\n')
			{
				line = tmpStr.substr(0, itr - tmpStr.begin());
				requestMsg_.erase(0, itr - tmpStr.begin() + 2);
				return LINE_OK;
			}
			else
				return LINE_BAD;
		}
	}
	
	return LINE_OPEN;
}

void * detail::prase(void * HttpParseObj)
{
	((HttpParse*)HttpParseObj)->praseRequest();
}
