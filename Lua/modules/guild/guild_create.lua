GuildCreate = GuildCreate or DeclareView("GuildCreate", "guild/guild_create")
function GuildCreate:GuildCreate()
    self.lang = Language.Guild
end
function GuildCreate:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Tab7, Vector2.New(692, 432))
    -- self.input_func = BindTool.Bind(self.ValueChange, self)
    -- UH.InputAddListener(self.Arms, self.input_func)
    -- UH.onEndEdit(self.Arms,self.input_func)
end
function GuildCreate:ValueChange(params)
    --检查是否写了然后现不现图标
end
function GuildCreate:OnFlush()
    local is_first = FirstChargeData.Instance:GuildCreateConsumeCoinBind() ~= 1
    if RechargeData.Instance:GetRechargeNum() > 0 and is_first then
        self.Cost.text = GuildData.Instance.guild_other.first_charge_create_cost    
        UH.SpriteName(self.CostSp, HuoBi[GuildData.Instance.guild_other.first_charge_create_cost_type])
    else 
        self.Cost.text = GuildData.Instance.guild_other.create_cost
        UH.SpriteName(self.CostSp, HuoBi[GuildData.Instance.guild_other.create_cost_type])
    end
    self.Level.text = GuildData.Instance.guild_other.level
end
function GuildCreate:OnClickEditor()
    self.Arms:ActivateInputField()
    self.EditorObj:SetActive(false)
end
function GuildCreate:OnClickCreate()
    if LoginData.Instance:IsOnCrossSever() then
        local param_t = {
        content = Language.Common.CrossTips,
        confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                    LoginCtrl:SendBackToOriginServer()
                end
            },
        }
        PublicPopupCtrl.Instance:DialogTips(param_t)
        return
    end
    if self.Name.text == "" then
        PublicPopupCtrl.Instance:Center(ErrorText[50])
        return
    end
    local count = DataHelper.GetStringWordNum(self.Name.text)
    if count > ROLE_NAME_COUNT then
        PublicPopupCtrl.Instance:Center(ErrorText[65])
        return
    end
    local name_and_arms = self.Name.text .. self.Arms.text
    -- if ChatFilter.Instance:IsIllegal(self.Name.text, true) then
    --     PublicPopupCtrl.Instance:Center(ErrorText[66])
    --     return
    -- end
    -- if ChatFilter.Instance:IsIllegal(self.Arms.text, true) then
    --     PublicPopupCtrl.Instance:Center(ErrorText[66])
    --     return
    -- end
    if ChatFilter.Instance:IsIllegal(name_and_arms, true) then
        PublicPopupCtrl.Instance:Center(ErrorText[66])
        return
    end
    if self.Arms.text == " " then
        PublicPopupCtrl.Instance:Center(ErrorText[190])
        return
    end
    if self.Notice.text == "" then
        self.Notice.text = Language.Guild.CreateNotice
    end
    if ChatFilter.Instance:IsIllegal(self.Notice.text) then
        PublicPopupCtrl.Instance:Center(ErrorText[66])
        return
    end
    local is_first = FirstChargeData.Instance:GuildCreateConsumeCoinBind() ~= 1
    if RechargeData.Instance:GetRechargeNum() > 0 and is_first then
        if not MallCtrl.IsNotCurrency(CurrencyType.CoinBind, GuildData.Instance.guild_other.first_charge_create_cost) then
            return
        end
    else
        if not MallCtrl.IsNotCurrency(CurrencyType.Gold, GuildData.Instance.guild_other.create_cost) then
            return
        end
    end
    GuildCtrl.SendCreateGuild({name = self.Name.text, notice = self.Notice.text, arms = self.Arms.text})
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end
function GuildCreate:CloseCallback()
    -- UH.InputRemoveListener(self.Arms, self.input_func)
    -- UH.removeEndEdit(self.Arms,self.input_func)
end