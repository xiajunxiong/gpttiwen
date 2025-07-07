WorkshopNevealAtlasView  = WorkshopNevealAtlasView or DeclareView("WorkshopNevealAtlasView", "workshop/workshop_neveal_atlas")
function WorkshopNevealAtlasView:WorkshopNevealAtlasView()

end

function WorkshopNevealAtlasView:LoadCallback()
    self.Board:SetData(self:GetType(),Language.Workshop.Neveal.AtlasView , Vector2.New(1083,562))

    self:FlushTabData()
end 

function WorkshopNevealAtlasView:OnCloseClick()
    ViewMgr:CloseView(WorkshopNevealAtlasView)
end

function WorkshopNevealAtlasView:FlushTabData()
    self:FlushShowData()
end

function WorkshopNevealAtlasView:OnClickShow(data)
    AudioMgr:PlayEffect(AudioType.UI,  CommonAudio.ClickGeneral2)
    local list = WorkshopData.Instance:GetNevealAtlasLevelList(data)

    self.levelList:SetData(list)
end

function WorkshopNevealAtlasView:FlushShowData(Param)
    local data_list = WorkshopData.Instance:GetNevealAtlasList()

    self.EffectList:SetData(data_list)
    self.EffectList:ClickItem(1)
end


WorkShopNAEffectCell = DeclareMono("WorkShopNAEffectCell", UIWidgetBaseItem)
function WorkShopNAEffectCell:WorkShopNAEffectCell()
    
end

function WorkShopNAEffectCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    end 
end

function WorkShopNAEffectCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetIcon(self.Icon,data.skill_icon)
    -- UH.SpriteName(self.Qua, MedalConfig.item_qua_sprite[data.qua])
    self.Name.text = data.name

    self.Inter.Interactable = true -- self.data.within > 0
    -- self.Special:SetActive(self.data.is_special == 1)
end

WorkShopNALevelCell = DeclareMono("WorkShopNALevelCell",UIWidgetBaseItem)
function WorkShopNALevelCell:WorkShopNALevelCell()
    -- body
end

function WorkShopNALevelCell:SetData(data)
    self.inter.Interactable = true -- data.pick_up
    UH.SetText(self.Desc,data.Desc)
    UH.SetText(self.Level,data.level)
end 
