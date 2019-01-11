/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "staking.hpp"
#include <eosiolib/transaction.hpp>


#define BEAN_TOKEN_CONTRACT N(thebeantoken)
#define BEAN_SYMBOL S(4, BEAN)

const uint32_t seconds_per_year = 52 * 7 * 24 * 3600;
const int64_t min_enterprise_stake = 10'000'0000;
const double continuous_rate = 0.05; // 5% annual rate

staking::staking(account_name _self)
    : eosio::contract(_self),
      _enterprises(_self, _self),
      _etp_offer(_self, _self)
{
}

// to stake send amount of bean token with message is account "enterprise"
void staking::transfer(uint64_t sender, uint64_t receiver)
{
    print("\n>>> sender >>>", sender, " - name: ", eosio::name{sender});
    print("\n>>> receiver >>>", receiver, " - name: ", eosio::name{receiver});

    auto transfer_data = eosio::unpack_action_data<transfer_args>();
    if (transfer_data.from == _self || transfer_data.to != _self)
    {
        return;
    }
    const account_name enterprise = eosio::string_to_name(transfer_data.memo.c_str());

    if (is_account(enterprise) && is_enterprise(enterprise))
    {
        eosio_assert(transfer_data.quantity.symbol == eosio::string_to_symbol(4, "BEAN"), "Only accepts BEAN");
        eosio_assert(transfer_data.quantity.is_valid(), "Invalid token transfer");
        eosio_assert(transfer_data.quantity.amount > 0, "Quantity must be positive");

        if (transfer_data.from == enterprise)
        {
            // enterprise is stake to itself

            auto etp = _enterprises.find(enterprise);

            if (etp != _enterprises.end())
            {
                auto total_staked = etp->total_stake + transfer_data.quantity.amount;
                bool approve = total_staked > min_enterprise_stake ? true : false;
                _enterprises.modify(etp, _self, [&](auto &info) {
                    info.total_stake = total_staked;
                    info.is_approve = approve;
                    info.last_claim_time = eosio::time_point_sec(now());
                });
            }
            else
            {
                bool approve = transfer_data.quantity.amount > min_enterprise_stake ? true : false;
                _enterprises.emplace(_self, [&](auto &info) {
                    info.owner = enterprise;
                    info.total_stake = transfer_data.quantity.amount;
                    info.is_approve = approve;
                    info.last_claim_time = eosio::time_point_sec(now());
                });
            }
        }
        else
        {
            // user stake for enterprise
            eosio_assert(is_enterprise_approved(enterprise), "enterprise doesn't meet requirement");

            auto offer = _etp_offer.find(enterprise);
            eosio_assert(offer != _etp_offer.end(), "no offer from this enterprise");
            eosio_assert(offer->stake_num == transfer_data.quantity.amount, "your tranfered amount doesn't match with enterprise offered amount");

            staker_infos stake_table(_self, enterprise);
            auto staker = stake_table.find(transfer_data.from);
            if (staker != stake_table.end())
            {
                eosio_assert(eosio::time_point_sec(now()) >= staker->end_at, "previous stake still exist");
                // if previous is out date, tranfer token back
                stake_table.modify(staker, _self, [&](auto &info) {
                    info.stake_num = transfer_data.quantity.amount;
                    info.start_at = eosio::time_point_sec(now());
                    info.end_at = eosio::time_point_sec(now()) + offer->duration_sec;
                    info.updated_at = eosio::time_point_sec(now());
                    info.free_cup += offer->free_cup;
                });
            }
            else
            {

                stake_table.emplace(_self, [&](auto &info) {
                    info.staker = transfer_data.from;
                    info.stake_num = transfer_data.quantity.amount;
                    info.start_at = eosio::time_point_sec(now());
                    info.end_at = eosio::time_point_sec(now()) + offer->duration_sec;
                    info.updated_at = eosio::time_point_sec(now());
                    info.free_cup = offer->free_cup;
                });
            }
            eosio::transaction transfer;
            transfer.actions.emplace_back(eosio::permission_level{_self, N(active)}, BEAN_TOKEN_CONTRACT, N(transfer), std::make_tuple(_self, transfer_data.from, transfer_data.quantity, std::string("Return staking token: eos.cafe")));
            transfer.send(offer->duration_sec, _self, false);
        }
    }
}

void staking::regetp(account_name enterprise, std::string name, std::string &url, uint16_t location)
{
    eosio_assert(name.size() < 128 && url.size() < 128, "url too long");
    require_auth(enterprise);

    auto etp = _enterprises.find(enterprise);

    if (etp != _enterprises.end())
    {
        _enterprises.modify(etp, enterprise, [&](auto &info) {
            info.name = name;
            info.name = name;
            info.url = url;
            info.location = location;
        });
    }
    else
    {
        _enterprises.emplace(enterprise, [&](auto &info) {
            info.owner = enterprise;
            info.name = name;
            info.is_approve = false;
            info.url = url;
            info.total_unpaid = 0;
            info.last_claim_time = eosio::time_point_sec(now());
            info.location = location;
        });
    }
}

// confirm received free cafe from shop
void staking::cupreceived(account_name account, account_name enterprise)
{
    require_auth(account);

    staker_infos stake_table(_self, enterprise);
    auto user = stake_table.find(account);
    eosio_assert((user != stake_table.end() && (user->free_cup > 0)), "You don't have free cafe in this enterprise");

    stake_table.modify(user, _self, [&](auto &info) {
        info.free_cup -= 1;
        info.updated_at = eosio::time_point_sec(now());
    });
}

void staking::claimrewards(account_name enterprise)
{
    eosio_assert(is_enterprise_approved(enterprise), "only enterprise can get staking reward");
    auto etp = _enterprises.find(enterprise);
    auto reward_token = eosio::asset(etp->total_unpaid, BEAN_SYMBOL);

    eosio::transaction transfer;
    transfer.actions.emplace_back(eosio::permission_level{_self, N(active)}, BEAN_TOKEN_CONTRACT, N(transfer), std::make_tuple(_self, enterprise, reward_token, std::string("Staking Reward: eos.cafe")));
    transfer.send(0, _self, false);
}

bool staking::is_enterprise(account_name account)
{
    auto etp = _enterprises.find(account);

    return etp != _enterprises.end() ? true : false;
}

bool staking::is_enterprise_approved(account_name enterprise)
{
    auto etp = _enterprises.find(enterprise);
    if (etp == _enterprises.end())
        return false;
    return etp->is_approve;
}

#define EOSIO_ABI_EX(TYPE, MEMBERS)                                                                             \
    extern "C"                                                                                                  \
    {                                                                                                           \
        void apply(uint64_t receiver, uint64_t code, uint64_t action)                                           \
        {                                                                                                       \
            auto self = receiver;                                                                               \
            if (code == self || code == N(eosio.token) || code == BEAN_TOKEN_CONTRACT || action == N(onerror))  \
            {                                                                                                   \
                if (action == N(transfer))                                                                      \
                {                                                                                               \
                    eosio_assert(code == N(eosio.token) || code == BEAN_TOKEN_CONTRACT, "Must transfer Token"); \
                }                                                                                               \
                TYPE thiscontract(self);                                                                        \
                switch (action)                                                                                 \
                {                                                                                               \
                    EOSIO_API(TYPE, MEMBERS)                                                                    \
                }                                                                                               \
                /* does not allow destructor of thiscontract to run: eosio_exit(0); */                          \
            }                                                                                                   \
        }                                                                                                       \
    }

EOSIO_ABI(staking, (transfer)(regetp)(claimrewards)(cupreceived))
