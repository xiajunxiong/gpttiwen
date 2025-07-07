-- 打开界面时的state参数
OfflineRewardState = {
	Default = -1,	--正常进入界面
	Faild = 0,		--输了（没用了）
	Win = 1,		--赢了（没用了）
	ShowReward = 2,	--展示奖励界面
}

OfflineRewardView = OfflineRewardView or DeclareView("OfflineRewardView", "offline_reward", Mod.OffLineReward.Main)

VIEW_DECLARE_LEVEL(OfflineRewardView,ViewLevel.L1)
VIEW_DECLARE_MASK(OfflineRewardView,ViewMask.BgBlock)

function OfflineRewardView:OfflineRewardView()
	
end

function OfflineRewardView:OnHelpClick()
	PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[110].desc)
end

function OfflineRewardView:OnCloseClick()
	ViewMgr:CloseView(OfflineRewardView)
end


--------------OfflineRewardPanel--------------
OfflineRewardPanel = OfflineRewardPanel or DeclareMono("OfflineRewardPanel", UIWFlushPanel)
function OfflineRewardPanel:OfflineRewardPanel()
	self.break_config = Config.break_cfg_auto.break_config
	self.now_config = {}
	self.break_item = Config.break_cfg_auto.break_item
	self.seq_time = Config.break_cfg_auto.seq_time
	self.other = Config.break_cfg_auto.other[1]
	self.data = RoleData.Instance.break_info
	self.language = Language.Role.Break
	self.end_pos = self.BallTarget.transform.localPosition
	self.fight_data = FightCtrl.Instance:Data()

	self.data_cares = {
        {data = self.data, func = BindTool.Bind(self.FlushBySeq, self), keys = {"seq"}, init_call = false},
		{data = self.data, func = self.FlushByCumulationExp, keys = {"cumulation_exp", "cumulation_time"}, init_call = false},
		{data = self.data, func = self.FlushByCumulationItem, keys = {"cumulation_item"}, init_call = false},
		{data = self.fight_data.fight_soul_info, func = self.FlushBySoulInfo, keys = {"seq"}},
		{data = RoleData.Instance.base_data, func = self.FlushBtnFightRemind, keys = {"level"}, init_call = false},
		{data = self.fight_data.fight_soul_has_remind, func = self.FlushBtnFightRemind, init_call = false},
	}

	-- 特效球最大存储数量
	self.ball_max = 10
	-- 当前轮到的特效球
	self.now_ball = 1
	-- 存储所有飘球
	self.flying_balls = {}

	-- 是否需要更新，如果是要展示突破成功和失败的就展示完才能更新
	self.is_need_flush = true
	self.is_win = false

	-- 飘字最大数量
	self.text_max = 20
	-- 当前轮到的飘字
	self.now_text = 1
	-- 存储所有飘字
	self.flying_texts = {}
	-- 当前领取经验飘出小球数量
	self.now_branch_count = 1
	-- 从秒转换成每小时的倍率
	self.rate = RoleData.Instance:GetBreakRateByHour()
end

function OfflineRewardPanel:Awake()
	UIWFlushPanel.Awake(self)
	UH.SetText(self.TxtNowLevelStr, self.language.NowLevelStr)
	UH.SetText(self.TxtNextLevelStr, self.language.NextLevelStr)
	self.ObjEffectStay:SetActive(true)
	self:FlushAll()
	if self.data.state == OfflineRewardState.ShowReward then
		self:OpenSuccessTip()
	end
end

function OfflineRewardPanel:OnEnable()
	UIWFlushPanel.OnEnable(self)
end

function OfflineRewardPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.down_handler)
	TimeHelper:CancelTimer(self.start_handler)
	TimeHelper:CancelTimer(self.playing_handler)
	TimeHelper:CancelTimer(self.ball_anim_handler)
	TimeHelper:CancelTimer(self.ball_anim_handler)
	TimeHelper:CancelTimer(self.effect_boom)
	TimeHelper:CancelTimer(self.stay_handler)
	TimeHelper:CancelTimer(self.exp_ball_handler)
	RoleCtrl.Instance:ReleaseDelayExp()
	self.data.state = OfflineRewardState.Default
