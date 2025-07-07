
FILE_REQ("modules/reward/reward_config")
FILE_REQ("modules/reward/reward_data")
FILE_REQ("modules/reward/reward_data_reward")
FILE_REQ("modules/reward/reward_pet_weapon_fanli_ctrl")

VIEW_REQ("modules/reward/reward_view")
VIEW_REQ("modules/reward/reward_panel")
VIEW_REQ("modules/reward/reward_dialog")

VIEW_REQ("modules/reward/reward_online_view")
VIEW_REQ("modules/reward/reward_upgrade_view")
VIEW_REQ("modules/reward/reward_rebate_view")
VIEW_REQ("modules/reward/reward_score_view")
VIEW_REQ("modules/reward/reward_roller_view")
VIEW_REQ("modules/reward/reward_fortune_bag_view")
VIEW_REQ("modules/reward/reward_immortal_fund_view")
VIEW_REQ("modules/reward/reward_daily_sign_in_view")
VIEW_REQ("modules/reward/reward_activity_back_view")
VIEW_REQ("modules/reward/reward_fund_exchange_view")
VIEW_REQ("modules/reward/reward_anti_fraud_view")
VIEW_REQ("modules/reward/reward_pet_intensify")

VIEW_REQ("modules/level_gift_notice/level_gift_notice")
FILE_REQ("modules/reward/reward_super_vip_ctrl")
VIEW_REQ("modules/reward/reward_month_card_view")
VIEW_REQ("modules/reward/month_card_alert")
VIEW_REQ("modules/reward/reward_lunch_supper_view")
VIEW_REQ("modules/reward/reward_fund_exchange_detail_view")
VIEW_REQ("modules/reward/god_beast_grace_view")

RewardCtrl = RewardCtrl or BaseClass(BaseCtrl)

function RewardCtrl:__init()
	if RewardCtrl.Instance ~= nil then
		Debuger.LogError("[RewardCtrl] attempt to create singleton twice!")
		return
	end	
	RewardCtrl.Instance = self
	self.data = RewardData.New()
	SuperVipCtrl.New()
	PetWeaponFanliCtrl.New()
    self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RoleNoticeComplete, self))
	EventSys:Bind(GlobalEventKeys.RoleLevelChange,BindTool.Bind(self.RoleLevelChange, self.data))
	self:RegisterAllProtocols()
end

function RewardCtrl:OnInit()
	RemindCtrl.Instance:Remind():Register(Mod.Reward.Online, self.data.online_red_point, function ()
		return self.data.online_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Reward.DailySignIn, self.data.daily_sign_in_red_point, function ()
		return self.data.daily_sign_in_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Activity.ActivityBack, self.data.activity_back_red_point, function ()
		return self.data.activity_back_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Reward.Upgrade, self.data.upgrade_red_point, function ()
		return self.data.upgrade_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Reward.Score, self.data.score_red_point, function ()
		return self.data.score_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Reward.Roller, self.data.roller_red_point, function ()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_CONSUMER_CAROUSEL) then
			return 0
		end
		return self.data.roller_red_point.val and 1 or 0
	end)
	
	RemindCtrl.Instance:Remind():Register(Mod.Reward.FortuneBag, self.data.fortune_bag_red_point, function ()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_FORTUNE_BAG) then
			return 0
		end
		return ServerActivityData.Instance:GetFortuneBagNum()
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Reward.SuperVip,SuperVipCtrl.Instance.data.red_piont_data,function()
		if not SuperVipCtrl.Instance.data:GetSuperVipEnd() then
			return 0
		end
        return SuperVipCtrl.Instance.data:GetRemindNum()
    end)
	
	-- 灵玉基金
	Remind.Instance:Register(Mod.Reward.ImmortalFund, self.data.immortal_fund_red_point, BindTool.Bind(self.data.GetImmortalFundRemind, self.data))
	RemindCtrl.Instance:Remind():Register(Mod.Reward.MonthCard, self.data.month_card_red_point, function()
        return self.data.month_card_red_point.val and 1 or 0
    end)
	
	RemindCtrl.Instance:Remind():Register(Mod.Reward.LunchSupper, self.data.day_food_red_point, function ()
		return self.data.day_food_red_point.val and 1 or 0
	end)

	-- 宠物强化福利
	RemindCtrl.Instance:Remind():Register(Mod.Reward.PetIntensify, self.data.pet_intensify_data, function ()
		return self.data:GetPetIntensifyRemind()
	end)
	FunOpen.Instance:SelfCondition(Mod.Reward.PetIntensify,function()
		return self.data:PetIntensifyRewardIsOpen()
	end)

	-- 宠技返利
	RemindCtrl.Instance:Remind():Register(Mod.Reward.ChongJiFanLi,ChongJiFanLiCtrl.Instance.data.remind_data,function()
		return ChongJiFanLiCtrl.Instance.data:RemindNum() 
	end)
	FunOpen.Instance:SelfCondition(Mod.Reward.ChongJiFanLi,function()
		return ChongJiFanLiData.Instance:IsOpen()
	end)

	-- 神兽恩泽
	Remind.Instance:Register(Mod.Reward.GodBeastGrace, self.data.god_beast_grace_data, BindTool.Bind(self.data.GodBeastGraceAllRemind, self.data))
	BagData.Instance:ListenItems({Config.shenshouenze_auto.other[1].consume_item}, function()
		self.data.god_beast_grace_data:SetDirty("remind_flag")
	end)
	ActivityRandData.Instance:Register(Mod.Reward.GodBeastGrace,function ()
		return self.data:GodBeastGraceIsOpen()
	end, self.data.god_beast_grace_data)

	--注册自定义监听方式
	ActivityRandData.Instance:Register(Mod.LevelGiftNotice.Main,function ()
		return self.data:LevelGiftNoticeShowCheck()
	end,self.data.upgrade_change)

	if GLOBAL_CONFIG:Flags().block_antifraud then
		FunOpen.Instance:SelfCondition(Mod.Reward.AntiFraud,function ()
			return false
		end)
	else
		RemindCtrl.Instance:Remind():Register(Mod.Reward.AntiFraud, self.data.anti_fraud_red_point, function ()
			return self.data.anti_fraud_red_point.val and 1 or 0
		end)
	end

	
