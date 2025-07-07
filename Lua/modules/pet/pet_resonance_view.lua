PetResonanceView = PetResonanceView or DeclareView("PetResonanceView", "pet/pet_resonance_view")
function PetResonanceView:PetResonanceView()
    self.Board:SetData(self:GetType(),Language.PetResonance.Title,Vector2.New(1148, 646))
end

function PetResonanceView:OnTipClick()
    PublicPopupCtrl.Instance:HelpTip(335)
end


PetResonancePanel = PetResonancePanel or DeclareMono("PetResonancePanel", UIWFlushPanel)
function PetResonancePanel:PetResonancePanel()
    self.data = PetResonanceData.Instance
    self.tab_list_data = {
        {qua = 2},
        {qua = 3},
        {qua = 4},
        {qua = 5},
    }
    self.sel_qua_data = self.tab_list_data[1]
    self.data_cares = {
        {data = self.data.info_data, func = self.FlushPanel, init_call = false},
    }
end

function PetResonancePanel:Awake()
    UIWFlushPanel.Awake(self)
    
    local tab_index = 1
    local pet = PetData.Instance:GetCurSelect()
    if pet then
        local qua = pet:Quality()
        if qua > 1 and qua <= 5 then
            tab_index = qua - 1
            self.sel_qua_data = self.tab_list_data[tab_index]
        end
    end

    self.TabList:SetData(self.tab_list_data)
    self.TabList:SetectItem(tab_index,true)
    self.AttriList:SetCompleteCallBack(function ()
        self.AttriList:SetObjActive(true)
    end)
    self:FlushPanel()
end

function PetResonancePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetSelCell(nil)
    TimeHelper:CancelTimer(self.j_time_ht)
    --TimeHelper:CancelTimer(self.time_ht)
end

function PetResonancePanel:FlushPanel()
    -- TimeHelper:CancelTimer(self.time_ht)
    -- self.time_ht = Invoke(function ()
    --     self.data:CheckResonance()
    -- end,1.5)
    
    self:FlushInfo()
    self:FlushAttriInfo()
end

function PetResonancePanel:FlushInfo()
    local qua = self.sel_qua_data.qua
    local max_cell = self.data:GetMaxCell(qua)
    local list = {}
    for i = 1, max_cell do
        local t = {}
        t.cell_index = i
        t.qua = qua
        table.insert(list, t)
    end

    table.sort(list, function (a,b)
        local a_num = 0
        local b_num = 0
        local a_info = self.data:GetCellInfo(qua, a.cell_index)
        local b_info = self.data:GetCellInfo(qua, b.cell_index)
        if a_info and a_info.uid_1 ~= 0 then
            a_num = 1000
            local pet = self.data:GetPet(a_info)
            if pet then
                a_num = a_num + pet:StrLevel()
            end
        end
        if b_info and b_info.uid_1 ~= 0 then
            b_num = 1000
            local pet = self.data:GetPet(b_info)
            if pet then
                b_num = b_num + pet:StrLevel()
            end
        end
        if a_num == b_num then
            return a.cell_index < b.cell_index
        end
        return a_num > b_num
    end)

    self.PetList:SetData(list)
end

function PetResonancePanel:FlushAttriInfo()
    local qua = self.sel_qua_data.qua
    local list_data = self.data:GetQuaCfgList(qua)

    -- 又不置顶了
    -- local res_level = PetResonanceData.Instance:GetQuaLevel(qua)
    -- table.sort(list_data, function (a,b)
    --     local a_num = a.resonance_stage
    --     local b_num = b.resonance_stage
    --     if a_num == res_level then
    --         a_num = a_num - 10000
    --     end
    --     if b_num == res_level then
    --         b_num = b_num - 10000
    --     end
    --     return a_num < b_num
    -- end)

    local res_level = PetResonanceData.Instance:GetQuaLevel(qua)
    local index
    for i,v in pairs(list_data) do
        if v.resonance_stage == res_level then
            index = i
            break
        end
    end

    self.AttriList:SetData(list_data)
    TimeHelper:CancelTimer(self.j_time_ht)
    self.j_time_ht = Invoke(function ()
        if index then
            self.AttriList:JumpVerticalIndex(index,157)
        else
            self.AttriList:ForceJumpVertical(0)
        end
    end,0.2)
