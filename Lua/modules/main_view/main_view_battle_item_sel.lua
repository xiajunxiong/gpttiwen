MainViewBattleItemSel = MainViewBattleItemSel or DeclareMono("MainViewBattleItemSel", UIWFlushPanel)
---ItemList====UIWidgetList

function MainViewBattleItemSel:MainViewBattleItemSel()
    self.battle_data = BattleData.Instance
    self.data_cares = {
        {data = BagData.Instance:GetColumnList(ItemColumnType.Item), func = self.FlushItemList},
        {data = BagData.Instance:GetColumnList(ItemColumnType.Supply), func = self.FlushItemList},
        {data = self.battle_data:BaseData(), func = self.FlushItemList, keys = {"fake_potion_num", "fake_potion_list"}}
    }

    self:RegisterGuideBtn("BattleReviveItem",CommonItemId.JiuZhuanHuiHun)
    self:RegisterGuideBtn("BattleMPItem",GuideData.Instance:MPItemId())
    self:RegisterGuideBtn("BattleHpItem",CommonItemId.XuePing3000)
    self:RegisterGuideBtn("BattleJieFengItem",CommonItemId.JieFengWan)
end

function MainViewBattleItemSel:RegisterGuideBtn(btn_key,item_id)
    GuideManager.Instance:RegisterGetGuideUi(
            btn_key,
            function()
                if self.SupplyList:SetupCompleted() then
                    for key, value in pairs(self.SupplyList:GetData()) do
                        if value.item_id == item_id then
                            return self.SupplyList.item_list[key].IconClick
                        end
                    end
                    GuideManager.Instance:ForceStop()
                end
                return nil, nil
            end)
end

function MainViewBattleItemSel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushItemList()
end
function MainViewBattleItemSel:OnClickAdd()
    CommerceData.Instance:SetJumpSub(4,0)
    ViewMgr:OpenViewByKey(Mod.Mall.Shop)