end

function RewardCtrl:__delete()
	EventSys:UnBind(self.complete_handle)
	RewardCtrl.Instance = nil
	self:UnRegisterAllProtocols()
end

function RewardCtrl:OnUnloadGameLogic()
	self.data:ClearData()
	self.data:ResetImmortalFundData()
	self.data:ResetPetIntensifyData()
	self.data:ResetGodBeastGraceData()
	PetWeaponFanliCtrl.Instance:OnUnloadGameLogic()
end

function RewardCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSFetchRewardReq)
    self:RegisterProtocol(CSFindRewardReq)
    self:RegisterProtocol(CSRewardFindNotifyCancelReq)
    self:RegisterProtocol(CSFangFraudRewardReq)
	self:RegisterProtocol(CSFundExchangeReq)
	self:RegisterProtocol(CSPetIntensifyRewardReq)
    self:RegisterProtocol(SCOnlineRewardInfo, "OnOnlineRewardInfo", true)
    self:RegisterProtocol(SCSignRewardInfo, "OnSignRewardInfo", true)
    self:RegisterProtocol(SCLevelRewardInfo, "OnLevelRewardInfo", true)
    self:RegisterProtocol(SCFindRewardInfo, "OnFindRewardInfo", true)
    self:RegisterProtocol(SCNoticeCfgUpdata, "OnNoticeCfgUpdata", true)
    self:RegisterProtocol(SCScoreRewardInfo, "OnScoreRewardInfo", true)
    self:RegisterProtocol(SCMonthInvestInfo, "OnMonthInvestInfo", true)
    self:RegisterProtocol(SCRAConsumeCarouselInfo, "OnRAConsumeCarouselInfo", true)
    self:RegisterProtocol(SCRAConsumeCarouselReward, "OnConsumeCarouselReward", true)
    self:RegisterProtocol(SCRAConsumeCarouselShow, "OnConsumeCarouselShow", true)
    self:RegisterProtocol(SCRewardFindNotify, "OnRewardFindNotify")
    self:RegisterProtocol(SCRALingYuJiJinInfo, "OnLingYuJiJinInfo")
    self:RegisterProtocol(CSMonthCardReq)
    self:RegisterProtocol(SCMonthCardInfo, "OnSCMonthCardInfo")
    self:RegisterProtocol(SCFangFraudRewardInfo, "OnFangFraudRewardInfo")
	self:RegisterProtocol(SCDayRewardDayFoodInfo, "OnSCDayRewardDayFoodInfo")
	self:RegisterProtocol(SCPetIntensifyRewardInfo, "OnSCPetIntensifyRewardInfo")
	self:RegisterProtocol(SCRAShenShouEnZeInfo, "OnSCRAShenShouEnZeInfo")
	self:RegisterProtocol(SCRAShenShouEnZeDraw, "OnSCRAShenShouEnZeDraw")
	self:RegisterProtocol(SCRAShenShouEnZeRecord, "OnSCRAShenShouEnZeRecord")
	--self:RegisterProtocol(SCRAHunQiFanLiInfo, "OnSCRAPetWeaponFanLiInfo")
