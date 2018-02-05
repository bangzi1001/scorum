#include <codecvt>
#include <boost/test/unit_test.hpp>

#include <graphene/utilities/key_conversion.hpp>

#include <scorum/chain/scorum_objects.hpp>

#include <scorum/chain/transaction_object.hpp>
#include <scorum/protocol/transaction.hpp>

#include <fc/crypto/elliptic.hpp>

#include "defines.hpp"

using namespace scorum::chain;
using namespace scorum::protocol;

BOOST_AUTO_TEST_CASE(test_one)
{
    create_budget_operation op;

    op.owner = "initdelegate";
    op.content_permlink = "";
    op.balance = ASSET("10000.000 SCR");
    op.deadline = fc::time_point_sec::from_iso_string("2018-01-23T10:17:29");

    signed_transaction tx;
    tx.operations.push_back(op);
    tx.validate();

    tx.ref_block_num = 15;
    tx.ref_block_prefix = 2399306698;
    tx.set_expiration(fc::time_point_sec::from_iso_string("2018-01-23T09:18:29"));

    const fc::optional<fc::ecc::private_key> private_key
        = graphene::utilities::wif_to_key("5JCvGL2GVVpjDrKzbKWPHEvuwFs5HdEGwr4brp8RQiwrpEFcZNP");

    chain_id_type chain_id = fc::sha256::hash("5ba96d2f6cbde94018464405519cd2ec16cbe6da4437849a7591a89d15abd494");

    tx.sign(*private_key, chain_id);

    std::cout << fc::json::to_string(tx) << std::endl;
}

BOOST_AUTO_TEST_CASE(test_two)
{
    const fc::optional<fc::ecc::private_key> private_key
        = graphene::utilities::wif_to_key("5JCvGL2GVVpjDrKzbKWPHEvuwFs5HdEGwr4brp8RQiwrpEFcZNP");

    chain_id_type chain_id("5ba96d2f6cbde94018464405519cd2ec16cbe6da4437849a7591a89d15abd494");

    digest_type::encoder enc;
    fc::raw::pack(enc, chain_id);

    fc::ecc::compact_signature sig = private_key->sign_compact(enc.result());

    BOOST_CHECK_EQUAL("2004ec9213ec914e5fcb1964ee3a2fc03666de517984ef3d84af73df32206f899d1f3b01eff2ec81e86c57b3cb77e161"
                      "76bcb5c82a82e7c5a0e1037b2b834035c7",
                      fc::json::to_string(sig));
}

BOOST_AUTO_TEST_CASE(test_sha)
{
    const fc::sha256 hash = fc::sha256::hash("text text text");
    BOOST_CHECK_EQUAL(hash, fc::sha256("95e4b20f5e669fab5fdaa2fc9f691192118f72900f9906f13b1883e2fb57aa43"));
}

std::string HexEscape(const std::string& buf)
{
    std::string result(buf.size() * 3 + 1, '\0');
    const char hex[] = "0123456789ABCDEF";

    for (size_t i = 0; i < buf.size(); ++i)
    {
        char* dest = &result[i * 3];
        *dest++ = hex[(buf[i] >> 4) & 0xF];
        *dest++ = hex[buf[i] & 0xF];
        *dest = ' ';
    }
    return result;
}

std::string HexEscape(const unsigned char* buf)
{
    size_t size = strlen((const char*)buf);

    std::string result(size * 3 + 1, '\0');
    const char hex[] = "0123456789ABCDEF";

    for (size_t i = 0; i < size; ++i)
    {
        char* dest = &result[i * 3];
        *dest++ = hex[(buf[i] >> 4) & 0xF];
        *dest++ = hex[buf[i] & 0xF];
        *dest = ' ';
    }
    return result;
}

BOOST_AUTO_TEST_CASE(test_signing_sha256)
{
    const fc::optional<fc::ecc::private_key> private_key
        = graphene::utilities::wif_to_key("5JCvGL2GVVpjDrKzbKWPHEvuwFs5HdEGwr4brp8RQiwrpEFcZNP");

    const fc::sha256 hash = fc::sha256::hash("text text text");

    fc::ecc::compact_signature sig = private_key->sign_compact(hash);

    // clang-format off
    BOOST_CHECK_EQUAL("1f29a517dfd87ea59774b0122eb8c75eff84ad5fed949c0588f91651925acf785714c332829b54bfec779f51e4a9f0a369fe7b259fe85276148447593542013a18", fc::json::to_string(sig));
    // clang-format on
}

BOOST_AUTO_TEST_CASE(test_signing_text)
{
    const fc::optional<fc::ecc::private_key> private_key
        = graphene::utilities::wif_to_key("5JCvGL2GVVpjDrKzbKWPHEvuwFs5HdEGwr4brp8RQiwrpEFcZNP");

    const fc::sha256 hash = fc::sha256::hash("text text text");

    fc::ecc::compact_signature sig = private_key->sign_compact(hash);

    std::cout << fc::json::to_string(sig) << "\n";

    scorum::protocol::public_key_type recover = fc::ecc::public_key(sig, hash);

    BOOST_CHECK_EQUAL("SCR5bgzuweaHx231escVuPVxgudSyUWdKAH7fKgxZfp3nKSirzFRa", (std::string)recover);
}

BOOST_AUTO_TEST_CASE(test_deser_signature)
{
    const fc::optional<fc::ecc::private_key> private_key
        = graphene::utilities::wif_to_key("5JCvGL2GVVpjDrKzbKWPHEvuwFs5HdEGwr4brp8RQiwrpEFcZNP");

    const fc::sha256 hash = fc::sha256::hash("text text text");

    std::cout << fc::json::to_string(sig) << "\n";

    scorum::protocol::public_key_type recover = fc::ecc::public_key(sig, hash);

    BOOST_CHECK_EQUAL("SCR5bgzuweaHx231escVuPVxgudSyUWdKAH7fKgxZfp3nKSirzFRa", (std::string)recover);
}
