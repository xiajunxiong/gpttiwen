--符玉升华
PartnerFuYuSublimation = PartnerFuYuSublimation or DeclareView("PartnerFuYuSublimation", "partner/partner_fuyu_sublimation")
VIEW_DECLARE_MASK(PartnerFuYuSublimation,ViewMask.BgBlockClose)
function PartnerFuYuSublimation:PartnerFuYuSublimation() 
    self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
end 
function PartnerFuYuSublimation:LoadCallback(param_t)
  --[[   if param_t.item ~= nil then 
        MedalData.Instance:SetSelectTargetMix(param_t.item,2)
        self.panel:FlushShow()
    end    ]]
end

function PartnerFuYuSublimation:OnClickClose()
    ViewMgr:CloseView(PartnerFuYuSublimation)
end

PartnerFuYuSubPanel = PartnerFuYuSubPanel or DeclareMono("PartnerFuYuSubPanel", UIWFlushPanel)
function PartnerFuYuSubPanel:PartnerFuYuSubPanel()
    self.data = PartnerData.Instance
    self.data_cares = {
        {data = self.data:GetSubFuYu(),func = self.FlushTarget},
        {data = self.data:GetSubFuYuTarget(),func = self.FlushTarget},
        {data = self.data:GetSubFuYuTarget(),func = self.FlushConsume},
    }

    self.select_data = nil
    self.select_mark = -1
    self.is_clear = false

    self.anim_mark = 0
end

function PartnerFuYuSubPanel:FlushTarget()

    if self.anim_mark > 0 then 
        self.CenterAnim:SetInteger(APH("status"),2)
    else 
        self.CenterAnim:SetInteger(APH("status"),0)
    end 
    local item = self.data:GetSubFuYu()
    self.ResultCell:SetData(item)

    local result = self.data:GetSubFuYuResult()
    local is_complete = item:IsGod()

    self.MainCell:SetObjActive(not is_complete)
    self.MixCell:SetObjActive(not is_complete)
    local extra_id = item:ExtraId()
    UH.SetText(self.TitleName, extra_id > 0 and Language.FuYu.FuYuSub2 or Language.FuYu.FuYuSub1)

    self.EffectBlue:SetActive(item:ExtraLevel() == 0)
    self.EffectOrange:SetActive(item:IsChaos())
    self.EffectRed:SetActive(is_complete)

    self.BtnUpgrade:SetActive(not is_complete)
    if self.Block.activeSelf then 
        self.Block:SetActive(false)
    end
    local attr_list = item:BaseAttr()
	local attr
	for i = 1, self.AttrList:Length() do
		attr = attr_list[i]
		if attr and attr.attr_value > 0 then
			UH.SetText(self.AttrList[i], Format("%s %s", Language.AttrItems.AttrNameSample[attr.attr_type], attr.attr_value) )
			self.AttrList[i].gameObject:SetActive(true)
		else
			self.AttrList[i].gameObject:SetActive(false)
		end
	end

    UH.SetText(self.SpName1, item:SubSpName(item:SpName()))
    UH.SetText(self.SpName2, item:SubExtraName(item:ExtraName()))
    local config = BagData.Instance:GetEquipEffectCfg(item:SpId())
    UH.SetText(self.SpDesc1, item:SubSpName(config.reward_description))
    config = BagData.Instance:GetEquipEffectCfg(item:ExtraId())
    self.SpObj2:SetActive(config ~= nil)
    if config then
        UH.SetText(self.SpDesc2, item:SubExtraName(config.reward_description))
    end
end
function PartnerFuYuSubPanel:FlushConsume()
    local consume = self.data:GetSubFuYuTarget()
    local item1 = consume.item1
    local item2 = consume.item2
    self.MainCell:SetData(item1)
    self.MixCell:SetData(item2)
end
function PartnerFuYuSubPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:ClearFuYuSub()
end

function PartnerFuYuSubPanel:OnClickComplete()
    local item = self.data:GetSubFuYu()
    if item:IsGod() then 
        return
    end 
    self.CenterAnim:SetInteger(APH("status"),0)
    self.anim_mark = 0
end

function PartnerFuYuSubPanel:SelectFlush(data)
    self.select_data = data
end

function PartnerFuYuSubPanel:OnClickArea(type)
    self:CutEffect()
    self.select_data = nil
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    self.select_mark = type
    UH.SetText(self.SelectTitle, self.select_mark == 0 and Language.FuYu.FuYuMixSelectSp or Language.FuYu.FuYuMixSelectPlus)
    self.SelectPart:SetActive(true)
    local item = self.data:GetSubFuYu()
    if self.select_mark == 0 then
        self.SelectList:SetData(self.data:GetSameNameFuYuItems(item))
    elseif self.select_mark == 1 then 
        self.SelectList:SetData(self.data:GetSubFuYuItems(item))
    end 
