MainViewBattleFabaoSel = MainViewBattleFabaoSel or DeclareMono("MainViewBattleFabaoSel", UIWFlushPanel)
---ItemList====UIWidgetList

function MainViewBattleFabaoSel:MainViewBattleFabaoSel()
    self.battle_data = BattleData.Instance
    self.data_cares = {
        {data = BattleData.Instance:FabaoList(), func = self.FlushFabaoList, init_call = true},
        {data = self.battle_data:BaseData(), func = self.FlushFabaoTimes, keys = {"fabao_remain_times"}},
    }
    GuideManager.Instance:RegisterGetGuideUi(
        "BattleFirstFabao",
        function()
            -- if self.FabaoList:SetupCompleted() then
                local fabao_id = GuideData.Instance:GetFabaoId()
                if fabao_id == nil then
                    return self.FabaoList.item_list[1]
                end
                for key, value in pairs(self.FabaoList:GetData()) do
                    if value.fabao_id == fabao_id then
                        return self.FabaoList.item_list[key]
                    end
                end
                if self.FabaoList.item_list and self.FabaoList.item_list[1] then
                    return self.FabaoList.item_list[1]
                end
            -- end
            return nil, nil
        end
    )
end

function MainViewBattleFabaoSel:FlushFabaoList()
    local list = BattleData.Instance:FabaoList()
    local count = table.nums(list)
    self.Times.gameObject:SetActive(count ~= 0)
    self.NoneObj:SetActive(count == 0)
    self.FabaoList:SetData(list)
end

function MainViewBattleFabaoSel:FlushFabaoTimes()
    local times = self.battle_data:FabaoInfoChange()
    local color = times > 0 and COLORSTR.Yellow18 or COLORSTR.Red15 
    UH.SetText(self.Times, string.format(Language.Main.Battle.FabaoRemainTime, color,times))
end

function MainViewBattleFabaoSel:OnClickClose()
    -- self.battle_data:SetSkillSelData(false)
    BattleData.Instance:ViewInfo().opened_sub = BattleSubView.None
end

function MainViewBattleFabaoSel:OnItemSelected(cell)
    if BattleData.Instance:ViewInfo().opened_sub == BattleSubView.Fabao then
        BattleData.Instance:ViewInfo().opened_sub = BattleSubView.None
    end
    local cell_data = cell:GetData()
    local ty = cell_data:Type()
    if ty == 1 or ty == 2 then --equipment
        -- command_i
        BattleCtrl.Instance:SetMove(self.battle_data.base.command_i, BATTLE_MOVE_TYPE.USE_ITEM, self.battle_data:CurCmdIdx() - 1, cell_data:GridColumn(), cell_data:GridIndex())
    else
        BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.USE_ITEM, cell_data:GridColumn(), cell_data:GridIndex())
    end
    -- Debuger.LogError("MainViewBattleFabaoSel:OnItemSelected!!!,%s",tostring(cell.data:Name()))
end

MainViewBattleFabaoItem = MainViewBattleFabaoItem or DeclareMono("MainViewBattleFabaoItem", UIWidgetBaseItem)

function MainViewBattleFabaoItem:MainViewBattleFabaoItem()
    self.data_care = BattleData.Instance:BaseData():Care(BindTool.Bind1(self.FlushFabaoCD, self), "fabao_cd")
end

function MainViewBattleFabaoItem:OnRelease()
    LogError("OnRelease")
    UIWidgetBaseItem.OnRelease(self)
    if self.data_care then
        BattleData.Instance:BaseData():Uncare(self.data_care)
        self.data_care = nil
    end
end

function MainViewBattleFabaoItem:FlushFabaoCD()
    local fabao_cd = BattleData.Instance:FabaoCD()
    self.CD.text = Language.Main.Battle.FabaoCD[fabao_cd]
    self.Is_in_CD = fabao_cd > 0
end

