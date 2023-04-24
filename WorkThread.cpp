#include "stdafx.h"
#include "WorkThread.h"

#include <QTimer>
#include <QEventLoop>
#include "ChargeDev.h"
#include "extern.h"


//方法1：在1个线程里run :qthread.run(){ dowork() }
//QThread只有run函数是在新线程里的，其他所有函数都在QThread生成的线程里

//方法2：直接继承QObject实现多线程
//QObject的线程转移函数是：void moveToThread(QThread * targetThread) ，通过此函数可以把一个顶层Object（就是没有父级）转移到一个新的线程里

//Qt4.8之前都是使用继承QThread的run这种方法1，但是Qt4.8之后，Qt官方建议使用第二种方法
//所以,这里我们采用方法2:
//UI主线程dojobs信号->新线程doWork()，执行完最后发workdone信号->主线程里再次触发dojbos

static const char* ConvertQstringToChar(QString str, char *szStr=NULL)
{
	static char *tmp = NULL;
	static int lMaxLen = 0;
	if (str.length()+1>lMaxLen)
	{
		lMaxLen = str.length()+1;
		if (tmp)
		{
			delete tmp;
		}

		tmp = new char[lMaxLen];
		memset(tmp, 0, lMaxLen);
	}

	QByteArray ba = str.toLocal8Bit();
	strcpy(tmp, ba.data());
	if (szStr!=NULL)
	{
		strcpy(szStr, ba.data());
	}

	return tmp;
}

CWorker::CWorker(QObject *parent)
{
	m_streamCount = 0;
	m_pParent = parent;
	pThread = NULL;
	m_jobid=0;

	for (int i=0; i<MAX_CHARGE_NUM_ONE_WORKTHREAD; i++)
		m_nodes[i] = 0;
}

CWorker::~CWorker()
{
	ClosePool();
	m_streamCount = 0;
	pThread->quit();
	pThread->wait();
}

void CWorker::Set_TcpServer(char*ip,int port)
{
	memset(tcpserver_ip,0,sizeof(tcpserver_ip));
	strcpy(tcpserver_ip,ip);

	tcpserver_port=port;
}

void CWorker::taskDelay(int ms)
{
	QEventLoop eventloop;
	QTimer::singleShot(ms, &eventloop, SLOT(quit()));
	eventloop.exec();
}

///need to do 
