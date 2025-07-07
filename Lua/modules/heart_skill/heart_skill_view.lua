HeartSkillView = HeartSkillView or DeclareMono("HeartSkillView", UIWFlushPanel)

function HeartSkillView:HeartSkillView()
    self.data = HeartSkillData.Instance
    self.data_cares = {
        {data = self.data.initiat_info, func = self.FlushInitiatList, init_call = false, keys = {"val"}},
        {data = self.data.passive_info, func = self.FlushPassiveList, init_call = false, keys = {"val"}},
        {data = self.data.equipes_info, func = self.OnFlushEquipList, init_call = true, keys = {"info"}},
        {data = self.data.red_point_data,func = self.FlushRemindLlist,init_call = false, keys = {"val"}},
        {data = BagData.Instance.item_grid,func = self.OnFlush, init_call = false},
    }
end

function HeartSkillView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.ShowTimer)
end

function HeartSkillView:Awake()
    UIWFlushPanel.Awake(self)
    self:OnClickToggle(1)
    self:FlushRedPiont()
end

function HeartSkillView:OnDisable()
    UIWFlushPanel.OnDisable(self)
    if self.effect_handle then
        self.Effect:Stop(self.effect_handle)
        self.effect_handle = nil
    end
end

function HeartSkillView:OnClickToggle(type)
    self.List[type]:SetDataList(self.data:GetSkillList(type))
    self:OnClickItem(true,self.data:GetSkillList(type)[1])
end

function HeartSkillView:OnFlushEquipList()
    for i=1,self.ItemList:Length() do
        self.ItemList[i]:SetData(self.data:GetHoleData(i))
    end
end

function HeartSkillView:OnFlush()
    self:FlushRemindLlist()
    self:OnClickItem(true,self.select_data or {})
end

function HeartSkillView:FlushInitiatList()
    self.List[1]:SetData(self.data:GetSkillList(1))
    self.List[1]:ClickItemData(self.select_data)
    self:OnClickItem(true,self.select_data or {})
end

function HeartSkillView:FlushPassiveList()
    self.List[2]:SetData(self.data:GetSkillList(2))
    self.List[2]:ClickItemData(self.select_data)
    self:OnClickItem(true,self.select_data or {})
end

function HeartSkillView:FlushRemindLlist()
    for i=1,self.ItemList:Length() do
        self.ItemList[i]:OnRemind()
        self.ItemList[i]:FlushLevel()
    end
    self:FlushRedPiont()
    for i=1,self.List:Length() do
        for k,item in pairs(self.List[i].index_item) do
            item:OnRemind()
        end
    end
end

function HeartSkillView:FlushRedPiont()
    for i=1,self.List:Length() do
        local list = self.data:GetSkillList(i)
        self.RedPiont[i]:SetNum(self.data:GetRemindListNum(list))
    end
end

function HeartSkillView:OnClickItem(cell,data)
    local Lang = Language.HeartSkill
    local level = self.data:GetSkillLevel(data)
    local max_level = data.limit_max_level

    UH.SetText(self.Name, self.data:GetSkillName(data,true))
    UH.SetText(self.Type,Lang.SkillType[data.heart_skill_type])
    UH.SetText(self.Level,Format(Lang.Level,level,max_level))

    local curr_config = self.data:GetUpgradeConfig(data.heart_skill_seq,level)
    local config = self.data:GetUpgradeConfig(data.heart_skill_seq,self.data:GetSkillItemLevel(data) + 1) or curr_config
    local item,cur_num,need_num = self.data:GetItemConsume(config)

    UH.SetText(self.ItemName,item:QuaName())
    UH.SetText(self.ItemNum,DataHelper.ConsumeNum(cur_num,need_num))
    UH.SetText(self.BtnName,Lang.BtnName[HeartSkillData.Instance:GetSkillActive(data) and 1 or 2])

    self.ItemCall:SetData(item)
    self.MaxLevel:SetActive(level == max_level)
    self.Consume:SetActive(level ~= max_level)

    local curr_skill_config = SkillData.GetDecConfig(data.heart_skill_id,level)
    local curr_attr_config = self.data:GetAttrConfig(data.heart_skill_seq,level)
    local curr_attr_desc = self.data:GetAttrDesc(curr_attr_config)
    UH.SetText(self.Desc,Format(Lang.Description,curr_attr_desc) .. (curr_skill_config.description or ""))

    local next_skill_config = SkillData.GetDecConfig(data.heart_skill_id,level + 1) or {}
    local next_attr_config = self.data:GetAttrConfig(data.heart_skill_seq,level + 1)
    local next_attr_desc = self.data:GetAttrDesc(next_attr_config)

    UH.SetText(self.Next,Format(Lang.Description,next_attr_desc) .. (next_skill_config.description or ""))
    self.SkillItem:SetData({skill_id = data.heart_skill_id,skill_level = level})
    self.GetWayItem = cur_num < need_num and item or nil
    self.select_data = data or {}
