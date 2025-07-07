ActPetStrCtrl = ActPetStrCtrl or BaseClass(BaseCtrl)

ActPetStrCtrl.ReqType = {
    info = 0,
    FreeReward = 1,
    GetReward = 2,  -- p1宠物品质， p2索引
}

function ActPetStrCtrl:__init()
	if ActPetStrCtrl.Instance ~= nil then
		Debuger.LogError("[ActPetStrCtrl] attempt to create singleton twice!")
		return
	end
	ActPetStrCtrl.Instance = self
	self.data = ActPetStrData.New()
    self:RegisterAllProtocols()

    Remind.Instance:Register(Mod.ServerActivity.PetStr,self.data.info_data, function () 
        return self.data:GetRemind() 
    end)
end

function ActPetStrCtrl:__delete()
    self:UnRegisterAllProtocols()
    ActPetStrCtrl.Instance = nil
end

function ActPetStrCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end


function ActPetStrCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCActPetStrInfo, "OnSCActPetStrInfo")
end

function ActPetStrCtrl:OnSCActPetStrInfo(protocol)
    self.data:SetActInfo(protocol)
end



ActPetStrData = ActPetStrData or BaseClass()

function ActPetStrData:__init()
    if ActPetStrData.Instance ~= nil then
        Debuger.LogError("[ActPetStrData] attempt to create singleton twice!")
        return
    end
    ActPetStrData.Instance = self
    self.cfg = Config.xinfuqianghua_auto

    self.info_data = SmartData.New()
end

function ActPetStrData:ClearData()
    self.info_data:Set({})
end

function ActPetStrData:SetActInfo(protocol)
    local info = protocol.info
    self.info_data:Set(info)
end


function ActPetStrData:GetTabList()
    local role_level = RoleData.Instance:GetRoleLevel()
    local cfg = self.cfg.open_sort
    local list = {}
    for i,v in pairs(cfg) do
        if role_level >= v.start_lv then
            table.insert(list, v)
        end
    end
    return list
end

function ActPetStrData:GetJumpList(qua)
    local cfg = self.cfg.page_jump
    local list = {}
    for i,v in pairs(cfg) do
        if v.type == qua then
            table.insert(list, v)
        end
    end
    return list
end

function ActPetStrData:GetRewardList(qua)
    local cfg = self.cfg.reward
    local list = {}
    for i,v in pairs(cfg) do
        if v.type == qua then
            table.insert(list, v)
        end
    end
    return list
end

function ActPetStrData:GetJumpTab()
    local cfg = self.cfg.reward
    for i,v in pairs(cfg) do
        local flag = self:GetFlag(v)
        if flag >= 0 then
            return v.type - 1
        end
    end
    return 1
end

-- 免费奖励是否可领取
function ActPetStrData:IsFreeReward()
    if TableIsEmpty(self.info_data) then
        return false
    end
    return self.info_data.free_reward_flag == 0
end

-- 1可领取，0不可领取，-1已领取
function ActPetStrData:GetFlag(data)
    if TableIsEmpty(self.info_data) then
        return 0
    end
    local qua = data.type
    local reward_flag = self.info_data.reward_finish_flag_list[qua][data.seq1]
    if reward_flag == 1 then
        return -1
    end

    local task_flag = self.info_data.task_finish_flag_list[qua][data.seq1]
    if task_flag == 1 then
        return 1
    else
        return 0
    end
end


function ActPetStrData:GetTabRed(qua)
    local list = self:GetRewardList(qua)
    for i,v in pairs(list) do
        local flag = self:GetFlag(v)
        if flag > 0 then
            return 1
        end
    end
    return 0
end

-- 活动红点

function ActPetStrData:GetRemind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_PET_STR) then
		return 0
    end
    if self:IsFreeReward() then
        return 1
    end
    local tab_list = self:GetTabList()
    for i,v in pairs(tab_list) do
        local qua = v.type
        if self:GetTabRed(qua) > 0 then
            return 1
        end
    end
    return 0
end