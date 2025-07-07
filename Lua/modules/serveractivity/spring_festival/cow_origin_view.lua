CowOriginCtrlType = {
    RequestInfo = 0, 	-- 请求信息
    UseDice = 1,		-- 使用骰子
    ShowEnd = 2, 	-- 展示结束
    SpecialPlay = 3,    -- 特殊玩法
    Answer = 4,  		-- 答题(param_1 = 答案id)
    ConvertItem = 5,    -- 兑换(param_1 = seq)
}
CowOriginItemType = {
	Fight = 1, 			-- 打怪
	Advance = 2, 		-- 前进
	Answer = 3, 		-- 答题
	Null = 4, 			-- 空
	Gift = 5, 			-- 礼包
	End = 6, 			-- 结束
}

CowOriginView = CowOriginView or DeclareView("CowOriginView","serveractivity/spring_festival/cow_origin_view",Mod.SpringFestival.CowOrigin)
function CowOriginView:CowOriginView()
	-- 牛运当头牛运兑换取消在一个界面了，这里屏蔽了
	-- self.tab_states = {}
	-- self.tab_states[1] = ActivityData.IsOpen(ACTIVITY_TYPE.RAND_COW_ORIGIN)
	-- self.tab_states[2] = ActivityData.IsOpen(ACTIVITY_TYPE.RAND_COW_CONVERT)
	-- self.ToggleList[1].gameObject:SetActive(self.tab_states[1])
	-- self.ToggleList[2].gameObject:SetActive(self.tab_states[2])
	-- self.TabImg2Obj:SetActive(self.tab_states[2] and self.tab_states[1])
	-- local tab_img_1_sprite = (self.tab_states[2] and self.tab_states[1]) and "ZongZiHeXian" or "LiuSu"
	-- UH.SpriteName(self.TabImg1,tab_img_1_sprite)
	-- UH.SetText(self.Tab1Text,ActivityRandData.GetName(ACTIVITY_TYPE.RAND_COW_ORIGIN))
	-- UH.SetText(self.Tab2Text,ActivityRandData.GetName(ACTIVITY_TYPE.RAND_COW_CONVERT))
	ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COW_ORIGIN,CowOriginCtrlType.RequestInfo)
end

-- 默认打开牛运当头
function CowOriginView:LoadCallback(param_t)
	-- self.Currency:DefaultSet()
	-- param_t = param_t or {}
	-- local tab_index = param_t.mod_key == Mod.SpringFestival.CowConvert and 2 or 1
	-- self:HeadleTab(1)
end

function CowOriginView:CloseCallback()
	-- body
end

function CowOriginView:HeadleTab(tab_index)
	if self.tab_states[tab_index] == false then
		tab_index = tab_index == 1 and 2 or 1
	end
	self.ToggleList[tab_index].isOn = true
end

function CowOriginView:OnCloseClick()
	ViewMgr:CloseView(CowOriginView)
end

function CowOriginView:OnAddScoreClick()
	-- self:HeadleTab(1)
end

----------------- CowOriginPanel -----------------
CowOriginPanel = CowOriginPanel or DeclareMono("CowOriginPanel",UIWFlushPanel)
function CowOriginPanel:CowOriginPanel()
	self.data = ServerActivityData.Instance
	self.data_cares = {
		{data = self.data.cow_origin_data, func = self.FlushMap, init_call = false , keys = {"map_info"}},
		{data = BagData.Instance.item_grid, func = self.FlushRemainCount, init_call = false},
		{data = self.data.cow_origin_data, func = self.FlushCurCell, init_call = false , keys = {"question_id"}},
    }
    self.go_num = 0
    self.is_handle_rand_id = false -- 是否处理过击鼓
    self.last_map_id = nil
    self.last_map_index = nil      -- 上一个格子--服务端的old_map_index 走完会清空掉。。。。。。
    self.max_item_count = 23

	-- local jump_cfg = ServerActivityCtrl.Instance:ActGetWayCfgById(ACTIVITY_TYPE.RAND_COW_ORIGIN)
	-- if jump_cfg ~= nil then
	-- 	self.consume_id = jump_cfg.item_id
	-- end
