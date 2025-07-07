MedalEffectShow  = MedalEffectShow or DeclareView("MedalEffectShow", "medal/medal_effect_show")

MedalEffectShow.Pos = {
    with_btn = {
        board_size = Vector2.New(1056,585),
        board_pos = Vector2.New(28,-21),
        total_pos = Vector2.New(0,61),
    },
    without = {
        board_size = Vector2.New(1056,505),
        board_pos = Vector2.New(28,-38),
        total_pos = Vector2.New(0,0),
    },
}

function MedalEffectShow:MedalEffectShow()
    self.AudioClose = CommonAudio.ItemInfoClose
end

function MedalEffectShow:LoadCallback(param_t)
    -- LogError("param_t",param_t)
    self.data = param_t -- PublicInfoShowCtrl.Instance.page_show_data
    local is_with = self.data.is_with or false
    local oper_cfg = is_with and MedalEffectShow.Pos.with_btn or MedalEffectShow.Pos.without
    self.Board:SetData(self:GetType(), self.data.title_name or "", oper_cfg.board_size)
    self:FlushTabData()
    
    self.Board.gameObject:SetLocalPosition(oper_cfg.board_pos)
    self.total:SetLocalPosition(oper_cfg.total_pos)
    UH.SetText(self.withDesc,Language.Medal.MedalEffectShowTips)
    self.with_obj:SetActive(is_with)

    self.cache_list = {}
end

function MedalEffectShow:TabDataChange()
    local data = {}
    
    for k,v in pairs(self.data.tab_list) do 
        for i,j in pairs(self.data.show_list) do 
            if v.tab_index == j.tab_index then 
                table.insert( data, v )
                break
            end 
        end 
    end 

    return data
end

function MedalEffectShow:CloseCallback()
    self.cache_list = nil
end

function MedalEffectShow:FlushTabData()

    self:OnClickTab(self.data)
    self.Empty.text = self.data.empty_lab 
end

function MedalEffectShow:FlushShowData(Param)
    local data_list = MedalData.Instance:CatchChangeShowList(Param.show_list, Param.ignore_gray)
    self.EffectList:SetData(data_list)
    self.EffectList:ClickItem(1)
end

function MedalEffectShow:OnClickTab(data)
    self:FlushShowData(data)
end

function MedalEffectShow:OnClickShow(data)
    AudioMgr:PlayEffect(AudioType.UI,  CommonAudio.ClickGeneral2)
    local list = MedalData.Instance:GetEffectShowList(data, self.data.ignore_gray)
    self.levelList:SetData(list)
end

function MedalEffectShow:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[136].desc})
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2) 
end


function MedalEffectShow:OnClickGo()
    if FunOpen.Instance:GetFunIsOpened(Mod.Medal.Build) then
        ViewMgr:CloseView(MedalEffectShow)
    end 

    ViewMgr:OpenViewByKey(Mod.Medal.Build)
end

InfoPageShowTabCell = DeclareMono("InfoPageShowTabCell", UIWidgetBaseItem)
function InfoPageShowTabCell:InfoPageShowTabCell()
    -- body
end

function InfoPageShowTabCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.Name.text = data.name
end

function InfoPageShowTabCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end 
end

EffectInfoShowCell = DeclareMono("EffectInfoShowCell", UIWidgetBaseItem)
function EffectInfoShowCell:InfoPageShowCell()
    -- body
end

function EffectInfoShowCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    end 
end


function EffectInfoShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetIcon(self.Icon,data.icon_id,data.icon_type)
    UH.SpriteName(self.Qua, MedalConfig.item_qua_sprite[data.qua])
    self.Name.text = data.reward_name
    self.Inter.Interactable = self.data.within > 0
    self.Special:SetActive(self.data.is_special == 1)
end

EffectlLevelShowCell = DeclareMono("EffectlLevelShowCell",UIWidgetBaseItem)
function EffectlLevelShowCell:EffectlLevelShowCell()
    -- body
end

function EffectlLevelShowCell:SetData(data)
    self.inter.Interactable = data.pick_up
    UH.SetText(self.Desc,data.Desc)
    UH.SetText(self.Level,data.level)
end 
