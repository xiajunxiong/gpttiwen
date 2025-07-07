GodPosData = GodPosData or BaseClass()

-- 操作请求类型
GodPosReqType = {
    Info = 0,       --请求下发
}

function GodPosData:__init()
    if GodPosData.Instance ~= nil then
        Debuger.LogError("[GodPosData] attempt to create singleton twice!")
        return
    end
    GodPosData.Instance = self
    self.config = Config.divine_domain_system_auto
    self.info_data = SmartData.New()

    self:ResetData()
    self:InitData()
end

function GodPosData:__delete()
    GodPosData.Instance = nil
end

-- 任务最大数量
function GodPosData:TaskMaxNum()
    return 5
end

function GodPosData:InitData()
    Cfg.Register("GodPosCfg", Cfg.RealTimeSearch(self.config.class, "class_seq"))

    self.max_pos_seq = #self.config.class - 1
end

function GodPosData:ResetData()
    local info = {}
    info.pos_seq = 0 --神位等级
    info.today_guide_flag = 0  --今日是否指引了
    info.task_flag = {}      --是否可领取，使用task_order做下标
    info.reward_flag = {}    --是否已领取，使用task_order做下标
    info.task_param = {}    --任务进度，使用task_order做下标
    for i = 0, self:TaskMaxNum() - 1 do
        info.task_param[i] = 0
    end
    self.info_data.info = info
end

-- 神位最大seq
function GodPosData:MaxPosSeq()
    return self.max_pos_seq
end

-- 当前的神位seq
function GodPosData:PosSeq()
    return self.info_data.info.pos_seq
end

-- 当前神位seq是否最大
function GodPosData:IsMaxPosSeq()
    return self:PosSeq() == self.max_pos_seq
end

function GodPosData:Remind()
    local cfg = self:TaskCfg(self.info_data.info.pos_seq)
    for k, v in pairs(cfg) do
        if self:IsTaskCanGet(v) then
            return 1
        end
    end
    return 0
end

function GodPosData:SetInfo(protocol)
    self.info_data.info = protocol.info
end

function GodPosData:TaskFlag(cfg)
    if self.info_data.info.task_flag[cfg.task_order] then
        return self.info_data.info.task_flag[cfg.task_order]
    end
    return 0
end

-- 任务进度
function GodPosData:TaskParam(cfg)
    local task_param=self.info_data.info.task_param[cfg.task_order-1] 
    if task_param then
        return task_param > cfg.need_num and cfg.need_num or task_param
    end
    return 0
end

-- 是否已领取
function GodPosData:IsTaskGeted(cfg)
    if self.info_data.info.reward_flag[cfg.task_order] then
        return self.info_data.info.reward_flag[cfg.task_order] == 1
    end
    return false
end

-- 是否能领取
function GodPosData:IsTaskCanGet(cfg)
    if self.info_data.info.task_flag[cfg.task_order] then
        return self.info_data.info.task_flag[cfg.task_order] == 1 and (not self:IsTaskGeted(cfg))
    end
    return false
end

-- 当前神位任务完成数量
function GodPosData:TaskFinishNum()
    local num = 0
    local cfg = self:TaskCfg(self.info_data.info.pos_seq)
    for k, v in pairs(cfg) do
        if self:IsTaskGeted(v) then
            num = num + 1
        end
    end
    return num
end

-- 当前神位任务可领取数量
function GodPosData:TaskCanGetNum()
    local num = 0
    local cfg = self:TaskCfg(self.info_data.info.pos_seq)
    for k, v in pairs(cfg) do
        if self:IsTaskCanGet(v) then
            num = num + 1
        end
    end
    return num
end

-- 神位信息
function GodPosData:GodPosCfg(seq)
    seq = seq or self:PosSeq()
    return Cfg.GodPosCfg(seq, false) or {}
end

function GodPosData:GetGodPosData()
    local jump_data
    local list ={}
    for k, v in pairs(self.config.class) do
        if v.display == 1 then 
            local vo = {}
            vo.class_seq = v.class_seq
            vo.name = v.name
            vo.rewards = #v.class_reward >0 and BagData.Instance:ItemListBuild(v.class_reward) or {}
            if v.quintessence > 0 then
                table.insert(vo.rewards,Item.Init(CommonItemId.GodSpaceEssence,v.quintessence))
            end
            if v.military_exploits > 0 then
                table.insert(vo.rewards,Item.Init(CommonItemId.GodSpaceExploits,v.military_exploits))
            end
            table.insert(list,vo)
        end 
        if v.class_seq == self:PosSeq() then
            jump_data = v
        end
    end
    
    return list, jump_data
end

-- 神位任务信息
function GodPosData:TaskCfg(seq)
    if not self.task_cfg then
        self.task_cfg = {}
    end
    if not self.task_cfg[seq] then
        self.task_cfg[seq] = {}
        for k, v in pairs(self.config.class_task) do
            if seq == v.class_seq then
                local vo = TableCopy(v)
                vo.task_desc = string.gsub(v.desc, "X", v.need_num)
                if v.param and v.param > 0 then
                    vo.task_desc = string.gsub(vo.task_desc, "Y", v.param .. "")
                end
                vo.rewards = BagData.Instance:ItemListBuild(v.task_reward)
                if v.military_exploits > 0 then
                    local item_military_exploits = Item.Init(CommonItemId.GodSpaceExploits, v.military_exploits)
                    table.insert(vo.rewards, item_military_exploits)
                end
                if v.quintessence > 0 then
                    local item_quintessence = Item.Init(CommonItemId.GodSpaceEssence, v.quintessence)
                    table.insert(vo.rewards, item_quintessence)
                end
                vo.task_order = v.task_order
                vo.need_num = v.need_num
                table.insert(self.task_cfg[seq], vo)
            end
        end
        table.sort(self.task_cfg[seq], DataSorter.KeyLowerSorter("task_order"))
    end
    return self.task_cfg[seq]
end


-- 神位任务排序
function GodPosData:TaskData(seq)
    local list = self:TaskCfg(seq)
    table.sort(list, DataHelper.WeightSort("task_order", function(data)
        if self:IsTaskCanGet(data) then
            return -1000
        elseif self:IsTaskGeted(data) then
            return 1000
        end
        return 0
    end))
    return list
end

--神位奖励是否领取
function GodPosData:IsRewardGeted(class_seq)
    return self.info_data.info.lv_reward_flag[class_seq-1]==1
end