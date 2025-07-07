-- 灵骑基础
BicycleBasicsPanel = BicycleBasicsPanel or DeclareMono("BicycleBasicsPanel",UIWFlushPanel)
function BicycleBasicsPanel:BicycleBasicsPanel()
    self.data = BicycleData.Instance
    self.data_cares = {
        {data = self.data.selected_bicycle_data, func = self.FlushPanel, init_call = true},
        {data = self.data.change_bicycle, func = self.FlushPanel, init_call = false, keys = {"bicycle"}},
        {data = BagData.Instance.item_grid, func = self.FlushUnlock, init_call = false},
        {data = RoleData.Instance.base_data, func = self.FlushUnlock, init_call = false,keys = {"level"}},
        {data = self.data.protocol_info_2108, func = self.FlushBicycle, init_call = false},
    }
    GuideManager.Instance:RegisterGetGuideUi("BicycleActive",function ()
        return self.ActiveBtn,BindTool.Bind(self.OnUnlockClick,self)
    end)
end

function BicycleBasicsPanel:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetText(self.BaseDesc,Config.language_cfg_auto.textdesc[112].desc)
end

function BicycleBasicsPanel:FlushPanel()
    self.selected_data = self.data:GetSelectedData()
    if self.selected_data == nil or next(self.selected_data) == nil then
        return nil
    end
    local is_unlock = self.data:BicycleIsActive(self.selected_data)
    self.UnblockObj:SetActive(not is_unlock)
    self.ShowObj:SetActive(is_unlock)
    if not is_unlock then
        self:FlushUnlock()
    else
        self:FlushBicycle()
    end
end

function BicycleBasicsPanel:FlushUnlock()
    local is_unlock = self.data:BicycleIsActive(self.selected_data)
    if is_unlock then
        return
    end
    local unlock_desc = self.data:GetUnlockDesc(self.selected_data.bicycle_id)
    UH.SetText(self.UnlockDesc,unlock_desc)

    local active_stuff = self.selected_data.activate_use
    
    if active_stuff > 0 then
        self.StuffCell:SetObjActive(true)
        local un_lock_item = Item.Create({item_id = active_stuff, num = DataHelper.ConsumeNum(Item.GetNum(active_stuff), self.selected_data.use_number, true)})
        self.StuffCell:SetData(un_lock_item)
        self.UnlockBtnObj:SetLocalPosition(Vector3.New(79.5,-205,0))
    else
        self.UnlockBtnObj:SetLocalPosition(Vector3.New(0,-205,0))
        self.StuffCell:SetObjActive(false)
    end
    local flag = self.data:ActiveBicycleFlag(self.selected_data)
    self.Interactor.Interactable = flag > 0
end

function BicycleBasicsPanel:FlushBicycle()
    local is_unlock = self.data:BicycleIsActive(self.selected_data)
    if not is_unlock then
        return
    end
    local bicycle = self.data:GetSelectedBicycle()
    local ornament = bicycle:Ornament()
    local ornament_is_show = ornament:IsShow()
    self.OrnamentLevelObj:SetActive(ornament_is_show == true)
    self.EquipCell:SetObjActive(ornament_is_show == true)
    if ornament_is_show == true then
        local ornament_item = Item.Create({item_id = ornament:IconId()})
        self.EquipCell:SetData(ornament_item)
        UH.SetText(self.OrnamentName,ornament_item:Name())
        UH.SetText(self.OrnamentLevel,string.format(Language.Bicycle.Level,ornament:Level()))
    else
        UH.SetText(self.OrnamentName,"")
    end
    local attri_data_list = self.data:ConvertAttriToData2(bicycle:Attri())
    local nature = ornament_is_show and ornament:Nature() or bicycle:Cfg().bicycle_grow_min
    nature = (nature * 100) .. "%"
    table.insert(attri_data_list,1,{name = Language.Bicycle.NatureAttriName,value = ColorStr(nature,COLORSTR.Yellow1)})
    self.AttriList:SetData(attri_data_list)
    local energy = bicycle:Energy()
    local max_energy = bicycle:Cfg().bicycle_energy
    self.EnergyProgress:SetProgress(energy/max_energy)
    self.EnergyProgress:SetText(string.format("%s/%s",energy,max_energy))
    self.energy_red_num = self.data:BicycleEnergyOneRemindNum(bicycle)
    self.EnergyRedPoint:SetNum(self.energy_red_num)
    self.OrnRedPoint:SetNum(self.data:BicycleOrnamentOneRemindNum(bicycle) > 0 and 1 or 0)
