MarryCertificateView = MarryCertificateView or DeclareView("MarryCertificateView", "marry/marry_certificate")

function MarryCertificateView:MarryCertificateView()
    self.data = MarryData.Instance
end

function MarryCertificateView:LoadCallback(param_t)
    local role_info = self.data:GetRoleInfo()
    local language = Language.Marry.Certificate
    
    for i=1,self.Head:Length() do
        UH.SetText(self.Name[i],role_info[i].role_name)
        local appearance = role_info[i].appearance or {}
        appearance.image_type = AvatarImageType.Other
        self.Head[i]:SetData(appearance)
    end

    UH.SetText(self.Intimacy,Format(Language.Society.ChatIntimacyShow,self.data:GetIntimacyNum()))
    UH.SetText(self.StartTime,Format(language.StartTime,self:GetStartTime()))

    local curr_config = self.data:GetAssessDayConfig(self.data:GetDayNum())
    local next_config = self.data:GetAssessConfig(curr_config.seq + 1)

    UH.SetText(self.TotalTime,Format(language.TotalTime,curr_config.marry_assess,self:GetTotalTime()))
    UH.SetText(self.NextTime,next_config and Format(language.NextTime,next_config.marry_time - self.data:GetDayNum()) or "")
end

function MarryCertificateView:OnClickClose()
    ViewMgr:CloseView(MarryCertificateView)
end

function MarryCertificateView:GetStartTime()
    local t = os.date("*t",self.data:GetCreateTime())
    return Format(Language.UiTimeMeter.TimeStr4, t.year, t.month ,t.day)
end

function MarryCertificateView:GetTotalTime()
    local total_time = TimeHelper:GetServerTime() - self.data:GetCreateTime()
    local t = TimeHelper.FormatDHMS(total_time)
    return Format(Language.UiTimeMeter.TimeStr6,t.day, t.hour)
end