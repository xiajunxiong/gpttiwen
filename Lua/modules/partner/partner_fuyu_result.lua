PartnerFuYuResult = PartnerFuYuResult or
                        DeclareView("PartnerFuYuResult", PartnerConfig.ResPath .. "partner_fuyu_result")

VIEW_DECLARE_MASK(PartnerFuYuResult, ViewMask.BlockClose)

PartnerFuYuResultPanel = PartnerFuYuResultPanel or DeclareMono("PartnerFuYuResultPanel", UIWFlushPanel)
function PartnerFuYuResultPanel:PartnerFuYuResultPanel()
    self.data = PartnerData.Instance

end
function PartnerFuYuResultPanel:onFlush()
    local is_chenggong = self.data.qiang_hua_jie_guo
   -- self.ShiBai:SetActive(not is_chenggong)
   -- self.ChengGong:SetActive(is_chenggong)
    self.EffectTool:Play(is_chenggong and 7165074 or 7165073)
    self.Anima:Play(is_chenggong and "show" or "fail")
    local item = self.data:GetQiangHuaItem()
    UH.SetText(self.Name, item:Name2())
    local attrs = item:BaseAttr()
    -- LogDG(attrs)
    for i = 1, self.AttrList:Length() do
        if attrs[i] and attrs[i].attr_value > 0 then
            UH.SetText(self.AttrList[i], Format(Language.FuYu.AttrDesc,
                Language.AttrItems.AttrNameSample[attrs[i].attr_type], attrs[i].attr_value))
        else
            UH.SetText(self.AttrList[i], "")
        end
    end
    local cfg = Cfg.ParnterFuYuByLevel2(item:FuYuLevel())
    if cfg then
        UH.SpriteName(self.IconSet, tostring(cfg.rank_up_icon))
    end
    local sp_id = item:SpId()
    local config = Cfg.ParnterFuYuEffect(sp_id, false)
    if config and config.limit_type == 1 then
        self.IconTop:SetObjActive(true)
        UH.SpriteName(self.IconTop, PartnerConfig.FuYuProfSp[config.param1])
    else
        self.IconTop:SetObjActive(false)
    end
    UH.SetText(self.Desc, item:FuYuEffectDesc())
    self.NewObj:SetActive(self.data.fuyu_effect_show_new == true)
    if item.param.expired_timesstamp > 0 then
        self.Time:StampTime(item.param.expired_timesstamp, TimeType.Type_Time_0, Language.FuYu.TimeTip2)
    end
end