PartnerFuYuEffectShow  = PartnerFuYuEffectShow or DeclareView("PartnerFuYuEffectShow", "partner/partner_fuyu_effect_show")

PartnerFuYuEffectShow.Pos = {
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

function PartnerFuYuEffectShow:PartnerFuYuEffectShow()
    self.AudioClose = CommonAudio.ItemInfoClose
end

function PartnerFuYuEffectShow:LoadCallback(param_t)
    -- LogError("param_t",param_t)
    self.data = param_t -- PublicInfoShowCtrl.Instance.page_show_data
    local is_with = self.data.is_with or false
    local oper_cfg = is_with and PartnerFuYuEffectShow.Pos.with_btn or PartnerFuYuEffectShow.Pos.without
    self.Board:SetData(self:GetType(), self.data.title_name or "", oper_cfg.board_size)
    self:FlushTabData()
    
    self.Board.gameObject:SetLocalPosition(oper_cfg.board_pos)
    self.total:SetLocalPosition(oper_cfg.total_pos)
    UH.SetText(self.withDesc,Language.Medal.MedalEffectShowTips)
    self.with_obj:SetActive(is_with)

    self.cache_list = {}
end

function PartnerFuYuEffectShow:TabDataChange()
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

function PartnerFuYuEffectShow:CloseCallback()
    self.cache_list = nil
end

function PartnerFuYuEffectShow:FlushTabData()

    self:OnClickTab(self.data)
    self.Empty.text = self.data.empty_lab 
end

function PartnerFuYuEffectShow:FlushShowData(Param)
    local data_list = Param.show_list --MedalData.Instance:CatchChangeShowList(Param.show_list, Param.ignore_gray)
    self.EffectList:SetData(data_list)
    self.EffectList:ClickItem(1)
end

function PartnerFuYuEffectShow:OnClickTab(data)
    self:FlushShowData(data)
end

function PartnerFuYuEffectShow:OnClickShow(data)
    AudioMgr:PlayEffect(AudioType.UI,  CommonAudio.ClickGeneral2)
--    local list = MedalData.Instance:GetEffectShowList(data, self.data.ignore_gray)
    local list = PartnerData.Instance:GetFuYuEffectLevelList(data.sp_type)
    self.levelList:SetData(list)
end

function PartnerFuYuEffectShow:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[136].desc})
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2) 
end


function PartnerFuYuEffectShow:OnClickGo()
    if FunOpen.Instance:GetFunIsOpened(Mod.Medal.Build) then
        ViewMgr:CloseView(PartnerFuYuEffectShow)
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

FuYuEffectInfoShowCell = DeclareMono("FuYuEffectInfoShowCell", UIWidgetBaseItem)
function FuYuEffectInfoShowCell:InfoPageShowCell()
    -- body
end

function FuYuEffectInfoShowCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    end 
end


function FuYuEffectInfoShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetIcon(self.Icon,data.icon_id,data.icon_type)
    UH.SpriteName(self.Qua, MedalConfig.item_qua_sprite[data.qua])
    self.Name.text = data.reward_name
    -- self.Inter.Interactable = self.data.within > 0
    local config = Cfg.ParnterFuYuEffect(data.sp_id)
    if config and config.limit_type == 2 then
        self.Special:SetActive(true)
    else
        self.Special:SetActive(false)
    end
    self.Inter.Interactable = true
    -- self.Special:SetActive(self.data.is_special == 1)
end

FuYuEffectlLevelShowCell = DeclareMono("FuYuEffectlLevelShowCell",UIWidgetBaseItem)
function FuYuEffectlLevelShowCell:FuYuEffectlLevelShowCell()
    -- body
end

function FuYuEffectlLevelShowCell:SetData(data)
    -- self.inter.Interactable = data.pick_up
    self.inter.Interactable= true
    UH.SetText(self.Desc,Format("%s：%s", data.name, data.desc))
    
    UH.SetText(self.Level,data.level)
end 