end

function CowOriginPanel:Awake()
	UIWFlushPanel.Awake(self)
	self:FlushPanel()
    UH.SetText(self.TitleText, ActivityRandData.GetName(ACTIVITY_TYPE.RAND_COW_ORIGIN))

	if self.consume_id ~= nil then
		UH.SetIcon(self.ItemIcon, Item.GetIconId(self.consume_id), ICON_TYPE.ITEM)
	end
end

function CowOriginPanel:FlushPanel()
	self:FlushMap()
	self:FlushRemainCount()
end

function CowOriginPanel:FlushMap()
	self.is_handle_rand_id = false
	self.map_data = self.data:GetCowOriginMapData() 
	self.map_id = self.data:GetCowOriginMapId()
	-- 击完鼓和换完图在刷新 
	if not self:IsChangingMap() and self.map_data.rand_point == 0 then
		self:InitMap()
	end
	self:HandleDice(self.map_data.rand_point)

	-- 处理前进的格子
	if self:IsNeedGo() and self.map_data.rand_point == 0 then
		self:HandlePoint(self.map_data.old_map_index,self.map_data.now_map_index)
	end
end

-- 刷新当前一个格子的行为
function CowOriginPanel:FlushCurCell()
	if self:IsCanRandPoint() or self:IsNeedGo() then
		-- 当前格子
		self.cur_map_cell = self.map_cell_list[self.map_data.old_map_index]
		self:SetRolePointPos(self.map_data.old_map_index)
	else
		self.cur_map_cell = self.map_cell_list[self.map_data.now_map_index]
		self:SetRolePointPos(self.map_data.now_map_index)
	end
	if self.map_data.has_play_special_plan == 0 and not self:IsCanRandPoint() then
		self.cur_map_cell:SetIconCanvas()
		if self.cur_map_cell.data.layer_number_type ~= CowOriginItemType.Fight then
			self.cur_map_cell:Behaviour()
		end
	end
	TimeHelper:CancelTimer(self.role_ht)
	self.role_ht = Invoke(function ()
		self:HandleRole()
	end,0.2)
end

function CowOriginPanel:FlushRemainCount()
	local is_remind = ServerActivityData.Instance:GetCowOriginTodayRemind()
	self.RedPoint:SetNum(self.data:CowOriginConvertRemindNum() + is_remind)
	UH.SetText(self.RemainNum,self.data:GetCowOriginUseCount())
	if self.consume_id ~= nil then
		UH.SetText(self.ItemText, Item.GetNum(self.consume_id))
	end
end

function CowOriginPanel:InitMap()
	self.last_map_id = self.map_id
	self.Map1:SetActive(self.map_id == 1)
	self.Map2:SetActive(self.map_id == 2)
	self:InitRoleParent()
	self.map_cell_list = self["MapCells" .. self.map_id]
	self.map_cell_list_data = self.data:GetCowOriginMapCellList()
	self.max_item_count = self.data:GetCowOriginItemMaxCount()
	for i = 1 , self.map_cell_list:Length() do
		self.map_cell_list[i]:SetData(self.map_cell_list_data[i])
	end
	self:FlushCurCell()
end

function CowOriginPanel:InitRoleParent()
	if self.map_id ==  1 then
		self.RoleTrans:SetParent(self.Map1CellsTrans)
	else
		self.RoleTrans:SetParent(self.Map2CellsTrans)
	end
end

-- 是否可击鼓
function CowOriginPanel:IsCanRandPoint()
	if self.map_data.rand_point == 0 or self.is_handle_rand_id == true then
		return false
	else
		return true
	end
end

-- 是否在换图中
function CowOriginPanel:IsChangingMap()
	if self.last_map_id == nil or self.map_id == self.last_map_id then
		return false
	else
		return true
	end
end

-- 是否需要前进
function CowOriginPanel:IsNeedGo()
	if self.last_map_index ~= nil and self.go_num > 0 and self.map_data.rand_point == 0 and self.map_data.now_map_index == self.go_num + self.last_map_index then
		return true
	else
		return false
	end
end

