LunJianSignupView = LunJianSignupView or DeclareView("LunJianSignupView", "lun_jian/lun_jian_signup",Mod.LunJian.Signup)
VIEW_DECLARE_LEVEL(LunJianSignupView,ViewLevel.L1)

function LunJianSignupView:LunJianSignupView()
 
end

function LunJianSignupView:LoadCallback()

end

function LunJianSignupView:CloseCallback()

end



------------------------------------------------------------------------

LunJianSignupPanel = LunJianSignupPanel or DeclareMono("LunJianSignupPanel", UIWFlushPanel)
--CountTxt========Text
--RewardItems=========UIWidgetList[UIWItemCell]
--BtnTxt==========Text
--SignupedNumTxt=========Text
--TimeDescTxt======Text
--SeasonTxt======Text
--RuleTitles===Text[4]
--RuleDescs===Text[4]
--SignupBtnInter===InteractorHandler
--SignupRewardItems===UIWidgetList[UIWItemCell]
--SignupRewardGetted==GameObject


function LunJianSignupPanel:LunJianSignupPanel()
    local cur_season = LunJianData.Instance:CurSeason()
    self.lang = Language.LunJian.Signup
    if cur_season <= 0 then
        self.SeasonTxt:SetObjActive(false)
        self.CountTxt:SetObjActive(false)
    else
        self.SeasonTxt:SetObjActive(true)
        UH.SetText(self.SeasonTxt,"S" .. tostring(cur_season))
        self.CountTxt:SetObjActive(true)
        UH.SetText(self.CountTxt,string.format(self.lang.SeasonCount,DataHelper.GetDaXie(cur_season,false)))
    end
    
    local mounth = os.date("%m")
    UH.SetText(self.TimeDescTxt,string.format(self.lang.SimpleDesc))
    for i=1,4 do
        UH.SetText(self.RuleTitles[i],self.lang.RuleTitles[i])
        UH.SetText(self.RuleDescs[i],self.lang.RuleDetails[i])
    end

    local reward_item_data = LunJianData.OtherCfg().item_list
    local reward_items = {}
    for _,v in pairs(reward_item_data) do
        table.insert(reward_items,Item.Create(v))
    end
    self.RewardItems:SetData(reward_items)
    self.data_cares = {
		{data = LunJianData.Instance:MyInfo(), func = self.FlushSignupInfo, keys = {"has_signup"}},
        {data = LunJianData.Instance:SeasonInfo(), func = self.FlushSignupInfo, keys = {"cur_status"},init_call = false},
        {data = LunJianData.Instance:SeasonInfo(), func = self.SignupedNum, keys = {"signup_num"}},
        {data = LunJianData.Instance:MyInfo(), func = self.FlushSignupReward, keys = {"has_fetch_signup_reward"}},
    }
end

function LunJianSignupPanel:SignupedNum()
    UH.SetText(self.SignupedNumTxt,string.format(self.lang.SignupedNum,LunJianData.Instance:SignupNum()))

end

function LunJianSignupPanel:FlushSignupReward()
    local list_data = DataHelper.FormatItemList(LunJianData.SignupRewardCfg(),0)
    self.SignupRewardItems:SetData(list_data)
    self.SignupRewardGetted.gameObject:SetSiblingIndex(self.SignupRewardItems.gameObject.transform.childCount)
    self.SignupRewardGetted:SetActive(LunJianData.Instance:HasFetchSignupReward())
end

function LunJianSignupPanel:FlushSignupInfo()
    local btn_txt
    if LunJianData.Instance:HasSignuped() then
        btn_txt = Language.LunJian.Signup.BtnSignuped
    else
        if LunJianData.Instance:SeasonStatus() == LunJianData.Status.Signup then
            btn_txt = Language.LunJian.Signup.BtnSignup
        elseif LunJianData.Instance:CurSeason() <= 0 then
            btn_txt = Language.LunJian.Signup.BtnComingSoon
        else
            btn_txt = Language.LunJian.Signup.BtnSignupEnd
        end
    end
    UH.SetText(self.BtnTxt,btn_txt)
    local show_gray = LunJianData.Instance:SeasonStatus() ~= LunJianData.Status.Signup or
        LunJianData.Instance:HasSignuped() or 
        not FunOpen.Instance:IsFunOpen(Mod.LunJian.Main)
    self.SignupBtnInter.Interactable = not show_gray
end

function LunJianSignupPanel:OnClickClose()
    ViewMgr:CloseView(LunJianSignupView)
end

function LunJianSignupPanel:OnClickInfo()
    LunJianCtrl.OpenMainView(LunJianMainView.Tab.Reward)
    -- ViewMgr:OpenView(LunJianMainView)
end

function LunJianSignupPanel:OnClickSignup()
    if  LunJianData.Instance:HasSignuped() then
        PublicPopupCtrl.Instance:Center(Language.LunJian.Signup.SignupedHint)
    elseif not FunOpen.Instance:IsFunOpen(Mod.LunJian.Main) then
        PublicPopupCtrl.Instance:Center(Language.LunJian.Signup.NeedLvHint)
    elseif LunJianData.Instance:SeasonStatus() ~= LunJianData.Status.Signup then
        if LunJianData.Instance:CurSeason() <= 0 then
            PublicPopupCtrl.Instance:Center(Language.LunJian.NoReportDataHint)
        else
            PublicPopupCtrl.Instance:Center(Language.LunJian.Signup.SignupEndTips)
        end
    else
        LunJianCtrl.Instance:ReqSignup()
        LunJianCtrl.Instance:ReqFetchSignupReward()
        LunJianCtrl.Instance:ReqSeasonInfo()
    end
    -- LogError("OnClickSignup")
end