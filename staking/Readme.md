STAKING CONTRACT
-----------------


#### Prerequisites


* Please use following accounts to test

guacamolenom -  EOS5ZwsT4k5szwhgbxyoWa5D2tHYAPxEwQKrHxWtU3xnPeJkudLfF
thebeantoken - EOS5ZwsT4k5szwhgbxyoWa5D2tHYAPxEwQKrHxWtU3xnPeJkudLfF

* Endpoint network:

https://api.eosdetroit.io:443

* Contract name: guacamolenom


##### Set contract to guacamolenom account
````bash

eosiocpp -o staking.wast staking.cpp

$ cleos -u https://api.eosdetroit.io:443 set contract guacamolenom ./staking/ -p guacamolenom
````

##### Check contract ram
````bash
cleos -u https://api.eosdetroit.io:443 get account guacamolenom
...
memory: 
  quota:      1010 KiB    used:     338.2 KiB
````

##### eosio.code permission, remember change publickey


````bash

$ cleos -u https://api.eosdetroit.io:443 set account permission guacamolenom active '{"threshold": 1,"keys": [{"key": "EOS6JcSNG8VSMgCkiPLLhuMsbNwX3q4xxfDd5CS49eTMUyeFmCgAd","weight": 1}],"accounts": [{"permission":{"actor":"guacamolenom","permission":"eosio.code"},"weight":1}]}' owner -p guacamolenom

````

##### initilize


````bash

#define GLOBAL_ID_ACTIVE 101
#define GLOBAL_ID_MIN_ETP_STAKE 102

$ cleos -u https://api.eosdetroit.io:443  push action guacamolenom initialize '[""]' -p guacamolenom

#set min enterprice stake = 100'000 BEAN = 100'000.0000
$ cleos -u https://api.eosdetroit.io:443  push action guacamolenom setglobal '[102, 1000000000]' -p guacamolenom

$ cleos -u https://api.eosdetroit.io:443  get table guacamolenom guacamolenom globals
{
  "rows": [{
      "id": 101,
      "val": 1
    },{
      "id": 102,
      "val": "10000000000"
    }
  ],
  "more": false
}

````

##### Register Enterprice: 

````bash
$ $ cleos -u https://api.eosdetroit.io:443 push action guacamolenom regetp '["enterprice12", "Cafe In The Moon", "moon.cafe", 123, "0.0000 MOON"]' -p enterprice12

$ cleos -u https://api.eosdetroit.io:443 push action guacamolenom regetp '["notokenfree1", "Get discount from abcdef", "abcdef.cafe", 213, "0.00 DIS"]' -p notokenfree1

$ cleos -u https://api.eosdetroit.io:443 get table guacamolenom guacamolenom enterprises
{
  "rows": [{
      "owner": "enterprice12",
      "name": "Cafe In The Moon",
      "url": "moon.cafe",
      "location": 123,
      "coupon_name": "0.0000 MOON",
      "deposit": "0.0000 BEAN",
      "total_stake": "0.0000 BEAN",
      "total_unpaid": "0.0000 BEAN",
      "last_claim_time": "2019-02-16T08:06:07",
      "is_approve": 0
    },{
      "owner": "notokenfree1",
      "name": "Get discount from abcdef",
      "url": "abcdef.cafe",
      "location": 213,
      "coupon_name": "0.00 DIS",
      "deposit": "0.0000 BEAN",
      "total_stake": "0.0000 BEAN",
      "total_unpaid": "0.0000 BEAN",
      "last_claim_time": "2019-02-16T08:06:42",
      "is_approve": 0
    }
  ],
  "more": false
}


````


##### Enterprice start staking, at least 10.000.000 BEAN to becomean enterprice
````bash


$ cleos -u https://api.eosdetroit.io:443 push action thebeantoken transfer '["notokenfree1", "guacamolenom", "10000001.0000 BEAN", "notokenfree1"]' -p notokenfree1