end
function MainViewBattleItemSel:FlushItemList()
    --当前持有假药显示假药列表
    local fake_potion_num = self.battle_data:BaseData().fake_potion_num
    local fake_potion_list = self.battle_data:BaseData().fake_potion_list
    if fake_potion_num > 0 then
        self.EquipList:SetData({})
        self.SupplyList:SetData(fake_potion_list)
        return
    end
    local item_list = BagData.Instance:GetColumnList(ItemColumnType.Item)
    local supply_list = BagData.Instance:GetColumnList(ItemColumnType.Supply)
    -- LogDG(item_list)
    -- Debuger.LogError("item_list===count=%d",#item_list)
    -- Debuger.LogError("supply_list===count=%d",#supply_list)
    local equip_list_data = {}
    local supply_list_data = {}
    local equipment_config = Config.equipment_auto
    local prof = RoleData.Instance:GetBaseData().profession * 100
    local level = RoleData.Instance:GetBaseData().level
    for _, it in pairs(item_list) do
        local ty = it:Type()
        local eq = equipment_config[it.item_id]
        if ty == 1 and eq and (eq.equip_type == 2 --[[ or eq.equip_type == 3 ]]) and (eq.limit_prof == 1 or eq.limit_prof == prof) and level >= eq.limit_level then
            table.insert(equip_list_data, it)
        end
    end
    for _, it in pairs(item_list) do
        if it.item_id == CommonItemId.XiangYaoBian then
			it.sortid = 0
            table.insert(supply_list_data, it)
        end
    end
    for _, it in pairs(supply_list) do
        if it:Type() == ItemType.Medicine then
            if it.item_id >= 23025 then
                it.sortid = 1
            else
                it.sortid = 2
            end
            table.insert(supply_list_data, it)
        end
    end

	table.sort(supply_list_data, function(a, b)
		if a.sortid ~= b.sortid then
			return a.sortid < b.sortid
		else
            if Item.GetUseType(a.item_id) ~= Item.GetUseType(b.item_id) then
                return Item.GetUseType(a.item_id) < Item.GetUseType(b.item_id)
            else
                return Item.GetConfig(a.item_id).param1 > Item.GetConfig(b.item_id).param1
            end
			return a.item_id < b.item_id
		end
	end)
    self.EquipList:SetData(equip_list_data)
    self.SupplyList:SetData(supply_list_data)
end

function MainViewBattleItemSel:OnClickClose()
    -- self.battle_data:SetSkillSelData(false)
    self.battle_data:ViewInfo().opened_sub = BattleSubView.None
end

function MainViewBattleItemSel:OnItemSelected(cell)
    local cell_data = cell:GetData()
    if cell.is_grey then
        return
    end
    if self.battle_data:ViewInfo().opened_sub == BattleSubView.Item then
        self.battle_data:ViewInfo().opened_sub = BattleSubView.None
    end

    local fake_potion_num = self.battle_data:BaseData().fake_potion_num
    if fake_potion_num > 0 then
        MainViewBattleItemSel.item_id = cell_data.item_id
        BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.USE_FAKE_POTION, cell_data.item_id, nil, cell_data)
        ---BattleCtrl.Instance:SetMove(self.battle_data.base.command_i, BATTLE_MOVE_TYPE.USE_FAKE_POTION, self.battle_data:CurCmdIdx() - 1, cell_data.item_id)
        return
    end
    local usty = Item.GetUseType(cell_data.item_id)
    if usty == ItemUseType.MPMedicine or usty == ItemUseType.HPMedicine then
        if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_BATTLEFIELD then
            if AnotherData.Instance:GetRoleLevel() < cell_data:LimitLv() then
                PublicPopupCtrl.Instance:Center(ErrorText[172])
                return
            end
        else
            if RoleData.Instance:GetRoleLevel() < cell_data:LimitLv() then
                PublicPopupCtrl.Instance:Center(ErrorText[172])
                return
            end
        end
    end
    if usty == ItemUseType.MPMedicine then
		local base = BattleData.Instance:BaseData()
		if base.mp_item_can_use_tiems <= 0 then
            if base.is_pvp then
				PublicPopupCtrl.Instance:Center(ErrorText[142])
			else
				local limit = Config.fight_other_auto.other[1].pve_mana_potions_limit
				PublicPopupCtrl.Instance:Center(string.format(ErrorText[173], limit))
			end
            self.battle_data:ViewInfo().opened_sub = BattleSubView.Item
            return
        end
    end
    local ty = cell_data:Type()
    if ty == 1 or ty == 2 then --equipment
        -- command_i
        BattleCtrl.Instance:SetMove(self.battle_data.base.command_i, BATTLE_MOVE_TYPE.USE_ITEM, self.battle_data:CurCmdIdx() - 1, cell_data:GridColumn(), cell_data:GridIndex())
    else
        MainViewBattleItemSel.item_id = cell_data.item_id
        BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.USE_ITEM, cell_data:GridColumn(), cell_data:GridIndex(), Item.GetUseType(cell_data.item_id))
    end
    -- local ty = cell_data:Type()
    local eq = Config.equipment_auto[cell_data.item_id]
    MainViewBattleItemSel.equip = ty == 1 and eq
    -- Debuger.LogError("MainViewBattleItemSel:OnItemSelected!!!,%s",tostring(cell.data:Name()))
end

function MainViewBattleItemSel:ToggleEquipView(toggle_val)
    -- LogError("ToggleEquipView",toggle_val)
    self.EquipList:SetObjActive(toggle_val)
    if toggle_val then
        self.ScrollRect.content = self.EquipContent
    end
end

function MainViewBattleItemSel:ToggleSupplyView(toggle_val)
    -- LogError("ToggleSupplyView",toggle_val)
    self.SupplyList:SetObjActive(toggle_val)
    if toggle_val then
        self.ScrollRect.content = self.SupplyContent
    end
end
