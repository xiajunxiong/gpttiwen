UIWScoreItem = UIWScoreItem or DeclareMono("UIWScoreItem")
---ScoreTxt=====Text
---ScoreLvImg===UIImageSpriteSet
---CancelClick===Bool --取消点击事件 
function UIWScoreItem:UIWScoreItem()
end

function UIWScoreItem:OnClickScore()
	-- PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[46].desc})
	if self.CancelClick ~= true then
		ViewMgr:OpenView(BianQiangView)
	end
end

function UIWScoreItem:SetScore(score, level)
	self.ScoreTxt.text = score
	if level == nil then
		self.ScoreLvImg.SpriteName = nil
	else
		self.ScoreLvImg.SpriteName = string.format("scoreLevel%d", level)
	end
end

UIWTotalScoreItem = UIWTotalScoreItem or DeclareMono("UIWTotalScoreItem", UIWFlushPanel)
---ScoreTxt=====Text
---ScoreLvImg===UIImageSpriteSet
---CancelClick===Bool --取消点击事件
function UIWTotalScoreItem:UIWTotalScoreItem()
	self.role_data = RoleData.Instance
	self.data_cares = {
		{data = self.role_data.role_total_score, func = self.FlushScore, init_call = false}
	}
end
function UIWTotalScoreItem:Awake()
	UIWFlushPanel.Awake(self)
	-- local score = RoleData.Instance:GetRoleScore()
	local score = RoleData.Instance:GetRoleTotalScore()
	self:SetScore(score)
end
function UIWTotalScoreItem:FlushScore()
	local old_score = tonumber(self.ScoreTxt.text)
	-- local score = RoleData.Instance:GetRoleScore()
	local score = RoleData.Instance:GetRoleTotalScore()
	if old_score == score then
		return
	end
	if ViewMgr:IsOpen(TotalScore) then
		ViewMgr:FlushView(TotalScore)
	else
		ViewMgr:OpenView(TotalScore)
	end
	self.EffectTool:Play(100108168)
	self.EffectAnim:Play()
	local offset = score - old_score
	TimeHelper:CancelTimer(self.handle_run)
	self.handle_run =
		TimeHelper:AddRunFrameTimer(
		function()
			old_score = old_score + (offset / 10)
			self:SetScore(math.floor(old_score))
		end,
		2,
		15,
		true
	)
	TimeHelper:CancelTimer(self.handle_delay)
	self.handle_delay =
		TimeHelper:AddDelayFrameTimer(
		function()
			TimeHelper:CancelTimer(self.handle_run)
			self:SetScore(score)
		end,
		13
	)
end

function UIWTotalScoreItem:OnClickScore()
	-- PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[46].desc})
	if self.CancelClick ~= true then
		ViewMgr:OpenView(BianQiangView)
	end
end
 
function UIWTotalScoreItem:SetScore(score)
	UH.SetText(self.ScoreTxt, score)
end
