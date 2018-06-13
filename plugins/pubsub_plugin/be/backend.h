#ifndef BACKEND_H
#define BACKEND_H

#include <memory>
#include <mutex>

#include <libkafka_asio/libkafka_asio.h>
#include <fc/log/logger.hpp>

namespace eosio {

class backend
{
public:
    backend(const std::string& uri, const std::string& topic, const int partition, const std::string& cid, const std::string& format);

    void wipe();

    void publish(const std::string &msg);
    void update(bool connect=true);
    void connect(const std::string &uri);
private:
    mutable std::mutex m_mux;
    unsigned long long m_count;
    std::string m_uri, m_topic, m_cid, m_format;
    int m_partition;
    std::shared_ptr<libkafka_asio::Connection> m_connection;
    std::shared_ptr<boost::asio::io_service> m_ios;
};

} // namespace

#endif // BACKEND_H
