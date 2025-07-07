--===============================FairyWhereSub===============================
FairyWhereSub = FairyWhereSub or DeclareMono("FairyWhereSub", UIWFlushPanel)

function FairyWhereSub:FairyWhereSub()
	self.data = FairyWhereData.Instance
    self.data_cares = {
		{data = self.data.info_data, func = self.FlushShowTip, init_call = false,keys = {"is_show"}},
    }
end

function FairyWhereSub:FlushShowTip()
    if self.data:GetShowData() then
        self.Sub:SetActive(true)
    end
end

function FairyWhereSub:Awake()
	UIWFlushPanel.Awake(self)
    UH.SetText(self.CickText,RichTextHelper.LinkStr(RichTextHelper.ColorStr(Language.FairyWhere.RichText,COLORSTR.Red20),"_ul",COLORSTR.Red20))
    self.Sub:SetActive(false)
end

function FairyWhereSub:OnClickEnter()
    ChatCtrl.Instance:SendChannelChatText(Language.FairyWhere.SendChannel, nil, ChatConfig.ChannelType.general)
    self:OnClickClose()
end

function FairyWhereSub:OnClickClose()
	self.Sub:SetActive(false)
    self.data:SetShowData(false)
end