end

function BicycleBasicsPanel:OnUnlockClick()
    local flag = self.data:ActiveBicycleFlag(self.selected_data)
    if flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.BasicsTip1)
    elseif flag == -1 then
        local item_id = self.selected_data.activate_use
        MainOtherCtrl.Instance:GetWayViewById(item_id)
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.BasicsTip2)
    elseif flag == 1 then
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.ACTIVE,self.selected_data.seq)
    end
end

function BicycleBasicsPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function BicycleBasicsPanel:OnAddEquipClick()
    ViewMgr:OpenView(BicycleOrnamentView)
end

function BicycleBasicsPanel:OnNatureClick()
    ViewMgr:OpenView(BicycleOrnamentView)
end

function BicycleBasicsPanel:OnEnergyClick()
    if self.energy_red_num > 0 then
        self.data:CheckRemind()
    end
    ViewMgr:OpenView(BicycleEnergyView)
end

-- 灵骑培养
BicycleFosterPanel = BicycleFosterPanel or DeclareMono("BicycleFosterPanel",UIWFlushPanel)
function BicycleFosterPanel:BicycleFosterPanel()
    self.data = BicycleData.Instance
    self.data_cares = {
        {data = self.data.selected_bicycle_data, func = self.FlushPanel, init_call = true},
        {data = self.data.change_bicycle, func = self.FlushPanel, init_call = false, keys = {"bicycle"}},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
    }
    GuideManager.Instance:RegisterGetGuideUi("BicycleFosterBtn",function ()
        return self.BicycleFosterBtn,function ()
            self:OnUpClick()
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("BicycleAttriGuide",function ()
        return self.BicycleAttriGuide
    end)
    GuideManager.Instance:RegisterGetGuideUi("BicycleStuffGuide",function ()
        return self.BicycleStuffGuide
    end)
end

function BicycleFosterPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function BicycleFosterPanel:FlushPanel()
    self.selected_data = self.data:GetSelectedData()
    if self.selected_data == nil or next(self.selected_data) == nil then
        return nil
    end
    local is_unlock = self.data:BicycleIsActive(self.selected_data)
    local bicycle = self.data:GetSelectedBicycle()
    if is_unlock then
        self.is_need_breach = self.data:IsNeedBreach(bicycle)
    else
        self.is_need_breach = false
    end
    self.flag = self.data:GetIsCanUpLevel(self.selected_data)
    self.UpTip:SetObjActive(self.flag == -2 or self.is_need_breach)
    self.StuffCell:SetObjActive(self.flag ~= -3)
    if self.is_need_breach then
        local bicylce = self.data:GetSelectedBicycle()
        local cfg = Cfg.GetBicycleBreachCfg(bicylce.id,bicylce:Level())
        local probability = (cfg.breach_chance / 100) .. "%"
        if cfg.breach_chance == 10000 then
            UH.SetText(self.UpTip,string.format(Language.Bicycle.BreachTip2,probability))
        else
            UH.SetText(self.UpTip,string.format(Language.Bicycle.BreachTip,probability))
        end
    elseif self.flag == -2 then
        local bicycle = self.data:GetSelectedBicycle()
        local up_cfg = Cfg.BicycleLvCfg(bicycle.id,bicycle:Level() + 1)
        UH.SetText(self.UpTip,string.format(Language.Bicycle.Tips.FosterTip5,up_cfg.rolelevel_limit))
    end
    self.Interactor.Interactable = self.flag > 0
    if self.flag ~= -3 then
        self:FlushStuff()
    end
    -- 需要展示升级界面时，延迟刷新属性
    if self.data:BicycleShowType() == BicycleCfg.SHOW_TYPE.UP_BICYCLE then
        TimeHelper:CancelTimer(self.attri_ht)
        self.attri_ht = Invoke(BindTool.Bind(self.FlushAttri,self),2)
    else
        self:FlushAttri()
    end
end

function BicycleFosterPanel:FlushStuff()
    local bicycle = self.data:GetSelectedBicycle()
    local stuff_id
    local need_num
    if self.is_need_breach then
        local breach_cfg = Cfg.GetBicycleBreachCfg(bicycle.id,bicycle:Level())
        stuff_id = breach_cfg.breach_consume
        need_num = breach_cfg.breach_consume_num
        UH.SetText(self.BtnText,Language.Bicycle.UpLevelBtn1)
    else
        local level = bicycle:Level() > 0 and bicycle:Level() or 1
        local up_cfg = Cfg.BicycleLvCfg(bicycle.id,level)
        stuff_id = up_cfg.level_up_use
        need_num = up_cfg.use_number
        UH.SetText(self.BtnText,Language.Bicycle.UpLevelBtn2)
    end
    self.stuff_id = stuff_id
    self.need_num = need_num
    self.StuffCell:SetData(Item.Create({item_id = stuff_id,num = DataHelper.ConsumeNum(Item.GetNum(stuff_id), need_num, true)}))
end

function BicycleFosterPanel:FlushAttri()
    local is_unlock = self.data:BicycleIsActive(self.selected_data)
    self.bicycle = self.data:GetSelectedBicycle()
    local cur_attri_list = {}
    local next_attri_list = {}
    local cur_level
    local next_level
    if not is_unlock then
        cur_level = 1
        next_level = 2
        cur_attri_list = self.data:CalculateAttri(self.bicycle, cur_level)
        next_attri_list = self.data:CalculateAttri(self.bicycle, next_level)
    else
        cur_level = self.bicycle:Level()
        local max_level = self.data:GetMaxLevel(self.bicycle.id)
        next_level = self.bicycle:Level() + 1 > max_level and max_level or (self.bicycle:Level() + 1)
        cur_attri_list = self.bicycle:Attri()
        next_attri_list = self.data:CalculateAttri(self.bicycle,next_level)
    end
    local attri_list_3 = self.data:ConvertAttriToData3(cur_attri_list,next_attri_list)
    self.AttriList:SetData(attri_list_3)
    UH.SetText(self.CurLevel,cur_level)
    UH.SetText(self.NextLevel,next_level)
    self.is_click = false
end

function BicycleFosterPanel:OnDestroy()
    TimeHelper:CancelTimer(self.attri_ht)
    UIWFlushPanel.OnDestroy(self)
end

function BicycleFosterPanel:OnTipClick()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[118].desc})
end

