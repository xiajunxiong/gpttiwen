FILE_REQ("modules/main_other/gather_data")
FILE_REQ("modules/main_other/attr_change_data")
FILE_REQ("modules/main_other/key_use_data")
FILE_REQ("modules/main_other/main_other_data")
FILE_REQ("modules/main_other/queue_helper")
FILE_REQ("modules/main_other/activity_remind_data")

VIEW_REQ("modules/main_other/level_up")
VIEW_REQ("modules/main_other/gather")
VIEW_REQ("modules/main_other/super_map")
VIEW_REQ("modules/main_other/attr_change")
VIEW_REQ("modules/main_other/key_use_view")
VIEW_REQ("modules/main_other/compass")
VIEW_REQ("modules/main_other/choose_gift_view")
VIEW_REQ("modules/main_other/choosel_gift_all_view")
VIEW_REQ("modules/main_other/preview_gift_view")
VIEW_REQ("modules/main_other/buff_view")
VIEW_REQ("modules/main_other/activity_remind_view")
VIEW_REQ("modules/main_other/quick_buy_view")
VIEW_REQ("modules/main_other/get_way_view")
VIEW_REQ("modules/main_other/sweep_result_view")
VIEW_REQ("modules/main_other/gift_open_view")
VIEW_REQ("modules/main_other/gift_open_shitu_cg_view")
VIEW_REQ("modules/main_other/gift_open_bun_come_view")
VIEW_REQ("modules/main_other/get_reward_view")
VIEW_REQ("modules/main_other/total_score")
VIEW_REQ("modules/main_other/partner_score")
VIEW_REQ("modules/main_other/joy_stick")
VIEW_REQ("modules/main_other/joy_stick_data")
VIEW_REQ("modules/main_other/pet_score")
VIEW_REQ("modules/main_other/power_saving_view")
VIEW_REQ("modules/main_other/poster_view")

MainOtherCtrl = MainOtherCtrl or BaseClass(BaseCtrl)

function MainOtherCtrl:__init()
	if MainOtherCtrl.Instance ~= nil then
		Debuger.LogError("[MainOtherCtrl] attempt to create singleton twice!")
		return
	end
	MainOtherCtrl.Instance = self
	self.gather_data = GatherData.New()
	self.attr_change_data = AttrChangeData.New()
	self.key_use_data = KeyUseData.New()
	self.queue_helper = QueueHelper.New()
	self.main_other_data = MainOtherData.New()
	self.joy_stick_data = JoyStickData.New()
	self.choose_gift_use_data = ChooseGiftUseData.New()
	self.activity_remind_data = ActivityRemindData.New()
	self.stop = false
	self.pause = false
	if not IS_STANDALONE() then
		TimeHelper:AddRunTimer(BindTool.Bind(self.CheckPowerSaveMode, self),10,-1,false)
	end
end

function MainOtherCtrl:OnUnloadGameLogic()
	self.stop = false
	self.pause = false
	self.key_use_data:ClearData()
	-- if PowerSavingView.IsInPowerSavingMode() then
		PowerSavingView.ExitSavingPowerMode()
	-- end
	UnityTouchState.ResetLastTouchTime()		--重置点击时间，进入省电模式重新计时，防止自动重连后马上又进入省电模式
end

function MainOtherCtrl:__delete()
	if MainOtherCtrl.Instance == self then
		MainOtherCtrl.Instance = nil
	end
end

function MainOtherCtrl:OnRoleLevelChange()
	self:IsFinishNormal(LevelUpView,"IsLevelUp")
end

function MainOtherCtrl:CheckPowerSaveMode()
	-- LogError("CheckPowerSaveMode===",LoginData.Instance:LoginState())
	if LoginData.Instance:LoginState() ~= LoginStateType.Logined then
		-- if PowerSavingView.IsInPowerSavingMode() then
			PowerSavingView.ExitSavingPowerMode()
		-- end
		return
	end
	if PowerSavingView.IsInPowerSavingMode() then		--已在省电模式中，不需要再判断时间
		return
	end
	if Time.time - UnityTouchState.LastTouchTime() >= PowerSavingView.EnterJudgeTime() then
		PowerSavingView.EnterSavingPowerMode()
	end
end

function MainOtherCtrl:IsFinishNormal(view,name)
	if FinishData.Instance:GetBattleMode() then
		self[name] = true
	else
		TimeHelper:AddDelayTimer(function()--等待1秒判断
			if FinishData.Instance:GetBattleMode() then
				self[name] = true
			else
				ViewMgr:OpenView(view)
			end
		end, 0.5)
	end
end

function MainOtherCtrl:OnFinishNormal(view,name)
	if self[name] then
		ViewMgr:OpenView(view)
		self[name] = nil
	end
end

function MainOtherCtrl:OnFinishNormalList()
	self:OnFinishNormal(LevelUpView,"IsLevelUp")
	self:OnFinishNormal(AttrChange,"IsAttrChange")
end

--- param_t = {desc = 1 ,time = 1, finish_func = func, seq = 0}
function MainOtherCtrl:OpenGather(param_t)
	-- 好礼转动
	if ServerActivityData.Instance:IsHeavenGiftCount(param_t.id) then
		self:CloseGather()
		return
	end
	-- 劳动好礼
	if ServerActivityData.Instance:IsLabourGiftCount(param_t.id) then
		self:CloseGather()
		return
	end
	-- 瑞雪丰年
	if AuspiciousSnowYearData.Instance:IsSnowGiftCount(param_t.id) then
		self:CloseGather()
		return
	end
	self.gather_data:SetGatherInfo(param_t)
	if param_t.seq and param_t.seq ~= 0 then
		if SceneData.Instance:GetMode() == SceneModeType.FightGather then
			if FightData.Instance:GetGatherEnergy() == 0 then
				PublicPopupCtrl.Instance:Center(Language.MainOther.GatherNotTimes)
				return
			end
		end
		SceneCtrl.RequestGather(param_t.seq,param_t.obj_id,param_t.is_auto)
	else
		ViewMgr:OpenView(GatherView)
	end