function CowOriginPanel:SetRolePointPos(point)
	point = (point == nil or point < 1 or point > self.max_item_count) and 1 or point
	self.RoleTrans.gameObject:SetLocalPosition(self.map_cell_list[point].gameObject:GetLocalPosition())
end

-- 处理服务端下发的骰子的值
function CowOriginPanel:HandleDice(num)
	if num < 1 then
		return
	end
	self.is_handle_rand_id = true
	self.DiceMaskObj:SetActive(true)
	self:ShowDiceEffect(num,function ()
		if self:IsChangingMap() then
			self:HandlePoint(self.map_data.old_map_index,self.max_item_count)
		else
			self:HandlePoint(self.map_data.old_map_index,self.map_data.now_map_index)
		end
	end)
end

-- 骰子特效
function CowOriginPanel:ShowDiceEffect(num,func)
	self.dice_step_count = 8 + num
	self.dice_num = 0
	self.dice_step_num = 1
	Runner.Instance:CancelRepeat(self.run_handle)
	self.run_handle = Runner.Instance:RepeatCall(BindTool.Bind(self.diceEffect,self,num,func),0.1)
end

function CowOriginPanel:diceEffect(num,func)
	if self.dice_step_count == 0 then
		Runner.Instance:CancelRepeat(self.run_handle)
		func()
		return
	end
	self.dice_step_count = self.dice_step_count - 1
	self.dice_num = self.dice_num + self.dice_step_num
	if self.dice_num >= 3 then
		self.dice_step_num = -1
	elseif self.dice_num <= 1 then
		self.dice_step_num = 1
	end
	self.DiceNums[self.dice_num].isOn = true
end

function CowOriginPanel:HandlePoint(old_point,now_point)
	local rand_point = self.map_data.rand_point
	rand_point = rand_point > 0 and rand_point or self.go_num
	if rand_point == 0 then
		return
	end
	if old_point == nil or now_point == nil then
		return
	end
	old_point = (old_point < 1 or old_point > self.max_item_count) and 1 or old_point
	now_point = (now_point < 1 or old_point > self.max_item_count) and 1 or now_point
	if old_point == now_point or now_point < old_point then
		self.DiceMaskObj:SetActive(false)
		return
	end
	self.go_num = 0
	self.point_list = {}
	local spill_num = (now_point - old_point) - rand_point
	local end_point = (old_point + rand_point) > self.max_item_count and self.max_item_count or (old_point + rand_point)
	table.insert(self.point_list,{origin_point = old_point,end_point = end_point})
	-- 因为活动如果走到前进格子，可额外前进1格，因为是1格，所以origin_point 与 end_point 的差值固定是1
	for i = 1, spill_num do
		end_point = (old_point + rand_point + i) > self.max_item_count and self.max_item_count or (old_point + rand_point + i)
		table.insert(self.point_list,{origin_point = (old_point + rand_point + i - 1),end_point = (old_point + rand_point + i)})
	end
	self.RoleCanvas.overrideSorting = true
	self:RoleMove(self.point_list[1].origin_point,self.point_list[1].end_point,BindTool.Bind(self.MoveEndCallback,self,1))
end

function CowOriginPanel:RoleMove(origin_point,end_point,end_func)
    -- 玩法上只允许前进
	if origin_point >= end_point then
		return
	end
	self.pos_list = {}
	for i = origin_point,end_point - 1 do
		local pos = {}
		pos.origin_pos = self.map_cell_list[i].gameObject:GetLocalPosition()
		pos.end_pos = self.map_cell_list[i + 1].gameObject:GetLocalPosition()
		table.insert(self.pos_list,pos)
	end
	self:StepMove(0,end_func)
end

function CowOriginPanel:StepMove(n,end_func)
	n = n + 1
	if n == #self.pos_list then
		self:Move(self.pos_list[n].origin_pos,self.pos_list[n].end_pos,end_func)
	else
		self:Move(self.pos_list[n].origin_pos,self.pos_list[n].end_pos,BindTool.Bind(self.StepMove,self,n,end_func))
	end
end