end

-- 这个突破成功和失败的特效姑且留着
function OfflineRewardPanel:DealFightResult()
	self.now_ball_num = 0
	if self.data.seq ~= 0 then
		self.now_ball_num = self.data.seq % self.other.break_num == 0 and self.other.break_num or self.data.seq % self.other.break_num
	end
	-- 战斗结束后播放特效
	if self.data.state ~= OfflineRewardState.Default then
		self.is_need_flush = false
		self.is_win = self.data.state == OfflineRewardState.Win
		self.ObjFaildHide:SetActive(false)
		self.IHBreakFalse.Interactable = self.is_win
		self:SetBallsIHandler(self.is_win)
		self.ObjEffectWinOrFaild:SetActive(true)
		self:SetBallsEffect(false, self.other.break_num)
		local now_show_ball_num = self.now_ball_num
		-- 胜利需要播放小球变大动画
		if self.is_win then
			-- 胜利需要少显示一个球，然后播动画来展示少了的那个球
			now_show_ball_num = now_show_ball_num - 1
			TimeHelper:CancelTimer(self.ball_anim_handler)
			self.ball_anim_handler = TimeHelper:AddDelayTimer(function()
				self.BallList[self.now_ball_num]:SetAnimTrigger()
			end, 2)
		end
		self:SetBallsActive(now_show_ball_num)
		TimeHelper:CancelTimer(self.stay_handler)
		self.stay_handler = TimeHelper:AddDelayTimer(function()
			self.ObjEffectStay:SetActive(self.is_win)
		end, 0.5)
		TimeHelper:CancelTimer(self.start_handler)
		-- 打开后开始播放特效
		self.start_handler = TimeHelper:AddDelayTimer(function()
			AudioMgr:PlayEffect(AudioType.UI, self.is_win and CommonAudio.QiangHuaChengGong or CommonAudio.QiangHuaShiBai)
			if self.win_or_faild_handler then
				self.EffectTool:Stop(self.win_or_faild_handler)
			end
			self.win_or_faild_handler = self.EffectTool:Play(self.is_win and "2161008" or "2161007")
			-- 突破失败才飘扣灵气的字
			if not self.is_win then
				self:FlyText(tonumber(self.break_config[self.data.seq] and self.break_config[self.data.seq].break_lose or 0) * -1)
			end
			TimeHelper:CancelTimer(self.playing_handler)
			self.playing_handler = TimeHelper:AddDelayTimer(function()
				self.ObjFaildHide:SetActive(true)
				self.ObjEffectStay:SetActive(true)
				self.ObjEffectWinOrFaild:SetActive(false)
				self.EffectTool:Stop(self.win_or_faild_handler)
				self:SetBallsIHandler(true)
				self:SetBallsEffect(true, self.now_ball_num)
				self.IHBreakFalse.Interactable = true
				self.is_need_flush = true
				self:FlushAll()
				if self.is_win then
					self:FlushSuccessTip()
					self.ObjSuccessTip:SetActive(true)
					RoleCtrl.Instance:ReleaseDelayExp()
					RoleData.Instance:ReleaseNeedDelayItem()
				end
			end, 2)
		end, 0.5)
		self.data.state = OfflineRewardState.Default
	else
		self:SetBallsActive(self.now_ball_num)
		self:SetBallsEffect(true, self.now_ball_num)
		self:FlushAll()
		TimeHelper:CancelTimer(self.stay_handler)
		self.ObjEffectStay:SetActive(true)
	end
end

function OfflineRewardPanel:OpenSuccessTip()
	self:FlushSuccessTip()
	self.ObjSuccessTip:SetActive(true)
	self.data.state = OfflineRewardState.Default
end

function OfflineRewardPanel:CloseSuccessTip()
	self.ObjSuccessTip:SetActive(false)
	FightData.Instance:SetFightSoulRewardData({})
	self:OnFightClick()
end

function OfflineRewardPanel:FlushAll()
	self:FlushBySeq()
	self:FlushByCumulationExp()
end

-- 设置所有球是否置灰
function OfflineRewardPanel:SetBallsIHandler(value)
	for i = 1, self.BallList:Length() do
		self.BallList[i]:SetIHandler(value)
	end
