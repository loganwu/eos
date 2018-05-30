#include "blocks_table.h"

namespace eosio {

blocks_table::blocks_table(std::shared_ptr<soci::session> session):
    m_session(session)
{

}

void blocks_table::drop()
{
    try {
        *m_session << "drop table blocks";
    } catch(std::exception& e){
        wlog(e.what());
    }
}

void blocks_table::create()
{
    ilog("about to create blocks table");
    *m_session << "create table blocks("
                  "block_num INT UNSIGNED NOT NULL,"
                  "block_id TEXT,"
                  "prev_block_id TEXT,"
                  "timestamp BIGINT UNSIGNED,"
                  "transaction_merkle_root TEXT,"
                  "transactions INT UNSIGNED,"
                  "producer_account_id TEXT,"
                  "pending BIT,"
                  "created_at BIGINT UNSIGNED,"
                  "updated_at BIGINT UNSIGNED)";
}

void blocks_table::insert(const chain::block_state_ptr &b)
{
    try {
        const chain::signed_block_ptr &block = b->block;
        const auto block_id = block->id().str();

        // ilog("insert block id ${i} ...", ("i", block_id));

        const auto prev_block_id = block->previous.str();
        const auto block_num = static_cast<int32_t>(block->block_num());
        const auto timestamp = std::chrono::milliseconds{
                            std::chrono::seconds{block->timestamp.operator fc::time_point().sec_since_epoch()}}.count();
        const auto transaction_merkle_root = block->transaction_mroot.str();
        const auto transactions = b->trxs.size();
        const auto producer_account_id = block->producer.to_string();
        const int pending = 0; // FIXME:
        const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::microseconds{fc::time_point::now().time_since_epoch().count()}).count();
        // FIXME: 
        const auto created_at = now;
        const auto updated_at = now;

        *m_session << "insert into blocks(block_num, block_id, "
                    "prev_block_id, timestamp, "
                    "transaction_merkle_root, transactions, "
                    "producer_account_id, pending, "
                    "created_at, updated_at) "
                    "values(:block_num, :block_id, "
                    ":prev_block_id, :timestamp, "
                    ":transaction_merkle_root, :transactions, "
                    ":producer_account_id, :pending, "
                    ":created_at, :updated_at)", 
                    soci::use(block_num), soci::use(block_id),
                    soci::use(prev_block_id), soci::use(timestamp),
                    soci::use(transaction_merkle_root), soci::use(transactions),
                    soci::use(producer_account_id), soci::use(pending),
                    soci::use(created_at), soci::use(updated_at);
    } catch(std::exception& e){
        wlog(e.what());
    }

}

} // namespace
