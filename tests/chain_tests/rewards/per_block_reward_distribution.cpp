#include <boost/test/unit_test.hpp>

#include <scorum/chain/services/account.hpp>
#include <scorum/chain/services/reward_balancer.hpp>
#include <scorum/chain/services/budget.hpp>
#include <scorum/chain/services/dev_pool.hpp>
#include <scorum/chain/services/reward_fund.hpp>
#include <scorum/chain/services/dynamic_global_property.hpp>
#include <scorum/protocol/config.hpp>

#include <scorum/chain/schema/account_objects.hpp>
#include <scorum/chain/schema/reward_balancer_objects.hpp>
#include <scorum/chain/schema/budget_object.hpp>
#include <scorum/chain/schema/dev_committee_object.hpp>
#include <scorum/chain/schema/scorum_objects.hpp>
#include <scorum/chain/schema/dynamic_global_property_object.hpp>

#include "database_default_integration.hpp"

#include <limits>

using namespace database_fixture;

namespace reward_distribution {

class dbs_reward_fixture : public database_integration_fixture
{
public:
    dbs_reward_fixture()
        : reward_service(db.obtain_service<dbs_reward>())
        , budget_service(db.obtain_service<dbs_budget>())
        , dev_service(db.obtain_service<dbs_dev_pool>())
        , content_reward_fund_scr_service(db.obtain_service<dbs_content_reward_fund_scr>())
        , content_reward_fund_sp_service(db.obtain_service<dbs_content_reward_fund_sp>())
        , account_service(db.obtain_service<dbs_account>())
        , dgp_service(db.obtain_service<dbs_dynamic_global_property>())
    {
        open_database();
    }

    dbs_reward& reward_service;
    dbs_budget& budget_service;
    dbs_dev_pool& dev_service;
    dbs_content_reward_fund_scr& content_reward_fund_scr_service;
    dbs_content_reward_fund_sp& content_reward_fund_sp_service;
    dbs_account& account_service;
    dynamic_global_property_service_i& dgp_service;

    const asset NULL_BALANCE = asset(0, SCORUM_SYMBOL);
};
}

BOOST_FIXTURE_TEST_SUITE(reward_distribution, reward_distribution::dbs_reward_fixture)

SCORUM_TEST_CASE(check_per_block_reward_distribution_with_fund_budget_only)
{
    generate_block();

    const auto& fund_budget = budget_service.get_fund_budget();
    asset initial_per_block_reward = fund_budget.per_block;

    auto witness_reward = initial_per_block_reward * SCORUM_WITNESS_PER_BLOCK_REWARD_PERCENT / SCORUM_100_PERCENT;
    auto content_reward = initial_per_block_reward - witness_reward;

    const auto& account = account_service.get_account(TEST_INIT_DELEGATE_NAME);

    BOOST_REQUIRE_EQUAL(dev_service.get().scr_balance, NULL_BALANCE);
    BOOST_REQUIRE_EQUAL(content_reward_fund_sp_service.get().activity_reward_balance, content_reward);
    BOOST_REQUIRE_EQUAL(account.scorumpower, witness_reward);
}

SCORUM_TEST_CASE(check_per_block_reward_distribution_with_fund_and_advertising_budgets)
{
    //          | r[i] + max(1, r[i]*5/100) if B > r[i]*D(100),
    // r[i+1] = | max(1, r[i] - r[i]*5/100) if B < r[i]*D(30),                          (1)
    //          | r[i]                      if B in [r[i]*D(30), r[i]*D(100)].
    //
    // TODO: there are not tests for this formula.

    const auto& account = account_service.get_account(TEST_INIT_DELEGATE_NAME);
    auto advertising_budget = ASSET_SCR(2e+9);
    auto deadline = db.get_slot_time(1);

    BOOST_CHECK_NO_THROW(budget_service.create_budget(account, advertising_budget, deadline));

    generate_block();

    auto dev_team_reward_scr = advertising_budget * SCORUM_DEV_TEAM_PER_BLOCK_REWARD_PERCENT / SCORUM_100_PERCENT;
    auto user_reward_scr = advertising_budget - dev_team_reward_scr;

    auto witness_reward_scr = user_reward_scr * SCORUM_WITNESS_PER_BLOCK_REWARD_PERCENT / SCORUM_100_PERCENT;
    auto content_reward_scr = user_reward_scr - witness_reward_scr;

    const auto& fund_budget = budget_service.get_fund_budget();
    auto user_reward_sp = fund_budget.per_block;
    auto witness_reward_sp = user_reward_sp * SCORUM_WITNESS_PER_BLOCK_REWARD_PERCENT / SCORUM_100_PERCENT;
    auto content_reward_sp = user_reward_sp - witness_reward_sp;

    BOOST_REQUIRE_EQUAL(dev_service.get().scr_balance, dev_team_reward_scr);

    BOOST_REQUIRE_EQUAL(content_reward_fund_scr_service.get().activity_reward_balance, content_reward_scr);
    BOOST_REQUIRE_EQUAL(content_reward_fund_sp_service.get().activity_reward_balance, content_reward_sp);

    BOOST_REQUIRE_EQUAL(account.scorumpower, witness_reward_sp);
    BOOST_REQUIRE_EQUAL(account.balance, witness_reward_scr);
}

BOOST_AUTO_TEST_SUITE_END()
