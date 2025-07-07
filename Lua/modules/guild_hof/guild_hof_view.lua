GuildHofView = GuildHofView or DeclareView("GuildHofView", "guild/guild_hof", Mod.Guild.GuildHof)
VIEW_DECLARE_LEVEL(GuildHofView, ViewLevel.L1)
function GuildHofView:GuildHofView()
    GuildHofCtrl.Instance:SendGuildHofRank()
    GuildHofCtrl.Instance:SendGuildHofReq(GuildHofReqType.Info)
end

function GuildHofView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[248].desc)
end

function GuildHofView:CloseCallback()
    GuildHofData.Instance:ResetData(true)
    GuildHofData.Instance:SetTodayRemind()
end

function GuildHofView:OnCloseClick()
    ViewMgr:CloseView(GuildHofView)
end

----------------GuildHofPanel----------------
GuildHofPanel = GuildHofPanel or DeclareMono("GuildHofPanel", UIWFlushPanel)
function GuildHofPanel:GuildHofPanel()
    self.Data = GuildHofData.Instance
    self.language = Language.GuildHof
    self.data = self.Data.info_data
    self.data_cares = {
        {data = self.data, func = self.FlushRight, keys = {"info"}},
        {data = self.data, func = self.FlushLeft, keys = {"rank_list"}},
        {data = self.Data.remind_data, func = self.FlushTodayRemind},
    }
end

function GuildHofPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function GuildHofPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function GuildHofPanel:FlushRight()
    UH.SetText(self.TxtProgress, Format(Language.Common.Percent, self.Data:GetPercent(true)))
    self.Rend.material:SetFloat("_range", self.Data:GetPercent())
    self.AttrList:SetData(self.Data:GetBlessAttr())
    self.ObjCanGet:SetActive(self.Data:IsCanGetAttr())
    self.Remind:SetNum(self.Data:IsCanGetAttr() and 1 or 0)
    self.ObjCanNotGet:SetActive(not self.Data:IsCanGetAttr() and not self.Data:IsGetAttr())
    self.ObjGeted:SetActive(self.Data:IsGetAttr())
    self:OnTipClose()
end

function GuildHofPanel:FlushTodayRemind()
    self.TodayRemind:SetNum(self.Data:TodayRemind())
end

function GuildHofPanel:FlushLeft()
    self.RankList:SetData(self.data.rank_list)
end

function GuildHofPanel:OnBless()
    self.Data:SetTodayRemind()
    if not self.Data:IsOpen() then
        PublicPopupCtrl.Instance:Center(self.language.NotOpenBless)
    elseif self.Data:IsBlessMax() then
        PublicPopupCtrl.Instance:Center(self.language.BlessMaxTip)
    else
        self.ObjTip:SetActive(true)
        self:FlushTip()
    end
end

function GuildHofPanel:FlushTip()
    local bless_config = self.Data:GetBlessConfig()
    if not TableIsEmpty(bless_config) then
        UH.SetText(self.TxtBlessTimes, Format(self.language.BlessTimes, self.data.info.today_bless_times, self.Data:MaxBlessCount()))
        local cost_name = Language.Common.HuoBi[bless_config.cost_type]
        local str = Format(self.language.BlessTip[bless_config.cost_type], cost_name, cost_name, RichTextHelper.SpriteStrAtlas(HuoBi[bless_config.cost_type], 40, 40), bless_config.bless_consume)
        UH.SetText(self.RichText, str)
    else
        self:OnTipClose()
    end
end

function GuildHofPanel:OnGetAttr()
    GuildHofCtrl.Instance:SendGuildHofReq(GuildHofReqType.Reward)
end

function GuildHofPanel:OnTipClose()
    self.ObjTip:SetActive(false)
end

function GuildHofPanel:OnBless2()
    local bless_config = self.Data:GetBlessConfig()
    if not TableIsEmpty(bless_config) and MallCtrl.IsNotCurrency(bless_config.cost_type, bless_config.bless_consume) then
        GuildHofCtrl.Instance:SendGuildHofReq(GuildHofReqType.Bless)
    end
end

--------------GuildHofRankItem--------------
GuildHofRankItem = GuildHofRankItem or DeclareMono("GuildHofRankItem", UIWidgetBaseItem)
function GuildHofRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_have_first = data.rank_count > 0
    if is_have_first then
        if not self.ui_obj then
            self.ui_obj = UIChDrawer.CreateByAppearance(data.appearance)
            self.ModelShow:SetShowData({ui_obj = self.ui_obj, view = self, show_shadow = false})
        else
            self.ui_obj:SetData(data.appearance)
        end
    end
    UH.SetText(self.TxtName, is_have_first and data.rank_list[1].name or Language.GuildHof.NoPerson)
    self.ObjModel:SetActive(is_have_first)
    UH.SetText(self.TxtTitle, Language.GuildHof.RankType[data.rank_type])
    UH.SetText(self.TxtValueType, Language.GuildHof.ValueType[data.rank_type])
    self.List:SetData(data.rank_list)
end

function GuildHofRankItem:OnDestroy()
    Delete(self, "ui_obj")
end

--------------GuildHofListItem--------------
GuildHofListItem = GuildHofListItem or DeclareMono("GuildHofListItem", UIWidgetBaseItem)
function GuildHofListItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ObjBg:SetActive((data.index % 2) == 0)
    UH.SetText(self.TxtRank, data.index)
    UH.SetText(self.TxtName, data.name or Language.GuildHof.NoPerson)
    if data.prof then
        UH.SpriteName(self.ImgSetProf, RoleData.GetProfSp(data.prof))
        self.ImgSetProf:SetObjActive(true)
    else
        self.ImgSetProf:SetObjActive(false)
    end
    UH.SetText(self.TxtValue, data.value or "")
end

--------------GuildHofAttrItem--------------
GuildHofAttrItem = GuildHofAttrItem or DeclareMono("GuildHofAttrItem", UIWidgetBaseItem)
function GuildHofAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.attr_name)
    UH.SetText(self.TxtValue, Format("+%s", data.attr_percent_value))
end