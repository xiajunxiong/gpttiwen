SwornAffirmView = SwornAffirmView or DeclareView("SwornAffirmView", SwornConfig.ResPath .. "sworn_affirm")
function SwornAffirmView:SwornAffirmView()
    self.sure_wait = 300 --确定等待300秒
end

function SwornAffirmView:CloseCallback()
    if self.handle_info ~= nil then
        SwornData.Instance.create_info:Uncare(self.handle_info)
        self.handle_info = nil
    end
end

function SwornAffirmView:LoadCallback()
    if self.handle_info == nil then
        self.handle_info = SwornData.Instance.create_info:Care(BindTool.Bind1(self.FlushView, self))
    end
    self:FlushView()
end

function SwornAffirmView:GetInfo()
    if SwornData.Instance.create_info.is_name_finish == nil then
        return nil
    end
    return SwornData.Instance.create_info
end

function SwornAffirmView:GetSelfInfo()
    local info = self:GetInfo()
    for i, v in pairs(info.role_info) do
        if v.uid == RoleData.Instance:GetRoleId() then
            return v
        end
    end
end

function SwornAffirmView:IsLeader()
    return self:GetSelfInfo().is_zu_zhang == 1
end
function SwornAffirmView:IsAffirm()
    return self:GetSelfInfo().is_confirm == 1
end

function SwornAffirmView:FlushView()
    local create_info = self:GetInfo()
    if create_info == nil then
        return
    end
    self.MemberList:SetData(create_info.role_info)

    local is_finish = create_info.is_name_finish == 1
    self.InputNode:SetActive(not is_finish)
    self.AffirmNode:SetActive(is_finish)
    if not is_finish then
        self:FlushInputView()
    else
        self:FlushAffirmView()
    end
    self.BtnClose:SetActive(is_finish or self:IsLeader())
end

function SwornAffirmView:FlushInputView()
    local create_info = self:GetInfo()
    local is_leader = self:IsLeader()
    -- self.BtnOk:SetActive(is_leader)
    self.InputState:SetActive(not is_leader)
    UH.SetText(self.TitleName, Language.Sworn.DIYTitle)
    UH.SetText(self.TitleMiddle, DataHelper.GetWord(create_info.jie_yi_name.member_num))
    if not is_leader then
        UH.SetInter(self.InputPrefixInter, false)
        UH.SetInter(self.InputSuffixInter, false)
        UH.SetText(self.InputPrefix, create_info.jie_yi_name.str1)
        UH.SetText(self.InputSuffix, create_info.jie_yi_name.str2)
    end
    UH.InputAddListener(self.InputPrefix, BindTool.Bind1(self.OnPrefixChange, self))
    UH.InputAddListener(self.InputSuffix, BindTool.Bind1(self.OnSuffixChange, self))
    self.last_preifx = self.InputPrefix.text
    self.last_suffix = self.InputSuffix.text
end

function SwornAffirmView:OnPrefixChange()
    if self:IsLeader() then
        if self.InputPrefix.text ~= self.last_preifx then
            self:TrySendValueChange()
        end
        self.last_preifx = self.InputPrefix.text
    end
end

function SwornAffirmView:OnSuffixChange()
    if self:IsLeader() then
        if self.InputSuffix.text ~= self.last_preifx then
            self:TrySendValueChange()
        end
        self.last_suffix = self.InputSuffix.text
    end
end

function SwornAffirmView:TrySendValueChange()
    local function func_send()
        self.value_timer = nil
        local create_info = self:GetInfo()
        SwornCtrl.Instance:SendUpdateSwornName(self.InputPrefix.text, self.InputSuffix.text, create_info.jie_yi_name.member_num)
    end
    if self.value_timer ~= nil then
        TimeHelper:CancelTimer(self.value_timer)
    end
    self.value_timer = Invoke(func_send, 0.5)
end

