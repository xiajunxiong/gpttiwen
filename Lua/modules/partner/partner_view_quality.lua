PartnerViewQuality = PartnerViewQuality  or DeclareMono("PartnerViewQuality", UIWFlushPanel)

function PartnerViewQuality:PartnerViewQuality()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner

    self.data_cares = {
        {data = self.data.item_flush, func = self.FlushQuaChange, init_call = false},
        {data = BagData.Instance.item_grid,func = self.FlushBagChange, init_call = false},
        {data = PartnerData.Instance.quality_flush,func = self.FlushQuaChange, init_call = false},
        {data = PartnerData.Instance.shut_eff_single,func = self.ShutQuaEff, init_call = false},
    }
    self.init_flag = false
    self.EffectLinkTime = 2
end 

function PartnerViewQuality:OnEnable()
    self.data.cur_sel_tabbar.val = PartnerConfig.PartnerViewTabs.Quality

    if self.init_flag then 
        self:onFlush()
    end 
end

function PartnerViewQuality:Awake()
    UIWFlushPanel.Awake(self)
    self:onFlush()
    self.init_flag = true
end 

function PartnerViewQuality:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.flush_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_item_handle)
        self.flush_item_handle = nil
    end

    if self.data.qua_up_timer ~= nil then 
        TimeHelper:CancelTimer(self.data.qua_up_timer)
        self.data.qua_up_timer = nil
    end 

    if self.Timer1 then
		TimeHelper:CancelTimer(self.Timer1)
		self.Timer1 = nil
	end

    if self.Timer2 then
		TimeHelper:CancelTimer(self.Timer2)
		self.Timer2 = nil
	end
end 

function PartnerViewQuality:onFlush( )
    self:FlushPanel(true) 
end

-- 不需要进行跳跃的刷新
function PartnerViewQuality:FlushBagChange()
    self:FlushPanel(false) 
end

-- 需要进行跳跃的刷新
function PartnerViewQuality:FlushQuaChange()
    self:FlushPanel(true) 
end