function MainViewBattleFabaoItem:SetData(data)
    self.data = data
    local cfg1 = FabaoData.Instance:FabaoAttrInfo(data.fabao_id)
    local advance = FabaoData.Instance:AdvanceCfg(data.fabao_id, data.fabao_jie or 0)[1]
    local fight_info = FabaoData.Instance:GetFabaoSkillInfo(data.fabao_skill_type)
    -- if advance then
        -- local add = 0
        -- local exp_info = FabaoData.Instance:FabaoListInfo(data.fabao_id)
        -- local jie_plus = cfg1.type == 1 and (cfg1.advanced_addition / 1000 * exp_info.fabao_jie) or cfg1.advanced_addition * exp_info.fabao_jie
        -- if cfg1.base_addition == 0 then
        --     add = exp_info.fabao_level * cfg1.level_addition / 1000 + jie_plus
        -- else
        --     add = cfg1.base_addition + ((exp_info.fabao_level * cfg1.level_addition) / 1000) + jie_plus
        -- end

        local is_percent = advance.percentage_show == 1
        local attri_str = is_percent and ((advance.advanced_addition / 1000) .. "%") or (advance.advanced_addition / 1000)
        local attri_str2 = is_percent and ((advance.advanced_addition2 / 1000) .. "%") or (advance.advanced_addition2 / 1000)

        if cfg1.max_advanced_addition > 0 and data.fabao_jie >= FabaoData.Instance:MaxJie() then
            local max_attri_show = cfg1.max_advanced_addition / 1000
            if is_percent then
                max_attri_show = tostring(max_attri_show) .. "%"
            end
            attri_str = string.format("%s(+%s)",tostring(attri_str),max_attri_show) 
            -- max_attri_show = string.format("<color=#%s>(+%s)</color>",COLORSTR.Red8,max_attri_show)
        end
        if cfg1.max_advanced_addition2 > 0 and data.fabao_jie >= FabaoData.Instance:MaxJie() then
            local max_attri_show = cfg1.max_advanced_addition2 / 1000
            if is_percent then
                max_attri_show = tostring(max_attri_show) .. "%"
            end
            attri_str2 = string.format("%s(+%s)",tostring(attri_str2),max_attri_show) 
            -- max_attri_show = string.format("<color=#%s>(+%s)</color>",COLORSTR.Red8,max_attri_show)
        end
        -- local jie_plus =
        -- data.type == 1 and (cfg1.advanced_addition / 1000 * jie_level) or cfg1.advanced_addition * jie_level
        -- local add_cur = attr_info.base_addition + ((level * cfg1.level_addition) / 1000) + jie_plus
        UH.SetText(self.Descr,string.format(fight_info.desc,tostring(attri_str), tostring(attri_str2)))
    -- else
    --     UH.SetText(self.Descr, cfg1.effect_desc)
    -- end
    UH.SetText(self.Name, cfg1.name)
    self:FlushFabaoCD()
    UH.SetIcon(self.Icon, cfg1.icon, ICON_TYPE.FABAO)
end

function MainViewBattleFabaoItem:OnDestroy()
    LogError("OnDestroy")
end

function MainViewBattleFabaoItem:OnClickItem()
    if self.Is_in_CD then 
        PublicPopupCtrl.Instance:Center(string.format(Language.Main.Battle.FabaoCDTime ,BattleData.Instance:FabaoCD()))
        return 
    end 
    if BattleData.Instance:FabaoInfoChange() == 0 then 
        PublicPopupCtrl.Instance:Center(Language.Main.Battle.FabaoNoTime)
        return 
    end 

    if BattleData.Instance:ViewInfo().opened_sub == BattleSubView.Fabao then
        BattleData.Instance:ViewInfo().opened_sub = BattleSubView.None
    end
    MainViewBattleFabaoItem.fabao_id = self.data.fabao_id
    BattleCtrl.Instance:EnterSelectTargetState(BATTLE_MOVE_TYPE.USE_FABAO, self.data.index)

    UIWidgetBaseItem.OnSelect(self, selected)
end