end

function HeartSkillView:OnClickTips()
    PublicPopupCtrl.Instance:HelpTip(self.data:GetClickDesc(self.select_data))
end

function HeartSkillView:OnClickUpgrade()
    if self.ShowTimer then
        return
    end
    local heart_skill_seq = self.select_data.heart_skill_seq
    if self.GetWayItem then
        MainOtherCtrl.Instance:GetWayView({item = self.GetWayItem})
        HeartSkillCtrl.Instance:SendHeartSkillReq(1,heart_skill_seq,2)
    else
        self:PlayEffect(3165011)
        self.ShowTimer = Invoke(function()
            HeartSkillCtrl.Instance:SendHeartSkillReq(1,heart_skill_seq,2)
            self.ShowTimer = nil
        end,1.5)
    end
end

function HeartSkillView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(139)
end

function HeartSkillView:OnClickResonate()
    ViewMgr:OpenView(HeartSkillResonateView)
end

function HeartSkillView:OnClickCompose()
	ViewMgr:OpenView(HeartSkillChangeView,{config = Config.heart_skill_cfg_auto.heart_skill_displace[1]})
end

function HeartSkillView:PlayEffect(key)
    if self.effect_handle then
        self.Effect:Stop(self.effect_handle)
        self.effect_handle = nil
    end
    if self.effect_handle == nil then
        self.effect_handle = self.Effect:Play(key)
    end
end

