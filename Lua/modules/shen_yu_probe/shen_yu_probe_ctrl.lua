FILE_REQ("modules/shen_yu_probe/shen_yu_probe_data")
VIEW_REQ("modules/shen_yu_probe/shen_yu_probe_view")
VIEW_REQ("modules/shen_yu_probe/shen_yu_probe_boos_rank")
VIEW_REQ("modules/shen_yu_probe/shen_yu_probe_up_fight")
VIEW_REQ("modules/shen_yu_probe/shen_yu_probe_tower")
VIEW_REQ("modules/shen_yu_probe/shen_yu_probe_level_reward")

-- ======= 神域探索 ======

-- ==== 备注
-- link_mark神域格子的唯一key

--  问题
-- 采集了5个领过了奖励，再采集一个的时候，又可以领了

-- 加经验的gm == explore_exp:1000

ShenYuProbeCtrl = ShenYuProbeCtrl or BaseClass(BaseCtrl)

ShenYuProbeCtrl.MaxTaskType = 3
ShenYuProbeCtrl.MaxBoss = 2
ShenYuProbeCtrl.MaxWell = 2

ShenYuProbeCtrl.TaskType = {
    ShenYu = 0,     --神域任务
    Camp = 1,       --阵营任务
    Role = 2,       --个人任务
}

-- 神域系统助战的系统类型
ShenYuSystemHelpType = {
    Probe = 0,      -- 神域探索
    ShenTan = 1,    -- 神域神坛
}

function ShenYuProbeCtrl:__init()
	if ShenYuProbeCtrl.Instance ~= nil then
		Debuger.LogError("[ShenYuProbeCtrl] attempt to create singleton twice!")
		return
    end
    ShenYuProbeCtrl.Instance = self
    ShenYuProbeData.New()
    self.data = ShenYuProbeData.Instance
    self:RegisterAllProtocols()

    -- RemindCtrl.Instance:Remind():Register(
    --     Mod.ShenYuProbe.Main,
    --     self.data.remind_data,
    --     function ()
    --         return self.data:GetAllRemind()
    --     end
    -- )
    LandsBetweenData.Instance:SetMainCallFunc(GoldLaw.CallBackType.Open,"ShenYuProbeCtrl",function ()
        self:ClearAll()
    end)

    FunOpen.Instance:SelfCondition(Mod.ShenYuProbe.Main, function()
        return self.data:IsOpen()
    end)

    LandsBetweenData.Instance:ExtraRemindMark({
        mod_key = Mod.ShenYuProbe.Main,
        target = self.data.remind_data,
        func = function ()
            return self.data:GetAllRemind()
        end,
    })
end

function ShenYuProbeCtrl:__delete()
    ShenYuProbeCtrl.Instance = nil
end

function ShenYuProbeCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end

function ShenYuProbeCtrl:OnInit()
end

function ShenYuProbeCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSGodSpaceExploreSetFighter)     --// 5537 神域探索boss设置上阵
    self:RegisterProtocol(SCGodSpaceFactionExploreInfo, "OnGodSpaceFactionExploreInfo")     --// 5530 神域探索阵营信息
    self:RegisterProtocol(SCGodSpaceUserExploreInfo, "OnSCGodSpaceUserExploreInfo")         --// 5531 神域探索玩家信息
    self:RegisterProtocol(SCGodSpaceUserHelperInfo, "OnSCGodSpaceUserHelperInfo")           --// 5533 神域玩家协战角色列表
    self:RegisterProtocol(SCGodSpaceExploreBossInfo, "OnSCGodSpaceExploreBossInfo")         --// 5535 神域探索己方boss信息
    self:RegisterProtocol(SCGodSpaceExploreBossRankList, "OnSCGodSpaceExploreBossRankList") --// 5536 排行榜
    self:RegisterProtocol(SCGodSpaceExploreWellInfo, "OnSCGodSpaceExploreWellInfo")         --// 5538 神域探索封妖井信息下发
    self:RegisterProtocol(SCGodSpaceExploreWellPos, "OnSCGodSpaceExploreWellPos")           --// 5539 封妖井位置信息下发
end


function ShenYuProbeCtrl:SendTemplateReq(param_t)
	LandsBetweenCtrl.Instance:GodSpaceOperate(param_t)
