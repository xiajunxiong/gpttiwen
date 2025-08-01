local _default_val={
    exchange_page={
        integral_type=0,
        integral_price=144,
        limit_type=0,
        limit_times=0,
        open_time=-1,
        sale_time=-1,
        price=0,
        money_type=0,
        is_bind=1
    }
}
Config.exchange_cfg_auto={
    exchange_page={
        {
            seq=1,
            sort=1,
            item_id=29005,
            integral_price=104
        },
        {
            seq=2,
            sort=2,
            item_id=26230
        },
        {
            seq=3,
            sort=3,
            item_id=26231
        },
        {
            seq=4,
            sort=4,
            item_id=26232
        },
        {
            seq=5,
            sort=5,
            item_id=26233
        },
        {
            seq=6,
            sort=6,
            item_id=26234
        }
    },
    level={
        {
            level=0,
            need_prestige=0
        },
        {
            level=1,
            need_prestige=500
        },
        {
            level=2,
            need_prestige=1500
        },
        {
            level=3,
            need_prestige=2500
        },
        {
            level=4,
            need_prestige=4000
        },
        {
            level=5,
            need_prestige=8000
        },
        {
            level=6,
            need_prestige=12000
        },
        {
            level=7,
            need_prestige=15000
        },
        {
            level=8,
            need_prestige=21000
        },
        {
            level=9,
            need_prestige=30000
        },
        {
            level=10,
            need_prestige=40000
        }
    }
}
SetConfigDefaultVal(Config.exchange_cfg_auto,_default_val,nil,nil,nil)