end

function RewardCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSFetchRewardReq)
	self:UnRegisterProtocol(CSFindRewardReq)
	self:UnRegisterProtocol(CSRewardFindNotifyCancelReq)
	self:UnRegisterProtocol(SCOnlineRewardInfo)
	self:UnRegisterProtocol(SCSignRewardInfo)
	self:UnRegisterProtocol(SCLevelRewardInfo)
	self:UnRegisterProtocol(SCNoticeCfgUpdata)
	self:UnRegisterProtocol(SCScoreRewardInfo)
	self:UnRegisterProtocol(SCMonthInvestInfo)
	self:UnRegisterProtocol(SCRAConsumeCarouselInfo)
	self:UnRegisterProtocol(SCRAConsumeCarouselReward)
	self:UnRegisterProtocol(SCRAConsumeCarouselShow)
	self:UnRegisterProtocol(SCRewardFindNotify)
	self:UnRegisterProtocol(SCRALingYuJiJinInfo)
	self:UnRegisterProtocol(SCRAShenShouEnZeInfo)
	self:UnRegisterProtocol(SCRAShenShouEnZeDraw)
	self:UnRegisterProtocol(SCRAShenShouEnZeRecord)
	self:UnRegisterProtocol(SCRAHunQiFanLiInfo)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Reward.Online)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Reward.DailySignIn)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Activity.ActivityBack)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Reward.Upgrade)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Reward.Score)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Reward.Roller)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Reward.FortuneBag)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Reward.ImmortalFund)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Reward.SuperVip)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Reward.AntiFraud)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Reward.MonthCard)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Reward.FundExchange)
	RemindCtrl.Instance:Remind():UnRegister(Mod.RandActivity.FirstCharge)
end

function RewardCtrl:RoleNoticeComplete()
	RechargeCtrl.Instance:SendRechargeReq()
end

function RewardCtrl:RoleLevelChange()
	RewardData.Instance:CheckFortuneBag()
end

function RewardCtrl:OnOnlineRewardInfo(protocol)
	self.data:SetOnlineRewardInfo(protocol)
end

function RewardCtrl:OnSignRewardInfo(protocol)
	self.data:SetSignRewardInfo(protocol)
end

function RewardCtrl:OnLevelRewardInfo(protocol)
	self.data:SetLevelRewardInfo(protocol)
end

function RewardCtrl:OnFindRewardInfo(protocol)
	self.data:SetFindRewardInfo(protocol)
end

function RewardCtrl:OnNoticeCfgUpdata(protocol)
	end

function RewardCtrl:SendFetchRewardReq(param_t)
	local protocol = GetProtocol(CSFetchRewardReq)
	protocol.oper_type = param_t.oper_type
	protocol.seq = param_t.seq or 0
	SendProtocol(protocol)
end

function RewardCtrl:SendOnlineRewardReq(index)
	local param_t = {
        oper_type = RewardConfig.OperType.online,
        seq = index,
    }
	self:SendFetchRewardReq(param_t)
end

-- function RewardCtrl:SendChongJiFanLiRewardReq(index)
-- 	local param_t = {
--         oper_type = RewardConfig.OperType.chongjifanli,
--         seq = index,
--     }
-- 	self:SendFetchRewardReq(param_t)
-- end

function RewardCtrl:SendDailySignInRewardReq(day)
	local param_t = {
        oper_type = RewardConfig.OperType.daily_sign_in,
        seq = day,
    }
	self:SendFetchRewardReq(param_t)
end

function RewardCtrl:SendUpgradeRewardReq(seq)
	local param_t = {
        oper_type = RewardConfig.OperType.upgrade,
        seq = seq,
    }
	self:SendFetchRewardReq(param_t)
end

function RewardCtrl:SendLunchSupperRewardReq(seq)
	local param_t = {
        oper_type = RewardConfig.OperType.lunchsupper,
        seq = seq,
    }
	self:SendFetchRewardReq(param_t)
end

function RewardCtrl:SendFindRewardReq(param_t)
	local protocol = GetProtocol(CSFindRewardReq)
	protocol.type = param_t.type or 0
	protocol.consume_type = (param_t.consume_type or 1) - 1
	protocol.count = param_t.count or 0
	SendProtocol(protocol)