end


function PetResonancePanel:OnSelTabClick(data)
    self.sel_qua_data = data
    self:FlushPanel()
end

function PetResonancePanel:OnAttriPreviewClick()
    -- self.AttriPreview:SetActive(true)
    -- local qua = self.sel_qua_data.qua
    -- local attri_list = self.data:GetAddAttriList(qua) or {}
    -- self.PreviewEmpty:SetActive(TableIsEmpty(attri_list))
    -- self.PreAttriList:SetData(attri_list)
    ViewMgr:OpenView(PetResonancePreview, {qua = self.sel_qua_data.qua})
end

function PetResonancePanel:OnCloseAttriPreview()
    self.AttriPreview:SetActive(false)
end


PetResonanceTabItem = PetResonanceTabItem or DeclareMono("PetResonanceTabItem", UIWidgetBaseItem)
function PetResonanceTabItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local name = Language.Pet.ColorLevelT[data.qua]
    UH.SetText(self.Name, name)
    UH.SpriteName(self.Icon, "_LocPetJibie" .. data.qua)
end


PetResonancePetItem = PetResonancePetItem or DeclareMono("PetResonancePetItem", UIWidgetBaseItem)
-- data == {qua,cell_index}
function PetResonancePetItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = PetResonanceData.Instance:GetCellData(data.qua, data.cell_index)
    self.info = info
    local state = info.state
    self.Lock:SetActive(state == -1)
    self.AddBtn:SetActive(state == 0)
    self.PetInfo:SetActive(state == 1)

    if state == 1 then
        local pet = info.pet
        UH.SetText(self.PetName, pet:Name())
        UH.SetText(self.Level, string.format("Lv.%s", pet:Level()))
        self.Cell:SetData(pet)
    end

    if self.last_qua == data.qua and self.last_state and self.last_state == -1 and state == 0 then
        self.EffectTool:Play(3165201)
    end
    self.last_state = state
    self.last_qua = data.qua
end

function PetResonancePetItem:OnAddClick()
    PetResonanceData.Instance:SetSelCell(self.data)
    ViewMgr:OpenView(PetResonanceSeletor)
end

function PetResonancePetItem:OnRemoveClick()
    -- 有预留弹窗时，不允许下宠物
    if PetResonanceData.Instance.qua_dialog then
        return
    end

    --LOG("移除了强化共鸣宠物：",PET_OP_TYPE.PetUnResonance, self.data.qua - 2, self.data.cell_index - 1)
    PetCtrl.SendPetOp(PET_OP_TYPE.PetUnResonance, self.data.qua - 2, self.data.cell_index - 1)
end

function PetResonancePetItem:OnTipClick()
    if self.info.pet then
        ViewMgr:OpenView(PetDetailView, self.info.pet)
    end
end

function PetResonancePetItem:OnLockClick()
    local need_level = PetResonanceData.Instance:GetCellUnlockLevel(self.data.qua, self.data.cell_index)
    PublicPopupCtrl.Instance:Error(string.format(Language.PetResonance.Tip1, need_level))
end


