-- ============== 宠物天赋 ================
PetInbornView = PetInbornView or DeclareView("PetInbornView", "pet/pet_inborn/pet_inborn", Mod.PetInborn.Main)
function PetInbornView:PetInbornView()
    self.Board:SetData(self:GetType(),Language.PetInborn.Title1,Vector2.New(1146, 642))
end

function PetInbornView:CloseCallback()
    local item_id = PetInbornData.Instance:StuffItemId()
    local num = Item.GetNum(item_id)
    PetInbornData.Instance:SetRemindStuffNum(num)
    PetInbornData.Instance.remind_data:Notify()
    PetData.Instance.pet_list:Notify()
    PetInbornData.Instance.inborn_unlock_data:Set({inborn_group_type = 0, grid_index = 0})
end

PetInbornPanel = PetInbornPanel or DeclareMono("PetInbornPanel", UIWFlushPanel)
function PetInbornPanel:PetInbornPanel()
    self.data = PetInbornData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
        {data = PetData.Instance.pet_list, func = self.FlushPanel, init_call = false},
        {data = self.data.inborn_unlock_data, func = self.FlushPanel, init_call = false}
    }
end

function PetInbornPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.pet = PetData.Instance:GetCurSelect()
    if not self.pet then
        ViewMgr:CloseView(PetInbornView)
        return
    end
    self.inborn_type = self.pet:PetCfg().inborn_type
    local list_data = self.data:GetInbornTypeGroup(self.inborn_type)
    if not PetFeiShengData.ModUnlocked(PetFeiShengData.Lv.Two) then
        self.list_data = {}
        for i,v in pairs(list_data) do
            if v.inborn_group_lock <= 10 then
                table.insert(self.list_data, v)
            end
        end
    else
        self.list_data = list_data
    end
    local item_id = self.data:StuffItemId()
    UH.SetIcon(self.ItemIcon, Item.GetIconId(item_id), ICON_TYPE.ITEM)
    self:FlushModel()
    self:FlushPanel()
end

function PetInbornPanel:FlushPanel()
    local unlock_inborn_group_type = self.data.inborn_unlock_data.inborn_group_type or 0
    self.LockMask:SetActive(unlock_inborn_group_type ~= 0)
    self.List:SetData(self.list_data)
    self:FlushItem()
end

function PetInbornPanel:FlushModel()
    UH.SetText(self.PetName,self.pet:Name())
    local str_num = self.pet:StrLevel()
    UH.SpriteName(self.StrImg, "PetNum" .. str_num)
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(self.pet:ResId()),self.pet:FeiShengTimes())
    self.ui_obj:PlayAnim(ChAnimType.Show)
    local cfg = self.pet:PetCfg()
    PetView.SetTypeBgImage(self.TypeBG,cfg.quality,cfg.mutant_times,true,self.pet:IsLianYao())
end

function PetInbornPanel:FlushItem()
    local item_id = self.data:StuffItemId()
    local num = Item.GetNum(item_id)
    UH.SetText(self.ItemNum, num)
end

function PetInbornPanel:OnResetClick()
    local need_gold = Config.pet_inborn_auto.other[1].return_consume
    local tip = string.format(Language.PetInborn.Tip1, ColorStr(need_gold, COLORSTR.Yellow6))
    local pet = PetData.Instance:GetCurSelect()
    if pet:IsInborn() then
        PublicPopupCtrl.Instance:AlertTip(tip, function ()
            PetCtrl.SendPetOp(PET_OP_TYPE.PetInbornSkillReset, pet.index)
        end)
    else
        PublicPopupCtrl.Instance:Center(Language.PetInborn.Tip2)
    end
end

function PetInbornPanel:OnIntensifyClick()
    ViewMgr:CloseView(PetInbornView)
    ViewMgr:OpenViewByKey(Mod.Pet.PetStrChange)
end

function PetInbornPanel:OnTipClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[291].desc)
end

