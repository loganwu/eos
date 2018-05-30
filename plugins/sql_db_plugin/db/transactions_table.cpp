#include "transactions_table.h"

namespace eosio {

transactions_table::transactions_table(std::shared_ptr<soci::session> session):
    m_session(session)
{

}

void transactions_table::drop()
{
    try {
        *m_session << "drop table transactions";
    } catch(std::exception& e){
        wlog(e.what());
    }
}



void transactions_table::create()
{
    ilog("about to create transactions table");
    *m_session << "create table transactions("
                  "sequence_num INT UNSIGNED NOT NULL,"
                  "transaction_id TEXT,"
                  "block_id TEXT,"
                  "ref_block_num INT UNSIGNED NOT NULL,"
                  "ref_block_prefix INT UNSIGNED NOT NULL,"
                  "status INT UNSIGNED,"
                  "expiration BIGINT UNSIGNED,"
                  "pending BIT,"
                  "created_at BIGINT UNSIGNED,"
                  "updated_at BIGINT UNSIGNED)";
}

void transactions_table::insert(const chain::block_state_ptr &b)
{
    try {
        int i = 0;
        const chain::signed_block_ptr &block = b->block;
        const auto block_id = block->id().str();
        const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::microseconds{fc::time_point::now().time_since_epoch().count()}).count();
        // FIXME: 
        const auto created_at = now;
        const auto updated_at = now;

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
            const auto sequence_num = i++; // FIXME:
            const auto transaction_id = tx->id.str(); // FIXME:
            const auto ref_block_num = tx->trx.ref_block_num;
            const auto ref_block_prefix = tx->trx.ref_block_prefix;
            
            const auto status = trx_status_map.at(tx->id);
            const auto expiration = std::chrono::milliseconds{std::chrono::seconds{tx->trx.expiration.sec_since_epoch()}}.count(); 

            const int pending = 0; // FIXME:
            // ilog("insert transaction id ${i} ...", ("i", transaction_id));
            // TODO
            *m_session << "insert into transactions(sequence_num, transaction_id, "
                    "block_id, ref_block_num, "
                    "ref_block_prefix, status, "
                    "expiration, pending, "
                    "created_at, updated_at) "
                    "values(:sequence_num, :transaction_id, "
                    ":block_id, :ref_block_num, "
                    ":ref_block_prefix, :status, "
                    ":expiration, :pending, "
                    ":created_at, :updated_at)", 
                    soci::use(sequence_num), soci::use(transaction_id),
                    soci::use(block_id), soci::use(ref_block_num),
                    soci::use(ref_block_prefix), soci::use(status),
                    soci::use(expiration), soci::use(pending),
                    soci::use(created_at), soci::use(updated_at); 
            // insert actions
            uint32_t total_actions = tx->total_actions();
        }

    } catch(std::exception& e){
        wlog(e.what());
    }

}

} // namespace
