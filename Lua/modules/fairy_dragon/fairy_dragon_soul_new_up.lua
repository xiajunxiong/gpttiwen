-- 龙魂升级
FiaryDragonSoulUpView = FiaryDragonSoulUpView or DeclareView("FiaryDragonSoulUpView", "fairy_dragon/fairy_dragon_soul_up_view_1")
function FiaryDragonSoulUpView:FiaryDragonSoulUpView()
    self.Board:SetData(self:GetType(),Language.FairyDragon.Title2,Vector2.New(470, 695))
end


FiaryDragonSoulUpPanel = FiaryDragonSoulUpPanel or DeclareMono("FiaryDragonSoulUpPanel", UIWFlushPanel)
function FiaryDragonSoulUpPanel:FiaryDragonSoulUpPanel()
    self.data = FairyDragonSoulData.Instance
    self.data_cares = {
        {data = self.data.sel_stuff_soul, func = self.FlushStuff, init_call = false},
        {data = self.data.soul_item_list, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushStuff, init_call = false},
    }
end

function FiaryDragonSoulUpPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function FiaryDragonSoulUpPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetUpSoul(nil)
    self.data:ClearSoulStuff()
end

function FiaryDragonSoulUpPanel:FlushPanel()
    self:FlushBaseInfo()
    self:FlushInfo()
end

function FiaryDragonSoulUpPanel:FlushBaseInfo()
    self.item = self.data:GetUpSoul()
    if TableIsEmpty(self.item) then
        ViewMgr:CloseView(FiaryDragonSoulUpView)
        return
    end
    self.ItemCell:SetData(self.item)
    UH.SetText(self.Name, self.item:QuaName())
    local is_max = self.item:IsFullLevel()
    local btn_text = is_max and Language.FairyDragon.MaxLevelText or Language.FairyDragon.LevelUp
    UH.SetText(self.BtnText, btn_text)
end

function FiaryDragonSoulUpPanel:FlushInfo()
    local cur_level = self.item:Level()
    local is_max = self.item:IsFullLevel()
    local preview_level = is_max and cur_level or cur_level + 1
    self.LevelUpFlag:SetActive(preview_level > cur_level)
    UH.SetText(self.Level1, string.format(Language.FairyDragon.Level2, cur_level))
    if preview_level > cur_level then
        UH.SetText(self.Level2, string.format(Language.FairyDragon.Level2, preview_level))
    end

    -- 随机属性
    local rand_attri_list = self.item:RandAttriList()
    self.EmptyState:SetActive(TableIsEmpty(rand_attri_list))
    local next_random_cfg = self.item:GetNextSpInCfg()
    local attri_cfg = self.item:AttriCfg()
    for i,v in pairs(rand_attri_list) do
        v.is_show_add = attri_cfg.is_random == 1
    end
    self.RandAttriList:SetData(rand_attri_list)
    if not next_random_cfg or TableIsEmpty(rand_attri_list) then
        UH.SetText(self.RandTip, "")
    else
        local num = self.data.other_cfg.random_level or 3
        UH.SetText(self.RandTip, string.format(Language.FairyDragon.Text16, num))
    end

    -- 基础属性
    local base_and_next_attri_list = self.item:BaseAndNextAttriList(preview_level)
    --LogError("升级属性预览", preview_level,base_and_next_attri_list)
    self.BaseAttriList:SetData(base_and_next_attri_list)

    -- 技能
    local skill = self.item:Skill()
    UH.SetText(self.SkillDesc, skill:Desc())

    self.StuffInfo:SetActive(not is_max)

    self:FlushStuff()
end

function FiaryDragonSoulUpPanel:FlushStuff()
    local stuff_soul_list = self.data.sel_stuff_soul:Val()
    local attri_cfg = self.item:AttriCfg()
    local soul_num = table.nums(stuff_soul_list)
    self.AddFlag:SetActive(soul_num == 0)
    local neee_soul_num = attri_cfg.cost_itself
    local stuff_soul_num_str = DataHelper.ConsumeNum(soul_num,neee_soul_num)
    UH.SetText(self.StuffSoulNum, stuff_soul_num_str)
    if soul_num == 0 then
        self.StuffSoulCell:SetData(Item.Create({item_id = 0}))
    else
        local stuff_soul = FairyDragonSoul.New({item_id = self.item.item_id})
        self.StuffSoulCell:SetData(stuff_soul)
    end
    self.is_soul_ok = soul_num >= neee_soul_num
    
    local stuff = Item.Create(attri_cfg.item_list[0])
    self.StuffCell:SetData(stuff)
    local has_num = Item.GetNum(stuff.item_id)
    local need_num = stuff.num
    local stuff_num_str = DataHelper.ConsumeNum(has_num,need_num)
    UH.SetText(self.StuffNum, stuff_num_str)
    self.is_stuff_ok = has_num >= need_num
    self.stuff_item_id = stuff.item_id
