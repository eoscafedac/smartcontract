STAKING CONTRACT
-----------------


#### Prerequisites


* Please use following accounts to test

etpcontract4 -  EOS5ZwsT4k5szwhgbxyoWa5D2tHYAPxEwQKrHxWtU3xnPeJkudLfF
thebeantoken - EOS5ZwsT4k5szwhgbxyoWa5D2tHYAPxEwQKrHxWtU3xnPeJkudLfF

* Endpoint network:

http://jungle2.cryptolions.io:80

* Contract name: etpcontract4


##### Set contract to etpcontract4 account
````bash
$ cleos -u http://jungle2.cryptolions.io:80 set contract etpcontract4 ./staking/ -p etpcontract4
````

##### Check contract ram
````bash
cleos -u http://jungle2.cryptolions.io:80 get account etpcontract4
...
memory: 
  quota:      1010 KiB    used:     338.2 KiB
````

##### eosio.code permission, remember change publickey


````bash

$ cleos -u http://jungle2.cryptolions.io:80 set account permission etpcontract4 active '{"threshold": 1,"keys": [{"key": "EOS5ZwsT4k5szwhgbxyoWa5D2tHYAPxEwQKrHxWtU3xnPeJkudLfF","weight": 1}],"accounts": [{"permission":{"actor":"etpcontract4","permission":"eosio.code"},"weight":1}]}' owner -p etpcontract4

````

##### Register Enterprice: 

````bash
$ $ cleos -u http://jungle2.cryptolions.io:80 push action etpcontract4 regetp '["enterprice12", "Cafe In The Moon", "moon.cafe", 123, "0.0000 MOON"]' -p enterprice12

$ cleos -u http://jungle2.cryptolions.io:80 push action etpcontract4 regetp '["notokenfree1", "Get discount from abcdef", "abcdef.cafe", 213, "0.00 DIS"]' -p notokenfree1

$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract4 etpcontract4 enterprises
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


$ cleos -u http://jungle2.cryptolions.io:80 push action thebeantoken transfer '["notokenfree1", "etpcontract4", "10000001.0000 BEAN", "notokenfree1"]' -p notokenfree1

$ cleos -u http://jungle2.cryptolions.io:80 push action thebeantoken transfer '["enterprice12", "etpcontract4", "3000000.0000 BEAN", "enterprice12"]' -p enterprice12

$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract4 etpcontract4 enterprises
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
$ cleos -u http://jungle2.cryptolions.io:80 push action thebeantoken transfer '["enterprice12", "etpcontract4", "2000000.0000 BEAN", "enterprice12"]' -p enterprice12

$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract4 etpcontract4 enterprises
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
$ cleos -u http://jungle2.cryptolions.io:80 push action etpcontract4 setoffer '["enterprice12", "Get 3 Free Cafe in Moon", "All detials offer here", "12345.0000 BEAN", "12345.0000 BEAN", 300, "3.0000 MOON", true]' -p enterprice12

$ cleos -u http://jungle2.cryptolions.io:80 push action etpcontract4 setoffer '["notokenfree1", "Create Video to get Discount", "All detail offer in here", "10000.0000 BEAN", "100000.0000 BEAN", 1000, "0.75 DIS", true]' -p notokenfree1

$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract4 etpcontract4 etpoffers
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

$ cleos -u http://jungle2.cryptolions.io:80  push action thebeantoken transfer '["wantfreecafe", "etpcontract4", "12345.0000 BEAN", "enterprice12"]' -p wantfreecafe
$ cleos -u http://jungle2.cryptolions.io:80  push action thebeantoken transfer '["getdiscount1", "etpcontract4", "500000.0000 BEAN", "notokenfree1"]' -p getdiscount1

$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract4 etpcontract4 stakerinfos
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
$ cleos -u http://jungle2.cryptolions.io:80 get table thebeantoken wantfreecafe accounts
{
  "rows": [{
      "balance": "950518.5310 BEAN"
    }
  ],
  "more": false
}

$ cleos -u http://jungle2.cryptolions.io:80  push action etpcontract4  refund '["wantfreecafe", 2]' -p wantfreecafe

$ cleos -u http://jungle2.cryptolions.io:80 get table thebeantoken wantfreecafe accounts
{
  "rows": [{
      "balance": "962863.5310 BEAN"
    }
  ],
  "more": false
}

$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract4 etpcontract4 stakerinfos
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
$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract4 etpcontract4 enterprises
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

$ cleos -u http://jungle2.cryptolions.io:80  push action etpcontract4  claimrewards '["enterprice12"]' -p enterprice12

$ cleos -u http://jungle2.cryptolions.io:80 get table etpcontract4 etpcontract4 enterprises
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
