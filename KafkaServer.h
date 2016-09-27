#ifndef _KAFKASERVER_H_
#define _KAFKASERVER_H_

#include <include/rdkafkacpp.h>
#include <string>
#include <map>
#include <vector>

#include "TL_Logger.h"

#define MAX_INFLATE_SIZE (40 * 1024 * 1024) //40MB

//extern int g_kafka_rewrite;
//extern int g_nums_rewrite;
//extern int g_show_retl_id;

//none|gzip|snappy
#define RD_COMPRESSION_NONE "none"
#define RD_COMPRESSION_GZIP "gzip"
#define RD_COMPRESSION_SNAPPY "snappy"

#define SET_KAFKA_OK 0
#define SET_KAFKA_BROKERLIST_FAILED -1
#define CREATE_KAFKA_PRODUCER_FAILED -2

#define WRITE_KAFKA_TYPE_RANDOM 1
#define WRITE_KAFKA_TYPE_POLL 2




using namespace std;
using namespace tidp;
struct KafkaConf {
        std::string brokerList;
        int maxBuffer;
        int batchNums;
        int isCallback;
        int retries;
        int copmType;
        map<std::string, std::string> m_conf;
        string clientId;
};

/**
 *
 *
 */
class KafkaServer 
{
public:
	KafkaServer();
	
	KafkaServer(KafkaConf& conf);
	/**
	 *
	 */
	virtual ~KafkaServer();
	

	/**
	 *
	 */
	virtual int initialize();

	/**
	 *
	 */
    virtual void destroy();

	void setKafkaConf(KafkaConf &conf);

	void setRertyConf(int isRetry,int retrytimes);

	int write2Kafka(const string& topicName,const vector<string> & v_msg,int max_size,int partition,int w_type);


private:
	int initKafka(KafkaConf &conf);


private:
	RdKafka::Conf *conf;
	RdKafka::Conf *tconf;
	RdKafka::Producer *producer;
	
        
        map<string,RdKafka::Topic *> _topics;
	string errstr;
	KafkaConf k_conf;
	unsigned char * buf;

	int _max_msg;
	int kafka_rewrite;
	int nums_rewrite;
};
/////////////////////////////////////////////////////
#endif

