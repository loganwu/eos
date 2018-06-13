#include "backend.h"

using libkafka_asio::Connection;
using libkafka_asio::ProduceRequest;
using libkafka_asio::ProduceResponse;
using libkafka_asio::MetadataRequest;
using libkafka_asio::MetadataResponse;


namespace eosio {

backend::backend(const std::string& uri, const std::string& topic, const int partition, const std::string& cid, const std::string& format):
    m_uri(""), m_topic(topic), m_partition(partition), m_cid(cid), m_format(format)
{
    if (m_partition < 0) {
        wlog("Unexpected partition ${p}, use default 0!!!", ("p", m_partition));
        m_partition = 0;
    }
    m_count = 0;

    m_ios = std::make_shared<boost::asio::io_service>();

    connect(uri);
    update(false);

    m_ios->run();
    m_ios->reset();

    wlog("created pubsub backend");
}

void backend::wipe()
{
    std::unique_lock<std::mutex> lock(m_mux);
    ilog("wipe backend??");
}


void backend::connect(const std::string &uri) 
{
    if (uri == m_uri) {
        wlog("Skip connection since uri is not changed");
        return;
    }

    m_uri = uri;
    Connection::Configuration configuration;
    configuration.auto_connect = true;
    configuration.client_id = m_cid;
    configuration.socket_timeout = 10000;
    // configuration.SetBrokerFromString("192.168.15.137:49162");
    configuration.SetBrokerFromString(m_uri);

    // auto remove?
    if (m_connection) {
        m_connection->Close();
        m_connection = NULL;
    }

    m_connection = std::make_shared<libkafka_asio::Connection>(*m_ios, configuration);

    wlog("connecting to ${u}", ("u", uri));
}

// update metadata for broker
void backend::update(bool connect) 
{
    MetadataRequest request;
    request.AddTopicName(m_topic);

    m_connection->AsyncRequest(
        request,
        [&](const Connection::ErrorCodeType& err,
            const MetadataResponse::OptionalType& response) {
            if (err || !response) {
                wlog("update error");
                wlog(boost::system::system_error(err).what());
                return;
            }
            // Find the leader for topic 'mytopic' and partition 0
            auto leader = response->PartitionLeader(m_topic, m_partition);
            if (!leader) {
                wlog("No leader found!");
                return;
            }
            wlog("Found leader ${h}:${p} for p${t} topic ${c}", ("h", leader->host)("p", leader->port)("t", m_partition)("c", m_topic));
            // FIXME: is that right?
            if (connect) {
                auto uri = leader->host + ":" + std::to_string(leader->port);
                m_ios->post(boost::bind(&backend::connect, this, uri));
            }
        }
    );

    wlog("updating pubsub backend");
}

void backend::publish(const std::string &msg) {
    std::unique_lock<std::mutex> lock(m_mux);
    // Create a 'Produce' request and add a single message to it. The value of
    // that message is set to "Hello World". The message is produced for topic
    // "mytopic" and partition 0.
    ProduceRequest request;
    request.AddValue(msg, m_topic, m_partition);

    // Send the prepared produce request.
    // The connection will attempt to automatically connect to one of the brokers,
    // specified in the configuration.
    m_connection->AsyncRequest(
        request,
        [&](const Connection::ErrorCodeType& err,
        const ProduceResponse::OptionalType& response) {
            m_count++;
            if (err) {
                wlog("asyncrequest error with uri:${u} topic:${t} partition:${p}", ("u", m_uri)("t", m_topic)("p", m_partition));
                wlog(boost::system::system_error(err).what());
                // FIXME: is that right?
                m_ios->post(boost::bind(&backend::update, this, true)); 
                return;
            }
           
            wlog("Successfully produced ${c} messages!", ("c", m_count));
        }
    );

    m_ios->run();
    m_ios->reset();
    ilog("Exit kafka io message queue!");
}

} // namespace
