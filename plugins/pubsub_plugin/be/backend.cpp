#include "backend.h"

using libkafka_asio::Connection;
using libkafka_asio::ProduceRequest;
using libkafka_asio::ProduceResponse;

namespace eosio {

backend::backend(const std::string& uri, const std::string& topic, const std::string& cid, const std::string& format):
    m_uri(uri), m_topic(topic), m_cid(cid), m_format(format)
{
    // Connection::Configuration configuration;
    // configuration.auto_connect = true;
    // configuration.client_id = m_cid;
    // configuration.socket_timeout = 10000;
    // // configuration.SetBrokerFromString("192.168.15.137:49162");
    // wlog("connecting to ${s}", ("s", m_uri));
    // configuration.SetBrokerFromString(m_uri);

    // m_ios = std::make_shared<boost::asio::io_service>();
    // m_connection = std::make_shared<libkafka_asio::Connection>(*m_ios, configuration);

    // m_ios->run();

    wlog("created pubsub backend");
}

void backend::wipe()
{
    std::unique_lock<std::mutex> lock(m_mux);

    ilog("wipe backend??");
}

// TODO: optimize!!!
void backend::publish(const std::string &msg) {
    Connection::Configuration configuration;
    configuration.auto_connect = true;
    configuration.client_id = m_cid;
    configuration.socket_timeout = 10000;
    configuration.SetBrokerFromString(m_uri);

    boost::asio::io_service ios;
    Connection connection(ios, configuration);

    // Create a 'Produce' request and add a single message to it. The value of
    // that message is set to "Hello World". The message is produced for topic
    // and partition 0.
    ProduceRequest request;
    request.AddValue(msg, m_topic, 0);

    // Send the prepared produce request.
    // The connection will attempt to automatically connect to one of the brokers,
    // specified in the configuration.
    connection.AsyncRequest(
        request,
        [&](const Connection::ErrorCodeType& err,
            const ProduceResponse::OptionalType& response) {
            if (err) {
                wlog(boost::system::system_error(err).what());
                return;
            }
           
            wlog("Successfully produced message!");
    });

    // Let's go!
    ios.run();
}

// void backend::publish(const std::string &msg) {
//     wlog("publish get called");
//     // Create a 'Produce' request and add a single message to it. The value of
//     // that message is set to "Hello World". The message is produced for topic
//     // "mytopic" and partition 0.
//     ProduceRequest request;
//     request.AddValue("Hello EOS", "mytopic", 0);

//     // Send the prepared produce request.
//     // The connection will attempt to automatically connect to one of the brokers,
//     // specified in the configuration.
//     m_connection->AsyncRequest(
//         request,
//         [&](const Connection::ErrorCodeType& err,
//         const ProduceResponse::OptionalType& response) {
//             if (err) {
//                 wlog(boost::system::system_error(err).what());
//                 return;
//             }
           
//             wlog("Successfully produced message!");
//         }
//     );

//     m_ios->run();
//     wlog("Exit message queue!");
// }

} // namespace
