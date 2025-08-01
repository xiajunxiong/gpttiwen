local _default_val={
    ghost_hunt_config={
        scene_id=2451
    },
    hunt_item_sell={
        is_need_chong_zhi_price=0,
        money_type=0,
        is_bind=1,
        limit_times=3
    }
}
local _itemlist={
    i_56626_5_1={
        item_id=56626,
        num=5,
        is_bind=1
    },
    i_56626_10_1={
        item_id=56626,
        num=10,
        is_bind=1
    }
}
Config.ghost_hunt_auto={
    ghost_hunt_config={
        [62500]={
            id=62500
        },
        [62501]={
            id=62501
        },
        [62502]={
            id=62502,
            scene_id=2452
        },
        [62503]={
            id=62503,
            scene_id=2452
        },
        [62504]={
            id=62504,
            scene_id=2453
        },
        [62505]={
            id=62505,
            scene_id=2453
        },
        [62506]={
            id=62506,
            scene_id=2454
        },
        [62507]={
            id=62507,
            scene_id=2454
        },
        [62508]={
            id=62508,
            scene_id=2455
        },
        [62509]={
            id=62509,
            scene_id=2455
        }
    },
    hunt_item_sell={
        {
            seq=0,
            reward=_itemlist.i_56626_5_1,
            price=25
        },
        {
            seq=1,
            reward=_itemlist.i_56626_10_1,
            price=60
        },
        {
            seq=2,
            reward=_itemlist.i_56626_10_1,
            is_need_chong_zhi_price=1,
            price=6
        }
    },
    hunt_sence={
        {
            sence_id=2451,
            need_level=60,
            level_min=60,
            level_max=74
        },
        {
            sence_id=2452,
            need_level=75,
            level_min=75,
            level_max=89
        },
        {
            sence_id=2453,
            need_level=90,
            level_min=90,
            level_max=99
        },
        {
            sence_id=2454,
            need_level=100,
            level_min=100,
            level_max=109
        },
        {
            sence_id=2455,
            need_level=110,
            level_min=110,
            level_max=120
        }
    },
    other={
        {
            normal_sealcard=56625,
            gold_sealcard=56626
        }
    }
}
SetConfigDefaultVal(Config.ghost_hunt_auto,_default_val,nil,nil,nil)