end

-- 5537
function ShenYuProbeCtrl:SendHelpInfo(info)
    local protocol = GetProtocol(CSGodSpaceExploreSetFighter)
    protocol.info = info
	SendProtocol(protocol)
end

--// 5530 神域探索阵营信息
function ShenYuProbeCtrl:OnGodSpaceFactionExploreInfo(protocol)
    self.data.camp_data:Set(protocol.info)
    self.data:RemindNotice()
end

--// 5531 神域探索玩家信息
function ShenYuProbeCtrl:OnSCGodSpaceUserExploreInfo(protocol)
    self.data.role_data:Set(protocol.info)
    for i,v in pairs(protocol.info.stones) do
        self:CreateGather(v,1,i)
    end
    for i,v in pairs(protocol.info.grasses) do
        self:CreateGather(v,2,i)
    end
    -- 强制刷新视野
    LandsBetweenData.Instance:ForceFlushVisibleOperate()
    self.data:RemindNotice()
end

-- 5533 神域玩家协战角色列表
function ShenYuProbeCtrl:OnSCGodSpaceUserHelperInfo(protocol)
    self.data.help_data:Set(protocol.info)
end

--// 5535神域探索己方boss信息
function ShenYuProbeCtrl:OnSCGodSpaceExploreBossInfo(protocol)
    self.data.boss_data:Set(protocol.info)
    for i,v in pairs(protocol.info) do
        self:CreateBoss(v)
    end
    -- 强制刷新视野
    LandsBetweenData.Instance:ForceFlushVisibleOperate()
end

--// 5536 boss伤害排行榜
function ShenYuProbeCtrl:OnSCGodSpaceExploreBossRankList(protocol)
    self.data.rank_data:Set(protocol.list)
end

--// 5538 神域探索封妖井信息下发
function ShenYuProbeCtrl:OnSCGodSpaceExploreWellInfo(protocol)
    self.data.well_view_data:Set(protocol.info)
end

--// 5539 封妖井位置信息下发
function ShenYuProbeCtrl:OnSCGodSpaceExploreWellPos(protocol)
    local info = protocol.info
    --LogError("封妖井位置信息下发",info)
    self.data.well_pos_data:Set(info)
    for i,v in pairs(info) do
        self:CreateWell(v)
    end
    -- 强制刷新视野
    LandsBetweenData.Instance:ForceFlushVisibleOperate()
end



-- ============= 逻辑区 ===============

-- 创建采集物 type == 1 石头，2花草
function ShenYuProbeCtrl:CreateGather(pos, type, index)
    --LogDG("创建采集物", type, pos.row, pos.col)
    if pos.row < 0 or pos.col < 0 then
        LogDG("创建采集物位置不对",pos.row, pos.col)
        return
    end
    local res_path = type == 1 and ShenYuProbeData.Instance:GetRockResPath() or ShenYuProbeData.Instance:GetGrasseResPath()
    local link_mark = LandsBetweenData.CfgLink({[1] = pos.row,[2] = pos.col})  
    local center_link_mark = link_mark
    local name = type == 1 and Language.ShenYuProbe.ObjName1 or Language.ShenYuProbe.ObjName2
    LandsBetweenData.Instance:SetResOfSpPicByLinkMark({
        server_type = type - 1, -- 服务端的类型，0矿石，1草药
        row = pos.row,
        col = pos.col,
        res_path = res_path,
        name = name .. index,
        link_mark = link_mark,
        ui_show_sp_sub = "shen_yu_probe_sub",
        around = {center_link_mark},
        center_link_mark = center_link_mark,
        extral_info = nil,
        faction = -1,
        task_type = ShenYuProbeCtrl.TaskType.Role,
        enable_func = function ()
            return true
        end
    },link_mark)
    --LandsBetweenData.Instance:ForceFlushVisibleOperate()
end

function ShenYuProbeCtrl.ClearObj(link_mark, is_flush)
    LandsBetweenData.Instance:SetResOfSpPicByLinkMark(nil,link_mark)
    if is_flush then
        LandsBetweenData.Instance:ForceFlushVisibleOperate()
    end
end

