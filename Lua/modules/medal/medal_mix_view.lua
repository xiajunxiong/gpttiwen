MedalMixView = MedalMixView or DeclareView("MedalMixView", "medal/medal_mix")
VIEW_DECLARE_MASK(MedalMixView,ViewMask.BgBlockClose)
function MedalMixView:MedalMixView() 
    self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
end 
function MedalMixView:LoadCallback(param_t)
    if param_t.item ~= nil then 
        MedalData.Instance:SetSelectTargetMix(param_t.item,2)
        self.panel:FlushShow()
    end   
end

function MedalMixView:OnClickClose()
    ViewMgr:CloseView(MedalMixView)
end

MedalMixPanel = MedalMixPanel or DeclareMono("MedalMixPanel", UIWFlushPanel)
function MedalMixPanel:MedalMixPanel()
    self.data = MedalData.Instance
    self.data_cares = {
        {data = self.data.mix_change,func = self.FlushShow,init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushShow,init_call = false},
    }

    self.select_data = nil
    self.select_mark = -1
    self.is_clear = false

    self.anim_mark = 0
end

function MedalMixPanel:OnDestroy()
    self.data:ClearMixChangeData()
    UIWFlushPanel.OnDestroy(self)
end

function MedalMixPanel:FlushShow()
    if self.is_clear then 
        self.is_clear = false
        return 
    end 
    self.main_cell:SetData(self.data.mix_change.select_data or {})
    self.mix_cell:SetData(self.data.mix_change.plus_data or {})

    self.data:UpDataMixMainData()
    local result_data = self.data:GetMixChangeResult()
    self.result_cell:SetData(result_data)

    local is_complete = self.data.mix_change.main_data.param.special_medal_grade == 2

    -- self.complete:SetActive(is_complete)
    self.main_cell.gameObject:SetActive(not is_complete)
    self.mix_cell.gameObject:SetActive(not is_complete)

    local is_plus = self.data.mix_change.main_data.param.second_sp_id > 0 
    UH.SpriteName(self.prog, is_plus and "_Loc_Er" or "_Loc_Yi")

    self.EffectBlue:SetActive(not is_plus and not is_complete)
    self.EffectOrange:SetActive(is_plus and not is_complete)
    self.EffectRed:SetActive(is_complete)

    if self.block.activeSelf then 
        self.block:SetActive(false)
    end 

    self.btn_upgrade:SetActive(not is_complete)

    if self.anim_mark > 0 then 
        self.center_anim:SetInteger(APH("status"),2)
    else 
        self.center_anim:SetInteger(APH("status"),0)
    end 

    for i = 1,4 do 
        local attr = result_data:BaseAttr()[i]
        self.neo_attr_list[i].gameObject:SetActive(attr ~= nil)
        if attr ~= nil then 
            UH.SetText(self.neo_attr_list[i],Format(Language.ItemInfo.ItemTitles.EquipAttr2, 
            Language.Common.AttrList[attr.attr_type], 
            DataHelper.IsPercent(attr.attr_type) and Percent(attr.attr_value) or attr.attr_value))
        end 
    end 

    for k,v in pairs(result_data:BaseAttr()) do
         
        self.neo_attr_list[k].gameObject:SetActive(v.attr_value ~= 0)

        UH.SetText(self.neo_attr_list[k],Format(Language.ItemInfo.ItemTitles.EquipAttr2, 
        Language.Common.AttrList[v.attr_type], 
        DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value))
    end 

    self.neo_sp_show_1:SetActive(result_data.param.sp_id > 0)
    if result_data.param.sp_id > 0 then
        local cfg = result_data:SpEffectCfg()
        UH.SetText(self.neo_sp_nanme_1,string.format(Language.Medal.MedalNameColor,result_data:ColorStr(),cfg.name))
        UH.SetText(self.neo_sp_desc_1,string.format(Language.Medal.MedalNameColor,result_data:ColorStr(),cfg.description))
    end 
    self.neo_sp_show_2:SetActive(result_data.param.second_sp_id > 0)
    if result_data.param.second_sp_id > 0 then
        local cfg = result_data:SpEffectSecondCfg()
        UH.SetText(self.neo_sp_nanme_2,string.format(Language.Medal.MedalNameColor,result_data:ColorSecondStr(),cfg.name))
        UH.SetText(self.neo_sp_desc_2,string.format(Language.Medal.MedalNameColor,result_data:ColorSecondStr(),cfg.description))
    end 
end

function MedalMixPanel:OnClickComplete()
    local is_complete = self.data.mix_change.main_data.param.special_medal_grade == 2
    if is_complete then 
        return 
    end 
    self.center_anim:SetInteger(APH("status"),0)
    self.anim_mark = 0
end

function MedalMixPanel:SelectFlush(data)
    self.select_data = data
end

function MedalMixPanel:OnClickArea(type)
    -- if type == 1 and self.data.mix_change.select_data == nil then 
    --     PublicPopupCtrl.Instance:Center(Language.Medal.MedalMixChangeSelectFirst)
    --     return 
    -- end 
    self:CutEffect()
    self.select_data = nil
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)

    self.select_mark = type

    self.select_part:SetActive(true)
    if self.select_mark == 0 then 
        self.select_list:SetData(self.data:GetMedalMixBagList())
    elseif self.select_mark == 1 then 
        self.select_list:SetData(self.data:GetMedalMixPlusList())
    end 

    UH.SetText(self.select_title, self.select_mark == 0 and Language.Medal.MedalMixSelectSp or Language.Medal.MedalMixSelectPlus)