function BicycleFosterPanel:OnUpClick()
    if self.flag == -4 then
        local item_id = self.stuff_id
        local num = Item.GetNum(item_id)
        local item = Item.Create({item_id = item_id, jump_need_num = self.need_num - num})
        MainOtherCtrl.Instance:GetWayView({item = item})
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.FosterTip1)
        return
    end
    if self.flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.FosterTip2)
        return
    end
    if self.flag == -3 then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.FosterTip3)
        return
    end
    if self.flag == -2 then
        local bicycle = self.data:GetSelectedBicycle()
        local up_cfg = Cfg.BicycleLvCfg(bicycle.id,bicycle:Level() + 1)
        PublicPopupCtrl.Instance:Error(string.format(Language.Bicycle.Tips.FosterTip5,up_cfg.rolelevel_limit))
        return
    end
    if self.flag == -1 then
        local item_id = self.stuff_id
        local num = Item.GetNum(item_id)
        local item = Item.Create({item_id = item_id, jump_need_num = self.need_num - num})
        MainOtherCtrl.Instance:GetWayView({item = item})
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.FosterTip4)
        return
    end
     -- 防止快速点击
     if self.is_click == true then
        return
    end
    self.is_click = true
    if self.is_need_breach then
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.BREAK_LEVEL,self.selected_data.seq)
    else
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.LEVEL_UP,self.selected_data.seq)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.PetStrength)
end

-- 灵骑灵技
BicycleSkillPanel = BicycleSkillPanel or DeclareMono("BicycleSkillPanel",UIWFlushPanel)
function BicycleSkillPanel:BicycleSkillPanel()
    self.data = BicycleData.Instance
    self.data_cares = {
        {data = self.data.selected_bicycle_data, func = self.FlushPanel, init_call = false},
        {data = self.data.selected_skill, func = self.FlushPanel, init_call = false},
        {data = self.data.change_bicycle, func = self.FlushPanel, init_call = false, keys = {"bicycle"}},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
        {data = self.data.protocol_info_2106, func = self.FlushPanel, init_call = false},
    }
    GuideManager.Instance:RegisterGetGuideUi("BicycleSkillGuideFrame",function ()
        return self.GuideFrame
    end)
