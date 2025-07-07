RoleXiuWeiPanel = RoleXiuWeiPanel or DeclareMono("RoleXiuWeiPanel", UIWFlushPanel)
function RoleXiuWeiPanel:RoleXiuWeiPanel()
    self.data = RoleXiuWeiData.Instance
    self.skill_group_data = {
        {title = Language.RoleXiuWei.Tip1, skill_type_list = {0, 2, 4}, sel_index = 1},
        {title = Language.RoleXiuWei.Tip2, skill_type_list = {1, 3, 5}},
    }
    self.data_cares = {
        {data = self.data.skill_info_list, func = self.FlushPanel, init_call = false},
        {data = RoleData.Instance:GetBaseData(), func = self.FlushPanel, init_call = false, keys = {"level"}},
    }
end

function RoleXiuWeiPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function RoleXiuWeiPanel:FlushPanel()
    self:FlushLeft()
    self:FlushRight()
end

function RoleXiuWeiPanel:FlushLeft()
    for i = 1, self.SkillGroups:Length() do
        self.SkillGroups[i]:SetData(self.skill_group_data[i])
    end
end

function RoleXiuWeiPanel:FlushRight()
    local skill_data = self.sel_skill_data
    if not skill_data then
        return
    end
    self.RightPanel:SetData(skill_data)
end

-- data:RoleXiuWeiSkillItem.data
function RoleXiuWeiPanel:OnSkillItemClick(data)
    self.sel_skill_data = data
    self.data.sel_skill_data = data
    self:FlushRight()
end

-- =========================== 右边的信息 =================================
RoleXiuWeiRightPanel = RoleXiuWeiRightPanel or DeclareMono("RoleXiuWeiRightPanel", UIWFlushPanel)
function RoleXiuWeiRightPanel:RoleXiuWeiRightPanel()
    self.ctrl_data = RoleXiuWeiData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushStuff, init_call = false},
    }
end

-- data : Config.xiuwei_auto.xiuwei_dec
function RoleXiuWeiRightPanel:SetData(data)
    self.data = data
    self:FlushPanel()
end

function RoleXiuWeiRightPanel:FlushPanel()
    if not self.data then
        return
    end
    local data = self.data
    UH.SetIcon(self.SkillIcon, data.skill_icon)
    UH.SetText(self.SkillName, data.xiuwei_name)
    local skill_type = data.xiuwei_type
    local skill_info = self.ctrl_data:GetSkillInfo(skill_type)
    UH.SetText(self.Level, string.format(Language.RoleXiuWei.Tip3, skill_info.level))

    local next_skill_cfg = self.ctrl_data:GetSkillCfg(skill_type, skill_info.level + 1) or {}
    local has_exp = skill_info.exp
    local need_exp = next_skill_cfg.xiuwei_exp or 0
    local progress_num = has_exp / need_exp
    local x
    if need_exp == 0 then
        self.ProgressEx:SetText(Language.RoleXiuWei.Tip11)
        x = -213
        progress_num = 1
    else
        self.ProgressEx:SetText(string.format("%s/%s", has_exp, need_exp))
        x = -213 - (need_exp - has_exp)/need_exp * 416
    end
    self.ProEffect:SetLocalPosition(Vector3.New(x,1,0))
    if self.last_exp and self.last_exp ~= stamp_exp and skill_type == self.last_skill_type then
        if self.last_level and skill_info.level > self.last_level then
            self.EffectTool:Play("3165094")
            self.ProgressEx:AddToProgress(1,function ()
                self.ProgressEx:AddToProgress(progress_num)
            end)
            ViewMgr:OpenView(RoleXiuWeiUpShow, {data = data})
        else
            self.ProgressEx:AddToProgress(progress_num)
        end
    else
        self.ProgressEx:SetProgress(progress_num)
    end
    self.last_exp = has_exp
    self.last_level = skill_info.level
    self.last_skill_type = skill_type

    UH.SetText(self.Tip, data.xiuwei_dec)

    local attri_list_data = self.ctrl_data:GetSkillAttriListData(skill_type)
    self.AttriList:SetData(attri_list_data)
    self:FlushStuff()

    local role_level = RoleData.Instance:GetRoleLevel()
    if not TableIsEmpty(next_skill_cfg) and role_level < next_skill_cfg.open_level then
        self.UpTip:SetObjActive(true)
        UH.SetText(self.UpTip, string.format(Language.RoleXiuWei.Tip12, next_skill_cfg.open_level))
    else
        self.UpTip:SetObjActive(false)
    end
end

function RoleXiuWeiRightPanel:FlushStuff()
    local other_cfg = self.ctrl_data.cfg.other[1]
    self.Stuff1:SetData({
        item_id = other_cfg.cost1,
        exp = other_cfg.cost1_exp,
    })
    self.Stuff2:SetData({
        item_id = other_cfg.cost2,
        exp = other_cfg.cost2_exp,
    })