end

-- 设置前几个的特效的value
function OfflineRewardPanel:SetBallsEffect(value, num)
	for i = 1, self.BallList:Length() do
		local v = value
		if i > num then
			v = not value
		end
		self.BallList[i]:SetObjEffect(v)
	end
end

-- 设置激活了几个球
function OfflineRewardPanel:SetBallsActive(num)
	for i = 1, self.BallList:Length() do
		self.BallList[i]:SetObjActive(num >= i)
	end
end

function OfflineRewardPanel:FlushSuccessTip()
	local last_config = self.break_config[self.data.seq - 1] or {}
	UH.SetText(self.TxtTipNameLeft, last_config.break_name or self.language.ZeroBreakName)
	UH.SetText(self.TxtTipNameRight, self.now_config.break_name or self.language.ZeroBreakName)
	-- UH.SetText(self.TxtTipExpLeft, Format(self.language.SuccessTipExp, (last_config.player_exp_time_s or 0) * self.rate))
	-- UH.SetText(self.TxtTipExpRight, Format(self.language.SuccessTipExp, (self.now_config.player_exp_time_s or 0) * self.rate))
	UH.SetText(self.TxtTipCoinLeft, Format(self.language.SuccessTipCoin, (last_config.player_coin_time_s or 0) * self.rate))
	UH.SetText(self.TxtTipCoinRight, Format(self.language.SuccessTipCoin, (self.now_config.player_coin_time_s or 0) * self.rate))
	self.TipRewardItemList:SetData(DataHelper.FormatItemList(FightData.Instance:GetFightSoulRewardData()))
	local add_exp = (self.now_config.player_exp_time_s or 0) * self.rate - (last_config.player_exp_time_s or 0) * self.rate
	-- self.ObjAddExp:SetActive(add_exp > 0)
	if add_exp > 0 then
		UH.SetText(self.TxtAddExp, add_exp)
	end
	local add_coin = (self.now_config.player_coin_time_s or 0) * self.rate - (last_config.player_coin_time_s or 0) * self.rate
	self.ObjAddCoin:SetActive(add_coin > 0)
	if add_coin > 0 then
		UH.SetText(self.TxtAddCoin, add_coin)
	end
end

function OfflineRewardPanel:FlushBySeq()
	self.now_config = self.break_config[self.data.seq] or {}
	if not self.is_need_flush then
		return
	end
	-- 防止重复刷新
	local _next_seq = self.data.seq + 1
	if _next_seq == self.next_seq then
		return
	end
	self.next_seq = self.data.seq + 1
	local next_config = self.break_config[self.next_seq] or {}
	UH.SetText(self.TxtBreakName, self.now_config.break_name or self.language.ZeroBreakName)
	UH.SetText(self.TxtNameLeft, self.now_config.break_name or self.language.ZeroBreakName)
	UH.SetText(self.TxtNameRight, next_config.break_name or self.language.ZeroBreakName)
	UH.SetText(self.TxtExpLeft, Format(self.language.ExpUp, (self.now_config.player_exp_time_s or 0) * self.rate))
	UH.SetText(self.TxtExpRight, Format(self.language.ExpUp, (next_config.player_exp_time_s or 0) * self.rate))
	UH.SetText(self.TxtCoinLeft, Format(self.language.CoinUp, (self.now_config.player_coin_time_s or 0) * self.rate))
	UH.SetText(self.TxtCoinRight, Format(self.language.CoinUp, (next_config.player_coin_time_s or 0) * self.rate))

	if not TableIsEmpty(self.now_config) then
		self.CurStageItemIcon:SetObjActive(true)
		UH.SetIcon(self.CurStageItemIcon, Item.GetIconId(self.now_config.lixian_item_hours), ICON_TYPE.ITEM)
		UH.SetText(self.CurStageItemNum, Format(self.language.ItemCount, self.now_config.lixian_item_hours_num))
	else
		self.CurStageItemIcon:SetObjActive(false)
	end
	UH.SetIcon(self.NextStageItemIcon, Item.GetIconId(next_config.lixian_item_hours), ICON_TYPE.ITEM)
	UH.SetText(self.NextStageItemNum, Format(self.language.ItemCount, next_config.lixian_item_hours_num))

	self.ObjRightNext:SetActive(not self.data.is_max_level)
	-- 刷新小球显示
	local ball_num = 0
	if self.data.seq ~= 0 then
		ball_num = self.data.seq % self.other.break_num == 0 and self.other.break_num or self.data.seq % self.other.break_num
	end
	self:SetBallsActive(ball_num)
	self:SetBallsEffect(true, ball_num)
