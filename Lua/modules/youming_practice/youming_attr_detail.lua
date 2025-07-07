YouMingAttrDetail = YouMingAttrDetail or DeclareView("YouMingAttrDetail", "youming_practice/youming_attr_detail")
VIEW_DECLARE_MASK(YouMingAttrDetail, ViewMask.BgBlockClose)
function YouMingAttrDetail:YouMingAttrDetail()
    self.data = YouMingData.Instance
end
function YouMingAttrDetail:LoadCallback()
    self.youming = self.data:GetSelectYouMing()
    self.List:SetData(self.youming:DetailAttrPoint())
end

YouMingAttrDetailItem = YouMingAttrDetailItem or DeclareMono("YouMingAttrDetailItem", UIWidgetBaseItem)
function YouMingAttrDetailItem:YouMingAttrDetailItem()

end
function YouMingAttrDetailItem:SetData(data)
    UH.SetText(self.Desc, Language.Common.AttrList[data.attr_type] .. ":")
    UH.SetText(self.Value, DataHelper.Percent(data.attr_type, data.attr_value))
end

YouMingPingJi = YouMingPingJi or DeclareView("YouMingPingJi", "youming_remodel/youming_rating_rules")
VIEW_DECLARE_MASK(YouMingPingJi, ViewMask.BgBlockClose)

YouMingRemodelRules = YouMingRemodelRules or DeclareView("YouMingRemodelRules", "youming_remodel/youming_remodel_rules")
function YouMingRemodelRules:YouMingRemodelRules()
    self.lang = Language.YouMingPractice
    self.data = YouMingData.Instance
end
function YouMingRemodelRules:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.GaiZaoGaiLv, Vector2.New(1037, 618))
    self.TabList:SetData({{
        index = 1,
        name = self.lang.TabNames3[1],
        tab_objs = {self.Tab1}
    }, {
        index = 2,
        name = self.lang.TabNames3[2],
        tab_objs = {self.Tab2}
    }})
    self.TabList:ClickItem(1)

    -- 显示改造书 成功升档的概率
    local list = self.data:ChangeItemList()
    local item_id
    local config
    local rate
    for i, v in ipairs(list) do
        for i2 = 1, 5 do
            item_id = tonumber(v)
            config = Cfg.YouMingGaiZao(i2-1, item_id,false)
            if config then
                rate = self.data:ChangeAttrRate(config.change_get_talent_0)
            else
                rate = 0
            end
            UH.SetText(self["GaiZao"..i][i2], Format("%.2f%%", rate/100))
        end
        UH.SetText(self.ItemNames[i], Item.GetName(tonumber(v)))
        UH.SetText(self.ItemNames2[i], Item.GetName(tonumber(v)))
        UH.SetText(self.GaiZaoNames[i], Language.YouMingPractice.RemodelTab[i])
    end
    local index_list = {}
    for i = 1, 5 do
        index_list[i] = {index = i, name = Language.YouMingPractice.RemodelTab[i]}
    end
    self.TabList2:SetData(index_list)

    for i = 1, 6 do
        UH.SetText(self.SkillNames[i], Format(Language.YouMingPractice.SkillRate, i - 1))
    end
end
function YouMingRemodelRules:OnClickLeftTab(data)
    if data.index == 2 and self.firs_click == nil then
        self.TabList2:ClickItem(1)
    end
end
function YouMingRemodelRules:OnClickTab(data)
    local list = self.data:ChangeItemList()
    local item_id
    local config
    local rate
    for i, v in ipairs(list) do
        item_id = tonumber(v)
        config = Cfg.YouMingGaiZao(data.index - 1, item_id,false)
        for i2 = 1, 6 do
            if config then
                rate = self.data:ChangeSkillRate(config.change_get_skill_hole)
            else
                rate = nil
            end
            UH.SetText(self["SkillRate"..i][i2], Format("%.2f%%", (rate and rate[i2] or 0)/100))
        end
    end
end