end



RoleXiuWeiSkillGroupItem = RoleXiuWeiSkillGroupItem or DeclareMono("RoleXiuWeiSkillGroupItem", UIWidgetBaseItem)
-- data : {title : string, [skill_type_list : RoleXiuWeiCtrl.SKILL_TYPE]}
function RoleXiuWeiSkillGroupItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Title, data.title)
    self.List:SetData(self:GetListData())
    if data.sel_index and not self.is_first then
        self.List:SetectItem(data.sel_index, true)
        self.is_first = true
    end
end

function RoleXiuWeiSkillGroupItem:GetListData()
    if not self.list_data then
        local skill_type_list = self.data.skill_type_list or {}
        local list_data = {}
        for i,v in ipairs(skill_type_list) do
            local data = RoleXiuWeiData.Instance:GetSkillDesc(v)
            table.insert(list_data, data)
        end
        self.list_data = list_data
    end
    return self.list_data
end



RoleXiuWeiSkillItem = RoleXiuWeiSkillItem or DeclareMono("RoleXiuWeiSkillItem", UIWidgetBaseItem)
-- data : Config.xiuwei_auto.xiuwei_dec
function RoleXiuWeiSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetIcon(self.Icon, data.skill_icon)
    UH.SetText(self.Name, data.xiuwei_name)
    local skill_type = data.xiuwei_type
    local max_level = RoleXiuWeiData.Instance:GetCanUpLevel(skill_type)
    local skill_info = RoleXiuWeiData.Instance:GetSkillInfo(skill_type)
    local skill_level = skill_info.level
    local color = skill_level < max_level and COLORSTR.Green10 or COLORSTR.Red10
    skill_level =ColorStr(skill_level, color)
    local level_str = string.format("%s/%s",skill_level, max_level)
    UH.SetText(self.Level, level_str)
end



RoleXiuWeiAttriItem = RoleXiuWeiAttriItem or DeclareMono("RoleXiuWeiAttriItem", UIWidgetBaseItem)
function RoleXiuWeiAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local flag = RoleXiuWeiData.Instance:GetUpFlag()
    self.Show1:SetActive(flag ~= 0)
    self.Show2:SetActive(flag == 0)
    if flag ~= 0 then
        UH.SetText(self.Name, data.name)
        UH.SetText(self.Value, data.value)
        UH.SetText(self.NextValue, data.next_value)
    else
        UH.SetText(self.Show2Name, data.name)
        UH.SetText(self.Show2Value, data.value)
    end
    if RoleXiuWeiData.Instance.sel_skill_data then
        if self.skill_type and RoleXiuWeiData.Instance.sel_skill_data.xiuwei_type == self.skill_type then
            if self.value and self.value ~= data.value then
                self.Effect:Play(3165091)
            end
        end
        self.skill_type = RoleXiuWeiData.Instance.sel_skill_data.xiuwei_type
        self.value = data.value
    end
end

RoleXiuWeiStuffItem = RoleXiuWeiStuffItem or DeclareMono("RoleXiuWeiStuffItem", UIWidgetBaseItem)
-- data : {item_id, exp}
function RoleXiuWeiStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item_id = data.item_id
    local num = Item.GetNum(item_id)
    local item = Item.Create({item_id = item_id, num =  DataHelper.ConsumeNum(num, 1, true)})
    self.Cell:SetData(item)
    UH.SetText(self.Name, item:QuaName())
    local exp = ColorStr(data.exp, COLORSTR.Red14)
    -- UH.SetText(self.Desc, string.format(Language.RoleXiuWei.Tip4, exp))
    UH.SetText(self.Desc, Format(Language.RoleXiuWei.ExpTip, item:QuaName(), exp))
    local use_count = 0
    local limit_count = 0
    if self.ItemType == RoleXiuWeiCtrl.ITEM_TYPE.QUA1 then
        use_count = RoleXiuWeiData.Instance.use_info.use_count_qua1
        limit_count = RoleXiuWeiData.Instance.cfg.other[1].use_times1
    else
        use_count = RoleXiuWeiData.Instance.use_info.use_count_qua2
        limit_count = RoleXiuWeiData.Instance.cfg.other[1].use_times
    end

    self.can_use_count = limit_count - use_count
    UH.SetText(self.Tip, string.format(Language.RoleXiuWei.Tip6, self.can_use_count))

    local flag = RoleXiuWeiData.Instance:GetUpFlag()
    self.Interactor.Interactable = flag ~= 0
    local btn_txt = flag == 0 and Language.RoleXiuWei.Btn2 or Language.RoleXiuWei.Btn1
    UH.SetText(self.BtnTxt, btn_txt)
