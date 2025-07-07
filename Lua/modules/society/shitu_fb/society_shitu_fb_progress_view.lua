
SocietyShiTuFBProgressView = SocietyShiTuFBProgressView or DeclareView("SocietyShiTuFBProgressView", SocietyConfig.ResPath .. "society_shitu_fb_progress")

function SocietyShiTuFBProgressView:SocietyShiTuFBProgressView()
    self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society
end

function SocietyShiTuFBProgressView:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.SocietyShiTuFB, Vector2.New(974, 510))
end

SocietyShiTuFBProgressViewF = SocietyShiTuFBProgressViewF or DeclareMono("SocietyShiTuFBProgressViewF", UIWFlushPanel)

function SocietyShiTuFBProgressViewF:SocietyShiTuFBProgressViewF()
    self.data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.ShiTuFB

    self.data_cares = {
		{data = self.data.shitu_fb_sm, func = self.FlushDetail},
	}
end

function SocietyShiTuFBProgressViewF:FlushDetail()
	local progress = self.data.shitu_fb_sm.progress
	local progress_t, rewards = self.data:GetShiTuFbDetail()
	local is_get_shi_tu_reward = self.data.is_get_shi_tu_reward
	if progress > 9 then
		self.Num1Sp.SpriteName = math.floor(progress / 10)
		self.Num2Sp.SpriteName = progress % 10
		self.Num2Sp:SetObjActive(true)
	else
		self.Num2Sp:SetObjActive(false)
		self.Num1Sp.SpriteName = progress
	end
	self.ProgressList:SetData(progress_t)
	self.IsGetObj:SetActive(is_get_shi_tu_reward)
	self.RewardList:SetObjActive(not is_get_shi_tu_reward)
	if not is_get_shi_tu_reward then
		self.RewardList:SetData(rewards)
	end
end

SocietyShiTuFBProgressViewItem = DeclareMono("SocietyShiTuFBProgressViewItem", UIWidgetBaseItem)

function SocietyShiTuFBProgressViewItem:SocietyShiTuFBProgressViewItem()
	self.society_data = SocietyData.Instance
	self.language = Language.Society.ShiTuFB
end

function SocietyShiTuFBProgressViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local detail = data.detail
	self.CompleteObj:SetActive(nil ~= data[0] and nil ~= data[1])
	self.BgSp.SpriteName = "DiTiao" .. data.bg_type
	for i = 1, self.Poems:Length() do
		local index = i - 1
		self.Poems[i].text = data[index] and detail[index].name or self.language.PoemEmpty
	end
end