function PetInbornPanel:OnAddItemClick()
    local item_id = self.data:StuffItemId()
    MainOtherCtrl.Instance:GetWayViewById(item_id)
end

PetInbornItem = PetInbornItem or DeclareMono("PetInbornItem", UIWidgetBaseItem)
function PetInbornItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    -- 都走到这了，pet不为空
    local pet = PetData.Instance:GetCurSelect()
    local inborn_index = data.inborn_group_type
    local inborn_info = pet:InbornInfo(inborn_index)
    self.inborn_info = inborn_info

    local unlock_inborn_group_type = PetInbornData.Instance.inborn_unlock_data.inborn_group_type
    if unlock_inborn_group_type and unlock_inborn_group_type == self.data.inborn_group_type then
        self:UnlockEvent()
    end

    local list_data = {}
    for i = 1,3 do
        local inborn_id = data["inborn_" .. i]
        local level = 1
        if inborn_info.inborn_level > 0 then
            level = inborn_info.inborn_level
        end
        local cfg = PetInbornData.Instance:GetInbornIdCfg(inborn_id, level)
        table.insert(list_data, cfg)
    end
    self.list_data = list_data
    self.List:SetData(list_data)
    self:FlushInfo(self.sel_data)
end

function PetInbornItem:GeteDefauletData()
    local index = self.inborn_info.inborn_grid or 0
    return self.list_data[index]
end

function PetInbornItem:FlushInfo(effect_item_data)
    effect_item_data = effect_item_data or self:GeteDefauletData()
    local inborn_level = self.inborn_info.inborn_level or 0
    self.LockObj:SetActive(inborn_level == 0)
    local pet = PetData.Instance:GetCurSelect()
    -- 强化等级
    local str_level = pet:StrLevel()
    if inborn_level == 0 then
        self.LockState1:SetActive(str_level >= self.data.inborn_group_lock)
        self.LockState2:SetActive(str_level < self.data.inborn_group_lock)
        if str_level < self.data.inborn_group_lock then
            UH.SpriteName(self.StrLevel, "PetNum" .. self.data.inborn_group_lock)
        end
    end

    self.DescObj:SetActive(effect_item_data ~= nil)
    self.EffectName:SetObjActive(not TableIsEmpty(effect_item_data))
    self.ItemInfo:SetActive(TableIsEmpty(effect_item_data) and str_level >= self.data.inborn_group_lock)
    if not TableIsEmpty(effect_item_data) then
        local sp_id = effect_item_data.sp_id
        local effect_cfg = effect_item_data.effect_cfg or Cfg.SpecialEffect(sp_id)
        UH.SetText(self.DescText, effect_cfg.reward_description)
        UH.SetText(self.EffectName, effect_cfg.name)
        local h = self.DescText.preferredHeight
        self.ScrollView.enabled = h > 104
    elseif str_level >= self.data.inborn_group_lock then
        local stuff_data = self.list_data[1]
        local item_id = stuff_data.strengthen_consume_item[0].item_id
        local need_num = stuff_data.strengthen_consume_item[0].num
        local has_num = Item.GetNum(item_id)
        UH.SetIcon(self.StuffIcon, Item.GetIconId(item_id) ,ICON_TYPE.ITEM)
        UH.SetText(self.StuffNum, DataHelper.ConsumeNum(has_num, need_num,true))
    end
end

function PetInbornItem:OnSelClick(data, value)
    if not value then
        self.sel_data = nil
        self.sel_index = nil
    else
        self.sel_data = data
        self.sel_index = self.List:GetSelectedIndex(data)
    end
    self:FlushInfo(self.sel_data)
end

function PetInbornItem:OnUnlockClick()
    if self.playing then
        return
    end
    local pet = PetData.Instance:GetCurSelect()
    local inborn_index = self.data.inborn_group_type
    local flag = PetInbornData.Instance:GetPetInbornUpFlag(inborn_index, pet)
    if flag == -4 then
        PublicPopupCtrl.Instance:Error(Language.PetInborn.Tip7)
        return
    end
    ViewMgr:OpenView(PetInbornUnlock, {data = self.data, sel_index = self.sel_index or 1})