function PartnerViewQuality:FlushPanel()
    if -1 == self.data.cur_sel_partner then return end 

    if self.data:PartnerQualityIsCaching() then return end 

    local pos_cur,pos_next,pos_after = self.data:GetPartnerQuaShowById(self.data.cur_sel_partner)
    self.ShowMaxObj:SetActive(pos_next == nil)
    self.ShowCurObj:SetActive(pos_next ~= nil)
    self.ShowNextObj:SetActive(pos_next ~= nil)
    if pos_next == nil then
        self.ShowMax:SetData(pos_cur)
    else 
        self.ShowCur:SetData(pos_cur)
        self.ShowNext:SetData({next = pos_next,after = pos_after})
    end 

    local info,cfg = self.data:GetPartnerQualityInfoById( self.data.cur_sel_partner,true )

    for i = 1, PartnerConfig.PARTNER_ATTR_MAX do 
        for k,v in pairs(cfg.attr_list) do
            if v.attr_type == self.AttrCells[i].AttrType then 
                self.AttrCells[i]:SetData(v)
                break
            end 
        end 
    end 

    local skill_list = self.data:GetPartnerQualitySkillShow(self.data.cur_sel_partner )
    self.SkillList:SetData(skill_list)

    self.max_skill:SetActive(#skill_list == 0)
    self.StuffNeed:SetActive(not cfg.is_max)
    self.StuffMax:SetActive(cfg.is_max)
    self.skill_title:SetActive(not cfg.is_max)
    UH.SetText(self.LblNeed, string.format(self.language.NeedLevelLimit,cfg.o_cfg.level_limit))
    self.QuaIneract.Interactable = RoleData.Instance:GetRoleLevel() >= cfg.o_cfg.level_limit
    self.LblNeedObj:SetActive( RoleData.Instance:GetRoleLevel() < cfg.o_cfg.level_limit)

    local stuff_item = Item.Create({item_id = cfg.o_cfg.need_item})
    self.StuffCell:SetData(stuff_item)
    self.StuffCell:SetDataByItemIdWithNeed(cfg.o_cfg.need_item,cfg.o_cfg.need_num)
    self.CenterCell:SetData(stuff_item)

    -- local str = DataHelper.ConsumeNum(Item.GetNum(cfg.o_cfg.need_item), cfg.o_cfg.need_num, true)
    -- self.StuffCellNeed.text =  str

    local partner_info = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    
    if cfg.is_max == false then 
        self.QualityDesc.text = string.format(self.language.QualityDesc,stuff_item:Name(),partner_info.info.name,self.data:GetPartnerQualityUpDesc(self.data.cur_sel_partner))
        self.QualityGetWay.text = string.format(self.language.QualityGetWay,stuff_item:Name(),Config.getway_cfg_auto.get_way[tonumber(stuff_item:GetWay()[1])].desc)
    end 
    self.stuff_info = {
        is_enough = Item.GetNum(cfg.o_cfg.need_item) >= cfg.o_cfg.need_num,
        _item = stuff_item,
    }
    local is_level = cfg.o_cfg.level_limit <= RoleData.Instance:GetRoleLevel()
    local is_unlock =  self.data:GetPartnerAllInfo()[self.data.cur_sel_partner].vo.is_unlocked == PartnerConfig.LockState.unlock
    
    self.Red_point:SetNum((Item.GetNum(cfg.o_cfg.need_item) >= cfg.o_cfg.need_num and is_unlock and is_level and not partner_info:ParnetQualityUpNeedSoar()) and 1 or 0)

    self.soar_need.gameObject:SetActive(partner_info:ParnetQualityUpNeedSoar())
    UH.SetText(self.soar_need,partner_info.vo.soar_count == 0 and self.language.NeedLowFlyUp or self.language.NeedLowFlyUpHigh)
end

function PartnerViewQuality:OnClickAttr()
    ViewMgr:OpenView(PartnerUpAttrView)
end

function PartnerViewQuality:ShutQuaEff()
    -- self.qua_eff:SetActive(false)
    if self.ShowAnimation == nil then return end
    self.ShowAnimation:SetTrigger("hide")
end

function PartnerViewQuality:OnClickSendSeq()
    if -1 == self.data.cur_sel_partner then return end
    local _info = self.data:GetPartnerInfoById(self.data.cur_sel_partner)

    local partner_info,partner_cfg = self.data:GetPartnerQualityInfoById(self.data.cur_sel_partner,false)
    local lock_state = PartnerConfig.LockState
    if _info.vo.is_unlocked == lock_state.lock then
        PublicPopupCtrl.Instance:Center(Language.Partner.UnLockTips)
        return
    end
    if self.stuff_info == nil then return end 
    
    if not self.stuff_info.is_enough then
        MainOtherCtrl.Instance:GetWayView({item_data =  self.stuff_info._item})
        return
    end
    
    if _info:ParnetQualityUpNeedSoar() then 
        local str = _info.vo.soar_count == 0 and self.language.QualityFlyUp or self.language.QualityFlyUpHigh
        PublicPopupCtrl.Instance:Center(str)
        return 
    end 

    if partner_cfg.o_cfg.level_limit > RoleData.Instance:GetRoleLevel() then
        local cur_name,next_name = self.data:GetPartnerQualityUpDesc(self.data.cur_sel_partner)
            PublicPopupCtrl.Instance:Center( 
                string.format(self.language.LevelQuaLowTips,cur_name,next_name
                ,partner_cfg.o_cfg.level_limit)
            )
        return 
    end 

    -- self:OnQualityEffectEnd()
    self.EffectTool:Play("100108186")
    self.ProtectArea:SetActive(true)
    if self.Timer1 then
		TimeHelper:CancelTimer(self.Timer1)
		self.Timer1 = nil
	end

    self.Timer1 = TimeHelper:AddDelayTimer(function()
        self.ShowPartEffect:Play("100108250")
    end, self.EffectLinkTime)

    self.Timer2 = TimeHelper:AddDelayTimer(function()
        if self.ShowAnimation == nil then return end 
        if partner_info.quality_segment == 4 and partner_info.quality == PartnerConfig.QuaFetter.Gold then 
           self.ShowAnimation:SetTrigger("endshow")
        else 
            self.ShowAnimation:SetTrigger("show")   
        end 
    end,self.EffectAnimTime)

	AudioMgr:PlayEffect(AudioType.UI, "progress_add")
end

function PartnerViewQuality:OnQualityEffectEnd()

    -- self.qua_eff:SetActive(true)


end

function PartnerViewQuality:OnQualityEffectAgain()
    local info,cfg = self.data:GetPartnerQualityInfoById( self.data.cur_sel_partner,false )
    PartnerCtrl.Instance:SendPartnerQualityUpReq({
        use_type = 26,
        up_type = cfg.o_cfg.quality_upgrade,
        partner_id = self.data.cur_sel_partner ,
    })
    self.ProtectArea:SetActive(false)

    if self.Timer1 then
        TimeHelper:CancelTimer(self.Timer1)
		self.Timer1 = nil
	end

    if self.Timer2 then
		TimeHelper:CancelTimer(self.Timer2)
		self.Timer2 = nil
	end
end 

function PartnerViewQuality:OnClickPopSkill()
    self.PopPart:SetActive(true)

    -- local info,cfg = self.data:GetPartnerQualityInfoById( self.data.cur_sel_partner )
    self.popskillList:SetData(self.data:GetPartnerQualitySkillShow(self.data.cur_sel_partner))
end

function PartnerViewQuality:OnClickPop(data)
    UH.SetText(self.popname,string.format( Language.Common.Xstr,
        PartnerQualityColor[data.cfg.quality_upgrade + PartnerConfig.QuaFetter.Grassy],
        self.language.QualityName[data.cfg.quality_upgrade].."·"..
        string.format(self.language.QualitySeg,DataHelper.GetDaXie(data.cfg.quality_segment))))

    UH.SetText(self.popSeg, string.format(Language.Partner.QualitySeg,DataHelper.GetDaXie(data.cfg.quality_segment)))
    UH.SpriteName(self.popQua, "_Loc_"..PartnerConfig.QuaQuaPicName[data.cfg.quality_upgrade])
    local index = 1
    local attr_list = self.data:GetPartnerQualityAttrList(data.cfg.partner_id,data.cfg.quality_upgrade,data.cfg.quality_segment)
    local content_1 = ""
    local content_2 = ""
    for k,v in pairs(attr_list) do 
        local operate_content = index % 2 == 0 and content_2 or content_1
        if Language.Common.AttrList[v.attr_type] ~= nil and v.attr_plus > 0 then 
            operate_content = Language.Common.AttrList[v.attr_type]..":"..string.format( Language.Common.Xstr,COLORSTR.Green3,"+"..v.attr_plus).."\n"
            if index % 2 == 0 then 
                content_2 = content_2 ..operate_content
            else 
                content_1 = content_1 ..operate_content
            end 
            index = index + 1
        end 
    end 
    
    self.popskillList:SetData(self.data:GetPartnerQualityPopSkillShow(data.cfg.partner_id,data.cfg.quality_upgrade,data.cfg.quality_segment))

    UH.SetText(self.popcontent1, content_1)
    UH.SetText(self.popcontent2, content_2)
    self.PopPart:SetActive(true)
end

function PartnerViewQuality:OnClickClosePop()
    self.PopPart:SetActive(false)
end

PartnerQualityAttrCell = DeclareMono("PartnerQualityAttrCell", UIWidgetBaseItem)
function PartnerQualityAttrCell:PartnerQualityAttrCell()

end

function PartnerQualityAttrCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    -- self.Image.SpriteName = self.data.flag_main and PartnerConfig.QuaPicName[self.AttrType].."_2" or PartnerConfig.QuaPicName[self.AttrType].."_1" 
    -- self.AttrText.SpriteName = self.data.flag_main and "_Loc_"..PartnerConfig.QuaPicName[self.AttrType].."_z_2" or  "_Loc_"..PartnerConfig.QuaPicName[self.AttrType].."_z_1"
    self.NumPlus.text = self.data.attr_plus
    self.NumTotal.text = self.data.attr_total
    self.NumObj:SetActive(not self.data.is_max and self.data.attr_plus > 0)
end

PartnerQuaPreCell = DeclareMono("PartnerQuaPreCell", UIWidgetBaseItem) 
function PartnerQuaPreCell:PartnerQuaPreCell()

end

function PartnerQuaPreCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SpriteName(self.qua, "_Loc_"..PartnerConfig.QuaQuaPicName[data.cfg.quality_upgrade])
    UH.SetText(self.seg, string.format(Language.Partner.QualitySeg,DataHelper.GetDaXie(data.cfg.quality_segment)))

    if self.qua_show then 
        UH.SpriteName(self.qua_show, "_Loc_"..PartnerConfig.QuaQuaPicName[data.cfg.quality_upgrade])
    end 
    if self.seg_show then 
        UH.SetText(self.seg_show, string.format(Language.Partner.QualitySeg,DataHelper.GetDaXie(data.cfg.quality_segment)))
    end 

    local name = Split(Language.Partner.QualityPreName[data.cfg.quality_upgrade],"|")
    for i = 1,2 do 
        UH.SetText(self["name_"..i],name[i])
        if self["name_"..i.."_show"] ~= nil then 
            UH.SetText(self["name_"..i.."_show"],name[i])
        end 
    end 

end 

PartnerQuaPreNextCell = DeclareMono("PartnerQuaPreNextCell", UIWidgetBaseItem) 
function PartnerQuaPreNextCell:PartnerQuaPreNextCell()
end

function PartnerQuaPreNextCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.qua_next_1, "_Loc_"..PartnerConfig.QuaQuaPicName[data.next.cfg.quality_upgrade])
    UH.SpriteName(self.qua_next_2, "_Loc_"..PartnerConfig.QuaQuaPicName[data.next.cfg.quality_upgrade])

    UH.SetText(self.seg_next_1, string.format(Language.Partner.QualitySeg,DataHelper.GetDaXie(data.next.cfg.quality_segment)))
    UH.SetText(self.seg_next_2, string.format(Language.Partner.QualitySeg,DataHelper.GetDaXie(data.next.cfg.quality_segment)))

    local name = Split(Language.Partner.QualityPreName[data.next.cfg.quality_upgrade],"|")
    for i = 1,2 do 
        for j = 1,2 do 
            UH.SetText(self["name_next_"..i.."_"..j],name[i])
        end 
    end 

    self.after_obj:SetActive(data.after ~= nil)
    if data.after ~= nil then 
        UH.SpriteName(self.qua_after_1, "_Loc_"..PartnerConfig.QuaQuaPicName[data.after.cfg.quality_upgrade])
        UH.SpriteName(self.qua_after_2, "_Loc_"..PartnerConfig.QuaQuaPicName[data.after.cfg.quality_upgrade])
    
        UH.SetText(self.seg_after_1, string.format(Language.Partner.QualitySeg,DataHelper.GetDaXie(data.after.cfg.quality_segment)))
        UH.SetText(self.seg_after_2, string.format(Language.Partner.QualitySeg,DataHelper.GetDaXie(data.after.cfg.quality_segment)))
    
        local name = Split(Language.Partner.QualityPreName[data.after.cfg.quality_upgrade],"|")
        for i = 1,2 do 
            for j = 1,2 do 
                UH.SetText(self["name_after_"..i.."_"..j],name[i])
            end 
        end 
    end 
end

PartnerViewQuaSkilItem = DeclareMono("PartnerViewQuaSkilItem", UIWidgetBaseItem)

function PartnerViewQuaSkilItem:SetData(data)
    self.DetailObj:SetActive(nil ~= data)
    if nil == data then return end
    local info = data.info
    local per_info = data.per_info
    self.SkillCell:SetData(data, true)
    self.Name.text = nil == per_info and info.name or per_info.name
    if self.Desc then 
        if self.Lock then 
            self.Desc.text = info.desc2
        else 
            self.Desc.text = info.desc_qua
        end 
    end 
    self.XinObj:SetActive(nil == per_info or info.skill_level == per_info.skill_level)
    self.UpObj:SetActive(nil ~= per_info and info.skill_level ~= per_info.skill_level)
    self.UpNum.text = DataHelper.GetRomanNum(info.skill_level)
    if self.CurNum and per_info ~=nil then 
        self.CurNum.text = DataHelper.GetRomanNum(per_info.skill_level)
    end 
end