end

function BicycleSkillPanel:FlushPanel()
    self.Remind:SetNum(0)
    self:SetStuff()
    self.selected_data = self.data:GetSelectedData()
    self.skill = self.data:GetSelectedSkill()
    local is_unlock = self.data:BicycleIsActive(self.selected_data)
    if self.selected_data == nil or next(self.selected_data) == nil then
        return nil
    end
    self.CtrlObj:SetActive(not TableIsEmpty(self.skill))
    self.bicycle = self.data:GetSelectedBicycle()
    if TableIsEmpty(self.skill) then
        self.DescObj:SetActive(true)
        self.ContentObj:SetActive(false)
        return
    end

    local skill_is_unlock = self.skill:IsUnlock()
    self.DescObj:SetActive(self.skill.skill_id == 0)
    self.ContentObj:SetActive(self.skill.skill_id > 0)
    local skill_hole = self.skill.index
    local hole_qua_str = Language.Bicycle.HoleQua[skill_hole]

    self.StudyTitle:SetActive(self.skill.skill_id == 0)
    if self.skill.skill_id > 0 then
        self.skill_flag,self.up_cfg,self.next_up_cfg = self.data:GetUpSkillLevelFlag(self.skill)
        self.SkillIcon:SetData(self.skill)
        UH.SetText(self.SkillTypeText,Language.Bicycle.SkillType[self.skill:SkillType()])
        UH.SetText(self.SkillDesc,self.skill:Desc())
        self.NextDescObj:SetActive(self.skill_flag ~= -2)

        UH.SetText(self.UpTip,"")
        if self.skill_flag ~= -2 then
            local next_effect_cfg = self.data:GetSkillEffect(self.skill.skill_id,self.skill:Level() + 1)
            if next_effect_cfg then
                UH.SetText(self.SkillNextDesc,next_effect_cfg.description)
            end
            if self.skill_flag == -4 then
                UH.SetText(self.UpTip,string.format(Language.Bicycle.Tips.SkillTip5,self.next_up_cfg.bicycle_level_limit))
            else
                UH.SetText(self.UpTip,"")
            end
        end
        UH.SetText(self.BtnText, Language.Bicycle.BtnUpLevel)
        if self.skill_flag == -2 then
            UH.SetText(self.AddTip, "")
        else
            if TableIsEmpty(self.stuff_data) then
                --UH.SetText(self.AddTip, string.format(Language.Bicycle.Text2, self.skill:Level(), hole_qua_str))
                local level = self.skill:Level()
                UH.SetText(self.AddTip, string.format(Language.Bicycle.Text4, level, hole_qua_str, level + 1))
            else
                local level = self.skill:Level()
                UH.SetText(self.AddTip, string.format(Language.Bicycle.Text4, level, hole_qua_str, level + 1))
            end
        end
    else
        self.skill_flag = 1
        UH.SetText(self.BtnText, Language.Bicycle.BtnStudy)
        UH.SetText(self.AddTip, string.format(Language.Bicycle.Text3, hole_qua_str))
    end

    if TableIsEmpty(self.stuff_data) then
        self.Remind:SetNum(self.skill:Remind())
    end
end

function BicycleSkillPanel:SetStuff()
    local item = self.stuff_data
    if item == nil then
        self.Stuff:SetObjActive(false)
    else
        self.Stuff:SetObjActive(true)
        self.Stuff:SetData(item)
    end
    self.Interactor.Interactable = not TableIsEmpty(item)
    self.RemoveBtn:SetActive(not TableIsEmpty(item))
end

function BicycleSkillPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    UH.SetText(self.DescText,Config.language_cfg_auto.textdesc[127].desc)
    self.stuff_data = nil
    self:FlushPanel()
end

function BicycleSkillPanel:OnDestroy()
    TimeHelper:CancelTimer(self.time_hd)
    UIWFlushPanel.OnDestroy(self)
