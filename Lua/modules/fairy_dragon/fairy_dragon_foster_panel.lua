-- 神龙培养
FairyDragonFosterPanel = FairyDragonFosterPanel or DeclareMono("FairyDragonFosterPanel", UIWFlushPanel)
function FairyDragonFosterPanel:FairyDragonFosterPanel()
    self.data = FairyDragonData.Instance
    self.dragon = FairyDragon.Instance
    self.data_cares = {
        {data = self.dragon.base_data, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushUpInfo, init_call = false}
    }
end

function FairyDragonFosterPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function FairyDragonFosterPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushPanel()
end

function FairyDragonFosterPanel:FlushPanel()
    local stage_str = ColorStr(string.format(Language.FairyDragon.Text2, self.dragon:Stage()), COLORSTR.Red8)
    UH.SetText(self.Name, string.format("%s·%s",stage_str, self.dragon:Name()))

    local skin = self.dragon:Skin()
    if skin then
        self.ShowCell:SetData(Item.Create({item_id = skin.item_id}))
        UH.SetText(self.StarNum, skin:StarLevel())
    else
        UH.SetText(self.StarNum, self.dragon:Star())
        self.ShowCell:SetData(Item.Create({item_id = self.dragon.item_id}))
    end

    local attri_list = self.data:FosterAttriList()
    self.AttriList:SetData(attri_list)

    local skill_list = self.dragon:SkillList()
    self.dragon:SetShowSkillList(skill_list)
    self:FlushUpInfo()
end

function FairyDragonFosterPanel:FlushUpInfo()
    local level = self.dragon:Level()
    local is_need_break = self.dragon:IsNeedBreak()
    local next_foster_cfg = self.data:NextFosterCfg()
    local is_max = self.dragon:IsMaxLevel()

    self.UpLevelObj:SetActive(not is_need_break and level > 0)
    self.BreakObj:SetActive(is_need_break and level > 0)
    self.UnlockBtn:SetActive(not self.dragon:IsActive())
    self.UnlockRemind:SetNum(self.data:RemindUnlockDragon())
    self.StuffInfo:SetActive(not is_max or is_need_break)
    self.MaxFlag:SetActive(is_max and not is_need_break)

    UH.SetText(self.Level, string.format(Language.FairyDragon.Text3, self.dragon:Level()))
    local cur_exp = self.dragon:CurExp()
    local need_exp = self.dragon:NextExp()
    local progress_num = 1
    if need_exp > 0 then
        progress_num = cur_exp / need_exp
    end
    
    if is_max then
        UH.SetText(self.ExpProgress, "")
    else
        UH.SetText(self.ExpProgress, string.format("%s/%s", cur_exp, need_exp))
    end
    self.ProgressBar:SetProgress(progress_num)

    if not is_need_break then
        local stuff_cfg = self.data:FosterStuffCfg()
        local stuff_list = {}
        local sel_index = 1
        if self.stuff_data then
            local has_num = Item.GetNum(self.stuff_data.item_id)
            if has_num < 1 then
                self.stuff_data = nil
                self.show_stuff_info = false
            end
        end
        local is_set_sel = true
        for i,v in pairs(stuff_cfg) do
            table.insert(stuff_list, {item_id = i, need_num = 1})
            if self.stuff_data then
                if self.stuff_data.item_id == i then
                    sel_index = #stuff_list
                end
            else
                if is_set_sel then
                    local has_num = Item.GetNum(i)
                    if has_num > 0 then
                        sel_index = #stuff_list
                        is_set_sel = false
                    end
                end
            end
        end
        self.UpStuffList:SetData(stuff_list)
        self.UpStuffList:SetectItem(sel_index, true)
        self.UpRemind:SetNum(self.data:RemindFoster())
        self.stuff_data = stuff_list[sel_index]
    else
        local cur_foster_cfg = self.data:GetFosterCfg()
        local stuff_data = {item_id = cur_foster_cfg.break_item_id, need_num = cur_foster_cfg.break_item_num}
        local has_num = Item.GetNum(cur_foster_cfg.break_item_id)
        self.break_item = nil
        if has_num < cur_foster_cfg.break_item_num then
            self.break_item = cur_foster_cfg.break_item_id
        end
        self.BreakStuff:SetData(stuff_data)
        self.BreakRemind:SetNum(self.data:RemindFoster())
    end
end

function FairyDragonFosterPanel:OnSelStuffClick(data)
    self.stuff_data = data
    if self.show_stuff_info then
        local data = Item.Create({item_id = data.item_id})
        if data == nil then
            return
        end
        local info = {
            item_list = {data},
        }
        ItemInfoCtrl.Instance:ItemInfoView(info)
        ItemInfoData.Instance:SetClickDataPos(Vector2.New(350,-50))
    end
    self.show_stuff_info = true
end

-- type == 0 使用一次， ==1 使用全部
function FairyDragonFosterPanel:OnUseClick(type)
    type = type or 0
    if TableIsEmpty(self.stuff_data) then
        return
    end
    if self.dragon:IsMaxLevel() then
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.MaxLevelTip)
        return
    end
    local item_id = self.stuff_data.item_id
    local has_num = Item.GetNum(item_id)
    if has_num < 1 then
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.NotItemTip)
        MainOtherCtrl.Instance:GetWayViewById(item_id)
        return
    end
    FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.USE_DRAGON_EXP_ITEM, item_id, type)
end

function FairyDragonFosterPanel:OnBreakClick()
    if self.break_item then
        PublicPopupCtrl.Instance:Error(Language.FairyDragon.NotItemTip)
        MainOtherCtrl.Instance:GetWayViewById(self.break_item)
        return
    end
    FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.BREAK)
end

function FairyDragonFosterPanel:OnUnlockClick()
    FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.UNLOCK_DRAGON)
end



FairyDragonFosterAttriItem = FairyDragonFosterAttriItem or DeclareMono("FairyDragonFosterAttriItem", UIWidgetBaseItem)
function FairyDragonFosterAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.AttriR:SetObjActive(data.next_attri_value ~= data.cur_attri_value)
    self.Img:SetActive(data.next_attri_value ~= data.cur_attri_value)
    local cur_attri_info = CommonStruct.AttrStruct(data.attri_type, data.cur_attri_value)
    local l_value = ColorStr(cur_attri_info.attr_percent_value, COLORSTR.Yellow8)
    UH.SetText(self.AttriL, string.format("%s：%s",cur_attri_info.attr_name, l_value))
    if data.next_attri_value ~= data.cur_attri_value then
        local next_attri_info = CommonStruct.AttrStruct(data.attri_type, data.next_attri_value)
        local r_value = ColorStr(next_attri_info.attr_percent_value, COLORSTR.Green3)
        UH.SetText(self.AttriR, string.format("%s：%s",next_attri_info.attr_name, r_value))
        self.AttriL.gameObject:SetLocalPosition(Vector3.New(-76,0,0))
        self.AttriL.alignment = 3
    else
        self.AttriL.gameObject:SetLocalPosition(Vector3.New(0,0,0))
        self.AttriL.alignment = 4
    end
end

FairyDragonStuffItem = FairyDragonStuffItem or DeclareMono("FairyDragonStuffItem", UIWidgetBaseItem)
function FairyDragonStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data.item_id})
    self.ItemCell:SetData(item)
    local num = Item.GetNum(data.item_id)
    local need_num = data.need_num or 1

    local color = num >= need_num and COLORSTR.Green3 or COLORSTR.Red8
    local num_str = ColorStr(num, color)
    UH.SetText(self.Text, string.format("%s/%s", num_str, need_num))    
end