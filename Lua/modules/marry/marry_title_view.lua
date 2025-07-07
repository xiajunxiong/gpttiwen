MarryTitleView = MarryTitleView or DeclareView("MarryTitleView", "marry/marry_title")

function MarryTitleView:MarryTitleView()
    self.Board:SetData(self:GetType(), Language.Marry.TitleNameList[1], Vector2.New(913, 604),true)
    self.care_handle = AppearanceData.Instance.appearance_red_piont:Care(BindTool.Bind(self.FlushItemView, self))
    self.title_config = Config.get_married_auto.marry_title
    self.data = MarryData.Instance
    self.make_num = 1
end

function MarryTitleView:CloseCallback()
	AppearanceData.Instance.appearance_red_piont:Uncare(self.care_handle)
	self.care_handle = nil
end

function MarryTitleView:LoadCallback(param_t)
    self:OnClickItem(0)
end

function MarryTitleView:OnClickItem(num)
    self.make_num = self.make_num + num
    self:FlushButtonView()
    self:FlushItemView()
end

function MarryTitleView:FlushButtonView()
    self.BtnLeft:SetActive(self.make_num > 1)
    self.BtnRight:SetActive(self.make_num < #self.title_config)
end

function MarryTitleView:FlushItemView()
    local config = self.title_config[self.make_num]
    if config.marry_intimacy ~= 0 then
        self:FlushProgressView(config)
    else
        self:FlushMaxProgressView()
    end
    UH.SetText(self.Desc,Format(Language.Marry.TitleCondition,config.marry_time))
    self.TitleIcon:SetData(TitleData.GetTitleConfig(config.title_id))
    self:FlushTitleState(config.title_id)
end

function MarryTitleView:FlushProgressView(config)
    local task_num,target_num = MarryData.Instance:GetIntimacyNum(),config.marry_intimacy
    UH.SetText(self.Intimacy,Format(Language.Marry.IntimacyNum,task_num,target_num))
    local target_progress = Valve((task_num/target_num)/2,0.5)
    local day_progress_num = Valve((self.data:GetDayNum()/config.marry_time)/2,0.5)
    local progress_num = target_progress + day_progress_num
    self.Progress:SetText(DataHelper.GetPreciseDecimal(progress_num, 2) * 100 .. "%")
    self.Progress:SetProgress(progress_num)
end

function MarryTitleView:FlushTitleState(title_id)
    local title_config = AppearanceData.Instance:GetTitleConfig(title_id)
    local is_active = AppearanceData.Instance:GetActiveFlag(title_config.seq) == 1
    local is_equip = AppearanceData.Instance:IsEquipData(AppearanceConfig.Type.Title,title_config.seq)
    UH.SetInter(self.BtnInter,is_active)
    UH.SetText(self.BtnName,Language.Marry.TitleBtnName[is_active and (is_equip and 1 or 2) or 3])
    self.select_data = title_config
end

function MarryTitleView:FlushMaxProgressView()
    self.Progress:SetText("100%")
    self.Progress:SetProgress(1)
    UH.SetText(self.Intimacy)
end

function MarryTitleView:OnClickUse()
    if AppearanceData.Instance:IsEquipData(self.select_data.res_type,self.select_data.seq) then
        AppearanceCtrl.Instance:SendAddReq(2,self.select_data.res_type)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[20])
    else
        AudioMgr:PlayEffect(AudioType.UI, "equip_equipment")
        AppearanceCtrl.Instance:SendAddReq(1,self.select_data.seq)
        PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[19])
    end
end