local _default_val={
    give_num={
        fifth_company=0
    }
}
local _itemlist={
    i_26812_2_1={
        item_id=26812,
        num=2,
        is_bind=1
    }
}
Config.pet_search_auto={
    other={
        {
            open=130,
            close=131,
            reward_item=_itemlist.i_26812_2_1
        }
    },
    give_num={
        {
            seq=1,
            single_draw=5
        },
        {
            seq=2,
            single_draw=11
        },
        {
            seq=3,
            single_draw=2,
            fifth_company=2
        }
    }
}
SetConfigDefaultVal(Config.pet_search_auto,_default_val,nil,nil,nil)