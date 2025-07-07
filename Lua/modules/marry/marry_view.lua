MarryView = MarryView or DeclareMono("MarryView", UIWFlushPanel)

function MarryView:MarryView()
    self.data = MarryData.Instance
    self.data_cares = {
        {data = self.data.info_data, func = self.FlushMarryView, init_call = false},
        {data = AppearanceData.Instance.appearance_red_piont,func = self.FlushTitleView, init_call = false},
        {data = MarryRechargeData.Instance.act_info, func = self.FlushActTips}
    }
    self.ModelGame = {}
    self.data.info_data.val = not self.data.info_data.val
end

function MarryView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self,[[ModelGame]])
end

function MarryView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if self.data:GetHusbandOrWifeUid() ~= 0 then
        self:FlushMarryView()
        self:FlushModelView()
    end
    self.SinglePanel:SetActive(self.data:GetHusbandOrWifeUid()==0)
end

function MarryView:OnClickTips()
    ViewMgr:OpenView(TeamKnightlyMarryView)
end

function MarryView:FlushMarryView()
    local role_info = self.data:GetRoleInfo()
    for i=1,self.Name:Length() do
        if role_info[i] then
            local pre_name = self.data:GetMarryPreName(role_info[i]) .. "："
            UH.SetText(self.Name[i],pre_name .. (role_info[i].role_name or ""))
        end
    end
    self.RedPiont:SetNum(self.data:GetRemindNum())
end

function MarryView:FlushModelView()
    local role_info = self.data:GetRoleInfo()
    for i=1,self.ModelShow:Length() do
        local role_appearance = role_info[i].appearance or {}
        local appearance = {
            color = role_appearance.color,
            prof_base = role_appearance.prof_base,
            avatar_type = role_appearance.avatar_type,
            advance_time = role_appearance.advance_time,
            fashion_body_id = MarryConfig.FASHION_BODY_ID,
        }
        if self.ModelGame[i] == nil then
            self.ModelGame[i] = UIChDrawer.CreateByAppearance(appearance)
            self.ModelShow[i]:SetShowData({ui_obj = self.ModelGame[i],view = self})
        end
    end
    self:FlushTitleView()
end

function MarryView:FlushTitleView()
    local config = self:GetTitleIcon()
    self.TitleIcon:SetData(TitleData.GetTitleConfig(config.title_id))
end

function MarryView:GetTitleIcon()
    local config = Config.get_married_auto.marry_title
    for i,v in ipairs(config) do
        local title_config = AppearanceData.Instance:GetTitleConfig(v.title_id)
        local is_active = AppearanceData.Instance:GetActiveFlag(title_config.seq) == 1
        if not is_active and config[i-1]then
            return config[i-1]
        end
        if is_active and i == #config then
            return v
        end
    end
    return config[1]
end

function MarryView:OnClickIntroduce()
    ViewMgr:OpenView(MarryCertificateView)
end

function MarryView:OnClickTitle()
    ViewMgr:OpenView(MarryTitleView)
end

function MarryView:OnClickSkill()
    ViewMgr:OpenView(MarrySkillView)
end

function MarryView:OnClickGift()
    ViewMgr:OpenView(MarryGiftView)
end

function MarryView:FlushActTips()
	UH.SetText(self.EnterName, RichTextHelper.LinkStr(ColorStr(Language.Common.GoTo, COLORSTR.Green9), "_ul", COLORSTR.Green9))
    self.ActTips:SetActive(MarryRechargeData.Instance:GetFunIsOpened())
end

function MarryView:OnClickToAct()
    ViewMgr:OpenViewByKey(Mod.Reward.MarryRecharge)
end

--未结婚状态
MarrySinglePanel = MarrySinglePanel or DeclareMono("MarrySinglePanel", UIWFlushPanel)

function MarrySinglePanel:MarrySinglePanel()
    self.page_num = 1
end

function MarrySinglePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:OnClickItem(0)
    self:FlushWeddingView()
end

function MarrySinglePanel:OnClickItem(num)
    self.page_num = self.page_num + num
    self:FlushButtonView()
    self:FlushItemView()
