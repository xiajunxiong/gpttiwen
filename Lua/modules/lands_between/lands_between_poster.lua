LandsBetweenPosterView = LandsBetweenPosterView or DeclareView("LandsBetweenPosterView", "lands_between/lands_between_poster",Mod.LandsBetween.Poster)
function LandsBetweenPosterView:LandsBetweenPosterView()

end

function LandsBetweenPosterView:OnCloseClick()
    ViewMgr:CloseView(LandsBetweenPosterView)
end

LandsBetweenPosterPanel = LandsBetweenPosterPanel or DeclareMono("LandsBetweenPosterPanel", UIWFlushPanel)
function LandsBetweenPosterPanel:LandsBetweenPosterPanel()
    self.data = LandsBetweenData.Instance

    self.data_cares = {
        {data = self.data.base_info, func = self.FlushPanel},
    }
end 

function LandsBetweenPosterPanel:FlushPanel()
    local param = self.data:GetPosterInfo()

    -- for i = 1,2 do 
    --     self["Obj"..i]:SetActive(i == param.season)
    -- end 
    local high_num = math.floor(param.season/10)
    local low_num = math.floor(param.season%10)
    if high_num == 0 and low_num == 0 then
        self.TenShow:SetActive(false)
        self.TwiceShow:SetActive(false)
        self.SingleShow:SetActive(true)
        low_num = 1
    elseif high_num == 0 and low_num ~= 0 then -- 1 2 3 4 
        self.TenShow:SetActive(false)
        self.TwiceShow:SetActive(false)
        self.SingleShow:SetActive(true)
    elseif (high_num ~= 0 and high_num ~= 1) and low_num == 0 then -- 30 40 50
        self.TenShow:SetActive(true)
        self.TwiceShow:SetActive(true)
        self.SingleShow:SetActive(false)
    elseif high_num == 1 and low_num == 0 then -- 10
        self.TenShow:SetActive(true)
        self.TwiceShow:SetActive(false)
        self.SingleShow:SetActive(false)
    elseif high_num == 1 and low_num ~= 0 then -- 11 12 13 
        self.TenShow:SetActive(true)
        self.TwiceShow:SetActive(false)
        self.SingleShow:SetActive(true)
    elseif (high_num ~= 0 and high_num ~= 1) and low_num ~= 0 then -- 21 32 43 
        self.TenShow:SetActive(true)
        self.TwiceShow:SetActive(true)
        self.SingleShow:SetActive(true)
    end 

    UH.SetText(self.SingleText,DataHelper.GetDaXie(low_num))
    UH.SetText(self.TwiceText,DataHelper.GetDaXie(high_num))

    -- self.TwiceText
    
    UH.SetText(self.CurStatus,param.status_str)

    self.Timer.gameObject:SetActive(param.is_running)
    self.TimeShow.gameObject:SetActive(not param.is_running)

    if param.is_running then 
        self.Timer:StampTime(param.time_stamp,TimeType.Type_Time_2,Language.LandsBetween.Desc.OpenWaitShow[5])
    else 
        UH.SetText(self.TimeShow,param.time_show)
    end 

    self.BtnInter.Interactable = not param.is_running
end

function LandsBetweenPosterPanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[386].desc})
end

function LandsBetweenPosterPanel:OnClickGoto()
    local param = self.data:GetPosterInfo()
    if not self.BtnInter.Interactable then 

        if self.data:GetCurStatus() < 3 then 
            PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.BtnActUnOpen)
        elseif self.data:GetCurStatus() == 6 and param.season == 0 then 
            PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.BtnActUnOpen)
        else 
            PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.BtnActComplete)
        end 
        
        if not DEBUG_MODE then 
            return 
        end 
    end 


    local role_level = RoleData.Instance:GetRoleLevel()

    if role_level < Config.divine_domain_system_auto.other[1].level_limit then
        PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.BtnActUnLevel)
        return 
    end


    ViewMgr:CloseView(LandsBetweenPosterView)
    LandsBetweenCtrl.Instance:TryOpenMain()
    -- LandsBetweenCtrl.Instance:OpenMainView()
end