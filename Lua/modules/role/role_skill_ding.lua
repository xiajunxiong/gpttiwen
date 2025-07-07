RoleSkillDingView = RoleSkillDingView or DeclareView("RoleSkillDingView", "role/role_skill_ding", Mod.GlassTripod.Main)
function RoleSkillDingView:LoadCallback()
    --[[ local v2 = {
        Vector2.New(503, 584),
        Vector2.New(673, 584),
        Vector2.New(820, 584)
    } ]]
    --获取技能数
    --[[ local num = SkillData.Instance.skill_num
    local index = 1
    index = num <= 9 and index or 2
    index = num <= 12 and index or 3 ]]
    --UH.SizeDelta(self.RootRect, v2[1])
    self.Board:SetData(self:GetType(), Language.Role.RoleSkillDing, Vector2.New(1228, 596))
end
function RoleSkillDingView:CloseCallback()

end
------------------------------
RoleSkillDingViewPanel = RoleSkillDingViewPanel or DeclareMono("RoleSkillDingViewPanel", UIWFlushPanel)
function RoleSkillDingViewPanel:RoleSkillDingViewPanel()
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushItemNum},
    }
end

function RoleSkillDingViewPanel:FlushItemNum()
    local new_num = Item.GetNum(HuoBi[CurrencyType.SkyGlass])
    if self.old_num ~= nil and self.old_num < new_num and self.sended_cuilian then
        self.EffectTool:Play("100108149")
    end
    self.sended_cuilian = false
    self.old_num = new_num
    UH.SetText(self.CurrencyNum, self.old_num)
end

function RoleSkillDingViewPanel:onFlush()
    if self.compose_data_list == nil then
        self.compose_data_list = TableCopy(Config.train_auto.glass_tripod)
    end
    self.ComposeList:SetData(self.compose_data_list)
    self.ComposeList:ClickItem(1)
    local item = Item.Init(CommonItemId.SkyGlass)
    self.ItemCell:SetData(item)
    local config = item:Config()
    local description = config ~= nil and config.description or ""
    UH.SetText(self.DescText, config ~= nil and config.description or "")
    UH.SetText(self.NameText, config ~= nil and config.name or "")
end

function RoleSkillDingViewPanel:OnClickToggleItem(data)
    self.cur_select_seq = data.seq
end

function RoleSkillDingViewPanel:OnClickCuiLian()
    self.sended_cuilian = true
    RoleCtrl.Instance:SendCSTrainReq(1, self.cur_select_seq)
end

function RoleSkillDingViewPanel:OnClickAdd()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.SkyGlass]})})
end
--------------------------------------
RoleSkillDingItem = RoleSkillDingItem or DeclareMono("RoleSkillDingItem", UIWidgetBaseItem)
function RoleSkillDingItem:RoleSkillDingItem()
end

function RoleSkillDingItem:OnDestroy()
    if self.care_handle ~= nil then
        BagData.Instance.item_grid[ItemColumnType.Item]:Uncare(self.care_handle)
        self.care_handle = nil
    end
end

function RoleSkillDingItem:FlushItemInfo()
    if self.data == nil then
        return
    end
    local item_list = {}
    self.can_compose_num = -1
    for i=1,6 do
        if self.data["collection_itemfee_"..i] ~= 0 then
            local item = Item.New({item_id = self.data["collection_itemfee_"..i], need_num = self.data["collection_itemnum_"..i]})
            table.insert(item_list,item)
            --任何一个物品达不到合成所需时，后续物品不再判断数量
            if self.can_compose_num ~= 0 and self.data["collection_itemnum_"..i] ~= nil and self.data["collection_itemnum_"..i] ~= 0 then
                local item_num = Item.GetNum(self.data["collection_itemfee_"..i])
                local has_num = math.floor(item_num / self.data["collection_itemnum_"..i])
                if has_num < 1 then
                    self.can_compose_num = 0
                else
                    -- if self.can_compose_num == -1 then
                    --     self.can_compose_num = has_num
                    -- else
                    --     self.can_compose_num = self.can_compose_num < has_num and self.can_compose_num or has_num
                    -- end
                    self.can_compose_num = 1
                end
            end
        else
            break
        end
    end
    self.ItemList:SetData(item_list)
    UH.SetText(self.NumText, string.format(Language.Common.Xstr, self.can_compose_num > 0 and COLORSTR.Green5 or COLORSTR.Red12, self.data.compose_num))
end

function RoleSkillDingItem:Click()
    self.Toggle.isOn = true
end

function RoleSkillDingItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.care_handle == nil then
        self.care_handle = BagData.Instance.item_grid[ItemColumnType.Item]:Care(BindTool.Bind(self.FlushItemInfo, self))
    end
    self:FlushItemInfo()
end