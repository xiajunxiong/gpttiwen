
FinishData = FinishData or BaseClass()

function FinishData:__init()
	if FinishData.Instance ~= nil then
		Debuger.LogError("[FinishData] attempt to create singleton twice!")
		return
	end
	FinishData.Instance = self

	self.ramdon_list = {}
end

function FinishData:__delete()
	FinishData.Instance = nil
end

function FinishData:SetBattleMode(battle_mode)
	self.battle_mode = battle_mode
end

function FinishData:GetBattleMode()
	return self.battle_mode
end

function FinishData:SetMonsterGroupId(monster_group_id)
	self.monster_group_id = monster_group_id
end

function FinishData:GetMonsterGroupId()
	return self.monster_group_id or -1
end

--[[
func_list = {reward_func,level_func,desc_func,value_func,show_type}
reward_func奖励配置 直接读表
level_func 通关关数 需要取服务端数据
desc_func  结算描述 如第XX关的奖励 需要取服务端数据
value_func 数值列表 如经验(name) +500 或者铜币(CommonItemId.Coin) +500
value_func返回一个list写法 如：list = {
	[1] = {name = "经验",num = "+500"},
	[2] = {item_id = CommonItemId.Coin,num = "+500"}
}
show_type 需要显示的特效类型 1 默认(胜利) 2 成功 3完胜 5结束
show_rank --是否显示排行固定格式 func return true or false
]]

function FinishData:PassRegister(mode,func_list)
	if self.pass_register_list == nil then
		self.pass_register_list = {}
	end
	self.pass_register_list[mode] = func_list
end

function FinishData:PassRegisterList(mode)
	return self.pass_register_list[mode or self:GetBattleMode()]
end
--[[
func_list = {desc_func,value_func}
reward_func奖励配置 直接读表
desc_func  结算描述 如第XX关的奖励 需要取服务端数据
value_func 数值列表 如经验(name) +500 或者铜币(CommonItemId.Coin) +500
value_func返回一个list写法 如：list = {
	[1] = {name = "经验",num = "+500"},
	[2] = {item_id = CommonItemId.Coin,num = "+500"}
}
]]

function FinishData:FailRegister(mode,func_list)
	if self.fail_register_list == nil then
		self.fail_register_list = {}
	end
	self.fail_register_list[mode] = func_list
end

function FinishData:FailRegisterList(mode)
	return self.fail_register_list[mode or self:GetBattleMode()]
end

function FinishData:PassDesc(mode)
	return Language.Finish.PassTip[mode or self:GetBattleMode()] or ""
end

function FinishData:FailDesc(mode)
	if self:IsDraw() then
		return Language.Finish.ModeDrawTip[mode or self:GetBattleMode()] or Language.Finish.DrawTip
	end
	return Language.Finish.FailTip[mode or self:GetBattleMode()] or Language.Finish.FailTip[0]
end

--[[
close_func:结束回调
is_close:是否关闭界面 默认自动关闭 true 不关闭
]]
--这里关闭结算界面 
function FinishData:SetCloseFunc(mode,close_func,is_close)
	if self.close_func_list == nil then
		self.close_func_list = {}
	end
	self.close_func_list[mode] = {
		close_func = close_func,
		is_close = is_close or false
	}
end

function FinishData:GetCloseFunc(mode)
	return self.close_func_list[mode or self:GetBattleMode()]
end

--界面自动关闭时间 这里写死
function FinishData:GetCloseTime(flag)
	return FinishConfig.CloseTime[SceneData.Instance:GetMode()] or FinishConfig.CloseTime[flag]
end

--协议下发的数据
function FinishData:SetFinishInfo(info)
	self.finish_info = info
end

function FinishData:GetFinishInfo()
	return self.finish_info or {}
end

function FinishData:Voluation(data)
	local list = {}
	for k,v in pairs(data or {}) do
		local key = Language.Finish.FuncList[k]
		if key ~= nil then
			list[key] = v()
		end
	end
	return list
end

function FinishData:GetPassData()
	return self:Voluation(self:PassRegisterList())
end

function FinishData:GetFailData()
	return self:Voluation(self:FailRegisterList())
end

--是否为平局
function FinishData:IsDraw()
	local info = self:GetFinishInfo()
	if info and info.result_type then
		if FinishConfig.IsDrawConfig[info.battle_mode] then
			return FinishConfig.IsDrawResult[info.result_type]
		end
		return info.result_type == RESULT_TYPE.DRAW
	end
	return false
end

--是否是胜利(单一判断)
function FinishData:IsWin()
	local info = self:GetFinishInfo()
	if info then
		return info.is_win == 1
	end
	return false
end

--获取随机的失败tip
function FinishData:GetRamdonTip()
	local tip = Language.Finish.RamdonTip
	local num = GameMath.Ramdon(1,#tip)
	while self:IsHasNum(num,#tip) do
		self.ramdon_list[num] = nil
		num = GameMath.Ramdon(1,#tip)
	end
	self.ramdon_list[num] = true
	return tip[num] or ""
end


function FinishData:GetEndTips()
	local battle_mode = self:GetBattleMode()
	local tip = Language.Finish.EndTips

	return tip[battle_mode]
end

--true随机掉 false 显示
function FinishData:IsCanRamdon(num)
	if num == 1 then
		return BagData.Instance:IsEquipLower()
	-- elseif num == 2 then
	-- 	return self:IsNoTeam()
	end
	return false
end

--平均每个至少提示出现一次
function FinishData:IsHasNum(num)
	if self.ramdon_list[num] or self:IsCanRamdon(num)then
		return true
	end
	return false
end
function FinishData:SetEleRewards(data)
	self.ele_reward_item_list = data
end
function FinishData:GetEleRewards()
	return self.ele_reward_item_list
end
-- function FinishData:IsNoTeam()
-- 	return FinishConfig.IsNoTeam[self:GetBattleMode()] or false
-- end