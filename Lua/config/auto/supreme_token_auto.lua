local _default_val={
    reward={
        box_type=1
    }
}
local _itemlist={
    i_24518_1_1={
        item_id=24518,
        num=1,
        is_bind=1
    },
    i_29021_1_1={
        item_id=29021,
        num=1,
        is_bind=1
    },
    i_28828_1_1={
        item_id=28828,
        num=1,
        is_bind=1
    },
    i_27187_1_1={
        item_id=27187,
        num=1,
        is_bind=1
    },
    i_25842_1_1={
        item_id=25842,
        num=1,
        is_bind=1
    },
    i_27620_1_1={
        item_id=27620,
        num=1,
        is_bind=1
    },
    i_28820_1_1={
        item_id=28820,
        num=1,
        is_bind=1
    },
    i_28555_1_1={
        item_id=28555,
        num=1,
        is_bind=1
    },
    i_28516_1_1={
        item_id=28516,
        num=1,
        is_bind=1
    },
    i_27399_3_1={
        item_id=27399,
        num=3,
        is_bind=1
    }
}
Config.supreme_token_auto={
    reward={
        {
            reward_item=_itemlist.i_24518_1_1
        },
        {
            reward_item=_itemlist.i_29021_1_1
        },
        {
            reward_item=_itemlist.i_28828_1_1
        },
        {
            reward_item=_itemlist.i_27187_1_1
        },
        {
            reward_item=_itemlist.i_25842_1_1
        },
        {
            reward_item=_itemlist.i_27620_1_1
        },
        {
            reward_item=_itemlist.i_28820_1_1
        },
        {
            reward_item=_itemlist.i_28555_1_1
        },
        {
            reward_item=_itemlist.i_28516_1_1
        },
        {
            reward_item=_itemlist.i_27399_3_1
        }
    },
    other={
        {
            activity_duration=3600,
            notice_interval=300,
            game_duration=60,
            reward_num=10,
            use_frequency=5
        }
    }
}
SetConfigDefaultVal(Config.supreme_token_auto,_default_val,nil,nil,nil)