function CowOriginPanel:Move(origin_pos,end_pos,func)
	self.RoleTrans.gameObject:SetLocalPosition(origin_pos)
	self.move_step_count = 20 --等同于小不点行走的速度
	local step_pos = Vector3.New((end_pos.x - origin_pos.x)/self.move_step_count,(end_pos.y - origin_pos.y)/self.move_step_count,0)
	TimeHelper:CancelTimer(self.time_handle)
	self.time_handle = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.move,self,step_pos,func),1,self.move_step_count)
end

function CowOriginPanel:move(step_pos,func)
	self.move_step_count = self.move_step_count - 1
	if self.move_step_count == 0 and func then
		func()
	end
	local role_pos = self.RoleTrans.gameObject:GetLocalPosition()
	self.RoleTrans.gameObject:SetLocalPosition(Vector3.New(role_pos.x + step_pos.x,role_pos.y + step_pos.y,0))
end

-- 这个也递归下，n 最开始等于 1 
function CowOriginPanel:MoveEndCallback(n,end_point)
	local end_point = self.point_list[n].end_point
    local item_mono = self.map_cell_list[end_point]
    self.last_map_index = end_point
    item_mono:SetIconCanvas()
    -- 除了打怪，其它的走到格子自动触发行为
    if item_mono.data.layer_number_type ~= CowOriginItemType.Fight then
		item_mono:Behaviour()
	end
	self.go_num = 0
	if item_mono.data.layer_number_type == CowOriginItemType.Advance then
		self.go_num = item_mono.data.parameter
	end

	if item_mono.data.layer_number_type == CowOriginItemType.End then
		self.map_data.old_map_index = 1
		self.last_map_id = self.map_id
	end

	n = n + 1
	-- 这里是彻底走完了
	if n > #self.point_list then
		self.map_data.rand_point = 0
		self:FlushPanel()
		self.DiceMaskObj:SetActive(false)
		--self.RoleCanvas.overrideSorting = false
		self:HandleRole(item_mono.data)
		return
	else
		-- 目前只有走到前进功能的格子才会触发这个，目的仅为中途执行特殊操作（停顿一秒）
		TimeHelper:CancelTimer(self.invoke_handle)
		self.invoke_handle = Invoke(function ()
			self:RoleMove(self.point_list[n].origin_point,self.point_list[n].end_point,BindTool.Bind(self.MoveEndCallback,self,n))
		end,1)
	end
end

function CowOriginPanel:OnTipClick()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[92].desc})
end

function CowOriginPanel:OnAddClick()
	local is_remind = ServerActivityData.Instance:GetCowOriginTodayRemind()
	if is_remind == 1 then
		ServerActivityData.Instance:SaveCowOriginTodayRemind()
		self.RedPoint:SetNum(self.data:CowOriginConvertRemindNum())
	end
	ViewMgr:OpenView(CowOriginConvertView)
end

function CowOriginPanel:OnPlayClick()
	if self.play_effect then
		self.EffectTool:Stop(self.play_effect)
	end
	self.play_effect = self.EffectTool:Play("100108173")
	self.DiceAnima:Play("play_gu")
	-- 还有特殊玩法没有执行
	if self.map_data.has_play_special_plan == 0 and not self:IsCanRandPoint() and 
		(self.cur_map_cell.data.layer_number_type == CowOriginItemType.Fight or self.cur_map_cell.data.layer_number_type == CowOriginItemType.Answer) then
		if self.cur_map_cell.data.layer_number_type == CowOriginItemType.Fight then
			PublicPopupCtrl.Instance:Center(Language.CowOrigin.PlayTip2) -- "请先通过战斗玩法"
		elseif self.cur_map_cell.data.layer_number_type == CowOriginItemType.Answer then
			PublicPopupCtrl.Instance:Center(Language.CowOrigin.PlayTip1) -- "请先通过答题玩法"
		end
		return
	end
	if self.data:GetCowOriginUseCount() > 0 then
		ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COW_ORIGIN,CowOriginCtrlType.UseDice)
	else
		PublicPopupCtrl.Instance:Center(Language.CowOrigin.PlayTip3) -- "次数用尽，请购买次数"
	end
end