end
function RoleXiuWeiStuffItem:OnClickOneKey()
    local skill_type = RoleXiuWeiData.Instance.sel_skill_data.xiuwei_type
    RoleXiuWeiCtrl.Instance:SendReq(RoleXiuWeiCtrl.REQ_TYPE.ONE_KEY, skill_type, self.ItemType)
end

function RoleXiuWeiStuffItem:OnUseClick()
    if TableIsEmpty(RoleXiuWeiData.Instance.sel_skill_data) then
        return
    end
    local skill_type = RoleXiuWeiData.Instance.sel_skill_data.xiuwei_type
    local skill_info = RoleXiuWeiData.Instance:GetSkillInfo(skill_type)
    local cfg = RoleXiuWeiData.Instance:GetSkillDesc(skill_type)
    if skill_info.level >= cfg.max_level then
        PublicPopupCtrl.Instance:Center(Language.RoleXiuWei.Tip7)
        TimeHelper:CancelTimer(self.down_ht)
        return
    end

    local skill_att_cfg = RoleXiuWeiData.Instance:GetSkillCfg(skill_type, skill_info.level + 1)
    local role_level = RoleData.Instance:GetRoleLevel()
    if role_level < skill_att_cfg.open_level then
        PublicPopupCtrl.Instance:Center(string.format(Language.RoleXiuWei.Tip12,skill_att_cfg.open_level))
        TimeHelper:CancelTimer(self.down_ht)
        return
    end

    local num = Item.GetNum(self.data.item_id)
    if num < 1 then
        PublicPopupCtrl.Instance:Center(Language.RoleXiuWei.Tip8)
        TimeHelper:CancelTimer(self.down_ht)
        local item = Item.Create({item_id = self.data.item_id})
        MainOtherCtrl.Instance:GetWayView({item = item})
        return
    end
    if self.can_use_count <= 0 then
        PublicPopupCtrl.Instance:Center(string.format(Language.RoleXiuWei.Tip9, Item.GetName(self.data.item_id)))
        TimeHelper:CancelTimer(self.down_ht)
        return
    end
    RoleXiuWeiCtrl.Instance:SendReq(RoleXiuWeiCtrl.REQ_TYPE.UP_LEVEL, skill_type,self.ItemType)
    if self.is_down then
        self:OnDown()
    end
end

function RoleXiuWeiStuffItem:OnDown()
    self.use_speed = self.use_speed or 0.3
    if not self.is_down then
        TimeHelper:CancelTimer(self.down_ht)
        self.is_down = true
        self.last_speed = self.use_speed
        self.down_ht = TimeHelper:AddRunTimer(BindTool.Bind(self.OnUseClick,self),self.use_speed)
    end
    self.use_speed = self.use_speed - 0.01
    if self.use_speed >= 0.1 and self.last_speed - self.use_speed >= 0.05 then
        self.is_down = false
        self:OnDown()
    end
end

function RoleXiuWeiStuffItem:OnUp()
    TimeHelper:CancelTimer(self.down_ht)
    self.is_down = false
    self.use_speed = 0.3
end

function RoleXiuWeiStuffItem:OnDestroy()
    TimeHelper:CancelTimer(self.down_ht)
end




-- ====================== 修为升级展示  = 修为升级展示 =========================
RoleXiuWeiUpShow = RoleXiuWeiUpShow or DeclareView("RoleXiuWeiUpShow", "role/role_xiu_wei_up_show")
VIEW_DECLARE_MASK(RoleXiuWeiUpShow,ViewMask.BgBlockClose)
function RoleXiuWeiUpShow:RoleXiuWeiUpShow()
    
end

function RoleXiuWeiUpShow:LoadCallback(param_t)
    param_t = param_t or {}
    local data = param_t.data
    if not data then
        return
    end
    local skill_type = data.xiuwei_type
    local skill_info = RoleXiuWeiData.Instance:GetSkillInfo(skill_type)
    local cur_level = skill_info.level
    local last_level = cur_level - 1
    UH.SetText(self.Name, data.xiuwei_name)
    UH.SetText(self.Desc, data.xiuwei_dec)
    UH.SetIcon(self.Icon, data.skill_icon)
    local attri_list_data = RoleXiuWeiData.Instance:GetSkillAttriListData(skill_type, last_level)
    table.insert(attri_list_data, 1, {is_level = true, value = last_level, next_value = cur_level})
    self.AttriList:SetData(attri_list_data)
end


RoleXiuWeiUpShowAttriItem = RoleXiuWeiUpShowAttriItem or DeclareMono("RoleXiuWeiUpShowAttriItem", UIWidgetBaseItem)
function RoleXiuWeiUpShowAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local name = data.name
    local value1 = data.value
    local value2 = data.next_value
    if data.is_level then
        name = Language.RoleXiuWei.Tip13
    end
    UH.SetText(self.Name, name)
    UH.SetText(self.Value1, value1)
    UH.SetText(self.Value2, value2)
end