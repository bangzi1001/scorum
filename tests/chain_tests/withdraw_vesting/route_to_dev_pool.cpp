#include <boost/test/unit_test.hpp>

#include <scorum/chain/services/account.hpp>
#include <scorum/chain/services/dev_pool.hpp>
#include <scorum/chain/services/withdraw_vesting_route.hpp>

#include <scorum/chain/schema/account_objects.hpp>
#include <scorum/chain/schema/dev_committee_object.hpp>

#include "database_trx_integration.hpp"

using namespace scorum::protocol;
using namespace scorum::chain;

BOOST_AUTO_TEST_SUITE(withdraw_vesting_dev_pool_tests)

struct route_to_dev_pool_fixture : public database_trx_integration_fixture
{
    route_to_dev_pool_fixture()
        : account_service(db.account_service())
        , pool_service(db.dev_pool_service())
        , withdraw_service(db.withdraw_vesting_route_service())
    {
        genesis_state_type genesis
            = database_integration_fixture::default_genesis_state().dev_supply(ASSET_NULL_SP).generate();

        open_database(genesis);

        generate_blocks(5);

        FC_ASSERT(!pool_service.is_exists());

        create_dev_pool();

        generate_blocks(5);

        ACTOR(alice);
        alice_key = alice_private_key;
    }

    void create_dev_pool()
    {
        db_plugin->debug_update([&](database&) { pool_service.create(); }, default_skip);
    }

    void set_withdraw_route(uint16_t percent)
    {
        set_withdraw_vesting_route_to_dev_pool_operation op;

        op.from_account = "alice";
        op.percent = percent * SCORUM_1_PERCENT;

        push_operation(op, alice_key);
    }

    void start_withdraw(const asset& amount)
    {
        withdraw_vesting_operation op;

        op.account = "alice";
        op.vesting_shares = amount;

        push_operation(op, alice_key);
    }

    account_service_i& account_service;
    dev_pool_service_i& pool_service;
    withdraw_vesting_route_service_i& withdraw_service;
    private_key_type alice_key;
};

BOOST_FIXTURE_TEST_CASE(set_withdraw_route_check, route_to_dev_pool_fixture)
{
    static const share_type balance = share_type(1000);
    share_type initial_balance = balance;

    const dev_committee_object& pool = pool_service.get();

    initial_balance += account_service.get_account("alice").vesting_shares.amount;

    vest("alice", balance);

    const account_object& alice_vested = account_service.get_account("alice");

    BOOST_REQUIRE_EQUAL(alice_vested.vesting_shares.amount, initial_balance);

    set_withdraw_route(50);

    BOOST_REQUIRE(withdraw_service.is_exists(alice_vested.id, pool.id));
}

// TODO

BOOST_AUTO_TEST_SUITE_END()
