/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "staking.hpp"
#include <eosiolib/transaction.hpp>

#define BEAN_TOKEN_CONTRACT N(thebeantoken)
#define BEAN_SYMBOL S(4, BEAN)

const uint64_t token_supply_amount = 4'000'000'000'0000;
const uint32_t seconds_per_year = 52 * 7 * 24 * 3600;
const uint64_t min_enterprise_stake = 1'000'000'0000;
#define GLOBAL_ID_ACTIVE 101
#define GLOBAL_ID_MIN_ETP_STAKE 102

const double continuous_rate = 0.05; // 5% annual rate

staking::staking(account_name _self)
    : eosio::contract(_self),
      _enterprises(_self, _self),
      _etp_offer(_self, _self),
      _staker_infos(_self, _self),
      _globals(_self, _self)
{
}

// to stake send amount of bean token with message is account "enterprise"
void staking::transfer(uint64_t sender, uint64_t receiver)
{
    auto transfer_data = eosio::unpack_action_data<transfer_args>();
    if (transfer_data.from == _self || transfer_data.to != _self)
    {
        return;
    }
    eosio_assert(transfer_data.quantity.symbol == eosio::string_to_symbol(4, "BEAN"), "Only accepts BEAN");
    eosio_assert(transfer_data.quantity.is_valid(), "Invalid token transfer");
    eosio_assert(transfer_data.quantity.amount > 0, "Quantity must be positive");

    const account_name enterprise = eosio::string_to_name(transfer_data.memo.c_str());
    eosio_assert(is_account(enterprise), "stake to account does not exist");

    auto etp = _enterprises.find(enterprise);
    eosio_assert(etp != _enterprises.end(), "enterprises does not exist");

    if (transfer_data.from == enterprise)
    {
        auto active_pos = _globals.find(GLOBAL_ID_ACTIVE);
        eosio_assert(active_pos != _globals.end() && active_pos->val, "Maintaining ...");

        auto main_stake_itr = _globals.find(GLOBAL_ID_MIN_ETP_STAKE);
        eosio_assert(main_stake_itr != _globals.end(), "global id does not exist");

        // deposit
        auto total_deposit = etp->deposit + transfer_data.quantity;
        bool approve = total_deposit.amount >= main_stake_itr->val ? true : false;
        // calculate last staked deposit
        auto staked_duration = eosio::time_point_sec(now()).sec_since_epoch() - etp->last_claim_time.sec_since_epoch();
        uint64_t reward_amount = continuous_rate * etp->deposit.amount * staked_duration / seconds_per_year;

        _enterprises.modify(etp, _self, [&](auto &info) {
            info.deposit = total_deposit;
            info.total_unpaid += eosio::asset(reward_amount, BEAN_SYMBOL);
            info.is_approve = approve;
            info.last_claim_time = eosio::time_point_sec(now());
        });
    }
    else
    {
        auto active_pos = _globals.find(GLOBAL_ID_ACTIVE);
        eosio_assert(active_pos != _globals.end() && active_pos->val, "Maintaining ...");

        // user stake for enterprise
        eosio_assert(etp->is_approve, "enterprise doesn't meet requirement");

        auto offer = _etp_offer.find(enterprise);
        eosio_assert((offer != _etp_offer.end()) && (offer->is_active == true), "no offer from this enterprise");
        eosio_assert((offer->min_stake <= transfer_data.quantity) && (offer->max_stake >= transfer_data.quantity), "your tranfered amount doesn't match with enterprise offered amount");

        uint64_t reward_etp = continuous_rate * transfer_data.quantity.amount * offer->duration_sec / seconds_per_year;

        _staker_infos.emplace(_self, [&](auto &info) {
            info.id = _staker_infos.available_primary_key();
            info.staker = transfer_data.from;
            info.enterprise = enterprise;
            info.stake_num = transfer_data.quantity;
            info.reward_etp = eosio::asset(reward_etp, BEAN_SYMBOL);
            info.coupon = offer->coupon_quantity;
            info.start_at = eosio::time_point_sec(now());
            info.end_at = eosio::time_point_sec(now()) + offer->duration_sec;
            info.status = STAKING;
        });
        _enterprises.modify(etp, _self, [&](auto &info) {
            info.total_stake += transfer_data.quantity;
        });
    }
}

void staking::initialize()
{
    require_auth(_self);

    _globals.emplace(_self, [&](auto &a) {
        a.id = GLOBAL_ID_ACTIVE;
        a.val = 1;
    });

    _globals.emplace(_self, [&](auto &a) {
        a.id = GLOBAL_ID_MIN_ETP_STAKE;
        a.val = min_enterprise_stake;
    });
}

void staking::setglobal(uint64_t id, uint64_t val)
{
    require_auth(_self);
    auto pos = _globals.find(id);
    if (pos == _globals.end())
    {
        _globals.emplace(_self, [&](auto &info) {
            info.id = id;
            info.val = val;
        });
    }
    else
    {
        _globals.modify(pos, 0, [&](auto &info) {
            info.val = val;
        });
    }
}