function SwornAffirmView:FlushAffirmView()
    local create_info = self:GetInfo()
    UH.SetText(self.TitleName, create_info.jie_yi_name.sworn_name)
    local time_t = os.date("*t", create_info.create_time)
    UH.SetText(self.SwornTime, Format(Language.Sworn.SwornTime, self:YearToWord(time_t.year), DataHelper.GetWord(time_t.month), DataHelper.GetWord(time_t.day)))

    local sure_time = create_info.create_time + self.sure_wait
    self.RemainTime:SetCallBack(BindTool.Bind1(self.OnTimeOver, self))
    self.RemainTime:StampTime(sure_time, TimeType.Type_Special_0)
    local is_affirm = self:IsAffirm()
    UH.SetText(self.BtnAffirmText, is_affirm and Language.Sworn.SwornSure[1] or Language.Sworn.SwornSure[0])
    UH.SetInter(self.BtnAffirmInter, not is_affirm)
end

function SwornAffirmView:YearToWord(year)
    local post, list = DataHelper.GetValueLength(2021)
    local word_str = ""
    for i = 1, post do
        word_str = word_str .. Language.DataHelper.Word[tonumber(list[i])]
    end
    return word_str
end

function SwornAffirmView:OnTimeOver()
    if self:GetInfo() ~= nil then
        local info = self:GetInfo()
        local no_agrees = {}
        for i, v in pairs(info.role_info) do
            if v.is_confirm ~= 1 then
                table.insert(no_agrees, v.name)
            end
        end
        if #no_agrees > 0 then
            local name_str = ""
            for i, v in ipairs(no_agrees) do
                if i == 1 then
                    name_str = name_str .. v
                else
                    name_str = name_str .. "、" .. v
                end
            end
            PublicPopupCtrl.Instance:Center(Format(Language.Sworn.SwornError6, name_str))
        end
    end
    ViewMgr:CloseView(SwornAffirmView)
end

--队长点击结义名称
function SwornAffirmView:OnClickOk()
    if self:GetInfo() == nil then
        return
    end
    if not self:IsLeader() then
        PublicPopupCtrl.Instance:Center(Language.Sworn.SwornError7)
        return
    end
    local prefix_str = self.InputPrefix.text
    local suffix_str = self.InputSuffix.text
    if DataHelper.GetStringWordNum(prefix_str) ~= 2 then
        PublicPopupCtrl.Instance:Center(Language.Sworn.RenameError3)
        return
    end
    if DataHelper.GetStringWordNum(suffix_str) ~= 1 then
        PublicPopupCtrl.Instance:Center(Language.Sworn.RenameError4)
        return
    end
    local create_info = self:GetInfo()
    local join_str = prefix_str .. DataHelper.GetWord(create_info.jie_yi_name.member_num) .. suffix_str
    if ChatFilter.Instance:IsIllegal(join_str, true) then
        PublicPopupCtrl.Instance:Center(Language.Chat.ErrorCode.IllegalContent)
        return
    end
    if SwornData.IsIllegal(join_str) then
        PublicPopupCtrl.Instance:Center(Language.Sworn.RenameError5)
        return
    end
    SwornCtrl.Instance:SendUpdateSwornName(self.InputPrefix.text, self.InputSuffix.text, create_info.jie_yi_name.member_num)
    SwornCtrl.Instance:SendLeaderNameFinishReq()
end

--队员同意
function SwornAffirmView:OnClickAffirm()
    SwornCtrl.Instance:SendMemberIsAffirm(true)
end

function SwornAffirmView:OnClickClose()
    local info = {
        content = Language.Sworn.SwornAlert1,
        confirm = {
            func = function()
                SwornCtrl.Instance:SendMemberIsAffirm(false)
                ViewMgr:CloseView(SwornAffirmView)
                PublicPopupCtrl.Instance:CloseDialogTips()
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

SwornAffirmMember = SwornAffirmMember or DeclareMono("SwornAffirmMember", UIWidgetBaseItem)
function SwornAffirmMember:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = data.info
    UH.SetText(self.NameText, data.name)
    self.SwornAvatar:SetData({avatar_type = data.avatar_type, avatar_id = data.avatar_id, avatar_quality = data.avatar_quality,level = data.level})
    self.ImgAffirm:SetActive(data.is_confirm == 1)
    self.LeaderTag:SetActive(data.is_zu_zhang == 1)
end
