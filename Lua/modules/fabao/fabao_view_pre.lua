FabaoUpPreView = FabaoUpPreView or DeclareView("FabaoUpPreView", "fabao/fabao_up_pre")
--LvupAttriText=====Text
function FabaoUpPreView:FabaoUpPreView()
    self.data = FabaoData.Instance
    self.lang = Language.Fabao
end

function FabaoUpPreView:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.UpPreTitle, Vector2.New(717, 528))


    local data = self.data:ViewData().select_data
    if data == nil then
        return
    end

    local lv_cfg1 = Cfg.FabaoUpgrade(data.id,1)[1]
    local lv_cfg2 = Cfg.FabaoUpgrade(data.id,2)[1]
    -- LogError(data.id,"|",lv_cfg1,"|",lv_cfg2)
    local attri_str = ""
    if lv_cfg2.attr_num_1 - lv_cfg1.attr_num_1 > 0 then
        attri_str = attri_str .. " " .. Language.Common.AttrList[lv_cfg1.attr_type_1] .. "+" .. DataHelper.Percent(lv_cfg1.attr_type_1,lv_cfg2.attr_num_1 - lv_cfg1.attr_num_1)
    end
    if lv_cfg2.attr_num_2 - lv_cfg1.attr_num_2 > 0 then
        attri_str = attri_str .. " " .. Language.Common.AttrList[lv_cfg1.attr_type_2] .. "+" .. DataHelper.Percent(lv_cfg1.attr_type_2,lv_cfg2.attr_num_2 - lv_cfg1.attr_num_2)
    end
    UH.SetText(self.LvupAttriText,string.format(Language.Fabao.UpPreAttriTitle,attri_str))

    -- local list1 = self.data:UpgradeList(data.id)
    local list2 = self.data:AdvanceList(data.id)
    -- local temp = {}
    -- for i, v in ipairs(list1) do
    --     table.insert(temp, v)
    --     if i % 3 == 0 then
    --         table.insert(temp, list2[i / 3])
    --     end
    -- end
    self.List:SetData(list2)
    if data.info.fabao_jie > 2 then
        self.List:AutoSlide(list2[data.info.fabao_jie],false)
    end
end

-- function FabaoUpPreView:OnFlush()
--     local data = self.data:ViewData().select_data
--     if data == nil then
--         return
--     end
--     local list1 = self.data:UpgradeList(data.id)
--     local list2 = self.data:AdvanceList(data.id)
--     local temp = {}
--     for i, v in ipairs(list1) do
--         table.insert(temp, v)
--         if i % 3 == 0 then
--             table.insert(temp, list2[i / 3])
--         end
--     end
--     self.List:SetData(temp)
-- end

FabaoPreItem = FabaoPreItem or DeclareMono("FabaoPreItem", UIWidgetBaseItem)
function FabaoPreItem:FabaoPreItem()
    self.data = FabaoData.Instance
end

