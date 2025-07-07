TreasureHuntRateView = TreasureHuntRateView or DeclareView("TreasureHuntRateView","treasure_hunt/treasure_hunt_rate")
-- Board = UIWSubstitute(UIWBoard3)
-- RateInfoList = UIWidgetList
function TreasureHuntRateView:TreasureHuntRateView()
    self.data = TreasureHuntData.Instance
end

function TreasureHuntRateView:LoadCallback(param_t)
    param_t = param_t or {}
    self.Board:SetData(self:GetType(), Language.TreasureHunt.RateTitle, Vector2.New(907, 556))

    if param_t.data_list then
        self.RateInfoList:SetData(param_t.data_list)
        UH.SetText(self.FewTimesMoreText, param_t.show_text or "")
    else
        if self.data:GetCfgLeftTimes() then
            local cfg = TreasureHuntData.Instance:GetCurSelectCfg()
            if cfg.treasure_id ~= 5 then
                if cfg.is_limit == 1 then
                    UH.SetText(self.FewTimesMoreText, Format(Language.TreasureHunt.FewTimesMoreText[3], self.data:GetCfgLeftTimes()))
                else
                    if cfg.treasure_type == HUNT_TYPE.XUN_QI then
                        local xunqi_type = self.data:XunQiType()
                        UH.SetText(self.FewTimesMoreText, Format(Language.TreasureHunt.FewTimesMoreTextXunQi[xunqi_type], self.data:GetCfgLeftTimes()))
                    else
                        UH.SetText(self.FewTimesMoreText, Format(Language.TreasureHunt.FewTimesMoreText[cfg.treasure_type], self.data:GetCfgLeftTimes()))
                    end
                end
            else
                UH.SetText(self.FewTimesMoreText, Format(Language.TreasureHunt.FewTimesMoreTextPartner, self.data:GetCfgLeftTimes()))
            end
        else
            UH.SetText(self.FewTimesMoreText,"")
        end
        local data_list = self.data:GetShowRateList()
        self.RateInfoList:SetData(data_list)
    end
end

-----------------------------------
TreasureHuntRateItem = TreasureHuntRateItem or DeclareMono("TreasureHuntRateItem", UIWidgetBaseItem)
-- ItemList = UIWidgetList
-- QuaImage = UIImageSpriteSet
-- RateText = Text
function TreasureHuntRateItem:TreasureHuntRateItem()
end

function TreasureHuntRateItem:SetData(data)
    self.data = data
    UH.SpriteName(self.QuaImage, "_LocHuntRateQua"..tostring(data.cfg.name_id))
    UH.SetText(self.RateText, Language.TreasureHunt.CallRate..tostring(data.cfg.rate / 100).."%")
    self.ItemList:SetData(self.data.item_list)
    self.ScrollRect.enabled = #self.data.item_list > 4
end