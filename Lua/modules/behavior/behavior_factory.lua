BehaviorFactory = BehaviorFactory or {}

function BehaviorFactory.Base(beh_type, p1, p2, p3, beh_id, con_func)
    local re = {
        behavior_id = beh_id,
        behavior_type = beh_type,
        param1 = p1,
        param2 = p2,
        param3 = p3,
        con_func = con_func,
    }
    return re
end

--	[8] = InteractionEnterFb,		--请求进入副本 param1:fb_type
function BehaviorFactory.EnterFb(fb_type, con_func)
    return BehaviorFactory.Base(8, fb_type, nil, nil, 0, con_func)
end

function BehaviorFactory.OpenView(view_tab)
    return BehaviorFactory.Base(100, view_tab)
end
-- 11 InteractionPatrol	--原地巡逻
function BehaviorFactory.Patrol()
    return BehaviorFactory.Base(11)
end
-- 101 挖宝
function BehaviorFactory.TreasureMap(item)
    return BehaviorFactory.Base(101, item)
end

function BehaviorFactory.GoPatrol()
    return BehaviorFactory.Base(102)
end