PetResonanceAttriItem = PetResonanceAttriItem or DeclareMono("PetResonanceAttriItem", UIWidgetBaseItem)
-- data == cfg
function PetResonanceAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local res_level = PetResonanceData.Instance:GetQuaLevel(data.pets_quality)
    local self_level = data.resonance_stage
    local progress_color
    local title_color
    local attri_color
    local buff_text_color
    local buff_name_color
    local buff_line_color

    if self_level < res_level then
        progress_color = COLORSTR.Yellow12
        title_color = COLORSTR.Yellow12
        attri_color = COLORSTR.Yellow2
        buff_text_color = COLORSTR.Yellow6
        buff_name_color = COLORSTR.Yellow25
        buff_line_color = COLORS.Yellow25
    elseif res_level == self_level then
        progress_color = COLORSTR.Green7
        title_color = COLORSTR.Yellow12
        attri_color = COLORSTR.Green3
        buff_text_color = COLORSTR.Yellow6
        buff_name_color = COLORSTR.Yellow25
        buff_line_color = COLORS.Yellow25
    else
        progress_color = COLORSTR.Red8
        title_color = COLORSTR.Gray4
        attri_color = COLORSTR.Gray2
        buff_text_color = COLORSTR.Gray2
        buff_name_color = COLORSTR.Gray2
        buff_line_color = COLORS.Gray2
    end
    local title_img = self_level < res_level and "BiaoTiDi1" or "BiaoTiDi2"
    UH.SpriteName(self.TitleImg, title_img)
    local title_str1 = ColorStr(string.format(Language.PetResonance.Level,data.resonance_stage), title_color)
    local progress,end_progress = PetResonanceData.Instance:GetProgress(data.pets_quality, data.resonance_stage)
    local title_str2 = ColorStr(string.format("(%s/3)", progress, end_progress), progress_color)
    UH.SetText(self.Title, string.format("%s  %s",title_str1, title_str2))

    self.BuffInfo:SetActive(data.sp_id > 0)
    if data.sp_id > 0 then
        local buff_cfg = Cfg.SpecialEffect(data.sp_id)
        UH.SetText(self.BuffText, ColorStr(Language.PetResonance.Text1, buff_text_color))
        UH.SetText(self.BuffName, ColorStr(buff_cfg.name, buff_name_color))
        UH.Color(self.BuffNameLine, buff_line_color)
    end

    local attri_info_list = PetResonanceData.Instance:GetAttriList(data)
    local attri_list = {}
    for i,v in pairs(attri_info_list) do
        local t = {}
        t.name = ColorStr(v.attr_name, attri_color)
        t.value = ColorStr(v.attr_percent_value, attri_color)
        table.insert(attri_list,t)
    end
    self.AttriList:SetData(attri_list)

    if self.last_qua == data.pets_quality and res_level == self_level and self.last_res_level < self_level then
        self.EffectTool:Play(3165202)
    end
    self.last_qua = data.pets_quality
    self.last_res_level = res_level

    local attri_h = math.floor(#attri_list / 2) * 38 - 8
    self.AttriRect:SetSizeWithCurrentAnchors(1,attri_h)

    local buff_h = data.sp_id > 0 and 30 or 0
    local item_h = 41 + attri_h + 10 + buff_h
    self.ItemRect:SetSizeWithCurrentAnchors(1,item_h)
    self.item_h = item_h
end

function PetResonanceAttriItem:OnBuffClick()
    if self.data.sp_id < 1 then
        return
    end
    --local buff_cfg = Cfg.SpecialEffect(self.data.sp_id)
    --PublicPopupCtrl.Instance:HelpTip(buff_cfg.description)
    ViewMgr:OpenView(PetBuffSkillTip,{buff_id = self.data.sp_id})
end

function PetResonanceAttriItem:GetH()
    return self.item_h
end




PetResonanceSeletor = PetResonanceSeletor or DeclareView("PetResonanceSeletor", "pet/pet_resonance_seletor")
function PetResonanceSeletor:LoadCallback()
    self.Board:SetData(self:GetType(), Language.PetResonance.Title2, Vector2.New(891, 665))

end




PetResonanceSeletorPanel = PetResonanceSeletorPanel or DeclareMono("PetResonanceSeletorPanel", UIWFlushPanel)
function PetResonanceSeletorPanel:PetResonanceSeletorPanel()
    self.data = PetResonanceData.Instance
    self.data_cares = {
        {data = self.data.selected_pets, func = self.FlushPanel, init_call = false}
    }
end

function PetResonanceSeletorPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function PetResonanceSeletorPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:ClearSel()
end

function PetResonanceSeletorPanel:FlushPanel()
    local target_data = self.data:GetSelCell()
    local pet_data_list = self.data:GetPetList(target_data.qua)
    self.PetList:SetData(pet_data_list)
    self.NoPetTipObj:SetActive(TableIsEmpty(pet_data_list))
end

function PetResonanceSeletorPanel:OnClickPetItem(mono)
    local data = mono:GetData()
    local target_data = self.data:GetSelCell()
    local result = self.data:SelPet(target_data.qua, data)
    if not result then
        PublicPopupCtrl.Instance:Error(Language.PetResonance.Tip2)
    end
end

function PetResonanceSeletorPanel:OnClickOK()
    ViewMgr:CloseView(PetResonanceSeletor)
    for pet,v in pairs(self.data.selected_pets) do
        local target_data = self.data:GetSelCell()
        --LOG("请求了添加宠物强化共鸣：",PET_OP_TYPE.PetResonanceSet, target_data.qua - 2,  v - 1, pet.index)
        PetCtrl.SendPetOp(PET_OP_TYPE.PetResonanceSet, target_data.qua - 2, v - 1, pet.index)
    end

    -- 这个时间调用不用销毁，关闭界面后继续检查
    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        PetResonanceData.Instance:CheckResonance()
    end,1.5)
