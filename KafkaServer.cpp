#include "KafkaServer.h"

KafkaServer::KafkaServer() {

}

KafkaServer::KafkaServer(KafkaConf& conf) {
    k_conf = conf;
}

KafkaServer::~KafkaServer() {
    destroy();
}

int KafkaServer::initialize() {

    return initKafka(k_conf);

}

void KafkaServer::destroy() {
    map<string, RdKafka::Topic *>::iterator it = _topics.begin();
    while (it != _topics.end()) {
        delete it->second;
        ++it;
    }
    _topics.clear();
    if (producer)
        delete producer;
    if (conf || tconf)
        RdKafka::wait_destroyed(1500);
}

void KafkaServer::setKafkaConf(KafkaConf &conf) {
    k_conf = conf;
}

void KafkaServer::setRertyConf(int isRetry, int retrytimes) {
    kafka_rewrite = isRetry;
    nums_rewrite = retrytimes;
}

int KafkaServer::initKafka(KafkaConf &k_conf) {
    conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    /*
     * Create configuration objects
     */
    string client_id = k_conf.clientId;
    if (conf->set("client.id", client_id, errstr) != RdKafka::Conf::CONF_OK) {
        LOG() << error << "producer client id failed." << errstr << endl;
    }

    /*
     * Set configuration properties
     */
    if (k_conf.brokerList.empty()) {
        LOG() << error << "set broker list failed,brokerlist is empty(" << k_conf.brokerList << ")" << endl;
        return SET_KAFKA_BROKERLIST_FAILED;
    }
    if (conf->set("metadata.broker.list", k_conf.brokerList, errstr) != RdKafka::Conf::CONF_OK) {
        LOG() << error << "set broker list failed,brokerlist(" << k_conf.brokerList << ")|" << errstr << endl;
        return SET_KAFKA_BROKERLIST_FAILED;
    }
    char tmp[32];
    if (k_conf.maxBuffer > 0) {
        snprintf(tmp, 32, "%d", k_conf.maxBuffer);
        if (conf->set("queue.buffering.max.messages", tmp, errstr) != RdKafka::Conf::CONF_OK)
            LOG() << error << "set max buffer failed,maxBuffer(" << k_conf.maxBuffer << ")|" << errstr << endl;
        _max_msg = k_conf.maxBuffer;
    } else {
        _max_msg = 0;
    }
    if (k_conf.batchNums > 0) {
        snprintf(tmp, 32, "%d", k_conf.batchNums);
        if (conf->set("batch.num.messages", tmp, errstr) != RdKafka::Conf::CONF_OK)
            LOG() << error << "set batch message num failed,nums(" << k_conf.batchNums << ")|" << errstr << endl;
    }
    if (k_conf.retries > 0) {
        snprintf(tmp, 32, "%d", k_conf.retries);
        if (conf->set("message.send.max.retries", tmp, errstr) != RdKafka::Conf::CONF_OK)
            LOG() << error << "set retry time failed,times(" << k_conf.retries << ")|" << errstr << endl;
    }
    if (k_conf.copmType > 0) {
        string codeType = RD_COMPRESSION_NONE;
        //compression.codec 
        if (k_conf.copmType == 1) {
            codeType = RD_COMPRESSION_GZIP;
        } else if (k_conf.copmType == 2) {
            codeType = RD_COMPRESSION_SNAPPY;
        }
        if (conf->set("compression.codec", codeType, errstr) != RdKafka::Conf::CONF_OK)
            LOG() << error << "set compression type failed,type(" << codeType << ")|" << errstr << endl;
    }

    //broker.version.fallback

    if (conf->set("broker.version.fallback", "0.8.2.1" , errstr) != RdKafka::Conf::CONF_OK)
   	LOG() << error << "set broker.version.fallback failed." << endl;
    /*
     * Create producer using accumulated global configuration.
     */
    producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
        LOG() << error << "failed to create producer: " << errstr << endl;
        return CREATE_KAFKA_PRODUCER_FAILED;
    }

    LOG() << debug << "created producer " << producer->name() << endl;

    return 0;
}

