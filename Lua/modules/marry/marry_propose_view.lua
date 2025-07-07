MarryProposeView = MarryProposeView or DeclareView("MarryProposeView", "marry/marry_propose")

function MarryProposeView:MarryProposeView()
    self.data = MarryData.Instance
end

function MarryProposeView:OnClickClose()
    ViewMgr:CloseView(MarryProposeView)
end

function MarryProposeView:LoadCallback(param_t)
    if param_t.propose_type ~= MarryConfig.PROPOSE_TYPE.WEDDING then
        local role_info = self:GetRoleInfoList(param_t.role_id)
        for i=1,self.Head:Length() do
            UH.SetText(self.Name[i],role_info[i].name)
            self.Head[i]:SetData(role_info[i].appearance or role_info[i])
        end
    end
    if param_t.propose_type == MarryConfig.PROPOSE_TYPE.INVITE then
        local role_info = self:GetRoleInfoList(param_t.role_id)
        self.cancel_func = function()
            MarryCtrl.Instance:SendMarryReq(MarryConfig.REQUEST_TYPE.INVITE_REPLY,param_t.role_id,0)
        end
        self.confirm_func = function()
            MarryCtrl.Instance:SendMarryReq(MarryConfig.REQUEST_TYPE.INVITE_REPLY,param_t.role_id,1)
            ViewMgr:OpenViewByKey(Mod.Society.Marry)
            PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[12])
        end
        local config = Config.get_married_auto.marry_base[param_t.marry_type]
        self.Time:SetCallBack(function()
            self:OnClickLeft()
        end)
        self.Time:TotalTime(Config.get_married_auto.other[1].add_times,TimeType.Type_Special_2,Language.Marry.ErrorText[4])
        self.Grid:SetData(DataHelper.FormatList(config.bugreward_show))
        UH.SetText(self.Hunter,role_info[1].name)
        UH.SetText(self.Desc,Format(Language.Marry.ErrorText[5],config.type_name))
        UH.SetText(self.LeftName,Language.Marry.BtnName[6])
        UH.SetText(self.RightName,Language.Marry.BtnName[5])
    elseif param_t.propose_type == MarryConfig.PROPOSE_TYPE.REPLY then
        self.confirm_func = function()
            PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[11])
        end
        self.cancel_func = function()
            ViewMgr:OpenView(MarryOrderView)
        end
        UH.SetText(self.LeftName,Language.Marry.BtnName[8])
        UH.SetText(self.RightName,Language.Marry.BtnName[7])
        PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[12])
    elseif param_t.propose_type == MarryConfig.PROPOSE_TYPE.WEDDING then
        if param_t.role_info then
            local role_info = param_t.role_info
            for i=1,self.Head:Length() do
                UH.SetText(self.Name[i],role_info[i].name)
                self.Head[i]:SetData(role_info[i].appearance or role_info[i])
            end
            for i=1,self.PreName:Length() do
                UH.SetText(self.PreName[i],self.data:GetMarryPreName(role_info[i]))
            end
        end
        UH.SetText(self.DateDesc,param_t.date_desc)
        UH.SetText(self.BtnName,param_t.btn_name)
        self.confirm_func = param_t.confirm_func
    end
    self.Panel[param_t.propose_type]:SetActive(true)
end

function MarryProposeView:OnClickLeft()
    self.Time:CloseCountDownTime()
    ViewMgr:CloseView(MarryProposeView)
    if self.cancel_func then
        self.cancel_func()
    end
end

function MarryProposeView:OnClickRight()
    self.Time:CloseCountDownTime()
    ViewMgr:CloseView(MarryProposeView)
    if self.confirm_func then
        self.confirm_func()
    end
end

function MarryProposeView:OnClickAppoint()
    if not LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:AlertTip(Language.Marry.WeddingCrossTips,function()
            LoginCtrl.Instance:SendCrossStartReq()
        end)
        return
    end
    ViewMgr:CloseView(MarryProposeView)
    if self.confirm_func then
        self.confirm_func()
    end
end

function MarryProposeView:GetRoleInfoList(role_id)
    if role_id ~= nil then
        return {SocietyData.Instance:GetFriendInfoByUid(role_id),RoleData.Instance:GetRoleData()}
    end
    return self.data:GetRoleInfo()
end