end

function OfflineRewardPanel:FlushByCumulationExp()
	UH.SetText(self.TxtCumulationTime, RoleData.Instance:BreakTimeDesc())
	self.RewardRedPoint:SetNum(self.data.hour >= Config.break_cfg_auto.seq_time[1].end_time and 1 or 0)
	self:FlushBranch()
	self:FlushByCumulationItem()
end

function OfflineRewardPanel:FlushByCumulationItem()
	local t = {}
	table.insert(t, {item_id = CommonItemId.Exp, num = self.data.cumulation_exp, is_bind = 0, is_grey = self.data.cumulation_exp == 0})
	table.insert(t, {item_id = CommonItemId.Coin, num = self.data.cumulation_coin, is_bind = 0, is_grey = self.data.cumulation_coin == 0})
	local item_config = RoleData.Instance:GetBreakCanGetItem()

	local item_data
	if not TableIsEmpty(self.now_config) then
		local item_id = self.now_config.lixian_item_hours
		item_data = {item_id = item_id, is_bind = 1, num = 0, is_grey = true}
	end

	for k, v in pairs(self.data.cumulation_item) do
		for l, w in pairs(item_config) do
			if v.item_id == w.item_id then
				w.num = w.num + v.num
				w.is_grey = false
				break
			end
		end
		if item_data then
			if v.item_id == item_data.item_id then
				item_data.num = item_data.num + v.num
				item_data.is_grey = false
			end
		end
	end
	if item_data then
		table.insert(t, item_data)
	end
	for k, v in pairs(item_config) do
		table.insert(t, v)
	end
	self.CumulationItemList:SetData(DataHelper.FormatItemList(t))
end

function OfflineRewardPanel:FlushBranch()
	self.now_branch_count = 1
	local len_seq_time = #self.seq_time
	for i, v in ipairs(self.seq_time) do
		if self.data.hour >= v.end_time and i ~= len_seq_time then
			self.now_branch_count = self.now_branch_count + 1
		else
			break
		end
	end
end

-- 飘飞球特效
function OfflineRewardPanel:UseBall(obj, end_pos, start_dir, end_dir, total_step, speed, is_straight, func)
	if not self.flying_balls[self.now_ball] then
		local clone = self.ObjFlyingBall:Clone(self.ObjFlyingBalls)
		clone.name = self.now_ball
		self.flying_balls[self.now_ball] = clone:GetLuaComponent("OfflineRewardFlyingBall")
	end
	end_pos = end_pos or self.end_pos
	start_dir = start_dir or Vector3.New(1, 1, 0)
	end_dir = end_dir or Vector3.New(1, -1, 0)
	total_step = total_step or 15
	speed = speed or 0.01
	self.flying_balls[self.now_ball]:ResetBall(UH.GetRootPosition(obj), end_pos, start_dir, end_dir, total_step, speed, is_straight, func)
	self.now_ball = self.now_ball % self.ball_max + 1
end

-- 飘字
function OfflineRewardPanel:FlyText(num)
	if num == 0 then
		return
	end
	if not self.flying_texts[self.now_text] then
		local clone = self.ObjFlyingText:Clone(self.ObjFlyingTexts)
		clone.name = self.now_text
		self.flying_texts[self.now_text] = clone:GetLuaComponent("OfflineRewardText")
	end
	self.flying_texts[self.now_text]:ResetText(num)
	self.now_text = self.now_text % self.text_max + 1
end