end



PetResonanceSeletorItem = PetResonanceSeletorItem or DeclareMono("PetResonanceSeletorItem", UIWidgetBaseItem)
function PetResonanceSeletorItem:PetResonanceSeletorItem()
    self.pet_data = PetData.Instance
    self.lang = Language.Pet
end

function PetResonanceSeletorItem:SetData(data)
    local pet = data
    self.Cell:SetData(pet)
    UH.SetText(self.PetLevel, string.format(self.lang.LevelStr, pet:Level()))
    self.PetOnDefending:SetActive(self.pet_data:GetIsArena(pet.index))
    UIWidgetBaseItem.SetData(self, data)
    local v1 = pet:TotalGrowth()
    local v2 = pet:ConfigGrowth()
    UH.SetText(self.LblName, pet.name)
    UH.SetText(self.LblValue, string.format(self.lang.GrowthStr, COLORSTR.Yellow10, COLORSTR.Green11, v1, v2))
    if self.PrayFlag then
        self.PrayFlag:SetActive(pet:PrayMainIndex() ~= nil)
    end
    self.pet = pet

    local is_sel = PetResonanceData.Instance:IsSel(pet)
    self.On:SetActive(is_sel)
    self.Off:SetActive(not is_sel)
end


function PetResonanceSeletorItem:OnClickInfo()
    ViewMgr:OpenView(PetDetailView, self.pet)
end



PetResonanceTip = PetResonanceTip or DeclareView("PetResonanceTip", "pet/pet_resonance_tip")
VIEW_DECLARE_MASK(PetResonanceTip,ViewMask.BgBlockClose)
function PetResonanceTip:PetResonanceTip()
    self.data = PetResonanceData.Instance
end

function PetResonanceTip:LoadCallback(param_t)
    local qua = param_t.qua
    if not qua then
        return
    end
    local last_level = param_t.last_level or 0
    local pet_list = self.data:GetResonanceSource(qua)
    self.PetList:SetData(pet_list)
    local level = self.data:GetQuaLevel(qua)
    local cfg = Cfg.PetResonancCfg(qua, level)
    self.LeftInfo:SetObjActive(last_level > 0)
    self.JianTou:SetActive(last_level > 0)
    if last_level > 0 then
        local last_cfg = Cfg.PetResonancCfg(qua, last_level)
        self.LeftInfo:SetInfo(last_cfg)
    end
    self.RightInfo:SetInfo(cfg)
end

function PetResonanceTip:CloseCallback()
    PetResonanceData.Instance.effect_play:Notify()
end


PetResonanceTipInfo = PetResonanceTipInfo or DeclareMono("PetResonanceTipInfo")
function PetResonanceTipInfo:SetInfo(cfg)

    local _buff_list = PetResonanceData.Instance:GetPreviewEffectList(cfg.pets_quality, cfg.resonance_stage)
    local buff_list = {}
    for i,v in pairs(_buff_list) do
        if not v.active_level then
            table.insert(buff_list, v)
        end
    end

    self.BuffInfo:SetActive(not TableIsEmpty(buff_list))
    local buff_name = ""
    for i,v in pairs(buff_list) do
        if v.buff_id > 0 then
            local buff_cfg = Cfg.SpecialEffect(v.buff_id)
            buff_name = buff_name .. buff_cfg.name
            if i ~= #buff_list then
                buff_name = buff_name .. "、"
            end
        end
    end
    if buff_name ~= "" then
        buff_name = Language.PetResonance.Tip3 .. buff_name
        UH.SetText(self.BuffName, buff_name)
    end

    local attri_info_list = PetResonanceData.Instance:GetAttriList(cfg)
    local attri_list = {}
    for i,v in pairs(attri_info_list) do
        local t = {}
        t.name = v.attr_name
        t.value = "+" .. v.attr_percent_value
        table.insert(attri_list,t)
    end
    self.AttriList:SetData(attri_list)

    local y = TableIsEmpty(buff_list) and 50 or 77
    local l_y = TableIsEmpty(buff_list) and 0 or 22.3
    self.AttriList.gameObject:SetLocalPosition(Vector3.New(0,y,0))
    self.Line:SetLocalPosition(Vector3.New(0,l_y,0))