end

function PartnerFuYuSubPanel:OnClickPanelClose()
    self:ShowEffect()
    self.Toggle:SetAllTogglesOff()
    self.SelectPart:SetActive(false)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

function PartnerFuYuSubPanel:OnClickSelectSure()
    if self.select_mark == 0 then        
        self.data:SetSubFuYuTarget1(self.select_data)
    elseif self.select_mark == 1 then
        self.data:SetSubFuYuTarget2(self.select_data)
    end
    self:ShowEffect()
    self.Toggle:SetAllTogglesOff()
    self.SelectPart:SetActive(false)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

function PartnerFuYuSubPanel:OnClickConfirm()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    local consume = self.data:GetSubFuYuTarget()
    if consume.item1 == nil then 
        PublicPopupCtrl.Instance:Center(Language.FuYu.MedalMixChangeLackSelectMedal)
        return 
    end 

    if consume.item2 == nil then
        PublicPopupCtrl.Instance:Center(Language.FuYu.MedalMixChangeLackPlusMedal)
        return 
    end

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
            self.CenterAnim:SetInteger(APH("status"),1)
        end, 3)
        self.Block:SetActive(true)
		ViewMgr:CloseView(DialogTipsView)
    end
    local item = self.data:GetSubFuYu()
    local result = self.data:GetSubFuYuResult()
    local info = {
		content = string.format(Language.FuYu.MixMedalSpecialExchange, 
            item:QuaName(), result:QuaName(nil, item:ExtraId() > 0 and 15 or 10)),
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

function PartnerFuYuSubPanel:EffectComplete()
    local item = self.data:GetSubFuYu()
    local consume = self.data:GetSubFuYuTarget()
    local item1 = consume.item1
    local item2 = consume.item2
    if item.in_bag then
        PartnerCtrl.Instance:ReqFuYuSublime2(item:GridColumn(), item:GridIndex(), item1:GridColumn(), item1:GridIndex(), item2:GridColumn(), item2:GridIndex())
    else
        PartnerCtrl.Instance:ReqFuYuSublime(item.partner_id, item.wear_idx, item1:GridColumn(), item1:GridIndex(), item2:GridColumn(), item2:GridIndex())
    end
    self.data:SetSubFuYuTarget1(nil)
    self.data:SetSubFuYuTarget2(nil)
    -- self.is_clear = true
end

function PartnerFuYuSubPanel:CutEffect()
    self.EffectBlue:SetActive(false)
    self.EffectOrange:SetActive(false)
    self.EffectRed:SetActive(false)
end

function PartnerFuYuSubPanel:ShowEffect()
    local item = self.data:GetSubFuYu()
    local is_complete = item:IsGod()
    self.EffectBlue:SetActive(item:ExtraLevel() == 0)
    self.EffectOrange:SetActive(item:IsChaos())
    self.EffectRed:SetActive(is_complete)
end

FuYuSubConsumeCell = DeclareMono("FuYuSubConsumeCell", UIWidgetBaseItem)
function FuYuSubConsumeCell:FuYuSubConsumeCell()
    self.partner_data = PartnerData.Instance
end

function FuYuSubConsumeCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.Operate < 3 then
        self.Cell:SetData(data)
        self.ItemSelect:SetActive(data == nil)
    else
        local cfg = Cfg.ParnterFuYuByLevel2(data:FuYuLevel())
        if cfg then
            UH.SpriteName(self.IconSet, tostring(cfg.rank_up_icon))
        end
        local sp_id = data:SpId()
        local config = Cfg.ParnterFuYuEffect(sp_id, false)
        if config and config.limit_type == 1 then
            self.IconTop:SetObjActive(true)
            UH.SpriteName(self.IconTop, PartnerConfig.FuYuProfSp[config.param1])
        else
            self.IconTop:SetObjActive(false)
        end
    end 

    UH.SetText(self.Name,"")
    if data then 
        UH.SetText(self.Name, data:FuYuTypeName()..data:QuaName())
    elseif self.Operate == 1 then 
        UH.SetText(self.Name, Language.FuYu.MedalMixMainFormat)
    elseif self.Operate == 2 then 
        local item = self.partner_data:GetSubFuYu()
        if not item:IsChaos() then 
            UH.SetText(self.Name, Language.FuYu.MedalMixNoPlusFormat)
        else 
            UH.SetText(self.Name, Language.FuYu.MedalMixPlusFormat)
        end 
    end 
end