end

function BicycleSkillPanel:OnResetClick()
    -- local is_unlock = self.data:BicycleIsActive(self.selected_data)
    -- if is_unlock then
    --     if self.skill:Level() <= 1 then
    --         PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.SkillTip6)
    --         return
    --     end
    --     local ok_func = BindTool.Bind(function()
    --         BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.RESET_SKILL,self.selected_data.seq,self.skill:SkillType(),self.skill:Index())
    --     end,self)
    --     ViewMgr:OpenView(BicycleSkillResetDialog,{content = Language.Bicycle.ResetSkillTip,ok_func = ok_func})
    -- else
    --     PublicPopupCtrl.Instance:Error(Language.Bicycle.ActiveTip)
    -- end
    if self.skill.skill_id > 0 then
        ViewMgr:OpenView(BicycleSkillConvertView, {skill = self.skill})
    end
end

function BicycleSkillPanel:OnUpClick()
    if self.skill_flag > 0 then
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.SKILL_UP,self.selected_data.seq,self.skill:SkillType(),self.skill:Index())
    elseif self.skill_flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.SkillTip1)
    elseif self.skill_flag == -3 then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.SkillTip2)
    elseif self.skill_flag == -1 then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.SkillTip3)
        local stuff_id = self.up_cfg.skillup_consume_item[0].item_id
        local item = Item.Create({item_id = stuff_id})
        MainOtherCtrl.Instance:GetWayView({item = item})
    elseif self.skill_flag == -2 then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.SkillTip4)
    elseif self.skill_flag == -4 then
        PublicPopupCtrl.Instance:Error(string.format(Language.Bicycle.Tips.SkillTip5,self.next_up_cfg.bicycle_level_limit))
    end
end

function BicycleSkillPanel:OnBtnClick()
    if not self.skill then
        return
    end
    if self.skill_flag == -2 then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.SkillTip4)
        return
    end
    if not self.stuff_data then
        PublicPopupCtrl.Instance:Center(Language.Bicycle.Text10)
        return
    end
    local hole_index = self.skill.index
    local col_type = self.stuff_data.column_type
    local bag_index = self.stuff_data.index
    if self.skill.skill_id > 0 then
        local stuff_skill_info = BicycleOtherData.Instance:GetItemSkillInfo(self.stuff_data.item_id)
        local tip = string.format(Language.Bicycle.SkillUpTip, stuff_skill_info.skill_level, self.stuff_data:QuaName(), self.skill:Level(), self.skill:QuaName(),self.skill:Level() + 1, self.skill:QuaName())
        PublicPopupCtrl.Instance:AlertTip(tip,function ()
            BicycleCtrl.Instance:ReqSkill(BicycleCfg.OP_TYPE_SKILL.SKILL_UP, self.bicycle:Index(), hole_index, col_type, bag_index)
            self.stuff_data = nil
            self:FlushPanel()
        end)
    else
        -- 学习
        BicycleCtrl.Instance:ReqSkill(BicycleCfg.OP_TYPE_SKILL.LEAEN_SKILL, self.bicycle:Index(), hole_index, col_type, bag_index)
        self.stuff_data = nil
        self:FlushPanel()
    end
end

function BicycleSkillPanel:OnAddClick()
    if self.skill_flag == -4 then
        PublicPopupCtrl.Instance:Error(string.format(Language.Bicycle.Tips.SkillTip5,self.next_up_cfg.bicycle_level_limit))
        return
    end
    if self.skill_flag == -2 then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.SkillTip4)
        return
    end
    if not self.skill then
        return
    end
    local ok_func = function (item)
        self.stuff_data = item
        self:FlushPanel()
    end
    if self.skill.skill_id > 0 then
        ViewMgr:OpenView(BicycleSelSkillStuffView, {show_type = BicycleSelSkillStuffView.ShowType.SkillUp, 
        t_qua = self.skill:Color(), 
        t_level = self.skill:Level(),
        ok_func = ok_func})
    else
        local skill_hole = self.skill.index
        ViewMgr:OpenView(BicycleTargetSkillView, {skill_hole = skill_hole, ok_func = ok_func})
    end
end

function BicycleSkillPanel:OnSkillPoolClick()
    ViewMgr:OpenView(BicycleSkillPool)