$ cleos -u https://api.eosdetroit.io:443 push action thebeantoken transfer '["enterprice12", "guacamolenom", "3000000.0000 BEAN", "enterprice12"]' -p enterprice12

$ cleos -u https://api.eosdetroit.io:443 get table guacamolenom guacamolenom enterprises
{
  "rows": [{
      "owner": "enterprice12",
      "name": "Cafe In The Moon",
      "url": "moon.cafe",
      "location": 123,
      "coupon_name": "0.0000 MOON",
      "deposit": "3000000.0000 BEAN",
      "total_stake": "0.0000 BEAN",
      "total_unpaid": "0.0000 BEAN",
      "last_claim_time": "2019-02-16T08:16:15",
      "is_approve": 0
    },{
      "owner": "notokenfree1",
      "name": "Get discount from abcdef",
      "url": "abcdef.cafe",
      "location": 213,
      "coupon_name": "0.00 DIS",
      "deposit": "10000001.0000 BEAN",
      "total_stake": "0.0000 BEAN",
      "total_unpaid": "0.0000 BEAN",
      "last_claim_time": "2019-02-16T08:14:33",
      "is_approve": 1
    }
  ],
  "more": false
}

// onely aprrove after deposit 10.000.000 BEAN
$ cleos -u https://api.eosdetroit.io:443 push action thebeantoken transfer '["enterprice12", "guacamolenom", "2000000.0000 BEAN", "enterprice12"]' -p enterprice12

$ cleos -u https://api.eosdetroit.io:443 get table guacamolenom guacamolenom enterprises
{
  "rows": [{
      "owner": "enterprice12",
      "name": "Cafe In The Moon",
      "url": "moon.cafe",
      "location": 123,
      "coupon_name": "0.0000 MOON",
      "deposit": "10000000.0000 BEAN",
      "total_stake": "0.0000 BEAN",
      "total_unpaid": "0.3338 BEAN",
      "last_claim_time": "2019-02-16T08:17:25",
      "is_approve": 1
    },{
      "owner": "notokenfree1",
      "name": "Get discount from abcdef",
      "url": "abcdef.cafe",
      "location": 213,
      "coupon_name": "0.00 DIS",
      "deposit": "10000001.0000 BEAN",
      "total_stake": "0.0000 BEAN",
      "total_unpaid": "0.0000 BEAN",
      "last_claim_time": "2019-02-16T08:14:33",
      "is_approve": 1
    }
  ],
  "more": false
}


````

##### enterprice set offer
````bash
$ cleos -u https://api.eosdetroit.io:443 push action guacamolenom setoffer '["enterprice12", "Get 3 Free Cafe in Moon", "All detials offer here", "12345.0000 BEAN", "12345.0000 BEAN", 300, "3.0000 MOON", true]' -p enterprice12

$ cleos -u https://api.eosdetroit.io:443 push action guacamolenom setoffer '["notokenfree1", "Create Video to get Discount", "All detail offer in here", "10000.0000 BEAN", "100000.0000 BEAN", 1000, "0.75 DIS", true]' -p notokenfree1

$ cleos -u https://api.eosdetroit.io:443 get table guacamolenom guacamolenom etpoffers
{
  "rows": [{
      "owner": "enterprice12",
      "is_active": 1,
      "offer_head": "Get 3 Free Cafe in Moon",
      "offer_details": "All detials offer here",
      "min_stake": "12345.0000 BEAN",
      "max_stake": "12345.0000 BEAN",
      "duration_sec": 300,
      "coupon_quantity": "3.0000 MOON"
    },{
      "owner": "notokenfree1",
      "is_active": 1,
      "offer_head": "Create Video to get Discount",
      "offer_details": "All detail offer in here",
      "min_stake": "10000.0000 BEAN",
      "max_stake": "100000.0000 BEAN",
      "duration_sec": 1000,
      "coupon_quantity": "0.75 DIS"
    }
  ],
  "more": false
}



