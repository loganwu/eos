/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 *  @author rixon vjoke@foxmail.com 
 */
#include <eosio/pubsub_plugin/pubsub_plugin.hpp>

#include "backend.h"

#include "consumer_core.h"
#include "irreversible_block_storage.h"
#include "applied_transaction_storage.h"
#include "block_storage.h"

namespace {
const char* BUFFER_SIZE_OPTION = "pubsub-queue-size";
const char* SQL_DB_URI_OPTION = "pubsub-uri";
const char* RESYNC_OPTION = "delete-all-blocks";
const char* REPLAY_OPTION = "replay-blockchain";
}

namespace fc { class variant; }

namespace eosio {

static appbase::abstract_plugin& _pubsub_plugin = app().register_plugin<pubsub_plugin>();

pubsub_plugin::pubsub_plugin():
    m_block_consumer(std::make_unique<block_storage>())
{

}

void pubsub_plugin::set_program_options(options_description& cli, options_description& cfg)
{
    dlog("set_program_options");

    cfg.add_options()
            (BUFFER_SIZE_OPTION, bpo::value<uint>()->default_value(256),
             "The queue size between nodeos and SQL DB plugin thread.")
            (SQL_DB_URI_OPTION, bpo::value<std::string>(),
             "Sql DB URI connection string"
             " If not specified then plugin is disabled. Default backend 'EOS' is used if not specified in URI.")
            ;
}

void pubsub_plugin::plugin_initialize(const variables_map& options)
{
    ilog("initialize");

    std::string uri_str = options.at(SQL_DB_URI_OPTION).as<std::string>();
    if (uri_str.empty())
    {
        wlog("db URI not specified => eosio::pubsub_plugin disabled.");
        return;
    }
    ilog("connecting to ${u}", ("u", uri_str));

    auto be = std::make_shared<backend>(uri_str);


    if (options.at(RESYNC_OPTION).as<bool>() ||
         options.at(REPLAY_OPTION).as<bool>())
    {
        ilog("Resync requested: wiping backend");
        be->wipe();
    }

    chain_plugin* chain_plug = app().find_plugin<chain_plugin>();
    FC_ASSERT(chain_plug);
    auto& chain = chain_plug->chain();

    m_irreversible_block_consumer = std::make_unique<consumer<chain::block_state_ptr>>(std::make_unique<irreversible_block_storage>(be));
    m_applied_transaction_consumer = std::make_unique<consumer<chain::transaction_trace_ptr>>(std::make_unique<applied_transaction_storage>(be));

   // chain.accepted_block.connect([=](const chain::block_state_ptr& b) {m_block_consumer.push(b);});
    m_irreversible_block_connection.emplace(chain.irreversible_block.connect([=](const chain::block_state_ptr& b) {m_irreversible_block_consumer->push(b);}));
    m_applied_transaction_connection.emplace(chain.applied_transaction.connect([=](const chain::transaction_trace_ptr& b) {m_applied_transaction_consumer->push(b);}));
}

void pubsub_plugin::plugin_startup()
{
    ilog("startup");
}

void pubsub_plugin::plugin_shutdown()
{
    ilog("shutdown");
    m_irreversible_block_connection.reset();
    m_applied_transaction_connection.reset();
}

} // namespace eosio
