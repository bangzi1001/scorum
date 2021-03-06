#include <scorum/app/chain_api.hpp>
#include <scorum/app/api_context.hpp>
#include <scorum/app/application.hpp>
#include <scorum/chain/services/dynamic_global_property.hpp>
#include <scorum/chain/services/hardfork_property.hpp>
#include <scorum/chain/services/reward_funds.hpp>
#include <scorum/chain/services/registration_pool.hpp>
#include <scorum/chain/services/budget.hpp>
#include <scorum/chain/services/reward_balancer.hpp>
#include <scorum/chain/schema/budget_object.hpp>
#include <scorum/chain/schema/scorum_objects.hpp>
#include <scorum/chain/schema/reward_balancer_object.hpp>

namespace scorum {
namespace app {

using namespace scorum::chain;

chain_api::chain_api(const api_context& ctx)
    : _db(*ctx.app.chain_database())
{
}

void chain_api::on_api_startup()
{
}

chain_properties_api_obj chain_api::get_chain_properties() const
{
    return _db.with_read_lock([&]() {

        chain_properties_api_obj ret_val;

        if (_db.has_index<witness::reserve_ratio_index>())
        {
            const auto& r = _db.find(witness::reserve_ratio_id_type());

            if (BOOST_LIKELY(r != nullptr))
            {
                ret_val = *r;
            }
        }

        const dynamic_global_property_object& dpo = _db.obtain_service<dbs_dynamic_global_property>().get();

        ret_val.head_block_id = dpo.head_block_id;
        ret_val.head_block_number = dpo.head_block_number;
        ret_val.last_irreversible_block_number = dpo.last_irreversible_block_num;
        ret_val.current_aslot = dpo.current_aslot;
        ret_val.time = dpo.time;
        ret_val.current_witness = dpo.current_witness;
        ret_val.majority_version = dpo.majority_version;
        ret_val.median_chain_props = dpo.median_chain_props;
        ret_val.chain_id = _db.get_chain_id();
        ret_val.hf_version = _db.obtain_service<dbs_hardfork_property>().get().current_hardfork_version;

        return ret_val;
    });
}

scheduled_hardfork_api_obj chain_api::get_next_scheduled_hardfork() const
{
    return _db.with_read_lock([&]() {
        scheduled_hardfork_api_obj shf;
        const auto& hpo = _db.obtain_service<dbs_hardfork_property>().get();
        shf.hf_version = hpo.next_hardfork;
        shf.live_time = hpo.next_hardfork_time;
        return shf;
    });
}

reward_fund_api_obj chain_api::get_reward_fund(reward_fund_type type_of_fund) const
{
    return _db.with_read_lock([&]() {
        switch (type_of_fund)
        {
        case reward_fund_type::reward_fund_scr:
        {
            auto& rf_service = _db.obtain_service<dbs_reward_fund_scr>();
            FC_ASSERT(rf_service.is_exists(), "${f} object does not exist", ("f", type_of_fund));
            return reward_fund_api_obj(rf_service.get());
        }
        case reward_fund_type::reward_fund_sp:
        {
            auto& rf_service = _db.obtain_service<dbs_reward_fund_sp>();
            FC_ASSERT(rf_service.is_exists(), "${f} object does not exist", ("f", type_of_fund));
            return reward_fund_api_obj(rf_service.get());
        }
        case reward_fund_type::fifa_world_cup_2018_bounty_reward_fund:
        {
            auto& rf_service = _db.obtain_service<dbs_fifa_world_cup_2018_bounty_reward_fund>();
            FC_ASSERT(rf_service.is_exists(), "${f} object does not exist", ("f", type_of_fund));
            return reward_fund_api_obj(rf_service.get());
        }
        default:
            FC_ASSERT(false, "Unknown fund");
            return reward_fund_api_obj();
        }
    });
}

chain_capital_api_obj chain_api::get_chain_capital() const
{
    return _db.with_read_lock([&]() {

        chain_capital_api_obj capital;

        const dynamic_global_property_object& dpo = _db.obtain_service<dbs_dynamic_global_property>().get();

        capital.total_supply = dpo.total_supply;
        capital.circulating_capital = dpo.circulating_capital;
        capital.total_scorumpower = dpo.total_scorumpower;

        capital.registration_pool_balance = _db.obtain_service<dbs_registration_pool>().get().balance;
        capital.fund_budget_balance = _db.obtain_service<dbs_budget>().get_fund_budget().balance;
        capital.reward_pool_balance = _db.obtain_service<dbs_reward>().get().balance;
        capital.content_reward_scr_balance = _db.obtain_service<dbs_reward_fund_scr>().get().activity_reward_balance;
        capital.content_reward_sp_balance = _db.obtain_service<dbs_reward_fund_sp>().get().activity_reward_balance;

        return capital;
    });
}
}
}
