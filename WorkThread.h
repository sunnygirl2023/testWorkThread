#ifndef WORKTHREAD_H
#define WORKTHREAD_H
#include <QThread>
#include <QObject>
#include "define.h"

#define COM_TASK_DELAY_TIME 200

class ChargeDev;
class CWorker : public QObject
{
	Q_OBJECT
public:
	CWorker(QObject *parent=NULL);
	virtual ~CWorker();

	bool InitWorker(int jobid);
	void Set_TcpServer(char*ip,int port);
	void taskDelay(int ms);

	void ClosePool();
	ChargeDev *FindNode(char* id);
	int AddNode(ChargeDev *node);
	int RemoveStream(ChargeDev *node);
	
	int GetStreamCount();

	int ProcessSession();
	int InitSocket(ChargeDev*p_dev);
	int ConnectToServer(ChargeDev*p_dev);
	int ProcessRecv();
	int RecvTele( char*tele,int &tele_len);
	int packet_process(int index,char*buffer,int len);
	int parser_one_packet(int index,char*buffer,int buf_len);

public slots:
	void doWork(int,QString, void *);
//	void dbUpdataOk(bool);
	 
signals:
	void WorkDone(int,QString, void *);

protected:
	//	void DoConnect(void *);
	//	void GetDevSettings(void *);

public:	 
	bool bDbUpdateOk;
	QObject *m_pParent;
	QThread *pThread;
	int m_jobid;
	
	char tcpserver_ip[32];
	int tcpserver_port;

private:
	ChargeDev *m_nodes[MAX_CHARGE_NUM_ONE_WORKTHREAD];
	int m_streamCount;
};


#endif