end

function BicycleSkillPanel:OnSkillRefineClick()
    -- local is_unlock = self.data:BicycleIsActive(self.selected_data)
    -- if not is_unlock then
    --     PublicPopupCtrl.Instance:Error(Language.Bicycle.ActiveTip)
    --     return
    -- end
    -- ViewMgr:OpenView(BicycleRefineSkillView)
end

function BicycleSkillPanel:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[147].desc)
end

function BicycleSkillPanel:OnSkillComposeClick()
    ViewMgr:OpenView(BicycleSkillComposeView)
end

function BicycleSkillPanel:OnRemoveClick()
    self.stuff_data = nil
    self:FlushPanel()
end

-- 灵骑御宠
BicyclePetPanel = BicyclePetPanel or DeclareMono("BicyclePetPanel",UIWFlushPanel)
function BicyclePetPanel:BicyclePetPanel()
    self.data = BicycleData.Instance
    self.data_cares = {
        {data = self.data.selected_bicycle_data, func = self.FlushPanel, init_call = false},
        {data = self.data.change_bicycle, func = self.FlushPanel, init_call = false, keys = {"bicycle"}},
        {data = self.data.protocol_info_2104, func = self.FlushPanel, init_call = false},
    }
    GuideManager.Instance:RegisterGetGuideUi("BicycleGuideAddPetBtn",function ()
        return self.Pet1,function ()
            self.Pet1:OnAddClick()
            local pet_list = self.data:GetPetList()
            if TableIsEmpty(pet_list) then
                GuideManager.Instance:ForceStop()
                return
            end
        end
    end)
end

function BicyclePetPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    UH.SetText(self.DescText,Config.language_cfg_auto.textdesc[116].desc)
    self:FlushPanel()
end

function BicyclePetPanel:FlushPanel()
    self.selected_data = self.data:GetSelectedData()
    if self.selected_data == nil or next(self.selected_data) == nil then
        return nil
    end
    self.bicycle = self.data:GetSelectedBicycle()
    local is_unlock = self.data:BicycleIsActive(self.selected_data)
    if is_unlock then
        UH.SetText(self.Tip,Language.Bicycle.Tips.PetTip2)
        local bag_has_pet = self.data:HasPet()
        local pet_index_list = self.bicycle:PetIndexList()
        self.Pet1:SetData({pet_index = pet_index_list[0],bag_has_pet = bag_has_pet,index = 0,is_unlock = true,bicycle_index = self.selected_data.seq})
        self.Pet2:SetData({pet_index = pet_index_list[1],bag_has_pet = bag_has_pet,index = 1,is_unlock = true,bicycle_index = self.selected_data.seq})
    else
       self.Pet1:SetData({pet_index = -1,pet_count = 0,index = 0,is_unlock = false})
       self.Pet2:SetData({pet_index = -1,pet_count = 0,index = 0,is_unlock = false})
       UH.SetText(self.Tip,Language.Bicycle.Tips.PetTip3)
    end
end

function BicyclePetPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end


BicyclePetItem = BicyclePetItem or DeclareMono("BicyclePetItem",UIWidgetBaseItem)

function BicyclePetItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.Name:SetObjActive(data.pet_index >= 0)
    self.PetItem:SetObjActive(data.pet_index >= 0)
    self.RemoveBtn:SetActive(data.pet_index >= 0)
    if data.pet_index >= 0 then
        local pet = PetData.Instance:GetPetInfo(data.pet_index)
        if not pet then
            LogError("BicyclePetItem Pet == nil !!! pet_index = ", data.pet_index)
            data.pet_index = -1
            self:SetData(data)
            return
        end
        self.PetItem:SetData(pet)
        UH.SetText(self.Name,pet:Name())
    end
    self.RemindObj:SetActive(data.bag_has_pet and data.is_unlock and data.pet_index < 0)
    self.AddTipObj:SetActive(data.pet_index < 0)
end

function BicyclePetItem:OnAddClick()
    if not self.data.is_unlock then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.ActiveTip)
        return
    end
    local pet_list = BicycleData.Instance:GetPetList()
    if next(pet_list) == nil then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.Tips.PetTip1)
        return
    end
    ViewMgr:OpenView(BicycleSelectPetView,{index = self.data.index})