function CowOriginPanel:OnDisable()
	if self.play_effect then
		self.EffectTool:Stop(self.play_effect)
	end
	UIWFlushPanel.OnDisable(self)
end

function CowOriginPanel:OnDestroy()
	self.go_num = 0
	self.data.cow_origin_data.map_info.rand_point = 0
	self.data.cow_origin_data.question_id = nil
	Runner.Instance:CancelRepeat(self.run_handle)
	TimeHelper:CancelTimer(self.time_handle)
	TimeHelper:CancelTimer(self.invoke_handle)
	TimeHelper:CancelTimer(self.role_ht)
	UIWFlushPanel.OnDestroy(self)
end

-- 处理role的canvas
function CowOriginPanel:HandleRole(cur_data)
	local cur_data = cur_data or self.cur_map_cell.data
	local is_played = ServerActivityData.Instance:IsPlayed(cur_data)
	self.RoleCanvas.overrideSorting = is_played
end

function CowOriginPanel:OnClickGetItem()
	if PlanActivityCtrl.Instance:ActGetWayViewById(ACTIVITY_TYPE.RAND_COW_ORIGIN) then
        ViewMgr:CloseView(CowOriginView)
    end
end

----------------- CowOriginItem ------------------
CowOriginItem = CowOriginItem or DeclareMono("CowOriginItem",UIWidgetBaseItem)
function CowOriginItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	self:ShowIocn()
	self:SetBg()
	self:SetIconCanvas()
end

function CowOriginItem:ShowIocn()
	local is_played = self:IsPlayed()
	if is_played then
		for i = 1 , self.IconObjs:Length() do
			self.IconObjs[i]:SetActive(i == self.data.layer_number_type)
		end
		for i = 1 , self.IconEffects:Length() do
			self.IconEffects[i]:SetActive(false)
		end
	else
		for i = 1 , self.IconObjs:Length() do
			if i == CowOriginItemType.Gift and i == self.data.layer_number_type then
				self.IconObjs[i]:SetActive(true)
			else
				self.IconObjs[i]:SetActive(false)
			end
		end
		for i = 1 , self.IconEffects:Length() do
			self.IconEffects[i]:SetActive(i == self.data.layer_number_type)
		end
	end
end

function CowOriginItem:SetBg()
	if self.data.layer_number_type == CowOriginItemType.Fight then
		UH.SpriteName(self.DiImg,"HongSeDi")
	elseif self.data.layer_number_type == CowOriginItemType.Advance then
		UH.SpriteName(self.DiImg,"LvSeDi")
	elseif self.data.layer_number_type == CowOriginItemType.Gift then
		UH.SpriteName(self.DiImg,"JinSeDi")
	elseif self.data.layer_number_type == CowOriginItemType.End then
		UH.SpriteName(self.DiImg,"HuangSeDi")
	else
		UH.SpriteName(self.DiImg,"PuTongDi")
	end
end

function CowOriginItem:Behaviour()
	if self.data.layer_number_type == CowOriginItemType.Fight then
		self:Fight()
	elseif self.data.layer_number_type == CowOriginItemType.Advance then
		self:Advance()
	elseif self.data.layer_number_type == CowOriginItemType.Answer then
		self:Answer()
	elseif self.data.layer_number_type == CowOriginItemType.Null then
		self:Common()
	elseif self.data.layer_number_type == CowOriginItemType.Gift then
		self:Gift()
	else
		self:Common()
	end
end

function CowOriginItem:ReqSpecialPlay()
	ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COW_ORIGIN,CowOriginCtrlType.SpecialPlay)
end

function CowOriginItem:ReqShowEnd()
	ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COW_ORIGIN,CowOriginCtrlType.ShowEnd)
end

function CowOriginItem:SetIconCanvas()
	-- 玩过了的置灰
	local is_played = self:IsPlayed()
	self.Interactor.Interactable = not is_played
end

function CowOriginItem:IsPlayed()
	local is_played = ServerActivityData.Instance:IsPlayed(self.data)
	return is_played
end

