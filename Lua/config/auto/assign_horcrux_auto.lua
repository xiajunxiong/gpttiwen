local _default_val={
    gift_configure={
        seq=0,
        is_borcast=1
    }
}
local _itemlist={
    i_29086_1_1={
        item_id=29086,
        num=1,
        is_bind=1
    },
    i_25542_1_1={
        item_id=25542,
        num=1,
        is_bind=1
    },
    i_27657_10_1={
        item_id=27657,
        num=10,
        is_bind=1
    },
    i_27656_30_1={
        item_id=27656,
        num=30,
        is_bind=1
    },
    i_27723_10_1={
        item_id=27723,
        num=10,
        is_bind=1
    },
    i_29087_1_1={
        item_id=29087,
        num=1,
        is_bind=1
    },
    i_27657_20_1={
        item_id=27657,
        num=20,
        is_bind=1
    }
}
Config.assign_horcrux_auto={
    gift_configure={
        {
            type=0,
            section_start=1,
            section_end=7,
            reward_item={
                [0]=_itemlist.i_29086_1_1,
                [1]=_itemlist.i_25542_1_1,
                [2]=_itemlist.i_27657_10_1,
                [3]=_itemlist.i_27656_30_1,
                [4]=_itemlist.i_27723_10_1
            },
            acc_price=30000,
            gift_item=_itemlist.i_29086_1_1
        },
        {
            type=1,
            section_start=8,
            section_end=0,
            reward_item={
                [0]=_itemlist.i_29087_1_1,
                [1]=_itemlist.i_25542_1_1,
                [2]=_itemlist.i_27657_20_1,
                [3]=_itemlist.i_27656_30_1,
                [4]=_itemlist.i_27723_10_1
            },
            acc_price=60000,
            gift_item=_itemlist.i_29087_1_1
        }
    }
}
SetConfigDefaultVal(Config.assign_horcrux_auto,_default_val,nil,nil,nil)