end

function PetInbornItem:OnLockClick()
    PublicPopupCtrl.Instance:Center(string.format(Language.PetInborn.Tip3, self.data.inborn_group_lock))
end

function PetInbornItem:OnTipClick()
    local inborn_data = self.sel_data or self:GeteDefauletData()
    ViewMgr:OpenView(PetInbornPreview, {id = inborn_data.inborn_id, level = self.inborn_info.inborn_level})
end

function PetInbornItem:UnlockEvent() 
    local unlock_grid_index = PetInbornData.Instance.inborn_unlock_data.grid_index
    if not unlock_grid_index or unlock_grid_index < 1 or unlock_grid_index > 3 then
        return
    end
    TimeHelper:CancelTimer(self.time_run)
    self.default_step = 10
    self.cur_step = 1
    self.playing = true
    self.start_pos = Vector3.New(-207,39,0)
    self.end_pos = self.PosList[unlock_grid_index]:GetLocalPosition()
    self.MoveEffectObj:SetActive(true)
    self.time_run = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.MoveEffect,self),1,self.default_step)
end

function PetInbornItem:OnDestroy()
    TimeHelper:CancelTimer(self.time_run)
end

function PetInbornItem:MoveEffect()
    local pos = FormatCurve({self.start_pos,self.end_pos}, self.cur_step / self.default_step)
    self.MoveEffectObj:SetLocalPosition(pos)
    self.cur_step = self.cur_step + 1
    if self.cur_step > self.default_step then
        self:EffectEndEvent()
	end
end

function PetInbornItem:EffectEndEvent()
    self.playing = false
    local unlock_grid_index = PetInbornData.Instance.inborn_unlock_data.grid_index
    PetInbornData.Instance.inborn_unlock_data:Set({inborn_group_type = 0, grid_index = 0})
    self.MoveEffectObj:SetActive(false)
    self.MoveEffectObj:SetLocalPosition(self.start_pos)
    if not unlock_grid_index or unlock_grid_index < 1 or unlock_grid_index > 3 then
        return
    end

    local pet = PetData.Instance:GetCurSelect()
    if not pet then
        return
    end

    local item_mono = self.List:GetItem(unlock_grid_index)
    if item_mono then
        item_mono:PlayEffect()
    end

    --LogError("解锁：：",PET_OP_TYPE.PetUnlockInborn, pet.index, self.data.inborn_group_type, unlock_grid_index)
    PetCtrl.SendPetOp(PET_OP_TYPE.PetUnlockInborn, pet.index, self.data.inborn_group_type, unlock_grid_index)
    
end


PetInbornEffectItem = PetInbornEffectItem or DeclareMono("PetInbornEffectItem", UIWidgetBaseItem)
function PetInbornEffectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local sp_id = data.sp_id
    local effect_cfg = Cfg.SpecialEffect(sp_id)
    data.effect_cfg = effect_cfg
    UH.SetIcon(self.SkillIcon, effect_cfg.icon_id, ICON_TYPE.BUFF)
    UH.SetText(self.SkillName, effect_cfg.name)

    local inborn_info = self.ParentMono.inborn_info
    local grid_index = self.ParentMono.List:GetSelectedIndex(data)
    self.grid_index = grid_index
    local max_level = PetInbornData.Instance:GetInbornMaxLevel(data.inborn_id)
    local level_str = string.format("%s/%s",inborn_info.inborn_level, max_level)
    self.is_max_level = inborn_info.inborn_level >= max_level
    UH.SetText(self.Level, level_str)
    self.LevelInfo:SetActive(inborn_info.inborn_level > 0 and inborn_info.inborn_grid == grid_index)

    self.UpBtn:SetActive(inborn_info.inborn_level > 0 and inborn_info.inborn_grid == grid_index and not self.is_max_level)
    self.ReaBtn:SetActive(inborn_info.inborn_level > 0 and inborn_info.inborn_grid ~= grid_index )
    self.UpInteractor.Interactable = not self.is_max_level
    local btn_text = self.is_max_level and Language.PetInborn.Btn2 or Language.PetInborn.Btn1
    UH.SetText(self.UpBtnText, btn_text)

    self.UpRemind:SetNum(0)
    local pet = PetData.Instance:GetCurSelect()
    local state = pet:State()
    if state == PetState.Fight or state == PetState.StandBy then
        if inborn_info.inborn_level > 0 and PetInbornData.Instance:IsCanRemind() then
            local flag = PetInbornData.Instance:GetPetInbornUpFlag(self.ParentMono.data.inborn_group_type, pet)
            self.UpRemind:SetNum(flag == 1 and 1 or 0)
        end
    end

    self.MaxFlag:SetActive(self.is_max_level and inborn_info.inborn_grid == self.grid_index)
    self:FlushMask()