end


PetResonanceTipPetItem = PetResonanceTipPetItem or DeclareMono("PetResonanceTipPetItem", UIWidgetBaseItem)
function PetResonanceTipPetItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.PetCell:SetData(data)
    local str_level = data:StrLevel()
    str_level = ColorStr("+" .. str_level, COLORSTR.Green3)
    UH.SetText(self.PetName, string.format("%s %s",data:Name(), str_level))
end




-- buff详情弹框
PetBuffSkillTip = PetBuffSkillTip or DeclareView("PetBuffSkillTip", "pet/pet_buff_skill_tip")
VIEW_DECLARE_MASK(PetBuffSkillTip, ViewMask.BlockClose)
function PetBuffSkillTip:PetBuffSkillTip()
    
end

function PetBuffSkillTip:LoadCallback(param_t)
    param_t = param_t or {}
    local buff_id = param_t.buff_id
    if not buff_id then
        return
    end

    local buff_cfg = Cfg.SpecialEffect(buff_id)
    if TableIsEmpty(buff_cfg) then
        LogDG("buff配置未找到，buff_cfg == ", buff_id)
        return
    end
    
    UH.SetText(self.LblName, buff_cfg.name)
    UH.SetText(self.LblDesc, buff_cfg.description)
    UH.SetIcon(self.BuffIcon, buff_cfg.icon_id, ICON_TYPE.SKILL)
    if param_t.pos ~= nil  then
        self:CheckPos(param_t.pos)
        UH.LocalPos(self.PosRect,param_t.pos)
    end 
end

function PetBuffSkillTip:CheckPos(pos)
    if pos.x > 390 then
        pos.x = 390
    end
    if pos.x < -390 then
        pos.x = -390
    end
    if pos.y > 88 then
        pos.y = 88
    end
    if pos.y < -450 then
        pos.y = -450
    end
end



PetResonancePreview = PetResonancePreview or DeclareView("PetResonancePreview", "pet/pet_resonance_preview")
function PetResonancePreview:PetResonancePreview()
    self.data = PetResonanceData.Instance
    self.Board:SetData(self:GetType(),Language.PetResonance.Title3,Vector2.New(638, 504))
end

function PetResonancePreview:LoadCallback(param_t)
    local qua = param_t.qua
    if not qua then
        return
    end

    local level = self.data:GetQuaLevel(qua)
    if not level or level < 1 then
        self.Panel:SetActive(false)
        self.PanelNone:SetActive(true)
        return
    else
        self.Panel:SetActive(true)
        self.PanelNone:SetActive(false)
    end

    local attri_list = self.data:GetAddAttriList(qua) or {}
    self.AttriList:SetData(attri_list)

    local effect_list = self.data:GetPreviewEffectList(qua)
    self.EffectList:SetData(effect_list)
    self.EffectNone:SetActive(TableIsEmpty(effect_list))
end


PetResonancePreviewEffectInfo = PetResonancePreviewEffectInfo or DeclareMono("PetResonancePreviewEffectInfo", UIWidgetBaseItem)
-- {buff_id,active_level}
function PetResonancePreviewEffectInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local buff_id = data.buff_id
    local buff_cfg = Cfg.SpecialEffect(buff_id)
    local name = ColorStr(buff_cfg.name .. "：", COLORSTR.Yellow1)
    local desc = name .. buff_cfg.description
    UH.SetIcon(self.Icon, buff_cfg.icon_id, ICON_TYPE.SKILL)
    UH.SetText(self.Desc, desc)
    UH.SetInter(self.Inter, data.active_level == nil)
    self.TipObj:SetActive(data.active_level ~= nil)
    if data.active_level ~= nil then
        local tip = string.format(Language.PetResonance.Text2, data.active_level)
        UH.SetText(self.Tip, tip)
    end
end