----------个人改后缀----
SwornRenameRoleView = SwornRenameRoleView or DeclareView("SwornRenameRoleView", SwornConfig.ResPath .. "sworn_rename_role")
function SwornRenameRoleView:SwornRenameRoleView()
    self.cost_item = SwornData.Instance:RenameRoleCost()
    self.Board:SetData(self:GetType(), Language.Sworn.ChangeName, Vector2.New(475, 362))
end

function SwornRenameRoleView:LoadCallback()
    local info = SwornData.Instance:GetSelfInfo()
    local sworn_info = SwornData.Instance:GetSwornInfo()
    UH.SetText(self.PrefixText, sworn_info.jie_yi_name.sworn_name)
    UH.SetText(self.InputField, info.member_name)
    local is_first = info.has_first_member_name == 0
    self.FreeNode:SetActive(is_first)
    self.CostNode:SetActive(not is_first)

    local item_num = Item.GetNum(self.cost_item.item_id)
    local color = item_num >= self.cost_item.num and COLORSTR.Green3 or COLORSTR.Red8
    UH.SetIcon(self.ItemIcon, Item.GetIconId(self.cost_item.item_id), ICON_TYPE.ITEM)
    UH.SetText(self.ItemNum, Format(Language.Sworn.ChangeItemNum, color, item_num, self.cost_item.num))
end

function SwornRenameRoleView:OnClickChange()
    local block, notice_str = LoginData.Instance:BlockChangeName()
    if block then
        PublicPopupCtrl.Instance:Center(notice_str)
        return
    end

    local function callback(str)
        SwornCtrl.Instance:SendChangeName(str)
        PublicPopupCtrl.Instance:Center(Language.Sworn.ChangeTitleSucc)
        ViewMgr:CloseView(SwornRenameRoleView)
    end
    local input_name = self.InputField.text
    if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(input_name, 5, callback)
    else
        local info = SwornData.Instance:GetSelfInfo()
        if StringIsEmpty(input_name) or input_name == info.member_name then
            PublicPopupCtrl.Instance:Center(Language.Sworn.ChangeTitleFail)
            -- ViewMgr:CloseView(SwornRenameRoleView)
            return
        elseif DataHelper.GetStringWordNum(input_name) > 2 then
            PublicPopupCtrl.Instance:Center(Format(Language.Sworn.RenameError2, 2))
            return
        elseif ChatFilter.Instance:IsIllegal(input_name, true) then
            PublicPopupCtrl.Instance:Center(Language.Chat.ErrorCode.IllegalContent)
            return
        elseif SwornData.IsIllegal(input_name) then
            PublicPopupCtrl.Instance:Center(Language.Sworn.RenameError5)
            return
        end
        local is_first = info.has_first_member_name == 0
        if not is_first and Item.GetNum(self.cost_item.item_id) < self.cost_item.num then
            MainOtherCtrl.Instance:GetWayViewById(self.cost_item.item_id)
            return
        end
        callback(input_name)
    end
end

function SwornRenameRoleView:OnClickClose()
    ViewMgr:CloseView(SwornRenameRoleView)
end

function SwornRenameRoleView:OnClickItem()
    MainOtherCtrl.Instance:GetWayViewById(self.cost_item.item_id)
end

----------组长改称号名----
SwornRenameTeamView = SwornRenameTeamView or DeclareView("SwornRenameTeamView", SwornConfig.ResPath .. "sworn_rename_team")
function SwornRenameTeamView:SwornRenameTeamView()
    self.cost_item = SwornData.Instance:RenameTeamCost()
    self.Board:SetData(self:GetType(), Language.Sworn.ChangeName, Vector2.New(475, 362))
end

function SwornRenameTeamView:LoadCallback()
    local sworn_info = SwornData.Instance:GetSwornInfo()
    UH.SetText(self.PrefixText, DataHelper.GetWord(sworn_info.jie_yi_name.member_num))
    UH.SetText(self.InputPrefix, sworn_info.jie_yi_name.str1)
    UH.SetText(self.InputSuffix, sworn_info.jie_yi_name.str2)

    local item_num = Item.GetNum(self.cost_item.item_id)
    local color = item_num >= self.cost_item.num and COLORSTR.Green3 or COLORSTR.Red8
    UH.SetIcon(self.ItemIcon, Item.GetIconId(self.cost_item.item_id), ICON_TYPE.ITEM)
    UH.SetText(self.ItemNum, Format(Language.Sworn.ChangeItemNum, color, item_num, self.cost_item.num))
end

