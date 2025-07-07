ExternalLuaCtrl = ExternalLuaCtrl or BaseClass(BaseCtrl)

local auto_rela_path = "config/auto/"

--枚举，用于注册刷新
ExternalLua = {
	-- Gift = "item/gift_auto.lua",
	-- Gift2 = "item/gift_2_auto.lua",
	-- RandAct1 = "randactivityconfig_1_auto.lua",
	-- CrossRandAct1 = "cross_randactivity_cfg_1_auto.lua",
	-- C_AutoTreasureActivity = auto_rela_path .. "treasure_activity_auto",
	C_AutoItemGift = auto_rela_path .. "item/gift_auto",
	C_AutoItemOther = auto_rela_path .. "item/other_auto",
	C_AutoItemExpense = auto_rela_path .. "item/expense_auto",
	C_AutoActivityMain = auto_rela_path .. "activity_main_auto",
	C_AutoAnimalRace = auto_rela_path .. "animal_race_auto",
	C_AutoBackReward = auto_rela_path .. "back_reward_auto",
	C_AutoColorfulIllustrations = auto_rela_path .. "colorful_illustrations_auto",
	C_AutoCoolSummer = auto_rela_path .. "cool_summer_auto",
	C_AutoDayReward = auto_rela_path .. "day_reward_auto",
	C_AutoDivinationActivity = auto_rela_path .. "divination_activity_auto",
	C_AutoExclusiveGift = auto_rela_path .. "exclusive_gift_auto",
	C_AutoExpPurchase = auto_rela_path .. "exp_purchase_auto",
	C_AutoFengShenZhanLing = auto_rela_path .. "fengshenzhanling_auto",
	C_AutoImmortalCoinFund = auto_rela_path .. "immortal_coin_fund_auto",
	C_AutoJoySeeksFavors = auto_rela_path .. "joy_seeks_favors_auto",
	C_AutoLuckyMaze = auto_rela_path .. "lucky_maze_auto",
	C_AutoMayDayActivity = auto_rela_path .. "may_day_activity_auto",
	C_AutoTaoYuanYiSanShi = auto_rela_path .. "taoyuanyisanshi_auto",
	C_AutoNationalDay = auto_rela_path .. "national_day_auto",
	C_AutoPetLoot = auto_rela_path .. "pet_loot_auto",
	C_AutoRandActivityConfig_1 = auto_rela_path .. "randactivityconfig_1_auto",
	C_AutoSealPromote = auto_rela_path .. "seal_promote_auto",
	C_AutoSpringFestivalStore = auto_rela_path .. "spring_festival_store_auto",
	C_AutoTreasureHunt = auto_rela_path .. "treasure_hunt_auto",
	C_AutoShanHaiOrigin = auto_rela_path .. "shanhai_origin_auto",
	C_AutoZhaoCaiJinBao = auto_rela_path .. "zhaocaijinbao_auto",
	C_AutoJiXingGaoZhao = auto_rela_path .. "jixinggaozhao_auto",
}

function ExternalLuaCtrl:__init()
	if nil ~= ExternalLuaCtrl.Instance then
		print("[ExternalLuaCtrl] attempt to create singleton twice!")
		return
	end
	ExternalLuaCtrl.Instance = self

	self.refresh_funcs = {}
	self:RegisterProtocol(SCActivityConfigReloadNotice, "OnActivityConfigReloadNotice")

	Runner.Instance:AddRunObj(self, 16)
end

function ExternalLuaCtrl:__delete()
	self:UnRegisterProtocol(SCActivityConfigReloadNotice)
end

function ExternalLuaCtrl:OnActivityConfigReloadNotice(protocol)
	if not EditorSettings:UpdateExternalLua() then		--EDITOR环境不更新
		return
	end
	self:PerformUpdate()
end

function ExternalLuaCtrl:PerformUpdate()
	if IS_EDITOR then
		print("开始更新外部Lua代码...")
	end
	ExternalLuaUpdater:PerformUpdate(function ()
		local err = ExternalLuaUpdater:GetError()
		if StringIsEmpty(err) then
			local updated_files = ExternalLuaUpdater:GetUpdateFiles()
			self:RefreshLuaScript(updated_files)
		else 
			LogError("ExternalLuaUpdater:PerformUpdate ERROR=" .. tostring(err))
			if err ~= ExternalLuaErrorCode.CancelByNewPerform then
				self:OnActivityConfigReloadNotice()
			end
		end
		if IS_EDITOR then
			print("更新外部Lua代码完成!")
		end
	end)
end

function ExternalLuaCtrl:RegisterRefresh(key,func,perform)
	local key_funcs = self.refresh_funcs[key]
	if key_funcs == nil then
		key_funcs = {}
		self.refresh_funcs[key] = key_funcs
	end
	table.insert(key_funcs,func)
	if perform then
		func()
	end
end

function ExternalLuaCtrl:RefreshLuaScript(file_names)
	--rerequire all
	for k,v in pairs(file_names) do
		ExternalLuaLoader:Require(v)
	end
	local executed_funcs = {}		--保证相同的刷新函数只执行一次

	for k,v in pairs(file_names) do
		local funcs = self.refresh_funcs[v]
		if funcs ~= nil then
			for fk,fv in pairs(funcs) do
				if fv ~= nil and not executed_funcs[fv] then
					executed_funcs[fv] = true
					fv()
				end
			end
		end
	end
end

function ExternalLuaCtrl:Update()
	ExternalLuaUpdater:Update()
end



ExternalLuaCtrlEditor = ExternalLuaCtrlEditor or {}

function ExternalLuaCtrlEditor.SimularUpdate()
	if ExternalLuaCtrl.Instance ~= nil then
		ExternalLuaCtrl.Instance:PerformUpdate()
	end
end