void staking::regetp(account_name enterprise, std::string name, std::string url, uint16_t location, eosio::asset coupon)
{
    require_auth(enterprise);

    eosio_assert(name.size() < 128 && url.size() < 128, "url too long");

    auto etp = _enterprises.find(enterprise);

    if (etp != _enterprises.end())
    {
        _enterprises.modify(etp, enterprise, [&](auto &info) {
            info.name = name;
            info.url = url;
            info.location = location;
            info.coupon_name = eosio::asset(0, coupon.symbol);
        });
    }
    else
    {
        _enterprises.emplace(enterprise, [&](auto &info) {
            info.owner = enterprise;
            info.name = name;
            info.is_approve = false;
            info.url = url;
            info.location = location;
            info.coupon_name = eosio::asset(0, coupon.symbol);
            info.last_claim_time = eosio::time_point_sec(now());
        });
    }
}

void staking::setoffer(account_name owner, std::string offer_head, std::string offer_details, eosio::asset min_stake, eosio::asset max_stake, uint64_t duration_sec, eosio::asset coupon_quantity, bool is_active)
{
    require_auth(owner);
    auto etp = _enterprises.find(owner);

    eosio_assert((etp != _enterprises.end()) && (etp->is_approve == true), "enterprise does not exist");
    eosio_assert(offer_head.size() < 128, "header is too long");
    eosio_assert(min_stake <= max_stake, "min stake should be smaller than max stake");
    eosio_assert(etp->coupon_name.symbol == coupon_quantity.symbol, "symbol mismatch");

    auto offer = _etp_offer.find(owner);

    if (offer != _etp_offer.end())
    {
        _etp_offer.modify(offer, _self, [&](auto &offer) {
            offer.offer_head = offer_head;
            offer.offer_details = offer_details;
            offer.min_stake = min_stake;
            offer.max_stake = max_stake;
            offer.duration_sec = duration_sec;
            offer.coupon_quantity = coupon_quantity;
            offer.is_active = is_active;
        });
    }
    else
    {
        _etp_offer.emplace(_self, [&](auto &offer) {
            offer.owner = owner;
            offer.offer_head = offer_head;
            offer.offer_details = offer_details;
            offer.min_stake = min_stake;
            offer.max_stake = max_stake;
            offer.duration_sec = duration_sec;
            offer.coupon_quantity = coupon_quantity;
            offer.is_active = is_active;
        });
    }
}

// confirm received free cafe from shop
void staking::docoupon(account_name enterprise, uint64_t staker_id)
{
    require_auth(enterprise);

    auto stake_info = _staker_infos.find(staker_id);
    eosio_assert(stake_info != _staker_infos.end(), "stake id does not exist");
    eosio_assert(stake_info->status == REFUND, "stake id out of REFUND state");
    eosio_assert(stake_info->enterprise == enterprise, "incorrect enterprise");
    _staker_infos.modify(stake_info, _self, [&](auto &info) {
        info.status = DONE;
    });
}

void staking::claimrewards(account_name enterprise)
{
    require_auth(enterprise);

    auto etp = _enterprises.find(enterprise);
    eosio_assert((etp != _enterprises.end()) && (etp->is_approve), "enterprise doesn't exist");

    auto staked_duration = eosio::time_point_sec(now()).sec_since_epoch() - etp->last_claim_time.sec_since_epoch();
    uint64_t deposit_reward = continuous_rate * etp->deposit.amount * staked_duration / seconds_per_year;

    auto total_unpaid_reward = etp->total_unpaid + eosio::asset(deposit_reward, BEAN_SYMBOL);

    _enterprises.modify(etp, _self, [&](auto &info) {
        info.total_unpaid = eosio::asset(0, BEAN_SYMBOL);
        info.last_claim_time = eosio::time_point_sec(now());
    });

    eosio::action(
        eosio::permission_level{_self, N(active)},
        BEAN_TOKEN_CONTRACT, N(transfer),
        std::make_tuple(_self, enterprise, total_unpaid_reward, std::string("Staking Reward For Enterprice from eos.cafe")))
        .send();
}

void staking::refund(account_name staker, uint64_t staker_id)
{
    auto stake_info = _staker_infos.find(staker_id);

    require_auth(stake_info->staker);

    eosio_assert(stake_info != _staker_infos.end(), "stake id does not exist");
    eosio_assert(stake_info->end_at <= eosio::time_point_sec(now()), "your stake is still in offer's duration");
    eosio_assert(stake_info->status == STAKING, "stake id out of STAKING state");

    auto etp = _enterprises.find(stake_info->enterprise);
    eosio_assert((etp != _enterprises.end()) && (etp->is_approve == true), "enterprise does not exist");

    _enterprises.modify(etp, _self, [&](auto &info) {
        info.total_stake -= stake_info->stake_num;
        info.total_unpaid += stake_info->reward_etp;
    });

    _staker_infos.modify(stake_info, _self, [&](auto &info) {
        info.status = REFUND;
    });

    eosio::action(
        eosio::permission_level{_self, N(active)},
        BEAN_TOKEN_CONTRACT, N(transfer),
        std::make_tuple(_self, stake_info->staker, stake_info->stake_num, std::string("Refund staked token from eos.cafe")))
        .send();
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

EOSIO_ABI_EX(staking, (transfer)(initialize)(setglobal)(regetp)(setoffer)(docoupon)(claimrewards)(refund))
