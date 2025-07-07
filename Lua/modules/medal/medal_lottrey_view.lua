MedalLottreyView = MedalLottreyView or DeclareView("MedalLottreyView", "medal/medal_lottrey_show")
VIEW_DECLARE_MASK(MedalLottreyView,ViewMask.BgBlockClose)
function MedalLottreyView:MedalLottreyView()
    -- body
end

function MedalLottreyView:LoadCallback(param_t)
    self.data = MedalData.Instance
    self.Board:SetData(self:GetType(), Language.Medal.BuildSpResultTitle or "", Vector2.New(867, 493))

    self:FlushListData()
end 


function MedalLottreyView:FlushListData( )
    local data_list = self.data:GetMedalLottreyInfo()

    self.ShowList:SetData(data_list)
end

function MedalLottreyView:OnClickShow()
    -- body
end


MedalLottreyShowCell = DeclareMono("MedalLottreyShowCell", UIWidgetBaseItem)
function MedalLottreyShowCell:MedalLottreyShowCell()
    -- body
end

function MedalLottreyShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item_ = Item.Create(data)

    -- local color = self.isDark and QualityColorStr or QualityColorStrShallow


    self.Cell:SetData(item_)
    self.Level.text = item_:LevelDesc()
    -- LogError()

    local name_color = data:Color()
    if data:WithSp() then 
        self.Desc.text = string.format(Language.Common.Xstr, QualityColorStr[data:Color()], data:SpEffectCfg().description)
    else
        self.Desc.text = Language.Medal.SpecialLotterySolo..Language.ItemInfo.None
    end

    self.Name.text = string.format( Language.Medal.MedalNameColor,QualityColorStr[name_color],item_:Name() )  

    for i = 1,4 do self.AttrObjList[i]:SetActive(false) end 
    for k, v in ipairs(item_:BaseAttr()) do
        if v.attr_value ~= 0 then
            local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, 
            Language.Common.AttrList[v.attr_type], 
            DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value)

            self.AttrObjList[k]:SetActive(true)
            UH.SetText(self.AttrList[k], str)
        else 
            self.AttrObjList[k]:SetActive(false)
        end 
    end 

end

function MedalLottreyShowCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end
