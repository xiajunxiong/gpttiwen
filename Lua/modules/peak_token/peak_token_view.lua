PeakTokenSub = PeakTokenSub or DeclareMono("PeakTokenSub")

PeakTokenView = PeakTokenView or DeclareView("PeakTokenView", "peak_token/peak_token_view")

VIEW_DECLARE_LEVEL(PeakTokenView,ViewLevel.L0)

function PeakTokenView:PeakTokenView()
    self.data = PeakTokenData.Instance
end

function PeakTokenView:CloseCallback()
    self.data:ClearData()
end

function PeakTokenView:LoadCallback(param_t)
	self.data:SetRoleInfoTimes(param_t.info.reward_times)
	self.SendPeaktokenFunc = function()
		PeakTokenCtrl.Instance:SendPeaktokenSeq(1,param_t.obj_id)
		self.data:SetRainState(PeakTokenState.Standy)
	end
    local limit_num = Config.supreme_token_auto.other[1].reward_num
    UH.SetText(self.Desc,Format(Language.PeakToken.ActivityTips,param_t.name,limit_num))
end

function PeakTokenView:OnClickClose()
    ViewMgr:CloseView(PeakTokenView)
end

function PeakTokenView:OnClickEnter()
	self.SendPeaktokenFunc()
end
--===========================PeakTokenPanel===========================
PeakTokenPanel = PeakTokenPanel or DeclareMono("PeakTokenPanel", UIWFlushPanel)

function PeakTokenPanel:PeakTokenPanel()
    self.data = PeakTokenData.Instance
    self.data_cares = {
        {data = self.data.game_data, func = self.OnFlush,keys = {"state"}},
        {data = self.data.game_data, func = self.FlushNumView,keys = {"num"},init_call = false},
    }
end

function PeakTokenPanel:OnFlush()
    local state = self.data:GetRainState()
    if state == PeakTokenState.Standy then
        local standy_time = 5
        self:SetTime(self.StartImg,standy_time)
		self.StartTime:TotalTime(standy_time)
        self.StartTime:SetCallBack(function()
            self.data:SetRainState(PeakTokenState.Open)
        end,function(start_time,total_time)
            self:SetTime(self.StartImg,total_time-start_time)
        end)
    elseif state == PeakTokenState.Open then
        local open_time = Config.supreme_token_auto.other[1].game_duration
        self:SetTime(self.GameImg,open_time)
		self.GameTime:TotalTime(open_time)
        self.GameTime:SetCallBack(function()
            self.data:SetRainState(PeakTokenState.Finish)
        end,function(start_time,total_time)
            self:SetTime(self.GameImg,total_time-start_time)
        end)
		local reward_times = self.data:GetRoleInfoTimes()
		local limit_num = Config.supreme_token_auto.other[1].reward_num - reward_times
        UH.SetText(self.Num,Format("%s/%s",0,limit_num))
        self:OnPlayGame(open_time)
    elseif state == PeakTokenState.Finish then
		self.GameTime:CloseCountDownTime()
		local reward_list = self.data:GetRewardList()
		if #reward_list > 0 then
			ViewMgr:OpenView(PeakTokenRewardView)
		else
			ViewMgr:CloseView(PeakTokenView)
		end
    end
    self.BaseGame:SetActive(state == PeakTokenState.None)
    self.PanelGame:SetActive(state ~= PeakTokenState.None)
    self.GamePanel:SetActive(state == PeakTokenState.Open)
    self.EffectGame:SetActive(state == PeakTokenState.Open)
    self.StartTime:SetObjActive(state == PeakTokenState.Standy)
end

function PeakTokenPanel:FlushNumView()
	local item_num = self.data:GetItemNum()
	local reward_times = self.data:GetRoleInfoTimes()
    local limit_num = Config.supreme_token_auto.other[1].reward_num - reward_times
	if item_num >= limit_num then
		self.data:SetRainState(PeakTokenState.Finish)
		ViewMgr:CloseView(PeakTokenGiftView)
	end
    UH.SetText(self.Num,Format("%s/%s",item_num,limit_num))
