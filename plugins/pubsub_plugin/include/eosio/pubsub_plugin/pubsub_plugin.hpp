/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#pragma once

#include <eosio/chain_plugin/chain_plugin.hpp>
#include <appbase/application.hpp>
#include <boost/signals2/connection.hpp>
#include <memory>

#include <eosio/chain_plugin/chain_plugin.hpp>
#include "consumer.h"

namespace eosio {



/**
 * @author rixon <vjoke@foxmail.com>
 *
 * Provides persistence to message broker for:
 *   Blocks
 *   Transactions
 *   Actions
 *   Accounts
 *
 *   See data dictionary (DB Schema Definition - EOS API) for description of SQL DB schema.
 *
 *   The goal ultimately is for all chainbase data to be output to kafka for consuming by other services
 *   Currently, only Blocks, Transactions, Messages, and Account balance it mirrored.
 *   Chainbase is being rewritten to be multi-threaded. Once chainbase is stable, integration directly with
 *   a mirror database approach can be followed removing the need for the direct processing of Blocks employed
 *   with this implementation.
 *  
 *   Tested ok with kafka 0.8.2.2
 */

namespace pubsub_message {
    struct ordered_action_result {
         uint64_t                     global_action_seq = 0;
         int32_t                      account_action_seq = 0;
         uint32_t                     block_num;
         chain::block_timestamp_type  block_time;
         fc::variant                  action_trace;
    };
    // Borrowed from history_plugin
    struct actions_result {
         vector<ordered_action_result> actions;
         uint32_t                      last_irreversible_block;
         optional<bool>                time_limit_exceeded_error;
      };

    using actions_result_ptr = std::shared_ptr<actions_result>;
} // pubsub_message

class pubsub_plugin final : public plugin<pubsub_plugin> {
public:
    APPBASE_PLUGIN_REQUIRES((chain_plugin))

    pubsub_plugin();

    virtual void set_program_options(options_description& cli, options_description& cfg) override;

    void plugin_initialize(const variables_map& options);
    void plugin_startup();
    void plugin_shutdown();

private:
    void on_message(const chain::transaction_trace_ptr& t);
    
private:
    std::unique_ptr<consumer<chain::block_state_ptr>> m_irreversible_block_consumer;
    // std::unique_ptr<consumer<chain::transaction_trace_ptr>> m_applied_transaction_consumer;
    consumer<chain::block_state_ptr> m_block_consumer;
    std::unique_ptr<consumer<pubsub_message::actions_result_ptr>> m_applied_action_consumer;

    fc::optional<boost::signals2::scoped_connection> m_irreversible_block_connection;
    fc::optional<boost::signals2::scoped_connection> m_applied_transaction_connection;

    chain_plugin*  m_chain_plug;
};


}

FC_REFLECT( eosio::pubsub_message::actions_result, (actions)(last_irreversible_block)(time_limit_exceeded_error) )
FC_REFLECT( eosio::pubsub_message::ordered_action_result, (global_action_seq)(account_action_seq)(block_num)(block_time)(action_trace) )
