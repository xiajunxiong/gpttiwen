VIEW_REQ("modules/reward/reward_pet_weapon_fan_li")

PetWeaponFanliCtrl = PetWeaponFanliCtrl or BaseClass(BaseCtrl)

PetWeaponFanliCtrl.OpType = {
    Info = 0,	  --获取信息
	Lottery = 1,  --抽奖
	Lottery_Count_Reward = 2, --param_1 领取的奖励档位 0开始
}

function PetWeaponFanliCtrl:__init()
	if PetWeaponFanliCtrl.Instance ~= nil then
		Debuger.LogError("[PetWeaponFanliCtrl] attempt to create singleton twice!")
		return
	end
	PetWeaponFanliCtrl.Instance = self
    self.data = PetWeaponFanliData.New()
    self:RegisterAllProtocols()

    --魂器返利
	RemindCtrl.Instance:Remind():Register(Mod.Reward.PetWeaponFanLi, self.data.remind_data, function ()
		return self.data:GetRemind()
	end)
end

function PetWeaponFanliCtrl:__delete()
    self:UnRegisterAllProtocols()
    PetWeaponFanliCtrl.Instance = nil
end

function PetWeaponFanliCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end


function PetWeaponFanliCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAHunQiFanLiInfo, "OnSCRAHunQiFanLiInfo")
end


function PetWeaponFanliCtrl:OnSCRAHunQiFanLiInfo(protocol)
    self.data:SetInfoData(protocol)
end


PetWeaponFanliData = PetWeaponFanliData or BaseClass()

function PetWeaponFanliData:__init()
    if PetWeaponFanliData.Instance ~= nil then
        Debuger.LogError("[PetWeaponFanliData] attempt to create singleton twice!")
        return
    end
    PetWeaponFanliData.Instance = self
    self.info_data = SmartData.New()
    self.remind_data = SmartData.New({first_open_flag = false})
    self.cfg = Config.hunqifanli_auto
    self.act_type = ACTIVITY_TYPE.Rand_HUN_QI_FAN_LI
    self.new_info_data = SmartData.New()
    self.play_seq = -1
end

function PetWeaponFanliData:ClearData()
    self.info_data:Set({})
    self.play_seq = -1
end

function PetWeaponFanliData:SetInfoData(protocol)
    local info = protocol.info
    local play_seq = nil
    if not TableIsEmpty(self.info_data.item_flag) then
        local old_item_flag = self.info_data.item_flag
        local count = 0
        for i = 0,7 do
            local v = old_item_flag[i]
			if v ~= 0 and info.item_flag[i] == 0 then
                count = count + 1
            else
                play_seq = i
			end
        end
        if count ~= 7 then
            play_seq = nil
        end
        if not play_seq then
            for i,v in pairs(old_item_flag) do
                if v ~= info.item_flag[i] then
                    play_seq = i		-- 抽到了哪个
                    break
                end
            end 
        end
    end
    if not play_seq then
        self.info_data:Set(info)
    else
        self.play_seq = play_seq
        self.new_info_data:Set(info)
    end
    self:NoticeRemind()
end

function PetWeaponFanliData:UseNewInfo()
    if TableIsEmpty(self.new_info_data) then
        return
    end
    self.info_data:Set(self.new_info_data:Val())
    self.new_info_data:Set({})
    self.play_seq = -1
    self:NoticeRemind()
end

-- 基础配置
function PetWeaponFanliData:GetItemListData()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(self.act_type,self.cfg.gift_configure)
    return cfg
end

-- 充值配置
function PetWeaponFanliData:GetCZCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(self.act_type,self.cfg.recharge_configure)
    return cfg
end

-- 阶段配置
function PetWeaponFanliData:GetStageCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(self.act_type,self.cfg.phase_reward)
    return cfg
end

function PetWeaponFanliData:GetItemDataBySeq(seq)
    local cfg = self:GetItemListData()
    return cfg[seq + 1]
end


function PetWeaponFanliData:GetRemainCount()
    return self.info_data.remain_count or 0
end

--今日抽奖是否完成
function PetWeaponFanliData:IsDayFull()
    if self:GetRemainCount() > 0 then
        return false
    end

    return self:IsCZFull()
end

--今日充值是否完成
function PetWeaponFanliData:IsCZFull()
    local num = self.info_data.today_recharge or 0
    local cfg = self:GetCZCfg()
    return num >= cfg[#cfg].price 
end

--当前充值配置
function PetWeaponFanliData:CurCZCfg()
    local num = self.info_data.today_recharge or 0
    local cfg = self:GetCZCfg()
    for i,v in pairs(cfg) do
        if num < v.price then
            return v
        end
    end
    return cfg[1]
end

function PetWeaponFanliData:NoticeRemind()
    self.remind_data:SetDirty("first_open_flag")
end

-- 1 已被抽取，0未抽取
function PetWeaponFanliData:GetItemState(data)
	if TableIsEmpty(self.info_data.item_flag) then
		return 0
	end
    return self.info_data.item_flag[data.seq]
end


-- 当前阶段
function PetWeaponFanliData:CurStageCfg()
    local num = self.info_data.all_count or 0
    local cfg = self:GetStageCfg()
    for i,v in pairs(cfg) do
        local flag = self:GetStageFlag(v)
        if flag >= 0 then
            return v
        end
    end
    return cfg[#cfg]
end

-- 1可领，0未达成，-1已领
function PetWeaponFanliData:GetStageFlag(data)
    if TableIsEmpty(self.info_data.lottery_reward_flag) then
        return 0
    end
    local num = self.info_data.all_count or 0
    if self.info_data.lottery_reward_flag[data.seq] == 1 then
        return -1
    end
    if num >= data.number then
        return 1
    else
        return 0
    end
end


--魂器返利红点
function PetWeaponFanliData:GetRemind()
    if not ActivityData.IsOpen(self.act_type) then
		return 0
    end
    if not self.remind_data.first_open_flag then
        return 1
    end
    local num = self:GetRemainCount()
    if num > 0 then
        return 1
    end
    local cfg = self:CurStageCfg()
    local flag = self:GetStageFlag(cfg)
    if flag > 0 then
        return 1
    end
    return 0
end