function SwornRenameTeamView:OnClickChange()
    local block, notice_str = LoginData.Instance:BlockChangeName()
    if block then
        PublicPopupCtrl.Instance:Center(notice_str)
        return
    end

    

    local prefix_str = self.InputPrefix.text
    local suffix_str = self.InputSuffix.text
    local sworn_info = SwornData.Instance:GetSwornInfo()
    if prefix_str == "" or suffix_str == "" or (prefix_str == sworn_info.jie_yi_name.str1 and suffix_str == sworn_info.jie_yi_name.str2) then
        PublicPopupCtrl.Instance:Center(Language.Sworn.ChangeTitleFail)
        -- ViewMgr:CloseView(SwornRenameTeamView)
        return
    end
    
    if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(prefix_str, 1, function (str_f_1)
            ChatSubCustom.PCChatFilter(suffix_str, 1, function (str_f_2)
                PublicPopupCtrl.Instance:Center(Language.Sworn.ChangeTitleSucc)
                SwornCtrl.Instance:SendChangeSwornNameReq(str_f_1, str_f_2)
                ViewMgr:CloseView(SwornRenameTeamView)
            end)
        end)
	else
        if DataHelper.GetStringWordNum(prefix_str) ~= 2 then
            PublicPopupCtrl.Instance:Center(Language.Sworn.RenameError3)
            return
        end
        if DataHelper.GetStringWordNum(suffix_str) ~= 1 then
            PublicPopupCtrl.Instance:Center(Language.Sworn.RenameError4)
            return
        end
        local join_str = prefix_str .. DataHelper.GetWord(sworn_info.jie_yi_name.member_num) .. suffix_str
        if ChatFilter.Instance:IsIllegal(join_str, true) then
            PublicPopupCtrl.Instance:Center(Language.Chat.ErrorCode.IllegalContent)
            return
        end
        if SwornData.IsIllegal(join_str) then
            PublicPopupCtrl.Instance:Center(Language.Sworn.RenameError5)
            return
        end
        if Item.GetNum(self.cost_item.item_id) < self.cost_item.num then
            MainOtherCtrl.Instance:GetWayViewById(self.cost_item.item_id)
            return
        end
        PublicPopupCtrl.Instance:Center(Language.Sworn.ChangeTitleSucc)
        SwornCtrl.Instance:SendChangeSwornNameReq(prefix_str, suffix_str)
        ViewMgr:CloseView(SwornRenameTeamView)
    end
end

function SwornRenameTeamView:OnClickClose()
    ViewMgr:CloseView(SwornRenameTeamView)
end

function SwornRenameTeamView:OnClickItem()
    MainOtherCtrl.Instance:GetWayViewById(self.cost_item.item_id)
end


-- 结义详情
SwornDetailView = SwornDetailView or DeclareView("SwornDetailView", SwornConfig.ResPath .. "sworn_detail")
function SwornDetailView:SwornDetailView()
    self.Board:SetData(self:GetType(), Language.Sworn.Title1, Vector2.New(355, 362))
    self.data = SwornData.Instance
end

function SwornDetailView:LoadCallback()
    self.care_ht = self.data.sworn_info:Care(BindTool.Bind(self.FlushPanel, self))
    self:FlushPanel()
end

function SwornDetailView:FlushPanel()
    local leader_info = self.data:LeaderInfo()
    if not leader_info then
        return
    end
    UH.SetText(self.Name, string.format(Language.Sworn.Text1, leader_info.name))

    local sworn_info = self.data.sworn_info
    local time_t = os.date("*t", sworn_info.create_time)
    UH.SetText(self.StartTime, Format(Language.Sworn.SwornTime3, time_t.year, time_t.month, time_t.day))

    local my_info = self.data:MyInfo() or {}
    time_t = os.date("*t", my_info.join_time or 0)
    UH.SetText(self.JoinTime, Format(Language.Sworn.SwornTime4, time_t.year, time_t.month, time_t.day))

    if self.data:IsTanHe() then
        UH.SetText(self.BtnText, Language.Sworn.Text2)
        UH.SetInter(self.BtnInter, false)
        if self.is_can_tanhe then
            ViewMgr:OpenView(SwornTanHeDesc)
        end
        self.is_can_tanhe = false
    else
        UH.SetText(self.BtnText, Language.Sworn.Text3)
        UH.SetInter(self.BtnInter, true)
        self.is_can_tanhe = true
    end
end

function SwornDetailView:CloseCallback()
    self.data.sworn_info:Uncare(self.care_ht)
end

function SwornDetailView:OnTanHeClick()
    local leader_info = self.data:LeaderInfo()
    if not leader_info then
        return
    end
    if not self.is_can_tanhe then
        PublicPopupCtrl.Instance:Center(Language.Sworn.Text4)
        return
    end

    local cur_time = TimeCtrl.Instance:GetServerTime()
    local need_time = Config.deep_friendship_auto.other[1].tan_he_not_online_time
    --LOG(">>leader_info",leader_info)
    if leader_info.online_state == ONLINE_STATUS_TYPE.TYOE_OFFLINE and cur_time - leader_info.last_login_time >= need_time then
        SwornCtrl.Instance:SendSwornReq(SwornConfig.JIE_YI_REQ_RET_TYPE.TYPE_TAN_HE)
    else
        local name = ColorStr(leader_info.name, COLORSTR.Yellow6)
        local day = ColorStr(math.ceil(need_time / 86400), COLORSTR.Yellow6)
        PublicPopupCtrl.Instance:Center(string.format(Language.Sworn.Text7, day))
    end
end


SwornTanHeDesc = SwornTanHeDesc or DeclareView("SwornTanHeDesc", SwornConfig.ResPath .. "sworn_tan_he_desc")
function SwornTanHeDesc:SwornTanHeDesc()
    self.Board:SetData(self:GetType(), Language.Sworn.Title2, Vector2.New(490, 350))
end

function SwornTanHeDesc:LoadCallback(param_t)
    local name = param_t.name or Language.Sworn.Text6
    local desc = string.format(Language.Sworn.TanheDesc, name, name, name, name)
    UH.SetText(self.Desc, desc)
end

function SwornTanHeDesc:OnOkClick()
    ViewMgr:CloseView(SwornTanHeDesc)
end