end

function RewardCtrl:OnScoreRewardInfo(protocol)
	self.data:SetScoreRewardInfo(protocol)
end

function RewardCtrl:OnMonthInvestInfo(protocol)
end

--消费转盘
function RewardCtrl:OnRAConsumeCarouselInfo(protocol)
	self.data:SetConsumeCarouselInfo(protocol)
end

function RewardCtrl:OnConsumeCarouselReward(protocol)
	self.data:SetConsumeCarouselReward(protocol)
end

function RewardCtrl:OnConsumeCarouselShow(protocol)
	self.data:SetConsumeCarouselShow(protocol)
end

function RewardCtrl:SendNotifyCancelReq()
	local protocol = GetProtocol(CSRewardFindNotifyCancelReq)
	SendProtocol(protocol)
end

function RewardCtrl:OnRewardFindNotify(protocol)
	self.data:SetRewardFindNotify(protocol)
end

function RewardCtrl:OnLingYuJiJinInfo(protocol)
	self.data:SetImmortalFundInfo(protocol)
end

function RewardCtrl:OnSCMonthCardInfo(protocol)
	self.data:SetMonthCardInfo(protocol)
end

--req_type-> RewardConfig.MonthCardReqType
function RewardCtrl:SendMonthCardReq(req_type, param1, param2)
	local protocol = GetProtocol(CSMonthCardReq)
	protocol.req_type = req_type
	protocol.param1 = param1
	protocol.param2 = param2
	SendProtocol(protocol)
end

function RewardCtrl:SendFangFraudRewardReq(answer_list)
	local protocol = GetProtocol(CSFangFraudRewardReq)
	protocol.answer_list = answer_list
	SendProtocol(protocol)
end

function RewardCtrl:OnFangFraudRewardInfo(protocol)
	self.data:SetFangFraudRewardInfo(protocol)
end

function RewardCtrl:OnSCDayRewardDayFoodInfo(protocol)
	self.data:SetDayFoodInfo(protocol)
end

--==========================灵玉基金兑换==========================
function RewardCtrl:SendFundExchangeReq(index)
	local protocol = GetProtocol(CSFundExchangeReq)
	protocol.index = index or 0
	SendProtocol(protocol)
end

--==========================宠物强化福利==========================
function RewardCtrl:OnSCPetIntensifyRewardInfo(protocol)
	RewardData.Instance:SetPetIntensifyData(protocol)
end

function RewardCtrl:SendPetIntensifyRewardReq(seq)
	local protocol = GetProtocol(CSPetIntensifyRewardReq)
	protocol.seq = seq or 0
	SendProtocol(protocol)
end

function RewardCtrl:PlayPetIntensifyRewardDialog()
	local reward_data = self.data:GetPetIntensifyNewReward()
	if not TableIsEmpty(reward_data) then
		local item_list = DataHelper.FormatItemList(reward_data.item_list_1)
		local param_t = {
			context = Language.Reward.RewardPetIntensify.dialog_text,
			right_btn_text = Language.Reward.RewardPetIntensify.text6,
			remind_key = DTNewTodayNotRemind.PetIntensifyDialog,
			item_list = item_list,
			right_btn_func = function ()
				ViewMgr:OpenViewByKey(Mod.Reward.PetIntensify)
				ViewMgr:CloseView(RewardDialog)
				RewardData.Instance:SetPetIntensifyNewReward(nil)
			end
		}
		PublicPopupCtrl.Instance:OpenRewardDialog(param_t)
	end
end

--  ========== 神兽恩泽 ============
function RewardCtrl:OnSCRAShenShouEnZeInfo(protocol)
	self.data:SetGodBeastGraceInfo(protocol)
end

function RewardCtrl:OnSCRAShenShouEnZeDraw(protocol)
	self.data:SetGodBeastGraceResult(protocol)
end

function RewardCtrl:OnSCRAShenShouEnZeRecord(protocol)
	self.data:SetGodBeastGraceRecord(protocol)
end

function RewardCtrl:GodBeastGraceSend(req_type, param1)
	ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_GOD_BEAST_GRACE, req_type, param1)
end
--  ========== 神兽恩泽 ============

--  ========== 魂器返利 ============
function RewardCtrl:OnSCRAPetWeaponFanLiInfo(protocol)
	self.data:SetPetWeaponFanLiInfo(protocol)
end

function RewardCtrl:PetWeaponFanLiSend(req_type, param1)
	ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.Rand_HUN_QI_FAN_LI, req_type, param1)
end
--  ========== 魂器返利 ============