--------------MarbasNeoComplete--------------
MarbasNeoComplete = MarbasNeoComplete or DeclareView("MarbasNeoComplete", "marbas/marbas_neo_complete")
VIEW_DECLARE_MASK(MarbasNeoComplete, ViewMask.BgBlockClose)
function MarbasNeoComplete:MarbasNeoComplete()
    self.data = MarbasData.Instance
end

function MarbasNeoComplete:LoadCallback(param) 
    local sp_bg = param.type == MarbasConfig.NeoType.Build and "mb_HuoDeWuPinDi" or "mb_HuoDeWuPinDi1"
    local sp_attr_bg = param.type == MarbasConfig.NeoType.Build and "mb_ShuXingDi" or "mb_ShuXingDi1"
    local sp_title = param.type == MarbasConfig.NeoType.Build and "_Loc_ChengGongHuoDe" or "_Loc_RuMoChengGong"

    UH.SpriteName(self.bg_sp,sp_bg)
    UH.SpriteName(self.attr_bg_sp,sp_attr_bg)
    UH.SpriteName(self.title_sp,sp_title)

    if self.data.demons_view_data.demons_mark == 1 then 
        self.data.demons_view_data.demons_mark = 0
    end 

    self["Eff"..MarbasConfig.NeoType.Build]:SetActive(MarbasConfig.NeoType.Build == param.type)
    self["Eff"..MarbasConfig.NeoType.Demons]:SetActive(MarbasConfig.NeoType.Demons == param.type)
end

function MarbasNeoComplete:OnFlush()

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ComposeSucc)
    local info = self.data:ComposeResult()
    local item = Item.Create(info.item)
    self.Cell:SetData(item)
    UH.SetText(self.Desc, item:QuaName(true))
    UH.SetText(self.LimitedDesc, item:LimitProfDesc())
    UH.SetText(self.EquipLevel, Language.Marbas.ItemInfo.LevelShow..item:LevelShow())

    local base_list = {}
    for k, v in ipairs(item:BaseAttr()) do
        if v.attr_value ~= 0 then
            local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, 
                Language.Common.AttrList[v.attr_type], 
                DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value)
            table.insert( base_list, {str,v.attr_type} ) 
        end
    end
    if #base_list % 2 > 0 then 
        local str = " "
        table.insert( base_list, str ) 
    end 
    
    self.BaseList:SetData(base_list)
    local list = self.data:GetItemInfoMarbasSuitShow(item)
    self.SuitList:SetData(list)
    self.HideSuit:SetActive(#list == 0)
    self.ShowSuit:SetActive(#list > 0 )

    -- local creater_list = {}
    -- local creater_str = string.format(Language.ItemInfo.ItemTitles.CreateName,item.creater_name)
    -- table.insert( creater_list, {creater_str} ) 
    -- self.CreaterList:SetData(creater_list)
    local item_desc_list = {}
    local desc_str = Item.GetDesc(item.item_id)
    table.insert( item_desc_list, {desc_str} ) 

    self.ItemDescList:SetData(item_desc_list)
end