end

function PetInbornEffectItem:FlushMask()
    self.Mask:SetActive(self.Selector.isOn ~= true and self.ParentMono.inborn_info.inborn_grid ~= self.grid_index)
end

function PetInbornEffectItem:OnSelClick()
    self:FlushMask()
end

function PetInbornEffectItem:OnUpClick()
    if not self.is_max_level then
        local pet = PetData.Instance:GetCurSelect()
        PetInbornData.Instance:SetInboenUpData({pet_index = pet.index, inborn_group_index = self.ParentMono.data.inborn_group_type, inborn_id = self.data.inborn_id})
        ViewMgr:OpenView(PetInbornUp)
    else
        PublicPopupCtrl.Instance:Center(Language.PetInborn.Tip4)
    end
end

function PetInbornEffectItem:OnReplaceClick()
    local need_gold = Config.pet_inborn_auto.other[1].replace_consume
    local tip = string.format(Language.PetInborn.Tip5,ColorStr(need_gold, COLORSTR.Yellow6))
    PublicPopupCtrl.Instance:AlertTip(tip, function ()
        local pet = PetData.Instance:GetCurSelect()
        local inborn_group_type = self.ParentMono.data.inborn_group_type
        PetCtrl.SendPetOp(PET_OP_TYPE.PetInbornSkillReplace, pet.index, inborn_group_type, self.grid_index)
        --if MallCtrl.IsNotCurrency(CurrencyType.Gold, need_gold) then
    end)
end

function PetInbornEffectItem:PlayEffect()
    self.EffectTool:Play("7165047")
end

-- ============ 宠物天赋解锁 ==========
PetInbornUnlock = PetInbornUnlock or DeclareView("PetInbornUnlock", "pet/pet_inborn/pet_inborn_unlock")
function PetInbornUnlock:PetInbornUnlock()
    self.Board:SetData(self:GetType(),Language.PetInborn.Title2,Vector2.New(746, 633))
end

-- param_t == Config.pet_inborn_auto.pet_inborn_base
function PetInbornUnlock:LoadCallback(param_t)
    param_t = param_t or {}
    if TableIsEmpty(param_t) then
        return
    end
    self.inborn_group_type = param_t.data.inborn_group_type
    local list_data = {}
    for i = 1,3 do
        local inborn_id = param_t.data["inborn_" .. i]
        local level = 1
        local cfg = PetInbornData.Instance:GetInbornIdCfg(inborn_id, level)
        local data = {grid_index = i, cfg = cfg}
        table.insert(list_data, data)
    end
    self.List:SetData(list_data)
    self.List:SetectItem(param_t.sel_index or 1, true)

    self.listen_key = BagData.Instance:ListenItems({PetInbornData.Instance:StuffItemId()},BindTool.Bind(self.FlushPanel, self))
end

function PetInbornUnlock:CloseCallback()
    TimeHelper:CancelTimer(self.time_ht)
    BagData.Instance:RemoveListenItems(self.listen_key)
end