end

function MainOtherCtrl:OpenAttrChange(list, title_info)
	if RoleCtrl.Instance.data:GetRoleLevel() < GuideData.Instance:GetAttrFlyLimitLevel() then
		return
	end
	local change_list = {}
	-- 屏蔽血量法术
	for at, val in pairs(list) do
		if BattleAttriType.CurHp ~= at and BattleAttriType.CurMp ~= at then
			val.sort_id = AttrTypeSort[at]
			change_list[at] = val
		end
	end
	--根据属性的显示顺序排序 需要修改,逻辑暂未实现
	-- table.sort(change_list,
	-- 	function (a,b)
	-- 	return a.sort_id < b.sort_id
	-- end)
	if next(change_list) then
		self.attr_change_data:SetChangeInfo(change_list, title_info)
		self:IsFinishNormal(AttrChange,"IsAttrChange")
	end
end

function MainOtherCtrl:CloseGather()
	AutoInvokerCtrl.Instance:ClearCommands()
	if ViewMgr:IsOpen(GatherView) then
		ViewMgr:CloseView(GatherView)
	end
	if MainPlayer then
		MainPlayer:StopMove()
	end
end

--弹窗一键使用窗口 外部接口
function MainOtherCtrl:OpenKeyUseView(param_t)
	if self.stop == true then return end
	-- if MainViewData.Instance:IsBattle() or TombRaiderData.Instance:IsTombRaider() or SceneData.Instance:GetMode() == SCENE_TYPE.SHI_TU_CHUANG_GONG then
	if BattleData.Instance.real_server_fight == true or TombRaiderData.Instance:IsTombRaider() or SceneData.Instance:GetMode() == SCENE_TYPE.SHI_TU_CHUANG_GONG then
        return
	end
	self.key_use_data:AddItem(param_t)
	if self.pause == true then return end
	if not ViewMgr:IsOpen(KeyUseView) and GuideManager.Instance:CheckCanOpenKeyUse(param_t) then
		if ViewMgr:IsOpen(NpcDialogView) then
			self:SetWaitViewFunc(function()
				ViewMgr:OpenView(KeyUseView)
			end)
		else
			ViewMgr:OpenView(KeyUseView)
		end
	end
end

--调用这个接口
function MainOtherCtrl:CloseKeyUse()
	self.key_use_data:ClearData()
	ViewMgr:CloseView(KeyUseView)
end

function MainOtherCtrl:RenewKeyUseView()
	if self.key_use_data:GetItem() ~= nil then
		ViewMgr:OpenView(KeyUseView)
	end
end

--打开快捷购买
function MainOtherCtrl:QuickBuyView(item_id,num)
	local _,config = MallData.Instance:GetShopItemConfig(item_id,Mod.Mall.Shop)
	if config.item_id ~= nil then
		ViewMgr:OpenView(QuickBuyView,{item_data = config,need_num = num})
	end
end

--打开物品不足
function MainOtherCtrl:GetWayView(param)
	ViewMgr:OpenView(GetWayView,param)
end

--通过物品id快速打开物品不足
function MainOtherCtrl:GetWayViewById(item_id)
	if item_id > 0 then
		local item = Item.Create({item_id = item_id})
		self:GetWayView({item = item})
	end
end

function MainOtherCtrl:SetWaitViewFunc(func)
	self.wait_view_func = func
end

function MainOtherCtrl:WaitViewFunc()
	if self.wait_view_func then
		self.wait_view_func()
		self.wait_view_func = nil
	end
end

--外部打开礼包开启 item_list 需要展示物品的列表（Item.New）
--可以使用 DataHelper.FormatItemList(item_list) 处理下item_list
--gift_type 动画展开类型 默认为 GIFT_OPEN_TYPE.SPECIAL
--title_name 动画标题图标名字 可不传
--is_not_show 是否不需要手动飘字（可能存在延迟展示） 默认会飘（需要服务端屏蔽通知飘字）
function MainOtherCtrl:OpenGiftView(item_list,gift_type,is_not_show,title_name,is_close_anim, auto_close, close_func)
	if item_list == nil or #item_list == 0 then
		return 
	end
	ViewMgr:OpenView(GiftOpenView,{item_list = item_list,gift_type = gift_type or GIFT_OPEN_TYPE.SPECIAL,
	title_name = title_name,is_not_show = is_not_show,is_close_anim = is_close_anim, auto_close = auto_close, close_func = close_func})
end

--打3v3对手信息
--left_user_list = {[1] = info...} right_user_list = {[1] = info...}
--info : [uid,prof,level,avatar_type,avatar_id,,name]
function MainOtherCtrl:OnTeamSport(param_t)
	TeamSportData.Instance:SetAllUserList(param_t)
    ViewMgr:OpenView(SportInfoView)
end

-- 礼包开启师徒传功
function MainOtherCtrl:OpenGifShiTuCGtView(item_list, the_score, score_record, auto_close)
	ViewMgr:OpenView(GiftOpenShiTuCGView,{item_list = item_list, the_score = the_score, score_record = score_record, auto_close = auto_close})
end

-- 礼包开启包子来了
function MainOtherCtrl:OpenGiftOpenBunComeView(item_list, is_win, auto_close)
	ViewMgr:OpenView(GiftOpenBunComeView,{item_list = item_list, is_win = is_win, auto_close = auto_close})
end