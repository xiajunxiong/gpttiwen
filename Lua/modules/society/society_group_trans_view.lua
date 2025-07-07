
SocietyGroupTransView = SocietyGroupTransView or DeclareView("SocietyGroupTransView", SocietyConfig.ResPath .. "society_group_trans")

function SocietyGroupTransView:SocietyGroupTransView()
    self.data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.ChatGroupInfo
end

function SocietyGroupTransView:LoadCallback()
	self.Board:SetData(self:GetType(), Language.Society.SocietyGroupTrans, Vector2.New(525, 366))
end

function SocietyGroupTransView:CloseCallback()
end

function SocietyGroupTransView:OnFlush()
    local not_cross, is_cross = self.data:GetChatGroupCount()
    self.NumTips.text = string.format(self.language.NumTips, SocietyConfig.CHAT_GROUP_NUM_MAX_IS_CROSS - is_cross)
end

function SocietyGroupTransView:OnClickCreate()
    local word_num = DataHelper.GetStringWordNum(self.InputField.text)
    if 0 == word_num then
        PublicPopupCtrl.Instance:Center(self.language.CreateTips2)
        return
    end
    if word_num > SocietyConfig.CHAT_GROUP_NAME_NUM_MAX then
        PublicPopupCtrl.Instance:Center(self.language.CreateTips1)
        return
    end
    if ChatFilter.Instance:IsIllegal(self.InputField.text) then
        PublicPopupCtrl.Instance:Center(self.language.IllegalName)
        return
    end
    SocietyCtrl.Instance:SendTempChatGroupBecomeOfficialReq(self.InputField.text)
    ViewMgr:CloseView(SocietyGroupTransView)
end