end

function MedalMixPanel:OnClickPanelClose()
    self:ShowEffect()
    self.toggle:SetAllTogglesOff()
    self.select_part:SetActive(false)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

function MedalMixPanel:OnClickSelectSure()
    self.data:SetSelectTargetMix(self.select_data,self.select_mark)
    -- if self.select_mark == 0 then 
    --     self.data.mix_change.plus_data = nil 
    -- elseif self.select_mark == 1 then 
    --     self.data.mix_change.select_data = nil 
    -- end 
    self:ShowEffect()
    self.toggle:SetAllTogglesOff()
    self.select_part:SetActive(false)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

function MedalMixPanel:OnClickConfirm()
    if self.data.mix_change.select_data == nil then 
        PublicPopupCtrl.Instance:Center(Language.Medal.MedalMixChangeLackSelectMedal)
        return 
    end 

    if self.data.mix_change.plus_data == nil then
        PublicPopupCtrl.Instance:Center(Language.Medal.MedalMixChangeLackPlusMedal)
        return 
    end 
    
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)

    local function send_msg()
        --粒子移动特效
        self.EffectTool:Play(4165053)
        self.EffectTool:Play(4165054)

        if self.EffectBlue.activeSelf then 
            self.EffectTool:Play(4165051)
        elseif self.EffectOrange.activeSelf then 
            self.EffectTool:Play(4165052)
        end 

        AudioMgr:PlayEffect(AudioType.UI, "medal_sp_mix")
        self.Timer = TimeHelper:AddDelayTimer(function()
            self.anim_mark = 1
            self.center_anim:SetInteger(APH("status"),1)
        end, 3)
        self.block:SetActive(true)
		ViewMgr:CloseView(DialogTipsView)
    end
    
    local complete_cell = self.data:GetMixChangeResult()
    local info = {
		content = string.format(Language.Medal.MixMedalSpecialExchange, 
            self.data.mix_change.main_data:QuaName(),complete_cell:QuaName()),
		cancel = {
			name = nil,
			func = nil
		},
		confirm = {
			name = nil,
			func = send_msg
		}
	}
	PublicPopupCtrl.Instance:DialogTips(info)

end

function MedalMixPanel:EffectComplete()
    local select_data = self.data.mix_change.select_data
    local plus_data = self.data.mix_change.plus_data
    local main_data =  self.data.mix_change.main_data

    local param_t = { 
        is_wear_grid = main_data.in_bag and 0 or 1,
        column = main_data.in_bag and main_data:GridColumn() or 0, 
        index = main_data.in_bag and main_data:GridIndex() or main_data.vo.grid_index , 
        consume_column_spe = select_data:GridColumn(),
        consume_index_spe = select_data:GridIndex(),
        consume_column = plus_data:GridColumn(),
        consume_index = plus_data:GridIndex(),
    }

    -- self.block:SetActive(false)
    BagCtrl.Instance:SendMedalUpGradeSpecialReq(param_t)

    -- self.data:ClearMixChangeData()
    self.data:ClearCacheMixChangeData()
    self.is_clear = true
end

function MedalMixPanel:CutEffect()
    self.EffectBlue:SetActive(false)
    self.EffectOrange:SetActive(false)
    self.EffectRed:SetActive(false)
end

function MedalMixPanel:ShowEffect()
    local is_complete = self.data.mix_change.main_data.param.special_medal_grade == 2
    local is_plus = self.data.mix_change.main_data.param.second_sp_id > 0 

    self.EffectBlue:SetActive(not is_plus and not is_complete)
    self.EffectOrange:SetActive(is_plus and not is_complete)
    self.EffectRed:SetActive(is_complete)
end

MedalMixOperateCell = DeclareMono("MedalMixOperateCell", UIWidgetBaseItem)
function MedalMixOperateCell:MedalMixOperateCell() end

function MedalMixOperateCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    if self.operate < 3 then 
        self.item_call:SetData(data)
        self.item_select:SetActive(not (data.item_id and data.item_id > 0))
    else 
        UH.SetIcon(self.medal_icon, data:BigIcon())
    end 

    UH.SetText(self.item_name,"")

    if self.data.item_id and self.data.item_id > 0 then 
        UH.SetText(self.item_name, Language.Medal.MedalMixNameFormatPlus[data.param.special_medal_grade]..data:QuaName())
    elseif self.operate == 1 then 
        UH.SetText(self.item_name, Language.Medal.MedalMixMainFormat)
    elseif self.operate == 2 then 
        if MedalData.Instance.mix_change.main_data and MedalData.Instance.mix_change.main_data.param.second_sp_id  == 0 then 
            UH.SetText(self.item_name, Language.Medal.MedalMixNoPlusFormat)
        else 
            UH.SetText(self.item_name, Language.Medal.MedalMixPlusFormat)
        end 
    end 
end
