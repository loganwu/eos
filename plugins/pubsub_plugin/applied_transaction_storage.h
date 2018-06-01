/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#pragma once

#include "consumer_core.h"

#include <memory>
#include <eosio/chain/trace.hpp>

#include "backend.h"

namespace eosio {

class applied_transaction_storage : public consumer_core<chain::transaction_trace_ptr>
{
public:
    applied_transaction_storage(std::shared_ptr<backend> be);

    void consume(const std::vector<chain::transaction_trace_ptr>& traces) override;

private:
    std::shared_ptr<backend> m_be;
    void on_action_trace(const chain::action_trace& at);

};

} // namespace