function CowOriginItem:Fight()
	local map_data = ServerActivityData.Instance:GetCowOriginMapData()
	if map_data.now_map_index - 1 == self.data.seq then
		if TeamData.Instance:InTeam() then
			PublicPopupCtrl.Instance:Center(Language.CowOrigin.TeamTip)
			return
		end
		self:ReqSpecialPlay()
		ViewMgr:CloseView(CowOriginView)
	end
end

function CowOriginItem:Answer()
	if TeamData.Instance:InTeam() then
		PublicPopupCtrl.Instance:Center(Language.CowOrigin.TeamTip2)
		return
	end
	local param_t = {}
	local question_id = ServerActivityData.Instance:GetCowOriginAnswerData()
	if question_id == nil then
		self:ReqSpecialPlay()
		return
	end
	param_t.answer_id = question_id
	param_t.end_func = function (data)
		ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COW_ORIGIN,CowOriginCtrlType.Answer,data.seq)
		ServerActivityData.Instance.cow_origin_data.question_id = nil
		ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COW_ORIGIN,CowOriginCtrlType.RequestInfo)
		ViewMgr:CloseView(CowOriginAnswerView)
	end
	ViewMgr:OpenView(CowOriginAnswerView,param_t)
end

function CowOriginItem:Gift()
	self:ReqSpecialPlay()
	ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COW_ORIGIN,CowOriginCtrlType.RequestInfo)
end

-- 停顿一秒后前进
function CowOriginItem:Advance()
	TimeHelper:CancelTimer(self.time_handle)
	self.time_handle = Invoke(function ()
		self:Common()
	end,1)
end

function CowOriginItem:Common()
	local map_data = ServerActivityData.Instance:GetCowOriginMapData()
	if map_data.now_map_index - 1 == self.data.seq then
		self:ReqShowEnd()
	end
end

function CowOriginItem:OnDownClick()
	self:Behaviour()
end

function CowOriginItem:OnDestroy()
	TimeHelper:CancelTimer(self.time_handle)
	UIWidgetBaseItem.OnDestroy(self)
end

----------------- CowConvertPanel 牛运兑换界面 ----------------
CowConvertPanel = CowConvertPanel or DeclareMono("CowConvertPanel",UIWFlushPanel)

function CowConvertPanel:CowConvertPanel()
	self.data = ServerActivityData.Instance
	self.data_cares = {
		{data = self.data.cow_convert_data, func = self.FlushPanel, init_call = false},
    }
end

function CowConvertPanel:Awake()
	UIWFlushPanel.Awake(self)
end

function CowConvertPanel:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self:FlushPanel()
end

function CowConvertPanel:FlushPanel()
	local list_data = self.data:GetCowConvertList()
	UH.SetText(self.ScoreNum,self.data:GetCowConvertData().score)
	self.List:SetData(list_data)
end

function CowConvertPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
end

CowConvertItem = CowConvertItem or DeclareMono("CowConvertItem",UIWidgetBaseItem)
function CowConvertItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	local item = Item.Create(data.reward_item)
	UH.SetText(self.Name,item:Name())
	self.ItemCell:SetData(item)
	local limit_str
	if data.price_type == 1 then
		limit_str = data.limit_type == 1 and Language.CowOrigin.LimitType3 or Language.CowOrigin.LimitType4
		UH.SetText(self.BtnText,Language.CowOrigin.BuyType1)
	else
		limit_str = data.limit_type == 1 and Language.CowOrigin.LimitType1 or Language.CowOrigin.LimitType2
		UH.SetText(self.BtnText,Language.CowOrigin.BuyType2)
	end
	local limit_num = data.limit_type == 1 and data.buy_times or data.today_times
	local buyed_num = limit_num - ServerActivityData.Instance:GetCowConvertBuyCount(data.seq,data.limit_type)
	limit_str = string.format("%s%s/%s",limit_str,buyed_num,limit_num)
	UH.SetText(self.LimitText,limit_str)
	if self.data.price_type == 3 then
		UH.SetText(self.FeeNum,data.price / 10)
	else
		UH.SetText(self.FeeNum,data.price)
	end
	self.FeeTypeImg.gameObject:SetActive(self.data.price_type == 0 or self.data.price_type == 2)
	self.RMBObj:SetActive(self.data.price_type == 3)
	self.ScoreObj:SetActive(self.data.price_type == 1)
	if data.price_type == 0 or data.price_type == 2 then
		UH.SetGold(self.FeeTypeImg,data.price_type)
	end
	self.data.buy_flag = ServerActivityData.Instance:GetCowConvertBuyFlag(data)
	self.Interactor.Interactable = self.data.buy_flag > 0
