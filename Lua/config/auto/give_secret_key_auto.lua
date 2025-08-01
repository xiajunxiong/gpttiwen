local _default_val={
    target={
        type=1
    }
}
local _itemlist={
    i_26812_2_1={
        item_id=26812,
        num=2,
        is_bind=1
    },
    i_26812_3_1={
        item_id=26812,
        num=3,
        is_bind=1
    },
    i_26813_1_1={
        item_id=26813,
        num=1,
        is_bind=1
    },
    i_27303_5_1={
        item_id=27303,
        num=5,
        is_bind=1
    },
    i_27303_10_1={
        item_id=27303,
        num=10,
        is_bind=1
    }
}
Config.give_secret_key_auto={
    other={
        {
            num_target=20
        }
    },
    target={
        {
            seq=0,
            parameter=20,
            item=_itemlist.i_26812_2_1
        },
        {
            seq=1,
            parameter=30,
            item=_itemlist.i_26812_3_1
        },
        {
            seq=2,
            parameter=40,
            item=_itemlist.i_26813_1_1
        },
        {
            seq=3,
            parameter=45,
            item=_itemlist.i_26813_1_1
        },
        {
            seq=4,
            parameter=50,
            item=_itemlist.i_26813_1_1
        },
        {
            seq=5,
            type=2,
            parameter=5,
            item=_itemlist.i_27303_5_1
        },
        {
            seq=6,
            type=2,
            parameter=7,
            item=_itemlist.i_27303_5_1
        },
        {
            seq=7,
            type=2,
            parameter=27,
            item=_itemlist.i_27303_10_1
        },
        {
            seq=8,
            type=2,
            parameter=29,
            item=_itemlist.i_27303_10_1
        }
    }
}
SetConfigDefaultVal(Config.give_secret_key_auto,_default_val,nil,nil,nil)