function FabaoPreItem:SetData(data)
    local attr_data = self.data:FabaoAttrInfo(data.id)
    -- if data.level then
    --     UH.SetText(self.Level, string.format("Lv.%d", data.level))
    --     local upgrade, next_upgrade
    --     local id = data.id
    --     local level = data.level
    --     local is_max = level == 36
    --     if is_max then
    --         upgrade = self.data:UpgradeCfg(id, level)
    --     else
    --         upgrade = self.data:UpgradeCfg(id, level)
    --         next_upgrade = self.data:NextUpgradeCfg(id, level + 1)
    --     end
    --     self:SetAttr(upgrade[1], next_upgrade)
    --     local advanced_level = math.ceil(level / 3)
    --     local advance = self.data:AdvanceCfg(id, advanced_level)[1]
    --     local is_percent = advance.percentage_show == 1
    --     local cur = is_percent and ((advance.advanced_addition / 1000) .. "%") or advance.advanced_addition / 1000
    --     UH.SetText(self.Desc, string.format(attr_data.desc_1, cur, ""))
    -- else
        -- local upgrade, next_upgrade
        local id = data.id
        -- local level = data.advanced_level * 3
        local advanced_level = data.advanced_level
        -- local is_max = level == 36
        -- if is_max then
        --     upgrade = self.data:UpgradeCfg(id, level)
        -- else
        --     -- next_upgrade = self.data:NextUpgradeCfg(id, level + 1)
        --     upgrade = self.data:UpgradeCfg(id, level)
        -- end
        -- self:SetAttr(upgrade[1], next_upgrade)

        local cur, next, cur2, next2
        local advance = data
        local is_percent = data.percentage_show == 1
        cur = is_percent and ((advance.advanced_addition / 1000) .. "%") or advance.advanced_addition / 1000
        cur2 = is_percent and ((advance.advanced_addition2 / 1000) .. "%") or advance.advanced_addition2 / 1000
        if attr_data.max_advanced_addition > 0 and data.advanced_level >= FabaoData.Instance:MaxJie() then
            local max_attri_show = attr_data.max_advanced_addition / 1000
            if is_percent then
                max_attri_show = tostring(max_attri_show) .. "%"
            end
            max_attri_show = string.format("<color=#%s>(+%s)</color>",COLORSTR.Red8,max_attri_show)
            cur = cur .. max_attri_show

            local max_attri_show = attr_data.max_advanced_addition2 / 1000
            if is_percent then
                max_attri_show = tostring(max_attri_show) .. "%"
            end
            max_attri_show = string.format("<color=#%s>(+%s)</color>",COLORSTR.Red8,max_attri_show)
            cur2 = cur2 .. max_attri_show
        end
        -- if is_max then
        --     next = ""
        -- else
            -- advance = self.data:AdvanceCfg(id, advanced_level)[1]
            -- next =
            --     "(+" ..
            --     (is_percent and ((advance.advanced_addition / 1000 * 100) .. "%") or
            --         advance.advanced_addition / 1000 * 100) ..
            --         ")"
        -- end
        local my_fabao_data = FabaoData.Instance:ViewData().select_data
        if my_fabao_data == nil then
            return
        end
        if my_fabao_data.info.fabao_jie >= data.advanced_level then
            UH.SetText(self.Desc, string.format(attr_data.desc_1, cur,"", cur2,""))
            self.Desc.color = COLORS.Yellow8
            self.Level.color = COLORS.Yellow8
        else
            UH.SetText(self.Desc, string.format(attr_data.desc_1, "",cur, "",cur2))
            self.Desc.color = COLORS.Yellow4
            self.Level.color = COLORS.Yellow4
        end
        UH.SetText(self.Level, string.format(Language.Fabao.UpPreJieTitle, data.advanced_level))

        self.CurJieFlag:SetActive(data.advanced_level == my_fabao_data.info.fabao_jie)
    -- end
end

-- function FabaoPreItem:SetAttr(upgrade, next_upgrade)
--     local attr_type
--     local value
--     local show_next = next_upgrade ~= nil
--     next_upgrade = show_next and next_upgrade[1] or next_upgrade
--     for i = 1, 2 do
--         attr_type = upgrade["attr_type_" .. i]
--         value = upgrade["attr_num_" .. i]
--         if value > 0 then
--             if show_next then
--                 UH.SetText(
--                     self.Attrs[i],
--                     string.format(
--                         Language.Fabao.AttrValue3,
--                         Language.Common.AttrList[attr_type],
--                         string.format(
--                             Language.Fabao.AttrValue,
--                             DataHelper.Percent(attr_type, value),
--                             DataHelper.Percent(attr_type, next_upgrade["attr_num_" .. i])
--                         )
--                     )
--                 )
--             else
--                 UH.SetText(
--                     self.Attrs[i],
--                     string.format(
--                         Language.Fabao.AttrValue3,
--                         Language.Common.AttrList[attr_type],
--                         DataHelper.Percent(attr_type, value)
--                     )
--                 )
--             end
--         else
--             UH.SetText(self.Attrs[i], "")
--         end
--     end
-- end
