#include "irreversible_block_storage.h"
#include <fc/io/json.hpp>
#include <eosio/pubsub_plugin/pubsub_plugin.hpp>

namespace eosio {

using namespace pubsub_message;

irreversible_block_storage::irreversible_block_storage(std::shared_ptr<backend> be):
    m_be(be)
{

}

void irreversible_block_storage::consume(const std::vector<chain::block_state_ptr>& blocks)
{
    for (const auto& block : blocks) {
        on_block(block);
    }
}

void irreversible_block_storage::on_block(const chain::block_state_ptr& b) {
    // const std::string &block_str = fc::json::to_pretty_string(b);
    // idump((block_str));
    // m_be->publish(block_str);

    block_result_ptr br = std::make_shared<block_result>();
    int i = 0;
    const chain::signed_block_ptr &block = b->block;
    const auto block_num = static_cast<int32_t>(block->block_num());
    const auto block_id = block->id().str();
    const auto prev_block_id = block->previous.str();
    const auto timestamp = std::chrono::milliseconds{
                            std::chrono::seconds{block->timestamp.operator fc::time_point().sec_since_epoch()}}.count();
    const auto transaction_merkle_root = block->transaction_mroot.str();
    const auto transaction_count = b->trxs.size(); // FIXME: 
    const auto producer = block->producer.to_string();

    br->block_num = block_num;
    br->block_id = block_id;
    br->prev_block_id = prev_block_id;
    br->timestamp = timestamp;
    br->transaction_merkle_root = transaction_merkle_root;
    br->transaction_count = transaction_count;
    br->producer = producer;
    // get status from receipt
    std::map<chain::transaction_id_type, int> trx_status_map;
    for (const auto&receipt : block->transactions) {
        if (receipt.trx.contains<chain::transaction_id_type>()) {
            trx_status_map[receipt.trx.get<chain::transaction_id_type>()] = receipt.status; 
        } else {
            // TODO:get transaction id
            wlog("missing txid in receipt");
            auto& pt = receipt.trx.get<chain::packed_transaction>();
            auto mtrx = std::make_shared<chain::transaction_metadata>(pt);
            trx_status_map[mtrx->id] = receipt.status;
        }
    }
    // insert transaction one by one
    for (const auto&tx : b->trxs) {
        const auto sequence_num = i++; 
        const auto trx_id = tx->id.str(); 
        const auto ref_block_num = tx->trx.ref_block_num;
        const auto ref_block_prefix = tx->trx.ref_block_prefix;
        
        const auto status = trx_status_map.at(tx->id);
        const auto expiration = std::chrono::milliseconds{std::chrono::seconds{tx->trx.expiration.sec_since_epoch()}}.count(); 

        br->transactions.emplace_back(transaction_result{
            trx_id,
            status,
            expiration
        });
    }

    const std::string &block_str = fc::json::to_pretty_string(*br);
    // idump((block_str));
    m_be->publish(block_str);
}

} // namespace
