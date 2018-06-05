/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#pragma once

#include "consumer_core.h"

#include <memory>
#include <eosio/chain/block_state.hpp>

#include "backend.h"

namespace eosio {

class irreversible_block_storage : public consumer_core<chain::block_state_ptr>
{
public:
    irreversible_block_storage(std::shared_ptr<backend> be);

    void consume(const std::vector<chain::block_state_ptr>& blocks) override;

private:
    std::shared_ptr<backend> m_be;
    void on_block(const chain::block_state_ptr& block); 

};

} // namespace