end

function FiaryDragonSoulUpPanel:OnAddClick()
    ViewMgr:OpenView(FairyDragonSoulStuffSel)
end

function FiaryDragonSoulUpPanel:OnUpClick()
    if self.item:IsFullLevel() then
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.MaxLevelTip)
        return
    end
    if not self.is_stuff_ok then
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.NotItemTip)
        MainOtherCtrl.Instance:GetWayViewById(self.stuff_item_id)
        return
    end

    if not self.is_soul_ok then
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.Tip6)
        return
    end
    --FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.SOUL_LEVEL_UP, self.item.index)
    FairyDragonCtrl.Instance:SendSoulLevelUpReq(self.item.index)
    self.data:ClearSoulStuff()
end

function FiaryDragonSoulUpPanel:OnLockClick()
    self.Preview:SetActive(true)
    self.ParentObj:SetLocalPosition(Vector3.New(-275,0,0))
    local list_data = self.item:AllEffectList()
    self.PreviewList:SetData(list_data)
end

function FiaryDragonSoulUpPanel:OnPreviewLockClick()
    self.ParentObj:SetLocalPosition(Vector3.New(0,0,0))
    self.Preview:SetActive(false)
end


FiaryDragonSoulUpPreviewItem = FiaryDragonSoulUpPreviewItem or DeclareMono("FiaryDragonSoulUpPreviewItem", UIWidgetBaseItem)
function FiaryDragonSoulUpPreviewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Level, string.format(Language.FairyDragon.Level2, data.soul_level))
    UH.SetText(self.Desc, data.desc)
end


FiaryDragonSoulUpAttriItem = FiaryDragonSoulUpAttriItem or DeclareMono("FiaryDragonSoulUpAttriItem", UIWidgetBaseItem)
function FiaryDragonSoulUpAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local attri_type = data.attri_type
    self.Img:SetActive(data.cur_attri_value ~= data.next_attri_value)
    self.AddValue:SetObjActive(data.cur_attri_value ~= data.next_attri_value)
    local attri_info = CommonStruct.AttrStruct(attri_type,data.cur_attri_value)
    UH.SetText(self.AttriValue, attri_info.attr_percent_value)
    UH.SetText(self.AttriName, string.format("%s",attri_info.attr_name))
    if data.cur_attri_value ~= data.next_attri_value then
        local add_info = CommonStruct.AttrStruct(attri_type,data.next_attri_value)
        UH.SetText(self.AddValue, add_info.attr_percent_value)
    end
end


FiaryDragonSoulUpRandAttriItem = FiaryDragonSoulUpRandAttriItem or DeclareMono("FiaryDragonSoulUpRandAttriItem", UIWidgetBaseItem)
function FiaryDragonSoulUpRandAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local attri_info = CommonStruct.AttrStruct(data.attri_type,data.attri_value)
    local t_soul_item = FairyDragonSoulData.Instance:GetUpSoul()
    if t_soul_item then
        local group_id = t_soul_item:RandAttriGroupId()
        local attri_index = data.index
        local attri_level = data.level
        local cfg = Cfg.FairyDragonRandAttriCfg(group_id, attri_index, attri_level)
        local next_cfg = Cfg.FairyDragonRandAttriCfg(group_id, attri_index, attri_level + 1, false)
        local qua = cfg.attri_color
        local color = QualityColorStr[qua]
        UH.SetText(self.Name, ColorStr(attri_info.attr_name, color))
        UH.SetText(self.Value, ColorStr(attri_info.attr_percent_value, color))
        self.Img:SetActive(next_cfg ~= nil and data.is_show_add)
        self.AddValue:SetObjActive(next_cfg ~= nil and data.is_show_add)
        if next_cfg then
            local next_info = CommonStruct.AttrStruct(next_cfg.attri_type,next_cfg.attri_num)
            UH.SetText(self.AddValue, ColorStr(next_info.attr_percent_value, color))
        end
    else
        self.Img:SetActive(false)
        self.AddValue:SetObjActive(false)
        UH.SetText(self.Name, attri_info.attr_name)
        UH.SetText(self.Value, attri_info.attr_percent_value)
    end
