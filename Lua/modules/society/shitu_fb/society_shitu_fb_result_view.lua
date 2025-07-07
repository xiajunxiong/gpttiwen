
SocietyShiTuFBResultView = SocietyShiTuFBResultView or DeclareView("SocietyShiTuFBResultView", SocietyConfig.ResPath .. "society_shitu_fb_result")

SocietyShiTuFBResultViewF = SocietyShiTuFBResultViewF or DeclareMono("SocietyShiTuFBResultViewF", UIWFlushPanel)

function SocietyShiTuFBResultViewF:SocietyShiTuFBResultViewF()
    self.data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.ShiTuFB

    self.data_cares = {
		{data = self.data.shitu_fb_sm, func = self.FlushDetail},
	}
end

function SocietyShiTuFBResultViewF:FlushDetail()
    local progress = self.data.shitu_fb_sm.progress
	local progress_t, rewards = self.data:GetShiTuFbResult()
	if progress > 9 then
		self.Num1Sp.SpriteName = math.floor(progress / 10)
		self.Num2Sp.SpriteName = progress % 10
		self.Num2Sp:SetObjActive(true)
	else
		self.Num2Sp:SetObjActive(false)
		self.Num1Sp.SpriteName = progress
    end
	self.ProgressList:SetData(progress_t)
    self.RewardList:SetData(rewards)
end

function SocietyShiTuFBResultViewF:OnClickConfirm()
    ViewMgr:CloseView(SocietyShiTuFBResultView)
end

SocietyShiTuFBResultViewItem = DeclareMono("SocietyShiTuFBResultViewItem", UIWidgetBaseItem)

function SocietyShiTuFBResultViewItem:SocietyShiTuFBResultViewItem()
	self.society_data = SocietyData.Instance
	self.language = Language.Society.ShiTuFB
end

function SocietyShiTuFBResultViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local detail = data.detail
	for i = 1, self.Poems:Length() do
		local index = i - 1
		self.Poems[i].text = data[index] and detail[index].name or self.language.PoemEmpty
	end
end