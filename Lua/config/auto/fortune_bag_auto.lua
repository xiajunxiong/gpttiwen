local _default_val={
    reward={
        discount=3.3,
        blessing_item=27397
    }
}
local _itemlist={
    i_25842_1_1={
        item_id=25842,
        num=1,
        is_bind=1
    },
    i_27399_1_1={
        item_id=27399,
        num=1,
        is_bind=1
    },
    i_26812_2_1={
        item_id=26812,
        num=2,
        is_bind=1
    },
    i_27402_2_1={
        item_id=27402,
        num=2,
        is_bind=1
    },
    i_25842_3_1={
        item_id=25842,
        num=3,
        is_bind=1
    },
    i_27399_3_1={
        item_id=27399,
        num=3,
        is_bind=1
    },
    i_26812_6_1={
        item_id=26812,
        num=6,
        is_bind=1
    },
    i_27402_6_1={
        item_id=27402,
        num=6,
        is_bind=1
    },
    i_22006_1_0={
        item_id=22006,
        num=1,
        is_bind=0
    }
}
Config.fortune_bag_auto={
    reward={
        {
            seq=0,
            money=10,
            rewards_item={
                [0]=_itemlist.i_25842_1_1,
                [1]=_itemlist.i_27399_1_1,
                [2]=_itemlist.i_26812_2_1,
                [3]=_itemlist.i_27402_2_1
            },
            praycoin=2,
            discount=0.5
        },
        {
            seq=1,
            money=30,
            rewards_item={
                [0]=_itemlist.i_25842_1_1,
                [1]=_itemlist.i_27399_1_1,
                [2]=_itemlist.i_26812_2_1,
                [3]=_itemlist.i_27402_2_1
            },
            praycoin=3,
            discount=1.6
        },
        {
            seq=2,
            money=60,
            rewards_item={
                [0]=_itemlist.i_25842_1_1,
                [1]=_itemlist.i_27399_1_1,
                [2]=_itemlist.i_26812_2_1,
                [3]=_itemlist.i_27402_2_1
            },
            praycoin=5
        },
        {
            seq=3,
            money=500,
            rewards_item={
                [0]=_itemlist.i_25842_3_1,
                [1]=_itemlist.i_27399_3_1,
                [2]=_itemlist.i_26812_6_1,
                [3]=_itemlist.i_27402_6_1
            },
            praycoin=10
        }
    },
    other={
        {
            rewards_item=_itemlist.i_22006_1_0,
            reward_1=_itemlist.i_27399_1_1,
            reward_2=_itemlist.i_25842_1_1,
            reward_3=_itemlist.i_27402_2_1,
            reward_4=_itemlist.i_26812_2_1,
            cfg_ver=3
        }
    },
    reward_group={
        {
            seq=0,
            money=500,
            rewards_item={
                [0]=_itemlist.i_25842_3_1,
                [1]=_itemlist.i_27399_3_1,
                [2]=_itemlist.i_26812_6_1,
                [3]=_itemlist.i_27402_6_1
            },
            praycoin=10,
            discount=0,
            blessing_item=27397,
            activity_days=7
        }
    }
}
SetConfigDefaultVal(Config.fortune_bag_auto,_default_val,nil,nil,nil)