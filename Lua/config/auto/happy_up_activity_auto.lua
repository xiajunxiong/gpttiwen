local _default_val={
    happy_up={
        section_start=1,
        section_end=7,
        item_seq=0,
        consume_item_id=26344,
        consume_item_count=5,
        today_times=10,
        buy_times=0,
        limit_type=2,
        time=0,
        show=0,
        time1=0,
        show1=0
    }
}
local _itemlist={
    i_28664_1_1={
        item_id=28664,
        num=1,
        is_bind=1
    },
    i_29017_1_1={
        item_id=29017,
        num=1,
        is_bind=1
    },
    i_27838_1_1={
        item_id=27838,
        num=1,
        is_bind=1
    },
    i_26073_1_1={
        item_id=26073,
        num=1,
        is_bind=1
    },
    i_31527_1_1={
        item_id=31527,
        num=1,
        is_bind=1
    },
    i_31528_1_1={
        item_id=31528,
        num=1,
        is_bind=1
    },
    i_28601_1_1={
        item_id=28601,
        num=1,
        is_bind=1
    },
    i_25614_1_1={
        item_id=25614,
        num=1,
        is_bind=1
    },
    i_28667_1_1={
        item_id=28667,
        num=1,
        is_bind=1
    }
}
Config.happy_up_activity_auto={
    happy_up={
        {
            exchange_item=_itemlist.i_28664_1_1,
            consume_item_count=20
        },
        {
            item_seq=1,
            exchange_item=_itemlist.i_29017_1_1
        },
        {
            item_seq=2,
            exchange_item=_itemlist.i_27838_1_1
        },
        {
            item_seq=3,
            exchange_item=_itemlist.i_26073_1_1,
            consume_item_count=8
        },
        {
            item_seq=4,
            exchange_item=_itemlist.i_31527_1_1,
            consume_item_count=25,
            today_times=0,
            buy_times=1,
            limit_type=1,
            time=72,
            show=1,
            time1=72,
            show1=1
        },
        {
            item_seq=5,
            exchange_item=_itemlist.i_31528_1_1,
            consume_item_count=25,
            today_times=0,
            buy_times=1,
            limit_type=1,
            time=120,
            show=1,
            time1=48,
            show1=1
        },
        {
            item_seq=6,
            exchange_item=_itemlist.i_28601_1_1,
            consume_item_count=88,
            today_times=0,
            buy_times=10,
            limit_type=1,
            time=216,
            show=1,
            time1=96
        },
        {
            section_start=8,
            section_end=21,
            exchange_item=_itemlist.i_25614_1_1,
            consume_item_count=30,
            today_times=5
        },
        {
            section_start=8,
            section_end=21,
            item_seq=1,
            exchange_item=_itemlist.i_29017_1_1
        },
        {
            section_start=8,
            section_end=21,
            item_seq=2,
            exchange_item=_itemlist.i_27838_1_1
        },
        {
            section_start=8,
            section_end=21,
            item_seq=3,
            exchange_item=_itemlist.i_26073_1_1,
            consume_item_count=8
        },
        {
            section_start=8,
            section_end=21,
            item_seq=4,
            exchange_item=_itemlist.i_31527_1_1,
            consume_item_count=25,
            today_times=0,
            buy_times=1,
            limit_type=1,
            time=72,
            show=1,
            time1=72,
            show1=1
        },
        {
            section_start=8,
            section_end=21,
            item_seq=5,
            exchange_item=_itemlist.i_31528_1_1,
            consume_item_count=25,
            today_times=0,
            buy_times=1,
            limit_type=1,
            time=120,
            show=1,
            time1=48,
            show1=1
        },
        {
            section_start=8,
            section_end=21,
            item_seq=6,
            exchange_item=_itemlist.i_28601_1_1,
            consume_item_count=88,
            today_times=0,
            buy_times=10,
            limit_type=1,
            time=216,
            show=1,
            time1=96
        },
        {
            section_start=22,
            section_end=0,
            exchange_item=_itemlist.i_28667_1_1,
            consume_item_count=35,
            today_times=3
        },
        {
            section_start=22,
            section_end=0,
            item_seq=1,
            exchange_item=_itemlist.i_29017_1_1
        },
        {
            section_start=22,
            section_end=0,
            item_seq=2,
            exchange_item=_itemlist.i_27838_1_1
        },
        {
            section_start=22,
            section_end=0,
            item_seq=3,
            exchange_item=_itemlist.i_26073_1_1,
            consume_item_count=8
        },
        {
            section_start=22,
            section_end=0,
            item_seq=4,
            exchange_item=_itemlist.i_31527_1_1,
            consume_item_count=25,
            today_times=0,
            buy_times=1,
            limit_type=1,
            time=72,
            show=1,
            time1=72,
            show1=1
        },
        {
            section_start=22,
            section_end=0,
            item_seq=5,
            exchange_item=_itemlist.i_31528_1_1,
            consume_item_count=25,
            today_times=0,
            buy_times=1,
            limit_type=1,
            time=120,
            show=1,
            time1=48,
            show1=1
        },
        {
            section_start=22,
            section_end=0,
            item_seq=6,
            exchange_item=_itemlist.i_28601_1_1,
            consume_item_count=88,
            today_times=0,
            buy_times=10,
            limit_type=1,
            time=216,
            show=1,
            time1=96
        }
    }
}
SetConfigDefaultVal(Config.happy_up_activity_auto,_default_val,nil,nil,nil)