-- 领取经验
function OfflineRewardPanel:ReceiveExp()
	if self.data.cumulation_exp > 0 or self.data.cumulation_coin > 0 or next(self.data.cumulation_item) ~= nil then
		if self.data.cumulation_exp > 0 then
			TimeHelper:CancelTimer(self.exp_ball_handler)
			self.exp_ball_handler = TimeHelper:AddRunTimer(function()
				local end_pos = self.MainViewExpMono:GetExpPos()
				self:UseBall(self.FlyExpStart, end_pos, nil, nil, nil, 30, true, BindTool.Bind(self.FlyExpEnd, self, end_pos))
			end, 0.2, self.now_branch_count, false)
		end
		-- 领取完后累计经验手动清零，防止连点
		self.data.cumulation_exp = 0
		self.data.cumulation_coin = 0
		self.data.cumulation_item = {}
		-- 领取时滑动到最上面
		self.ScrollRectRewardList:StopMovement()
		UH.AnchoredPosition(self.ObjCumulationItemList, Vector2.New(-150, 0))
		RoleCtrl.Instance:SendOfflineBreak(2)
	else
		PublicPopupCtrl.Instance:Center(self.language.HaveNotExp)
	end
end

function OfflineRewardPanel:FlyExpEnd(end_pos)
	UH.LocalPosG(self.ObjFlyExpEnd, end_pos)
	if self.fly_exp_end_handler ~= nil then
		self.EffectTool:Stop(self.fly_exp_end_handler)
	end
	self.fly_exp_end_handler = self.EffectTool:Play(10010266)
end

function OfflineRewardPanel:OnFightClick()
	ViewMgr:OpenView(FightSoulView)
end

function OfflineRewardPanel:FlushBySoulInfo()
	local value, cap = self.fight_data:GetFightSoulCapShow()
	self.SoulCapObj:SetActive(value)
	if value then
		local sum_cap = RoleData.Instance:GetSumScore()
		UH.SetText(self.SoulCap, Format(self.language.SoulCap, RichTextHelper.ColorStr(sum_cap, sum_cap >= cap and COLORSTR.Green4 or COLORSTR.Red8), cap))
	end
	self:FlushBtnFightRemind()
end

function OfflineRewardPanel:FlushBtnFightRemind()
	self.BtnFightRedPoint:SetNum(FightData.Instance:GetFightSoulRemind())
	local can_fight_num, can_fight_total_num = self.fight_data:GetCanFightAndTotal()
	-- local color = can_fight_num > 0 and COLORSTR.Green4 or COLORSTR.Red8
	local color = COLORSTR.Green4
	UH.SetText(self.TxtBattledCount, Format(self.language.BattledCount, RichTextHelper.ColorStr(can_fight_num, color), can_fight_total_num))
end

function OfflineRewardPanel:OnCurIconClick()
	local item_id = self.now_config.lixian_item_hours
	if item_id then
		ItemInfoCtrl.Instance:ShowItemInfo(item_id)
	end
end

function OfflineRewardPanel:OnNextIconClick()
	local next_config = self.break_config[self.next_seq] or {}
	local item_id = next_config.lixian_item_hours
	if item_id then
		ItemInfoCtrl.Instance:ShowItemInfo(item_id)
	end
end


--------------OfflineRewardItem--------------没用了
OfflineRewardItem = OfflineRewardItem or DeclareMono("OfflineRewardItem", UIWidgetBaseItem)
function OfflineRewardItem:OfflineRewardItem()
	self.language = Language.Role.Break
	self.break_info = RoleData.Instance.break_info
end

function OfflineRewardItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local item = Item.New({item_id = self.data.break_item_id})
	self.ItemCell:SetData(item)
	UH.SetText(self.TxtName, item:QuaName())
	UH.SetText(self.TxtDesc, Format(self.language.ItemDesc, self.data.exp))
	UH.SetText(self.TxtCount, Format(self.language.ItemCount, Item.GetNum(self.data.break_item_id)))
end

function OfflineRewardItem:OnClick()
	if Item.GetNum(self.data.break_item_id) > 0 then
		if self.break_info.is_max_level and self.break_info.is_max_exp then
			PublicPopupCtrl.Instance:Center(self.language.LevelMaxTip)
			return
		end
		self.break_info.use_id = self.data.seq
		self.break_info.use_flag = not self.break_info.use_flag
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemGet)
		BagCtrl.Instance:SendUseSpecialLogicItem(UseSpecialLogicItem.BreakItem, self.data.break_item_id)
	else
		ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.data.break_item_id})})
	end