//������ʽ

int KafkaServer::write2Kafka(const string& topicName, const vector<string> & v_msg, int max_size, int partition, int w_type) {
    //LOG() << debug << "topicName=" << topicName << "|partition=" << partition << "|wtype=" << w_type << endl;
    //int msgCount = v_msg.size();
    int sendCount = 0;
    if (max_size > 0) {
        RdKafka::Topic *topic = NULL;
	if (_topics.find(topicName) != _topics.end()) {
            topic = _topics[topicName];
        } else {
            topic = RdKafka::Topic::create(producer, topicName.c_str(), tconf, errstr);
            _topics[topicName] = topic;
        }
        //topic = RdKafka::Topic::create(producer, topicName.c_str(), tconf, errstr);
        //topic.
        RdKafka::ErrorCode resp;
        //send message
        //int index_c = 0;
        int partition_index = RdKafka::Topic::PARTITION_UA;
        //for (std::vector<string>::iterator it = v_msg.begin(); it != v_msg.end(); ++it) {
        for (int i = 0; i < max_size; ++i) {
            if (w_type == WRITE_KAFKA_TYPE_POLL && partition > 0) {
                partition_index = i % partition;
                //i++;
            }
            const string& msg = v_msg[i];
            //int size = msg.size();
            resp = producer->produce(topic, partition_index, RdKafka::Producer::MSG_COPY, const_cast<char *> (msg.c_str()), msg.size(), NULL, NULL);
            if (resp != RdKafka::ERR_NO_ERROR) {
                //send fail,trigger rewrite
                LOG() << error << "Send Error|errno=" << errno << "|" << strerror(errno) << "|resp=" << resp
                        << "|Produce failed: " << RdKafka::err2str(resp) << endl;
                producer->poll(10);
                //check is need rewrite
                if (kafka_rewrite == 1) {
                    if (nums_rewrite > 0) {
                        //rewrite Xnums
                        int t_count = 0;
                        while (t_count < nums_rewrite) {
                            t_count++;
                            resp = producer->produce(topic, partition_index, RdKafka::Producer::MSG_COPY, const_cast<char *> (msg.c_str()), msg.size(), NULL, NULL);
                            if (resp == RdKafka::ERR_NO_ERROR) {
                		RLOG("kafka") << debug << partition_index << "|1|" <<  msg << endl; 
                                sendCount++; //rewrite succ
                                break;
                            }
                            producer->poll(10);
                        }
                        if (resp != RdKafka::ERR_NO_ERROR) {
                            LOG() << debug << "ReWrite Failed,errno=" << errno << "|" << strerror(errno) << "|resp=" << resp << endl;
                            LOG() << debug << msg << endl;
                        }
                    } else if (nums_rewrite < 0) {
                        //rewrite until succ
                        //send buff log
                        while (producer->outq_len() >= (_max_msg * 90 / 100)) {
                            producer->poll(10);
                        }
                        //resend message
                        resp = producer->produce(topic, partition_index, RdKafka::Producer::MSG_COPY, const_cast<char *> (msg.c_str()), msg.size(), NULL, NULL);
                        if (resp != RdKafka::ERR_NO_ERROR) {
                            LOG() << debug << "Rewrite until Succ,errno=" << errno << "|" << strerror(errno) << "|ret=" << resp << endl;
                            LOG() << debug << msg << endl;
                        } else {
                	    RLOG("kafka") << debug << partition_index << "|2|" <<  msg << endl; 
                            sendCount++; //rewrite succ
                        }
                    }
                } else {
                    //curr log write fail
                    LOG() << debug << "write failed:" << msg << endl;
                }
            } else {
                RLOG("kafka") << debug << partition_index << "|0|" <<  msg << endl; 
                ++sendCount;
            }
            producer->poll(0);
        }
    }
    return sendCount;
}