end

function MarrySinglePanel:FlushButtonView()
    local length = self.PanelList:Length()
    for i=1,self.PanelList:Length() do
        self.PanelList[i]:SetActive(i==self.page_num)
    end
    self.BtnLeft:SetActive(self.page_num > 1)
    self.BtnRight:SetActive(self.page_num < length)
end

function MarrySinglePanel:FlushItemView()
    local config = Language.Marry.Single[self.page_num]
    local split = Split(Language.GetHelpDesc(config.desc[1]),"|")
    for i=1,self.Desc:Length() do
        UH.SetText(self.Desc[i],split[i])
    end
    for i=1,self.Title:Length() do
        UH.SetText(self.Title[i],config.title[i])
    end
    UH.SetText(self.Tips,Language.GetHelpDesc(config.desc[2]))
end

function MarrySinglePanel:FlushWeddingView()
    for i=1,self.Wedding:Length() do
        self.Wedding[i]:SetData(i)
    end
    local config = Config.get_married_auto.float_parade
    local path_1 = DrawerHelper.GetRidePath(config[1].res_id)
    if not self.WeddingGame1 then
        self.WeddingGame1 = UIChDrawer.New()
        self.WeddingGame1:SetMainSolo(path_1)
        self.WeddingShow1:SetShowData({ui_obj = self.WeddingGame1, view = self})
    else
        self.WeddingGame1:SetMainSolo(path_1)
    end
    local path_2 = DrawerHelper.GetRidePath(config[2].res_id)
    if not self.WeddingGame2 then
        self.WeddingGame2 = UIChDrawer.New()
        self.WeddingGame2:SetMainSolo(path_2)
        self.WeddingShow2:SetShowData({ui_obj = self.WeddingGame2, view = self})
    else
        self.WeddingGame2:SetMainSolo(path_2)
    end
end

function MarrySinglePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self,[[WeddingGame1]])
    Delete(self,[[WeddingGame2]])
end

function MarrySinglePanel:OnClickEnter()
    if not LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:AlertTip(Language.Marry.WeddingCrossTips,function()
            LoginCtrl.Instance:SendCrossStartReq()
        end)
        return
    end
    if not TeamCtrl.Instance:Data():InTeam() or #TeamData.Instance:GetMemberRoleList() == 1 then
        ViewMgr:OpenView(MarryFancyView)
    else
        SceneLogic.Instance:AutoToNpcs(MarryData.Instance:GetNpcId())
        ViewMgr:CloseView(SocietyView)
    end
end

----------------------------MarrySingleItem----------------------------
MarrySingleItem = MarrySingleItem or DeclareMono("MarrySingleItem", UIWidgetBaseItem)

function MarrySingleItem:SetData(data)
    local config = Config.get_married_auto.marry_base[data]
    local role_num = Config.get_married_auto.other[1].invite_number
    local item = Item.New(Config.get_married_auto.other[1].candy_reward)
    UH.SetText(self.Title,config.type_name)
    UH.SetText(self.Desc,Format(Language.Marry.SingleItemTip,role_num,item.num))
    self.Grid:SetData(DataHelper.FormatList(config.bugreward_show))
    if self.Price ~= nil then
        UH.SetText(self.Price,config.price)
        UH.SetGold(self.Gold,config.money_type)
    end
end

----------------------------TeamKnightlyMarryView----------------------------
TeamKnightlyMarryView = TeamKnightlyMarryView or DeclareView("TeamKnightlyMarryView", TeamConfig.ResPath .. "team_knightly_marry")

function TeamKnightlyMarryView:OnClickClose()
	ViewMgr:CloseView(TeamKnightlyMarryView)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

function TeamKnightlyMarryView:LoadCallback()
	UH.SetText(self.Desc,Language.GetHelpDesc(258))
end

function TeamKnightlyMarryView:OnClickEnter()
    SceneLogic.Instance:AutoToNpcs(MarryData.Instance:GetNpcId())
    ViewMgr:CloseView(TeamKnightlyMarryView)
    ViewMgr:CloseView(SocietyView)
end