end

function PeakTokenPanel:SetTime(obj,time)
    UH.SpriteName(obj[1],time % 10)
    UH.SpriteName(obj[2],math.floor(time / 10) % 10)
end

function PeakTokenPanel:OnPlayGame(time)
	self.time_speed = 50
	self.item_count = 10 * self.time_speed
	self.add_score_count = 0
	self.move_item_list = {}
	self.cur_time = 0
	self.speed_time = 1 / self.time_speed
	local game_time = TimeManager.GetServerTime() + time
	self.time_ramdon = {}
	for i=1,self.item_count do
		local index = GameMath.Ramdon(0, time * self.time_speed)
		self.time_ramdon["ramdon_key_" .. index] = self.time_ramdon["ramdon_key_" .. index] and self.time_ramdon["ramdon_key_" .. index] + 1 or 1
	end
	self.CountDownTimer = TimeHelper:AddCountDownCT(BindTool.Bind(self.UpdateFunc,self), BindTool.Bind(self.CompleteFunc,self), game_time,self.speed_time,false)
end

function PeakTokenPanel:UpdateFunc()
	self.cur_time = self.cur_time + self.speed_time
	local value = tostring(math.ceil(self.cur_time * self.time_speed))
	if self.time_ramdon["ramdon_key_" .. value] then
		for i=1,self.time_ramdon["ramdon_key_" .. value] do
			local obj = self.ItemMono.gameObject:Clone(self.ItemPool)
			obj:SetActive(true)
			local item_mono = obj:GetLuaComponent("PeakTokenItem")
			item_mono:SetView(self)
			local item_witch = 100
			local item_x = GameMath.Ramdon(self.StartPos.localPosition.x / item_witch, self.EndPos.localPosition.x / item_witch)
			item_mono:SetStartPosition(Vector2.New(item_x * item_witch,self.StartPos.localPosition.y))
			item_mono:SetEndPosition(self.EndPos.localPosition)
			self.move_item_list[item_mono] = 1
		end
	end
	for v,_ in pairs(self.move_item_list) do
		v:FlushPosition()
	end
end

function PeakTokenPanel:CompleteFunc()
end

function PeakTokenPanel:AddScoreCount(item_mono)
	self.add_score_count = self.add_score_count + 1
	self.move_item_list[item_mono] = nil
end

function PeakTokenPanel:RemoveMoveItem(item_mono)
	self.move_item_list[item_mono] = nil
end

function PeakTokenPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.CountDownTimer)
end

--------------------------PeakTokenItem--------------------------
PeakTokenItem = PeakTokenItem or DeclareMono("PeakTokenItem")

function PeakTokenItem:PeakTokenItem()
	self.speed = GameMath.Ramdon(20, 24)
end

function PeakTokenItem:SetView(view)
	self.view = view
end

function PeakTokenItem:SetStartPosition(pos)
	self.Position = pos
end

function PeakTokenItem:SetEndPosition(pos)
	self.EndPos = pos
end

function PeakTokenItem:FlushPosition()
	self.Position.y = self.Position.y - self.speed
	self.RectTransform.localPosition = self.Position
	if self.Position.y <= self.EndPos.y then
		self.view:RemoveMoveItem(self)
		UnityGameObj.Destroy(self.gameObject)
	end
end

function PeakTokenItem:OnClickItem()
	self.view:AddScoreCount(self)
	UnityGameObj.Destroy(self.gameObject)
	local item_num = PeakTokenData.Instance:GetItemNum()
	local reward_times = PeakTokenData.Instance:GetRoleInfoTimes()
    local limit_num = Config.supreme_token_auto.other[1].reward_num - reward_times
	if item_num < limit_num then
		ViewMgr:OpenView(PeakTokenGiftView)
	end
end