end

function CowConvertItem:OnBuyClick()
	if self.data.buy_flag == 0 then
		PublicPopupCtrl.Instance:Center(string.format(Language.CowOrigin.BlockTip,self.data.unlock_integral))
		return
	end
	if self.data.buy_flag == -1 then
		if self.data.price_type == 1 then
			PublicPopupCtrl.Instance:Center(Language.CowOrigin.NotBuy2)
		else
			PublicPopupCtrl.Instance:Center(Language.CowOrigin.NotBuy1)
		end
		return
	end
	if self.data.price_type == 3 then
		if ChannelAgent ~= nil then
          	ChannelAgent:RequestPay(self.data.price / 10, BUY_ITEM_TYPE.TYPE_COW_CONVERT,self.data.seq)
 		end
	else
		local param_t = {}
		param_t.rand_activity_type = ACTIVITY_TYPE.RAND_COW_CONVERT
		param_t.opera_type = 1
		param_t.param_1 = self.data.type
		param_t.param_2 = self.data.seq
		ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
	end
end

-------------- 牛运当头的内部兑换二级界面 ------------------
CowOriginConvertView = CowOriginConvertView or DeclareView("CowOriginConvertView","serveractivity/spring_festival/cow_origin_convert_view")
-- VIEW_DECLARE_LEVEL(CowOriginConvertView,ViewLevel.L1)
VIEW_DECLARE_MASK(CowOriginConvertView, ViewMask.BgBlock)
function CowOriginConvertView:CowOriginConvertView()
	-- body
end

function CowOriginConvertView:OnCloseClick()
	ViewMgr:CloseView(CowOriginConvertView)
end

CowOriginConvertPanel = CowOriginConvertPanel or DeclareMono("CowOriginConvertPanel",UIWFlushPanel)
function CowOriginConvertPanel:CowOriginConvertPanel()
	self.data = ServerActivityData.Instance
	self.data_cares = {
		{data = self.data.cow_origin_data, func = self.FlushPanel, init_call = false, keys = {"convert_infos"}},
		{data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false}
    }
end

function CowOriginConvertPanel:Awake()
	UIWFlushPanel.Awake(self)
	self:FlushPanel()
end

function CowOriginConvertPanel:FlushPanel()
	self.List:SetData(self.data:GetCowOriginConvertList())
end

CowOriginConvertItem = CowOriginConvertItem or DeclareMono("CowOriginConvertItem",UIWidgetBaseItem)
function CowOriginConvertItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	local item = Item.Create(data.exchange_item)
	self.ItemCell:SetData(item)
	UH.SetText(self.ItemName,item:Name())
	local today_times = ServerActivityData.Instance.cow_origin_data.convert_infos[data.item_seq] or 0
	local limit_times = data.today_times
	local num_str
	if today_times >= limit_times then
		num_str = ColorStr(tostring(today_times),COLORSTR.Red8)
		self.data.convert_flag = 2
	else
		num_str = ColorStr(tostring(today_times),COLORSTR.Green9)
		self.data.convert_flag = 1
	end
	self.data.convert_flag = self:IsItemEnough() == true and self.data.convert_flag or 0
	UH.SetText(self.ConvertNum,string.format("%s/%s",num_str,limit_times))
	local consume_items = DataHelper.FormatItemList(data.consume_item)
	for i = 1,self.Cells:Length() do
		if consume_items[i] then
			self.Cells[i]:SetData(consume_items[i])
			self.Cells[i].gameObject:SetActive(true)
			local need_num = data.consume_item[i - 1].num
			CellFlushs.SetNum(self.Cells[i],DataHelper.ConsumeNum(Item.GetNum(consume_items[i].item_id), need_num, true))
		else
			self.Cells[i].gameObject:SetActive(false)
		end
	end
	self.RedPointObj:SetActive(self.data.convert_flag == 1)
