VIEW_REQ("modules/pet/pet_tale/pet_tale_view")
PetTaleCtrl = PetTaleCtrl or BaseClass(BaseCtrl)

-- ================= 宠物传记：异闻录 =================

function PetTaleCtrl:__init()
    if PetTaleCtrl.Instance ~= nil then
		Debuger.LogError("[PetTaleCtrl] attempt to create singleton twice!")
		return
	end
    PetTaleCtrl.Instance = self
    self.data = PetTaleData.New()

    self:RegisterProtocol(SCPetTaleReward,"OnSCPetTaleReward")
    -- 宠物传记红点
    RemindCtrl.Instance:Remind():Register(
        Mod.Pet.PetBook,
        self.data.tale_data,
        function ()
            return self.data:TaleRemind()
        end
    )
end

function PetTaleCtrl:__delete()
    PetTaleCtrl.Instance = nil
    self.data = nil
end

function PetTaleCtrl:OnUnloadGameLogic()
    self.data:Reset()
end

function PetTaleCtrl:OnSCPetTaleReward(protocol)
    self.data.tale_data.info_list = protocol.info_list
end



PetTaleData = PetTaleData or BaseClass()

function PetTaleData:__init()
    if PetTaleData.Instance ~= nil then
		Debuger.LogError("[PetTaleData] attempt to create singleton twice!")
		return
	end
    PetTaleData.Instance = self
    self.tale_data = SmartData.New({info_list = {}})
    self:InitCfg()
end

function PetTaleData:Reset()
    self.tale_data:Set({info_list = {}})
end

function PetTaleData:InitCfg()
    self.tale_group_cfg = DataHelper.ConvertGroup(Config.pet_cfg_auto.text_unlock, "mutant_id")
end

-- 获取传记章节组
function PetTaleData:GetTaleGroup(mutant_id)
    return self.tale_group_cfg[mutant_id]
end


-- 获取传记配置
function PetTaleData:GetTaleCfg(mutant_id)
    for i,v in pairs(Config.pet_cfg_auto.pet_text) do
        if v.mutant_id == mutant_id then
            return v
        end
    end
end

-- 获取传记信息 解锁了传记条件才有信息
function PetTaleData:GetTaleInfo(mutant_id)
    local info = self.tale_data.info_list[mutant_id]
    return info
end

-- 是否开启传记
function PetTaleData:IsOpenTale(pet)
    local mutant_id = pet:PetCfg().mutant_id
    local info = self:GetTaleInfo(mutant_id)
    if TableIsEmpty(info) then
        return false
    end
    return true
end

-- 章节是否解锁
function PetTaleData:GetIsUnlock(mutant_id, seq)
    local info = self:GetTaleInfo(mutant_id)
    if TableIsEmpty(info) then
        return false
    end
    local flag = info.finish_flag[8 - seq]
    if flag and flag > 0 then
        return true
    end
    return false
end


-- 是否可领传记奖励
function PetTaleData:GetTaleRewardFlag(mutant_id,seq)
    if not mutant_id then
        return false
    end
    local info = self.tale_data.info_list[mutant_id] or {}
    if TableIsEmpty(info) then
        return false
    end
    if info.fetch_flag[8 - seq] == 0 and info.finish_flag[8 - seq] == 1 then
        return true
    end
    return false
end

function PetTaleData:TaleOneRemind(mutant_id)
    local list_data = self:GetTaleGroup(mutant_id) or {}
    for i,v in pairs(list_data) do
        if self:GetTaleRewardFlag(mutant_id, v.seq) then
            return 1
        end
    end
    return 0
end

-- 传记红点提醒
function PetTaleData:TaleRemind()
    for _,v in pairs(Config.pet_cfg_auto.pet_text) do
        local flag = self:TaleOneRemind(v.mutant_id)
        if flag > 0 then
            return 1
        end
    end
    return 0
end
