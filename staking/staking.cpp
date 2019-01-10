/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "staking.hpp"

#define ENTERPRISE_MIN_DEPOSIT 10000000
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
                bool approve = total_staked > ENTERPRISE_MIN_DEPOSIT ? true : false;
                _enterprises.modify(etp, _self, [&](auto &info) {
                    info.total_stake = total_staked;
                    info.is_approve = approve;
                    info.last_claim_time = eosio::time_point_sec(now());
                });
            }
            else
            {
                bool approve = transfer_data.quantity.amount > ENTERPRISE_MIN_DEPOSIT ? true : false;
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

            stake_infos stake_table(_self, enterprise);

            stake_table.emplace(_self, [&](auto &info) {
                info.id = stake_table.available_primary_key();
                info.staker = transfer_data.from;
                info.stake_num = transfer_data.quantity.amount;
                info.start_at = eosio::time_point_sec(now());
                info.end_at = eosio::time_point_sec(now()) + offer->duration_sec;
            });

            user_infos user_table(_self, enterprise);
            auto user = user_table.find(transfer_data.from);

            if (user != user_table.end())
            {

                user_table.modify(user, _self, [&](auto &info) {
                    info.free_cup += offer->free_cup;
                    info.updated_at = eosio::time_point_sec(now());
                });
            }
            else
            {
                user_table.emplace(_self, [&](user_info &info) {
                    info.user = enterprise;
                    info.free_cup += offer->free_cup;
                    info.updated_at = eosio::time_point_sec(now());
                });
            }
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

void staking::cupreceived(account_name account, account_name enterprise)
{
    require_auth(account);

    user_infos user_table(_self, enterprise);
    auto user = user_table.find(account);
    eosio_assert((user != user_table.end() && (user->free_cup > 0)), "You don't have free cafe in this enterprise");

    user_table.modify(user, _self, [&](auto &info) {
        info.free_cup -= 1;
        info.updated_at = eosio::time_point_sec(now());
    });
}
EOSIO_ABI(staking, (transfer)(regetp)(claim)(cupreceived))