end

function CowOriginConvertItem:IsItemEnough()
	for i,v in pairs(self.data.consume_item) do
		if Item.GetNum(v.item_id) < v.num then
			return false
		end
	end
	return true
end

function CowOriginConvertItem:OnConvertClick()
	if self.data.convert_flag == 1 then
		local param_t = {}
		param_t.rand_activity_type = ACTIVITY_TYPE.RAND_COW_ORIGIN
		param_t.opera_type = CowOriginCtrlType.ConvertItem
		param_t.param_1 = self.data.item_seq
		ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
	elseif self.data.convert_flag == 2 then
		PublicPopupCtrl.Instance:Center(Language.CowOrigin.ConvertFull)
	else
		PublicPopupCtrl.Instance:Center(Language.CowOrigin.NotConvert)
	end
end

------------- 牛运当头内部的答题界面 -----------------
CowOriginAnswerView = CowOriginAnswerView  or DeclareView("CowOriginAnswerView","serveractivity/spring_festival/cow_origin_answer_view",0,ViewLayer.Top)
function CowOriginAnswerView:CowOriginAnswerView()
	-- body
end

function CowOriginAnswerView:LoadCallback(param_t)
	param_t = param_t or {}
	local answer_id = param_t.answer_id
	self.end_func = param_t.end_func
	local answer_cfg = AnswerData.Instance:GetDataById(answer_id)
	if answer_cfg == nil then
		LogError("牛运当头答题界面错误 answer_id = ",answer_id)
		self:OnCloseClick()
		return
	end
	UH.SetText(self.Tip,answer_cfg.question)
	self.list_data = {}
	for i = 1, 4 do
		if answer_cfg["anwser_" .. i] then
			local data = {}
			data.answer_text = answer_cfg["anwser_" .. i]
			if i == answer_cfg.answer_id then
				data.is_correct = true
			else
				data.is_correct = false
			end
			data.seq = #self.list_data + 1
			table.insert(self.list_data,data)
		end
	end
	self.List:SetData(self.list_data)
end

function CowOriginAnswerView:CloseCallback()
	TimeHelper:CancelTimer(self.time_handle)
	TimeHelper:CancelTimer(self.time_handle2)
end

function CowOriginAnswerView:OnToggleClick(data)
	self.MaskObj:SetActive(true)
	self.List.item_list[data.seq]:ShowResult()
	if data.is_correct == true then
		if self.end_func then
			TimeHelper:CancelTimer(self.time_handle2)
			self.time_handle2 = Invoke(function ()
				self.end_func(data)
			end,1)
		end
		return
	end
	TimeHelper:CancelTimer(self.time_handle)
	self.time_handle = Invoke(function ()
		for i,v in pairs(self.list_data) do
			if v.is_correct == true then
				self.List.item_list[v.seq]:ShowResult()
			end
		end
	end,1)
	if self.end_func then
		TimeHelper:CancelTimer(self.time_handle2)
		self.time_handle2 = Invoke(function ()
			self.end_func(data)
		end,2)
	end
end

function CowOriginAnswerView:OnCloseClick()
	ViewMgr:CloseView(CowOriginAnswerView)
end

CowOriginAnswerItem = CowOriginAnswerItem or DeclareMono("CowOriginAnswerItem",UIWidgetBaseItem)

function CowOriginAnswerItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	UH.SetText(self.AnswerText,data.answer_text)
	local seq_str = ""
	if data.seq == 1 then
		seq_str = "A"
	elseif data.seq == 2 then
		seq_str = "B"
	elseif data.seq == 3 then
		seq_str = "C"
	elseif data.seq == 4 then
		seq_str = "D"
	end
	UH.SetText(self.SeqText,seq_str)
end

function CowOriginAnswerItem:ShowResult(bool)
	bool = bool or self.data.is_correct
	self.ResultImg.gameObject:SetActive(true)
	if bool == true then
		UH.SpriteName(self.ResultImg,"XuanDui")
	else
		UH.SpriteName(self.ResultImg,"XuanCuo")
	end
end