function HeartSkillView:OnClickAttribute()
    local attr_list = self.data:GetAttrMainList()
    self.TotalAttriBgRect.sizeDelta = Vector2.New(400, #attr_list <= 8 and 290 or 330)
    self.AttrList:SetData(attr_list)
    self.EmptyState:SetActive(#attr_list == 0)
    self.EmptyImage:SetActive(#attr_list ~= 0)
    self.AttributeBack:SetActive(true)
end

function HeartSkillView:OnClickBack()
    self.AttributeBack:SetActive(false)
end
----------------------------HeartSkillItem----------------------------
HeartSkillItem = HeartSkillItem or DeclareMono("HeartSkillItem", UIWidgetBaseItem)

function HeartSkillItem:SetData(data)
    local level = data.deblocking_level
    local is_open = RoleData.Instance:GetRoleLevel() >= level
    local index = HeartSkillData.Instance:GetEquipSkill(data.hole_seq)
    data.skill_seq = HeartSkillData.Instance:GetSkillSeq(data.heart_skill_type,index)
    local config = HeartSkillData.Instance:GetSkillConfig(data.skill_seq)
    if config ~= nil then
        UH.SetIcon(self.Icon,config.item_id)
        UH.SetText(self.Name,HeartSkillData.Instance:GetSkillName(config))
        UH.SpriteName(self.QuaIcon,COLOR_FRAME_SPRITE[Item.GetColor(config.item_id)])
        UH.SetText(self.Level,HeartSkillData.Instance:GetSkillLevel(config))
    end
    UH.SetText(self.LockLevel,Format(Language.HeartSkill.LimitLevel,level))

    self.Panel:SetActive(is_open and index ~= -1)
    self.Block:SetActive(is_open and index == -1)
    self.Lock:SetActive(not is_open)
    UIWidgetBaseItem.SetData(self, data)

    TimeHelper:CancelTimer(self.EffectTimer)
    self.EffectTimer = Invoke(function()
        self:OnRemind()
    end,0.5)
end

function HeartSkillItem:OnRemind()
    local had_remind = HeartSkillData.Instance:GetUnlockFlag(self.data.hole_seq) == 0
    local can_remind = HeartSkillData.Instance:GetItemRemindFlag(self.data)
    self.RedPiont:SetActive(can_remind)
    self.Effect:SetObjActive(had_remind and can_remind)
end

function HeartSkillItem:FlushLevel()
    local config = HeartSkillData.Instance:GetSkillConfig(self.data.skill_seq)
    if config ~= nil then
        UH.SetText(self.Level,HeartSkillData.Instance:GetSkillLevel(config))
    end
end

function HeartSkillItem:OnClickBlock()
    ViewMgr:OpenView(HeartSkillSelectView,self.data)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function HeartSkillItem:OnClickLock()
    PublicPopupCtrl.Instance:Center(Format(Language.HeartSkill.LimitTip,self.data.deblocking_level))
end

function HeartSkillItem:OnClickEffect()
    self.effect_handle = self.Effect:Play(3161046)
    self.Image:SetActive(false)
    self.ShowTimer = Invoke(function()
        HeartSkillCtrl.Instance:SendHeartSkillReq(6,self.data.hole_seq)
        self.Effect:Stop(self.effect_handle)
    end,1.5)
end

function HeartSkillItem:OnDestroy()
    TimeHelper:CancelTimer(self.ShowTimer)
    TimeHelper:CancelTimer(self.EffectTimer)
end

----------------------------HeartSkillCall----------------------------
HeartSkillCall = HeartSkillCall or DeclareMono("HeartSkillCall", UIWidgetBaseItem)

function HeartSkillCall:SetData(data)
    local is_active = HeartSkillData.Instance:GetSkillActive(data)

	UH.SetIcon(self.Icon, data.item_id)
    UH.SetText(self.Name,HeartSkillData.Instance:GetSkillName(data))
    
	UH.SpriteName(self.QuaIcon,COLOR_FRAME_SPRITE[Item.GetColor(data.item_id)])
    UH.SetText(self.Level,"Lv." .. HeartSkillData.Instance:GetSkillLevel(data))

    self.LevelGame:SetActive(is_active)
    self.Inter.Interactable = is_active
    if self.BlockGame then
        self.BlockGame:SetActive(is_active)
    end
    if self.BtnUse then
        self.BtnUse:SetActive(HeartSkillData.Instance:IsUseItemCell(data))
    end
    if self.Mark then
        self.Mark:SetActive(not is_active)
    end
    UIWidgetBaseItem.SetData(self, data)
    self:OnRemind()
end

function HeartSkillCall:OnRemind()
    if self.RedPiont then
        self.RedPiont:SetNum(HeartSkillData.Instance:GetRemindItemNum(self.data) and 1 or 0)
    end
end

function HeartSkillCall:Click()
	self.ToggleClick:Trigger()
end

----------------------------HeartSkillSelectView----------------------------
HeartSkillSelectView = HeartSkillSelectView or DeclareView("HeartSkillSelectView", "role/role_heart_skill_select")

VIEW_DECLARE_MASK(HeartSkillSelectView,ViewMask.BlockClose)

function HeartSkillSelectView:HeartSkillSelectView()
    self.data = HeartSkillData.Instance
    self.Board:SetData(self:GetType(),Language.HeartSkill.SelectTitle,Vector2.New(700,584))
    UH.SetText(self.Tips,Config.language_cfg_auto.textdesc[140].desc)
end

function HeartSkillSelectView:LoadCallback(param_t)
    local list = self.data:GetSkillList(param_t.heart_skill_type)
    local index = DataHelper.SimpleIndex(list,[[heart_skill_seq]],param_t.skill_seq) or 1
    self.List:SetDataList(list,index)
    self.param_t = param_t
    self:OnClickItem(true,list[index or 1])
end

function HeartSkillSelectView:OnClickItem(cell,data)
    local config = SkillData.GetDecConfig(data.heart_skill_id,self.data:GetSkillLevel(data))
    UH.SetText(self.Desc,config.description)
    local BtnName = Language.HeartSkill.SelectBtnName
    local info = self.data:GetSkillInfo(data) or {}
    local index = self.data:GetEquipSkill(self.param_t.hole_seq)
    UH.SetText(self.BtnName,BtnName[index == -1 and 1 or (info.index == index and 2 or 3)])
    self.select_data = data or {}
end

function HeartSkillSelectView:OnClickUse()
    local info = self.data:GetSkillInfo(self.select_data)
    if info == nil or not self.data:GetSkillActive(self.select_data) then
        PublicPopupCtrl.Instance:Center(Language.HeartSkill.NotActiveTip)
        return
    end
    local index = self.data:GetEquipSkill(self.param_t.hole_seq)
    if index == -1 then
        HeartSkillCtrl.Instance:SendHeartSkillReq(2,self.param_t.hole_seq,info.index)
    elseif index == info.index then
        HeartSkillCtrl.Instance:SendHeartSkillReq(3,self.param_t.hole_seq)
    else
        HeartSkillCtrl.Instance:SendHeartSkillReq(2,self.param_t.hole_seq,info.index)
    end
    ViewMgr:CloseView(HeartSkillSelectView)
end

----------------------------SkillItem----------------------------
SkillItem = SkillItem or DeclareMono("SkillItem", UIWidgetBaseItem)

function SkillItem:SetData(data)
    local config = SkillData.GetDecConfig(data.skill_id,data.skill_level)
    if config ~= nil then
        UH.SetIcon(self.Icon,config.icon_id,ICON_TYPE.SKILL)
    else
        UH.SetIcon(self.Icon)
    end
    UIWidgetBaseItem.SetData(self, data)
end

function SkillItem:OnClickBlock()
end

----------------------------HeartSkillResonateView----------------------------
HeartSkillResonateView = HeartSkillResonateView or DeclareView("HeartSkillResonateView", "role/role_heart_skill_resonate")

VIEW_DECLARE_MASK(HeartSkillResonateView,ViewMask.Block)

function HeartSkillResonateView:HeartSkillResonateView()
    self.data = HeartSkillData.Instance
    self.Board:SetData(self:GetType(),Language.HeartSkill.Resonate,Vector2.New(856,646))
end

function HeartSkillResonateView:LoadCallback(param_t)
    self.List:SetData(self.data:GetResonanceList())
end

----------------------------HeartSkillResonateItem----------------------------
HeartSkillResonateItem = HeartSkillResonateItem or DeclareMono("HeartSkillResonateItem", UIWidgetBaseItem)

function HeartSkillResonateItem:SetData(data)
    self.Grid:SetData(data.item_list)
    local level = HeartSkillData.Instance:GetResonateMinLevel(data.item_list)
    local is_active = HeartSkillData.Instance:IsResonateActive(data.item_list)
    UH.SetText(self.Title,is_active and data.title or Format(Language.HeartSkill.ResonateTitle,data.title,level))
    self.Item:SetData(HeartSkillData.Instance:GetResonanceConfig(data.resonance_group,level,not is_active))
    self.AttrList:SetData(HeartSkillData.Instance:GetResonateMain(data.attr_list,level,not is_active))
    self.Active:SetActive(is_active)
    UIWidgetBaseItem.SetData(self, data)
end

function HeartSkillResonateItem:OnClickToggle()
    if self.data.resonance_group == self.data.max_group then
        self.ShowTimer = Invoke(function()
            self.ScrollRect.verticalNormalizedPosition = 0
        end,0.1)
    end
end

function HeartSkillResonateItem:OnDestroy()
    TimeHelper:CancelTimer(self.ShowTimer)
end

----------------------------HeartSkillResonateAttrItem----------------------------
HeartSkillResonateAttrItem = HeartSkillResonateAttrItem or DeclareMono("HeartSkillResonateAttrItem", UIWidgetBaseItem)

function HeartSkillResonateAttrItem:SetData(data)
    local is_main = self.IsMain or data.is_main == true
    local level = Format(Language.HeartSkill.ResonateLevel,data.resonance_level)
    UH.SetText(self.Level,(is_main and data.is_active) and ColorStr(level,COLORSTR.Yellow21) or level)
    UH.SetText(self.Desc,self:GetAttrDesc(HeartSkillData.Instance:GetAttrList(data),is_main and data.is_active))
    UH.Alpha(self.ItemAlpha,data.is_active and 1 or 0.2352)
    UH.SpriteName(self.ImgSet,(is_main and data.is_active) and "GongMingBiaoTiDi" or "BiaoTiDi")
end

function HeartSkillResonateAttrItem:GetAttrDesc(attr_list,is_main)
    local space,desc = "",""
    for i=1,5-#attr_list do
        space = space .. "  "
    end
    for i,v in ipairs(attr_list) do
        local main_value = ColorStr(v.attr_name,COLORSTR.Yellow16) .. ColorStr("+" .. v.attr_value,COLORSTR.Green4)
        desc = desc .. (is_main and main_value or v.attr_name .. "+" .. v.attr_value) .. (i~=#attr_list and space or "")
    end
    return desc
end

----------------------------HeartSkillItemCall----------------------------
HeartSkillItemCall = HeartSkillItemCall or DeclareMono("HeartSkillItemCall", UIWidgetBaseItem)

function HeartSkillItemCall:SetData(data)
    local config = HeartSkillData.Instance:GetSkillConfig(data.id) or {}
	UH.SetIcon(self.Icon,config.item_id)
	UH.SpriteName(self.QuaIcon,COLOR_FRAME_SPRITE[Item.GetColor(config.item_id)])
end

function HeartSkillItemCall:MakeGrey(grey)
    self.Inter.Interactable = not grey
    self.Mark:SetActive(grey)
end

----------------------------HeartSkillAttrItem----------------------------
HeartSkillAttrItem = HeartSkillAttrItem or DeclareMono("HeartSkillAttrItem", UIWidgetBaseItem)

function HeartSkillAttrItem:SetData(data)
    local attr_name = Language.Common.AttrList[data.attr_type]
    local attr_value = DataHelper.Percent(data.attr_type,data.attr_value)
    UH.SetText(self.Desc,Format("%s: <color=#" ..COLORSTR.Green3 .. ">+%s</color>",attr_name,attr_value))
end

ItemInfoViewHeartSkillInfo = ItemInfoViewHeartSkillInfo or DeclareMono("ItemInfoViewHeartSkillInfo", ItemInfoViewItem)

function ItemInfoViewHeartSkillInfo:OnEnable()
    ItemInfoViewItem.OnEnable(self)
    self.List:SetData(HeartSkillData.Instance:GetConsumeItemData(self.item1.item_id) or {})
end

ItemInfoViewHeartSkillItem = ItemInfoViewHeartSkillItem or DeclareMono("ItemInfoViewHeartSkillItem", UIWidgetBaseItem)
function ItemInfoViewHeartSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Level,Format(Language.Bicycle.Level,data.equip_level))
    local config = data.skill_cfg or {}
    UH.SetText(self.Desc,(data.attr_desc or "") .. config.description)
end