````

##### User stake to get free Cafe or get deal from ETP

````bash

$ cleos -u https://api.eosdetroit.io:443  push action thebeantoken transfer '["wantfreecafe", "guacamolenom", "12345.0000 BEAN", "enterprice12"]' -p wantfreecafe
$ cleos -u https://api.eosdetroit.io:443  push action thebeantoken transfer '["getdiscount1", "guacamolenom", "500000.0000 BEAN", "notokenfree1"]' -p getdiscount1

$ cleos -u https://api.eosdetroit.io:443 get table guacamolenom guacamolenom stakerinfos
{
  "rows": [{
      "id": 2,
      "staker": "wantfreecafe",
      "enterprise": "enterprice12",
      "stake_num": "12345.0000 BEAN",
      "reward_etp": "0.0058 BEAN",
      "coupon": "3.0000 MOON",
      "start_at": "2019-02-16T08:32:32",
      "end_at": "2019-02-16T08:37:32",
      "is_done": 0
    },{
      "id": 3,
      "staker": "getdiscount1",
      "enterprise": "notokenfree1",
      "stake_num": "50000.0000 BEAN",
      "reward_etp": "0.0794 BEAN",
      "coupon": "0.75 DIS",
      "start_at": "2019-02-16T08:37:50",
      "end_at": "2019-02-16T08:54:30",
      "is_done": 0
    }
  ],
  "more": false

````

##### Get refund:

````bash
$ cleos -u https://api.eosdetroit.io:443 get table thebeantoken wantfreecafe accounts
{
  "rows": [{
      "balance": "950518.5310 BEAN"
    }
  ],
  "more": false
}

$ cleos -u https://api.eosdetroit.io:443  push action guacamolenom  refund '["wantfreecafe", 2]' -p wantfreecafe

$ cleos -u https://api.eosdetroit.io:443 get table thebeantoken wantfreecafe accounts
{
  "rows": [{
      "balance": "962863.5310 BEAN"
    }
  ],
  "more": false
}

$ cleos -u https://api.eosdetroit.io:443 get table guacamolenom guacamolenom stakerinfos
{
  "rows": [{
      "id": 2,
      "staker": "wantfreecafe",
      "enterprise": "enterprice12",
      "stake_num": "12345.0000 BEAN",
      "reward_etp": "0.0058 BEAN",
      "coupon": "3.0000 MOON",
      "start_at": "2019-02-16T08:32:32",
      "end_at": "2019-02-16T08:37:32",
      "is_done": 1
    }
  ],
  "more": false
}

````

##### Claim reward:

````bash
$ cleos -u https://api.eosdetroit.io:443 get table guacamolenom guacamolenom enterprises
{
  "rows": [{
      "owner": "enterprice12",
      "name": "Cafe In The Moon",
      "url": "moon.cafe",
      "location": 123,
      "coupon_name": "0.0000 MOON",
      "deposit": "10000000.0000 BEAN",
      "total_stake": "24690.0000 BEAN",
      "total_unpaid": "0.3396 BEAN",
      "last_claim_time": "2019-02-16T08:17:25",
      "is_approve": 1
    }
  ],
  "more": false
}

$ cleos -u https://api.eosdetroit.io:443  push action guacamolenom  claimrewards '["enterprice12"]' -p enterprice12

$ cleos -u https://api.eosdetroit.io:443 get table guacamolenom guacamolenom enterprises
{
  "rows": [{
      "owner": "enterprice12",
      "name": "Cafe In The Moon",
      "url": "moon.cafe",
      "location": 123,
      "coupon_name": "0.0000 MOON",
      "deposit": "10000000.0000 BEAN",
      "total_stake": "24690.0000 BEAN",
      "total_unpaid": "0.0000 BEAN",
      "last_claim_time": "2019-02-16T08:41:07",
      "is_approve": 1
    }
  ],
  "more": false
}

````