end

function BicyclePetItem:OnRemoveClick()
    PublicPopupCtrl.Instance:Center(Language.Bicycle.Tips.PetSelectTip3)
    BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.CANCEL_DOMINANCE,self.data.bicycle_index,self.data.index)
end



--  ====================================== 点化 ===================
BicycleDianHuaPanel = BicycleDianHuaPanel or DeclareMono("BicycleDianHuaPanel", UIWFlushPanel)
function BicycleDianHuaPanel:BicycleDianHuaPanel()
    self.data = BicycleData.Instance
    self.data_cares = {
        {data = self.data.selected_bicycle_data, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
        {data = self.data.change_bicycle, func = self.FlushPanel, init_call = false, keys = {"bicycle"}},
    }
end

function BicycleDianHuaPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function BicycleDianHuaPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_ht)
end

function BicycleDianHuaPanel:FlushPanel()
    local bicycle = self.data:GetSelectedBicycle()
    if TableIsEmpty(bicycle) then
        return
    end
    local dianhua_level = bicycle:DianHuaLevel()
    UH.SetText(self.Level, string.format(Language.Bicycle.Level, dianhua_level))
    local is_max_full = bicycle:IsDianHuaFullLevel()
    self.ShowNormal:SetActive(not is_max_full)
    self.ShowMax:SetActive(is_max_full)
    local title = is_max_full and Language.Bicycle.DianHuaTitle2 or Language.Bicycle.DianHuaTitle1
    UH.SetText(self.Title, title)
    
    local attri_list
    local score
    if is_max_full then
        attri_list = bicycle:GetDianHuaAttriList()
        score = bicycle:DianHuaScore()
    else
        attri_list = bicycle:GetNextDianHuaAttriList()
        local up_cfg = bicycle:GetNextDianHuaCfg()
        local item_id = up_cfg.level_up_use
        self.stuff_item_id = item_id
        local has_item_num = Item.GetNum(item_id)
        local stuff_item = Item.Create({item_id = item_id, num = DataHelper.ConsumeNum(has_item_num, up_cfg.use_number, true)})
        self.StuffCell:SetData(stuff_item)
        score = up_cfg.score

        local flag = self.data:BicycleDianHuaOneRemindNum(bicycle)
        UH.SetInter(self.UpBtnInter, flag > 0)
        self.RedPoint:SetNum(flag > 0 and 1 or 0)
    end
    self.AttriList:SetData(attri_list)
    UH.SetText(self.Score, "+" .. score)
end

function BicycleDianHuaPanel:OnUpLevelClick()
    local bicycle = self.data:GetSelectedBicycle()
    if TableIsEmpty(bicycle) then
        return
    end
    local flag = self.data:BicycleDianHuaOneRemindNum(bicycle)
    if flag == 1 then
        local effect_info = {effect = BicycleCfg.EFFECT.UP_DIAN_HUA}
        self.data:CurEffect(effect_info)
        TimeHelper:CancelTimer(self.time_ht)
        self.time_ht = Invoke(function ()
            BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.UP_DIAN_HUA,bicycle:Cfg().seq)
            self.data:CurEffect({})
        end,3)
    elseif flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.StuffTip)
        if self.stuff_item_id then
            MainOtherCtrl.Instance:GetWayViewById(self.stuff_item_id)
        end
    elseif flag == -1 then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.ActiveTip)
    elseif flag == -3 then
        PublicPopupCtrl.Instance:Error(Language.Bicycle.LevelTip)
    end
end

function BicycleDianHuaPanel:OnTipClick()
    PublicPopupCtrl.Instance:HelpTip(334)
end

function BicycleDianHuaPanel:OnOpenPreviewClick()
    ViewMgr:OpenView(BicycleDianHuaDialog)
end

BicycleDianHuaAttriItem = BicycleDianHuaAttriItem or DeclareMono("BicycleDianHuaAttriItem", UIWidgetBaseItem)
function BicycleDianHuaAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local attri_info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
    UH.SetText(self.AttriName, attri_info.attr_name)
    UH.SetText(self.AttriValue, "+" .. attri_info.attr_percent_value)
    if data.obj_name then
        self.gameObject.name = data.obj_name
    end
end