function ShenYuProbeCtrl:ClearAllGather()
    local info = self.data.role_data:Val()
    if info.stones and info.grasses then
        for i,v in pairs(info.stones) do
            local link_mark = LandsBetweenData.CfgLink({[1] = v.row,[2] = v.col})
            ShenYuProbeCtrl.ClearObj(link_mark)
        end
        for i,v in pairs(info.grasses) do
            local link_mark = LandsBetweenData.CfgLink({[1] = v.row,[2] = v.col})
            ShenYuProbeCtrl.ClearObj(link_mark)
        end
        info.stones = {}
        info.grasses = {}
    end
    -- 强制刷新视野
    --LandsBetweenData.Instance:ForceFlushVisibleOperate()
end

-- 创建锁妖井
function ShenYuProbeCtrl:CreateWell(info)
    if info.ul_row < 1 or info.ul_col < 1 then
        return
    end
    local link_mark = LandsBetweenData.CfgLink({[1] = info.row,[2] =  info.col})
    --LOG("创建锁妖井",info)
    local res_path = ShenYuProbeData.Instance:GetWellResPath()
    local center_link_mark = link_mark
    local index = info.well_type + 1
    LandsBetweenData.Instance:SetResOfSpPicByLinkMark({
        well_type = info.well_type,
        res_path = res_path,
        name = Language.ShenYuProbe.ObjName4 .. index,
        link_mark = link_mark,
        ui_show_sp_sub = "shen_yu_probe_sub",
        around = {link_mark},
        center_link_mark = center_link_mark,
        extral_info = nil,
        faction = -1,
        task_type = ShenYuProbeCtrl.TaskType.Camp,
        enable_func = function ()
            return true
        end,
    },link_mark)
end

function ShenYuProbeCtrl:ClearAllWell()
    --LogDG("清除锁全部妖井")
    local data = self.data.well_pos_data
    for i, v in pairs(data) do
        local link_mark = LandsBetweenData.CfgLink({[1] = v.row,[2] =  v.col})
        ShenYuProbeCtrl.ClearObj(link_mark)
    end
    self.data.well_pos_data:Set({})
    --LandsBetweenData.Instance:ForceFlushVisibleOperate()
end

-- 创建boss
function ShenYuProbeCtrl:CreateBoss(info)
    if info.ul_row < 1 or info.ul_col < 1 then
        return
    end
    local link_mark = LandsBetweenData.CfgLink({[1] = info.row,[2] =  info.col})
    if info.boss_remain_hp < 1 then
        --LOG("怪物死亡", info)
        ShenYuProbeCtrl.ClearObj(link_mark, true)
        return
    end
    --LogDG("创建Boss", link_mark)
    local res_path = ShenYuProbeData.Instance:GetBossResPath()
    local center_link_mark = link_mark
    local index = info.id + 1
    local link_mark_list = {}
    -- for i = 0, 2 do
    --     for j = 0, 2 do
    --         local x = info.ul_row + i
    --         local y = info.ul_row + i
    --         local link = LandsBetweenData.CfgLink({[1] = x,[2] =  y})
    --         table.insert(link_mark_list, link)
    --     end
    -- end
    LandsBetweenData.Instance:SetResOfSpPicByLinkMark({
        id = info.id,
        res_path = res_path,
        name = Language.ShenYuProbe.ObjName3 .. index,
        link_mark = link_mark,
        ui_show_sp_sub = "shen_yu_probe_sub",
        around = {link_mark},
        center_link_mark = center_link_mark,
        extral_info = nil,
        faction = -1,
        task_type = ShenYuProbeCtrl.TaskType.ShenYu,
        enable_func = function ()
            return true
        end,
    },link_mark)
end

function ShenYuProbeCtrl:ClearAllBoss()
    --LogDG("清除锁全部Boss")
    local data = self.data.boss_data
    for i, v in pairs(data) do
        local link_mark = LandsBetweenData.CfgLink({[1] = v.row,[2] =  v.col})
        ShenYuProbeCtrl.ClearObj(link_mark)
    end
    self.data.boss_data:Set({})
    --LandsBetweenData.Instance:ForceFlushVisibleOperate()
end


function ShenYuProbeCtrl:ClearAll()
    self:ClearAllGather()
    self:ClearAllWell()
    self:ClearAllBoss()
    LandsBetweenData.Instance:ForceFlushVisibleOperate()
end