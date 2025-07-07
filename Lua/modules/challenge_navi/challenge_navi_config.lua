ChallengeNaviConfig = {}

ChallengeNaviFunc = {}

ChallengeNaviConfig.CompleteShowType = {
    None = 0,   --无完成判断和表现  a
    Zero = 1,   --left为0表示完成
    Full = 2,   --left >= total表示完成
}      
-- {
    --     mod[modKey]     --条目对应的modKey
    --     show_complete_flag[ChallengeNaviConfig.CompleteShowType]   --显示今日已完成的规则标识位
    --     guide[bool]     --决定是否会向指引模块注册按钮，指引按钮名： "ChallengeNavi_" .. tostring(mod)
    --     desc_val1[string] --主要数据显示的标题，比如"当前剩余次数"
    --     desc_val2[string] --补充数据显示的标题，比如"下次刷新时间"
    --     variant_func[function]    --详情参见RegisterVariantShowFunc函数注释
    --     open_req_func[function] --详情参见RegisterOpenReqFunc函数注释
    --     show_complete_desc --默认显示"今日已完成" 2="本周已完成" 3="已完成"
    -- 
--}
--left,total===number
--返回 [left/total]
--total为空时返回 [left]
function ChallengeNaviFunc.DescMapCommom(left,total)
    if left == nil then
        return ("L:NULL" .. "|TO:" .. tostring(total))
    end
    if total == nil then
        return tostring(left)
    else
        return Format("<color=#%s>%d</color>/%d",COLORSTR.Yellow1,left,total)
    end
end
--level===number
--返回 [level级]
function ChallengeNaviFunc.DescMapLevel(level)
    return Format(Language.ChallengeNavi.Ji,level)
end

function ChallengeNaviFunc.DescMapLevels(level)
    return Format(Language.ChallengeNavi.Level,level)
end

function ChallengeNaviFunc.DescGoThroughLevel()
    local data = GoThroughData.Instance:GetPassLevelData()
    if data ~= nil then
        return data.reorder .. "•" .. data.name
    end
    return Language.GoThrough.FinishTip
end

function ChallengeNaviFunc.DescEndlessTowerLevel()
    local data = EndlessTowerData.Instance:GetPassLevelData()
    if data ~= nil then
        return Format(Language.ChallengeNavi.Level,data.seq)
    end
    return Language.MazeRun.StageClear
end

function ChallengeNaviFunc.GetModName(mod)
    return "ChallengeNavi_" .. tostring(mod)
end

ChallengeNaviConfig.Config = {
    {   --挑战任务
        mod = Mod.Experience.Task,
        desc_val1 = Language.ChallengeNavi.DescLblTiaoZhan,
        show_complete_flag = ChallengeNaviConfig.CompleteShowType.Zero,
    },
    {   --校场演武
        mod = Mod.Experience.SchoolExercise,
        desc_val1 = Language.ChallengeNavi.DescLblJiaoChang,
        sort = 2,       --与挑战任务同等级开启用sort确定先后
        show_complete_flag = ChallengeNaviConfig.CompleteShowType.Zero,
    },
    {   --试炼之地(远征魔冢)
        mod = Mod.Modrons.Main,
        desc_val1 = Language.ChallengeNavi.DescLblYuanZheng,
        show_complete_flag = ChallengeNaviConfig.CompleteShowType.Zero,
        show_complete_desc = 2
    },
    {   --锢魔之塔
        mod = Mod.Experience.GuMo,
        desc_val1 = Language.ChallengeNavi.DescLblGuMo,
        variant_map_func = ChallengeNaviFunc.DescMapLevel,
    },
    {   --轮回忘川     
        mod = Mod.Experience.ChallengeTest,
        desc_val1 = Language.ChallengeNavi.DescLblLunHui,
        desc_val2 = Language.ChallengeNavi.ResetTime,

    },
    {   --稀有副本(幻境试炼)
        mod = Mod.Challenge.RareDungeon,
        desc_val1 = Language.ChallengeNavi.DescLblRareDungeon,
        show_complete_flag = ChallengeNaviConfig.CompleteShowType.Zero,
    },
    {   --幽冥炼狱
        mod = Mod.DeepDarkFantasy.Main,
        desc_val1 = Language.ChallengeNavi.DescLblTodayTimes,
        desc_val2 = Language.ChallengeNavi.ResetTime,
    },
    {   --北斗七星
        mod = Mod.Challenge.BigDipper,
        desc_val1 = Language.ChallengeNavi.DescLblBigDipper,
        desc_val2 = Language.ChallengeNavi.ResetTime,
    },
    {   --渡劫
        mod = Mod.Challenge.GoThrough,
        desc_val1 = Language.ChallengeNavi.DescProgressTip,
        variant_map_func = ChallengeNaviFunc.DescGoThroughLevel,
    },
    {   --上古遗迹
        mod = Mod.Challenge.FightElite,
        desc_val1 = Language.ChallengeNavi.DescLblElite,
    },
    {   --无尽之塔
        mod = Mod.Experience.EndlessTower,
        desc_val1 = Language.ChallengeNavi.DescProgressTip,
        variant_map_func = ChallengeNaviFunc.DescEndlessTowerLevel,
    },
    {   --琉璃蜃境
        mod = Mod.Experience.MirageTerritory,
        desc_val1 = Language.ChallengeNavi.DescLblSweep,
    },
    {   --陨神之地
        mod = Mod.Challenge.FallenGod,
        desc_val1 = Language.ChallengeNavi.DescLblFallenGod,
        desc_val2 = Language.ChallengeNavi.ResetTime,
    },
    {   --妖行试炼
        mod = Mod.GhostBust.Main,
        desc_val1 = Language.ChallengeNavi.DescLblGuMo,
    },
    {   --塔楼冒险
        mod = Mod.Challenge.TowerAdventure,
        desc_val1 = Language.ChallengeNavi.DescLblMengYuan,
        show_complete_flag = ChallengeNaviConfig.CompleteShowType.Zero,
        show_complete_desc = 3,
    },
    {   --牛郎织女
        mod = Mod.Challenge.CowBoy,
        desc_val1 = Language.ChallengeNavi.DescLblMengYuan,
        show_complete_flag = ChallengeNaviConfig.CompleteShowType.Zero,
        show_complete_desc = 3,
    },
    {
        mod = Mod.Experience.ShiShen,
        desc_val1 = Language.ChallengeNavi.DescLblLunHui,
        variant_map_func = ChallengeNaviFunc.DescMapLevel,
    }
}