end



-- ================ 龙魂材料选择 ================
FairyDragonSoulStuffSel = FairyDragonSoulStuffSel or DeclareView("FairyDragonSoulStuffSel", "fairy_dragon/fairy_dragon_soul_stuff_sel")
function FairyDragonSoulStuffSel:FairyDragonSoulStuffSel()
    self.Board:SetData(self:GetType(),Language.FairyDragon.Title4,Vector2.New(588, 520))
    self.data = FairyDragonSoulData.Instance
end

function FairyDragonSoulStuffSel:LoadCallback(param_t)
    -- 需要升级的龙魂
    self.soul_item = param_t.item or self.data:GetUpSoul()
    local attri_cfg = self.soul_item:AttriCfg()
    self.max_sel_count = attri_cfg.cost_itself or 1
    self:FlushPanel()
end

function FairyDragonSoulStuffSel:FlushPanel()
    local list_data = self:GetListData(self.soul_item)
    self.List:SetData(list_data)
    self.NotTip:SetActive(TableIsEmpty(list_data))
end

-- 同名同品质1级龙魂+材料道具
function FairyDragonSoulStuffSel:GetListData(item)
    local list_data = {}
    local item_list = self.data:GetItemList()
    local t_qua = item:Color()
    local t_type = item:SoulType()
    for i,v in pairs(item_list) do
        -- if v.index ~= self.soul_item.index and v:Level() == 1 and v:SoulType() == t_type and v:Color() == t_qua then
        --     table.insert(list_data, v)
        -- end
        if v.item_id == item.item_id and v.index ~= self.soul_item.index and v:Level() == 1 then
            table.insert(list_data, v)
        end
    end
    for i = #list_data + 1, 15 do
        table.insert(list_data, Item.Create({item_id = 0}))
    end
    return list_data
end

function FairyDragonSoulStuffSel:OnSelClick(data)
    if self.is_downed then
        self.is_downed = nil
        return
    end
    if data.item_id == 0 then
        return
    end
    local is_add = not self.data.sel_stuff_soul[data]
    local nums = table.nums(self.data.sel_stuff_soul:Val())
    if is_add and nums >= self.max_sel_count then
        PublicPopupCtrl.Instance:Center(string.format(Language.FairyDragon.Text23, self.max_sel_count))
        return
    end
    self.data:AddSoulStuff(data)
    self:FlushPanel()
end

function FairyDragonSoulStuffSel:OnBtnClick()
    ViewMgr:CloseView(FairyDragonSoulStuffSel)
end



FairyDragonSoulStuffSelItem = FairyDragonSoulStuffSelItem or DeclareMono("FairyDragonSoulStuffSelItem", UIWidgetBaseItem)
function FairyDragonSoulStuffSelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(data)
    self.SelImg:SetActive(FairyDragonSoulData.Instance.sel_stuff_soul[data] == 1)
    self.LevelObj:SetActive(data.item_id ~= 0)
    if data.item_id >  0 then
        UH.SetText(self.Level, "Lv" .. data:Level())
    end
end

function FairyDragonSoulStuffSelItem:OnDestroy()
    TimeHelper:CancelTimer(self.down_time_ht)
    TimeHelper:CancelTimer(self.time_ht_2)
end

function FairyDragonSoulStuffSelItem:OnDown()
    TimeHelper:CancelTimer(self.down_time_ht)
    self.down_time_ht = Invoke(function ()
        local info = {
            item_list = {self.data},
        }
        ItemInfoCtrl.Instance:ItemInfoView(info)
        self.is_downed = true
        TimeHelper:CancelTimer(self.time_ht_2)
        self.time_ht_2 = Invoke(function ()
            self.is_downed = nil
        end)
    end, 1)
end

function FairyDragonSoulStuffSelItem:OnUp()
    TimeHelper:CancelTimer(self.down_time_ht)
end