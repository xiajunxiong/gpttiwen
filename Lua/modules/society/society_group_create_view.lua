
SocietyGroupCreateView = SocietyGroupCreateView or DeclareView("SocietyGroupCreateView", SocietyConfig.ResPath .. "society_group_create")

function SocietyGroupCreateView:SocietyGroupCreateView()
    self.data = SocietyCtrl.Instance:Data()
    self.language = Language.Society.ChatGroupInfo

    -- self.word_num = 0

    -- self.InputField.onValueChanged:AddListener(function ()
    --     self.word_num = DataHelper.GetStringWordNum(self.InputField.text)
	-- 	if self.word_num > SocietyConfig.CHAT_GROUP_NAME_NUM_MAX then
	-- 		PublicPopupCtrl.Instance:Center(self.language.CreateTips1)
	-- 	end
	-- end)
end

function SocietyGroupCreateView:LoadCallback()
	self.Board:SetData(self:GetType(), Language.Society.SocietyGroupCreate, Vector2.New(450, 325))
end

function SocietyGroupCreateView:CloseCallback()
    if SocietyConfig.ChatQuickCreateState.state1 == self.data.guide_group_create then
        self.data.guide_group_create = SocietyConfig.ChatQuickCreateState.state0
        ViewMgr:CloseView(SocietyView)
        GuideManager.Instance:Start(55)
    end
end

function SocietyGroupCreateView:OnFlush()
    local not_cross, is_cross = self.data:GetChatGroupCount()
    self.NumTips.text = string.format(self.language.NumTips, SocietyConfig.CHAT_GROUP_NUM_MAX_IS_CROSS - is_cross)
end

function SocietyGroupCreateView:OnClickCreate()
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
    SocietyCtrl.Instance:SendCreateGroupReq(true, self.InputField.text)
    if SocietyConfig.ChatQuickCreateState.state1 == self.data.guide_group_create then
        self.data.guide_group_create = SocietyConfig.ChatQuickCreateState.state2
        GuideManager.Instance:Start(55)
    end
    ViewMgr:CloseView(SocietyGroupCreateView)
end