function PetInbornUnlock:FlushPanel()
    if not self.sel_data then
        return
    end
    local cfg = self.sel_data.cfg
    local item_id = cfg.strengthen_consume_item[0].item_id
    local need_num = cfg.strengthen_consume_item[0].num
    local has_num = Item.GetNum(item_id)
    self.is_can_unlock = has_num >= need_num
    self.item_id = item_id
    UH.SetIcon(self.Icon, Item.GetIconId(item_id) ,ICON_TYPE.ITEM)
    UH.SetText(self.Num, DataHelper.ConsumeNum(has_num, need_num,true))
end

function PetInbornUnlock:OnSelClick(data)
    self.sel_data = data
    self:FlushPanel()
end

function PetInbornUnlock:OnUnlockClick()
    if not self.sel_data then
        return
    end
    if not self.is_can_unlock then
        PublicPopupCtrl.Instance:Error(Language.PetInborn.Tip6)
        MainOtherCtrl.Instance:GetWayViewById(self.item_id)
        return
    end

    PublicPopupCtrl.Instance:AlertTip(Language.PetInborn.Tip8,function ()
        TimeHelper:CancelTimer(self.time_ht)
        if not ViewMgr:IsOpen(PetInbornUnlock) then
            return
        end
        self.Effect:SetActive(true)
        self.time_ht = Invoke(function ()
            ViewMgr:CloseView(PetInbornUnlock)
            PetInbornData.Instance.inborn_unlock_data:Set({inborn_group_type = self.inborn_group_type, grid_index = self.sel_data.grid_index})
        end,1.1)
    end)
    --LogD("请求了解锁",PET_OP_TYPE.PetUnlockInborn, pet.index, self.inborn_group_type, self.sel_data.grid_index)
    -- PetCtrl.SendPetOp(PET_OP_TYPE.PetUnlockInborn, pet.index, self.inborn_group_type, self.sel_data.grid_index)
    -- ViewMgr:CloseView(PetInbornUnlock)
end

PetInbornUnlockItem = PetInbornUnlockItem or DeclareMono("PetInbornUnlockItem", UIWidgetBaseItem)
function PetInbornUnlockItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local index = data.grid_index
    local cfg = data.cfg
    UH.SpriteName(self.TabImg, "BiaoQian" .. index)
    UH.SetText(self.TabText, Language.PetInborn.TabName[index])
    local sp_id = cfg.sp_id
    local effect_cfg = Cfg.SpecialEffect(sp_id)
    UH.SetIcon(self.SkillIcon, effect_cfg.icon_id, ICON_TYPE.BUFF)
    UH.SetText(self.SkillName, effect_cfg.name)
    UH.SetText(self.SkillDesc, effect_cfg.reward_description)
end

function PetInbornUnlockItem:OnPreviewClick()
    ViewMgr:OpenView(PetInbornPreview, {id = self.data.cfg.inborn_id, level = 0})
end

-- ============ 宠物天赋升级 ==========
PetInbornUp = PetInbornUp or DeclareView("PetInbornUp", "pet/pet_inborn/pet_inborn_up")
function PetInbornUp:PetInbornUp()
    self.Board:SetData(self:GetType(),Language.PetInborn.Title3,Vector2.New(654, 536))
end

PetInbornUpPanel = PetInbornUpPanel or DeclareMono("PetInbornUpPanel", UIWFlushPanel)
function PetInbornUpPanel:PetInbornUpPanel()
    self.data = PetInbornData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
        {data = PetData.Instance.pet_list, func = self.FlushPanel, init_call = false},
    }
end

function PetInbornUpPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function PetInbornUpPanel:FlushPanel()
    local info = self.data:GetInboenUpData()
    self.pet = PetData.Instance:GetPetInfo(info.pet_index) or PetData.Instance:GetCurSelect()
    local inborn_info = self.pet:InbornInfo(info.inborn_group_index)
    local inborn_id = info.inborn_id
    local data1 = self.data:GetInbornIdCfg(inborn_id, inborn_info.inborn_level)
    local data2 = self.data:GetInbornIdCfg(inborn_id, inborn_info.inborn_level + 1)
    if TableIsEmpty(data2) then
        PublicPopupCtrl.Instance:Center(Language.PetInborn.Tip4)
        ViewMgr:CloseView(PetInbornUp)
        return
    end
    local list_data = {}
    table.insert(list_data, data1)
    table.insert(list_data, data2)
    self.List:SetData(list_data)

    local item_id = data2.strengthen_consume_item[0].item_id
    local need_num = data2.strengthen_consume_item[0].num
    local has_num = Item.GetNum(item_id)
    UH.SetIcon(self.Icon, Item.GetIconId(item_id) ,ICON_TYPE.ITEM)
    UH.SetText(self.Num, DataHelper.ConsumeNum(has_num, need_num,true))
    self.is_can_up = has_num >= need_num
    self.item_id = item_id
end

function PetInbornUpPanel:OnDestroy()
    PetInbornData.Instance:SetInboenUpData({})
    UIWFlushPanel.OnDestroy(self)
end

function PetInbornUpPanel:OnUpClick()
    if self.is_can_up then
        local info = self.data:GetInboenUpData()
        PetCtrl.SendPetOp(PET_OP_TYPE.PetInbornSkillLevelUp, self.pet.index, info.inborn_group_index)
        self.EffectTool:Play("7165044")
    else
        PublicPopupCtrl.Instance:Error(Language.PetInborn.Tip6)
        MainOtherCtrl.Instance:GetWayViewById(self.item_id)
    end
end

PetInbornUpItem = PetInbornUpItem or DeclareMono("PetInbornUpItem", UIWidgetBaseItem)
function PetInbornUpItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local cfg = data
    local sp_id = cfg.sp_id
    local effect_cfg = Cfg.SpecialEffect(sp_id)
    UH.SetIcon(self.SkillIcon, effect_cfg.icon_id, ICON_TYPE.BUFF)
    UH.SetText(self.SkillName, string.format(Language.PetInborn.Text1,effect_cfg.name, cfg.inborn_level))
    UH.SetText(self.SkillDesc, effect_cfg.reward_description)
end


-- ============ 宠物天赋总览 ==========
PetInbornPreview = PetInbornPreview or DeclareView("PetInbornPreview", "pet/pet_inborn/pet_inborn_preview")
function PetInbornPreview:PetInbornPreview()
    UH.SetText(self.Title, Language.PetInborn.Title4)
end

-- param_t = id, level
function PetInbornPreview:LoadCallback(param_t)
    param_t = param_t or {}
    local cfg = PetInbornData.Instance:GetInbornIdCfg(param_t.id, param_t.level > 0 and param_t.level or 1)
    local sp_id = cfg.sp_id
    local effect_cfg = Cfg.SpecialEffect(sp_id)
    UH.SetIcon(self.Icon, effect_cfg.icon_id, ICON_TYPE.BUFF)
    UH.SetText(self.Name, effect_cfg.name)
    local level_str = param_t.level > 0 and string.format(Language.PetInborn.Text2,param_t.level) or ColorStr(Language.PetInborn.Text3,COLORSTR.Red8)
    UH.SetText(self.Level, string.format(Language.PetInborn.Text4,level_str))

    local list_data = PetInbornData.Instance:GetInbornIdGroup(param_t.id)
    self.List:SetData(list_data)
end

function PetInbornPreview:OnCloseClick()
    ViewMgr:CloseView(PetInbornPreview)
end

PetInbornPreviewItem = PetInbornPreviewItem or DeclareMono("PetInbornPreviewItem", UIWidgetBaseItem)
function PetInbornPreviewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Level, string.format(Language.PetInborn.Text2, data.inborn_level))
    local sp_id = data.sp_id
    local effect_cfg = Cfg.SpecialEffect(sp_id)
    UH.SetText(self.Desc, effect_cfg.reward_description)
end