end

function OfflineRewardItem:FlushCount()
	UH.SetText(self.TxtCount, Format(self.language.ItemCount, Item.GetNum(self.data.break_item_id)))
end


--------------OfflineRewardBallItem--------------
OfflineRewardBallItem = OfflineRewardBallItem or DeclareMono("OfflineRewardBallItem", UIWidgetBaseItem)
function OfflineRewardBallItem:SetIHandler(value)
	self.IHandler.Interactable = value
end

function OfflineRewardBallItem:SetObjActive(value)
	self.ObjActive:SetActive(value)
end

function OfflineRewardBallItem:SetObjEffect(value)
	self.ObjEffect:SetActive(value)
end

function OfflineRewardBallItem:SetAnimTrigger()
	self.ObjActive:SetActive(true)
	self.Anim:SetTrigger("light")
end


--------------OfflineRewardFlyingBall--------------
OfflineRewardFlyingBall = OfflineRewardFlyingBall or DeclareMono("OfflineRewardFlyingBall", UIWidgetBaseItem)
function OfflineRewardFlyingBall:ResetBall(start_pos, end_pos, start_dir, end_dir, total_step, speed, is_straight, func)
	TimeHelper:CancelTimer(self.t_handler)
	if self.curve then
		self.curve = nil
	end
	self.step = 0
	self.total_step = total_step
	self.speed = speed
	self.func = func
	UH.LocalPosG(self.ObjSelf, start_pos)
	self.ObjSelf:SetActive(true)
	if not is_straight then
		self.curve = Game.CurveParabola(start_pos, start_dir, end_pos, end_dir)
		self.t_handler = TimeHelper:AddRunTimer(BindTool.Bind(self.DoCurve, self), self.speed, self.total_step + 1)
	else
		Runner.Instance:RemoveRunUntilTrue(self.straight_handler)
		self.straight_handler = UH.MoveToTarget(self.ObjSelf, end_pos, self.speed, BindTool.Bind(self.DoStraightEnd, self))
	end
end

function OfflineRewardFlyingBall:DoCurve()
	if self.step > self.total_step then
		TimeHelper:CancelTimer(self.t_handler)
		self.ObjSelf:SetActive(false)
		self.e_handler = nil
		self.curve = nil
		return
	end
	UH.LocalPosG(self.ObjSelf, self.curve:GetPos(self.step / self.total_step))
	self.step = self.step + 1
end

function OfflineRewardFlyingBall:DoStraightEnd()
	self.ObjSelf:SetActive(false)
	if self.func ~= nil then
		self.func()
	end
	Runner.Instance:RemoveRunUntilTrue(self.straight_handler)
end

function OfflineRewardFlyingBall:OnDestroy()
	TimeHelper:CancelTimer(self.t_handler)
	Runner.Instance:RemoveRunUntilTrue(self.straight_handler)
end


--------------OfflineRewardText--------------没用了
OfflineRewardText = OfflineRewardText or DeclareMono("OfflineRewardText", UIWidgetBaseItem)
function OfflineRewardText:ResetText(num)
	local str = num >= 0 and "+" .. num or num
	UH.SetText(self.TxtFly, Format(Language.Role.Break.BreakUp, str))
	self:StartFly()
end

function OfflineRewardText:StartFly()
	self.ObjSelf:SetActive(true)
	TimeHelper:CancelTimer(self.move_handler)
	self.move_handler = TimeHelper:AddDelayTimer(function()
		self.ObjSelf:SetActive(false)
	end, 1.2)
end

function OfflineRewardText:OnDestroy()
	TimeHelper:CancelTimer(self.move_handler)
end


--------------BranchItem--------------没用了
BranchItem = BranchItem or DeclareMono("BranchItem", UIWidgetBaseItem)
function BranchItem:SetBranchActive(value)
	self.ObjBranch:SetActive(value)
end

function BranchItem:SetFruitActive(value)
	self.ObjFruit:SetActive(value)
end
