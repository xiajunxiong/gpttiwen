MedalBuildRateView = MedalBuildRateView or DeclareView("MedalBuildRateView","medal/medal_build_rate")
-- Board = UIWSubstitute(UIWBoard3)
-- RateInfoList = UIWidgetList
function MedalBuildRateView:MedalBuildRateView()
    self.data = MedalData.Instance
end

function MedalBuildRateView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.TreasureHunt.RateTitle, Vector2.New(907, 556))
    -- if self.data:GetCfgLeftTimes() then
    --     UH.SetText(self.FewTimesMoreText, string.format(Language.TreasureHunt.FewTimesMoreText, self.data:GetCfgLeftTimes()))
    -- else
    --     UH.SetText(self.FewTimesMoreText,"")
    -- end

    self.job_type = (param_t.job_type == 0 or param_t.job_type == nil)and RoleData.Instance:ProfType() or param_t.job_type
    if param_t.job_type == 0 then 
        self.NomalClick:Trigger()
    else 
        self.ProfClick:Trigger()
    end 

    -- self:OnClickColumn(param_t.job_type)
end

function MedalBuildRateView:OnClickColumn(index)
    local type = index == 0 and 0 or self.job_type
    local data_list = self.data:GetRewardRateShow(type)
    self.RateInfoList:SetData(data_list)
    self.RateInfoList:ForceJumpVertical(0)
    self.ListRect.enabled = #data_list > 3
end

-----------------------------------
MedalBuildRateItem = MedalBuildRateItem or DeclareMono("MedalBuildRateItem", UIWidgetBaseItem)
-- ItemList = UIWidgetList
-- QuaImage = UIImageSpriteSet
-- RateText = Text
function MedalBuildRateItem:MedalBuildRateItem()
end

function MedalBuildRateItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.QuaImage, "qua_"..tostring(data.color))
    UH.SetText(self.RateText, Language.Medal.MedalEffectBuildRate..tostring(data.rate_show / 100).."%")
    local items = Split(data.item_id,":")
    local list = {}
    for k,v in pairs(items) do 
        local vo = Item.Create({item_id = tonumber(v)})
        local param = BagData.Instance:CatchMedalParam(vo.item_id)
        if param ~= nil then 
            vo.param.grade = param.grade
        elseif data.color == 1 then  
            vo.param.grade = vo.item_id - 8000 --青肃令
        end 
        table.insert(list,vo)
    end 

    self.ItemListNomal:SetData(list)
    self.list_nomal_rect.enabled = #list > 4
    -- local is_red = data.color == 5
    -- self.ItemListRed.gameObject:SetActive(is_red)
    -- self.ItemListNomal.gameObject:SetActive(not is_red)
    -- local operate = is_red and {"ItemListRed","list_red_rect"} or {"ItemListNomal","list_nomal_rect"}
    -- self[operate[1]]:SetData(list)
    -- self[operate[2]].enabled = #list > 4



    -- self.ItemList:SetData(self.data.item_list)
    -- self.ScrollRect.enabled = #self.data.item_list > 4
end