MedalBuildFlushPanel = MedalBuildFlushPanel or DeclareMono("MedalBuildFlushPanel", UIWFlushPanel)

function MedalBuildFlushPanel:MedalBuildFlushPanel()
    self.data = MedalData.Instance
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item),func = self.FlushDust},
        {data = self.data.medal_lottrey_flag,func = self.FlushLottrey,init_call = false},  
    }

    self.send_data = {}

    self.BoxAnim:ListenEvent("Open", function(eve_param,anim_st_info)
        if "show" == eve_param then
            self.EffectTool:Play("10010846")

            local data_ = self.data:GetMedalLottreyInfo()
            if self.send_data.send_type == 10  then 
                self.JumpGroupObj:SetActive(true)
                for i = 1, 10 do 
                    if data_[i] ~= nil then 
                        self.JumpCells[i]:SetData(data_[i]) 
                    end 
                end 
            else
                self.JumpCellObj:SetActive(true)
                if data_[1]~= nil then 
                    self.JustJumpCell:SetData(data_[1]) 
                end 
            end 

            self.send_click_count = self.send_data.send_type

            if self.try_timer == nil then 
                TimeHelper:CancelTimer(self.try_timer)
                self.try_timer = nil
            end

            self.try_timer = TimeHelper:AddDelayTimer(function()
                self.try_timer = nil
                if ViewMgr:IsOpen(MedalAtelierView) then 
                    if self.Mask ~= nil then self.Mask:SetActive(true) end 
                    if self.GroupBlock ~= nil then self.GroupBlock:SetActive(false) end 
                end 
            end ,1.2)
        end
    end)

    self.JumpCellAnmi:ListenEvent("JustJump", function(eve_param,anim_st_info)
        if "end" == eve_param then
            self.JustJumpCell:OnClickEffect()
        end 
    end)

    local param_t = {
        mono = self,
        pre_pause = 0.4,
        end_func = function ()
            self.JumpInfoObj:SetActive(true)
            self.effect_Scan = self.EffectTool:Play(tostring(self.JumpInfo.scan_eff_id)) 
        end
    }

    for i = 1,10 do self.JumpEffectCells[i]:SetData(param_t) end 

    self.try_timer = nil

    self.flush_left_item_handle = nil

end 

function MedalBuildFlushPanel:Awake()
    UIWFlushPanel.Awake(self)
    
    self:FlushLevelCell()
end

function MedalBuildFlushPanel:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(MedalAtelierView)
end

function MedalBuildFlushPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.try_timer == nil then 
        TimeHelper:CancelTimer(self.try_timer)
        self.try_timer = nil
    end 

    if self.flush_left_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_left_item_handle)
        self.flush_left_item_handle = nil
    end
end 

function MedalBuildFlushPanel:OnClickRate()
    -- self.send_data.level_data.seq
    -- LogError("?!?!?", self.send_data.level_data)
    ViewMgr:OpenView(MedalBuildRateView,self.send_data.level_data)
end

function MedalBuildFlushPanel:FlushLevelCell()
    self.LevelList:SetData(Config.medal_cfg_auto.medal_exchange)
    -- local level = RoleData.Instance:GetRoleLevel()
    local prof = RoleData.Instance:GetRoleProf()
    local pos_ = 1
    for k,v in pairs(Config.medal_cfg_auto.medal_exchange) do 
        if v.job_type == prof then 
            pos_ = k
        end 
    end 
    if self.flush_left_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_left_item_handle)
        self.flush_left_item_handle = nil
    end
    -- self.flush_left_item_handle = TimeHelper:AddDelayTimer(function()
    --     self.flush_left_item_handle = nil
    --     self.LevelList:ClickItem(pos_)
    --     if pos_ <= #Config.medal_cfg_auto.medal_exchange - 5 then 
    --         self.LevelList:JumpVerticalIndex(pos_,127,-40)
    --     else 
    --         self.LevelList:JumpVerticalIndex(#Config.medal_cfg_auto.medal_exchange -5,127,-40)
    --     end 
    -- end,0.2)
    self.flush_left_item_handle = TimeHelper:AddDelayFrameTimer(function()
        self.flush_left_item_handle = nil
        self.init_ = true
        self:OnDragLeftList( )
    end,5+math.floor(#Config.medal_cfg_auto.medal_exchange/self.LevelList.StepCount))
    -- self.LevelList:ClickItem(pos_)

    self.LevelList:ClickItem(1)
end 

function MedalBuildFlushPanel:OnClickLevel(data)
    self.send_data.level_data = data
    self:FlushDust()
    self:FlushMedalRed()
    self:FlushMedalRareShow()

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
end

function MedalBuildFlushPanel:OnClickBuild(_type)
    if self.send_data.level_data == nil then 
        PublicPopupCtrl.Instance:Center(Language.Medal.BuildBuildNeedSelect)
        return 
    end 

    local flag_bag_full = not BagData.Instance:TryFindEmptyInBagByTypeNum(nil,_type)
    local flag_temp_full = not BagData.Instance:CheckTempBagEmptyNum(_type)
    if flag_bag_full and flag_temp_full then 
        if flag_temp_full and not flag_bag_full then 
            PublicPopupCtrl.Instance:Center(Language.Bag.BagTemporaryWithoutEmpty) 
            return 
        elseif flag_bag_full and not flag_temp_full then 
            PublicPopupCtrl.Instance:Center(Language.Bag.BagFetchWithoutEmpty) 
            return 
        elseif flag_temp_full then 
            PublicPopupCtrl.Instance:Center(Language.Bag.BagTemporaryWithoutEmpty) 
            return 
        end 
    end 

    if self:FlagSendLottrey(_type) then 
        local cur_num = Item.GetNum(CommonItemId.XianFen)
        local lack_num = _type == 10 and self.send_data.level_data.ten_num - cur_num or  self.send_data.level_data.one_num - cur_num
        -- local key,config = MallData.Instance:GetShopItemConfig(CommonItemId.XianFen)
        -- LogError("?",key,config)
        -- StoreData.Instance:SetJumpIndex(config,config.item_id)
        -- MallData.JumpItemNum = math.floor(lack_num / 10 + 1)
        -- ViewMgr:OpenViewByKey(Mod.Store.Gold)

        local param = {item_id = CommonItemId.XianFen, draw_times = lack_num%10 == 0 and math.floor(lack_num / 10) or math.floor(lack_num / 10 + 1) }
        ViewMgr:OpenView(MedalBuildQuickBuy,param)
        return 
    end 
        
    self:SendClickBuild(_type)
end

function MedalBuildFlushPanel:FlushLottrey()
    self:PlayEffect()   
end

function MedalBuildFlushPanel:FlagSendLottrey(_type)
    local cur_num = Item.GetNum(CommonItemId.XianFen)
    local cost_num = _type == 1 and self.send_data.level_data.one_num or self.send_data.level_data.ten_num
    return cur_num < cost_num 
end

function MedalBuildFlushPanel:PlayEffect()
    self.BoxAnim:SetTrigger(APH("Open"))
end

function MedalBuildFlushPanel:ClearAllEffect()
    self.JustJumpCell:ChangeEffect()
    for i = 1,10 do 
        self.JumpCells[i]:ChangeEffect()
    end 

    self.JumpGroupAnmi:SetTrigger(APH("completer"))
    self.JumpCellAnmi:SetTrigger(APH("completer"))
    self.JumpGroupObj:SetActive(false)
    self.JumpCellObj:SetActive(false)

    self.JumpInfoObj:SetActive(false)
    self.JumpInfo:EffectShut()
    self.Mask:SetActive(false)

    self.BtnSkipObj:SetActive(true)
end

function MedalBuildFlushPanel:OnClickRedTips()

    -- LogError(self.send_data.level_data)
    self.data:SetMedalRedTips(self.send_data.level_data.seq)
    self.RedGou:SetActive(not self.RedGou.activeSelf)

end

function MedalBuildFlushPanel:FlushMedalRed()
--     LogError("FlushMedalRed")
--     local cfg = Config.medal_cfg_auto.medal_exchange
    local seq_list = self.data:GetMedalRedTips()
    for k,v in pairs(seq_list) do 
        if v == self.send_data.level_data.seq then 
            self.RedGou:SetActive(true)
            return 
        end 
    end 
    self.RedGou:SetActive(false)
end

function MedalBuildFlushPanel:FlushMedalRareShow()
    local data_list = self.data:GetMedalSpRareShowList(self.send_data.level_data.seq)
    self.rare_show:SetActive(#data_list > 0)
    self.rare_list:SetData(data_list)
end

function MedalBuildFlushPanel:SendClickBuild(_type)
    self.send_data.send_type = _type

    local param_t = {
        req_type = MEDAL_REQ_TYPE.LOTTREY,
        param1 = self.send_data.level_data.seq,
        param2 = self.send_data.send_type, 
    }
    BagCtrl.Instance:SendMedalReq(param_t)

    self.GroupBlock:SetActive(true)
end

function MedalBuildFlushPanel:OnClickSpShow()
    local param_t = MedalData.Instance:GetMedalSpShowList(self.send_data.level_data.seq)
    -- PublicInfoShowCtrl.Instance:SetPublicPageShow(param_t)
	ViewMgr:OpenView(MedalEffectShow,param_t)--{seq = self.send_data.level_data.seq})
end

function MedalBuildFlushPanel:FlushDust()
    if self.send_data.level_data == nil then return end 

    local cur_num = Item.GetNum(CommonItemId.XianFen)
    local flag_ = cur_num < self.send_data.level_data.one_num
    local _str = string.format(Language.Common.Cstr,flag_ and COLORSTR.Red8 or COLORSTR.Green3,cur_num,self.send_data.level_data.one_num)
    self.one_num.text = _str
    flag_ = cur_num < self.send_data.level_data.ten_num
    _str = string.format(Language.Common.Cstr,flag_ and COLORSTR.Red8 or COLORSTR.Green3,cur_num,self.send_data.level_data.ten_num)
    self.ten_num.text = _str

    self:OnDragLeftList()
end

function MedalBuildFlushPanel:OnClickSkip()
    self:ClearAllEffect()
	ViewMgr:OpenView(MedalLottreyView)

    MedalData.Instance:PlayMedalMarquee(nil,true)
end

function MedalBuildFlushPanel:OnClickBack()
    if not self.JumpInfo.is_complete and self.send_click_count > 1 then 
        return 
    end 
    
    self.send_click_count = self.send_click_count - 1
    if self.send_click_count == 0 then 
        self:ClearAllEffect()
    else
        self.BtnSkipObj:SetActive(true)
        self.JumpInfoObj:SetActive(false)
        self.JumpInfo:EffectShut()
    end 

    self.GroupBlock:SetActive(false)
end

function MedalBuildFlushPanel:OnDragLeftList( )
    if not self.init_ then return end 
    local list = self.data:GetMedalRedTips()
    local ContentRect = self.LevelList.ContentParent:GetComponent(typeof(UnityEngine.RectTransform))

    local check_lits = {}
    for k,v in pairs(list) do 
        local obj = self.LevelList:GetItem(v+1)
        local vo = {}
        vo.index = k
        vo.pos = obj.RectTrans.localPosition.y + ContentRect.localPosition.y 
        vo.red_num = obj:RedNum()
        table.insert( check_lits, vo )
    end 

    local up_num = 0
    if self.red_up == nil then
        self.red_up = UiRedPoint.New(self.up_arrow, Vector3.New(-15,-8,0))
    end

    local down_num = 0
    if self.red_down == nil then
        self.red_down = UiRedPoint.New(self.down_arrow, Vector3.New(15,8,0))
    end

    for k,v in pairs(check_lits) do 
        if v.pos > 0 and v.red_num >0 then up_num = up_num + 1 end 
        if v.pos < -470 and v.red_num >0 then down_num = down_num + 1 end 
    end 

    self.red_up:SetNum(up_num)
    self.red_down:SetNum(down_num)
end

MedalBuildMedalJumpInfo = MedalBuildMedalJumpInfo or DeclareMono("MedalBuildMedalJumpInfo", UIWFlushPanel)

function MedalBuildMedalJumpInfo:MedalBuildMedalJumpInfo()

end

function MedalBuildMedalJumpInfo:SetData(data)
    if data.Color == nil then 
        data = Item.Create(data)
    end 

    self.Name.text = string.format( Language.Medal.MedalNameColor,QualityColorStr[data:Color()],data:Name()) 
    self.Cell:SetData(data)
    self.Level.text = data:LevelDesc()
    local sp_cfg = nil
    if data.param.sp_id ~= nil and data.param.sp_id > 0 then 
        sp_cfg = BagData.Instance:GetEquipEffectCfg(data.param.sp_id)
        self.Special.text = string.format(Language.Common.Xstr, QualityColorStr[data:Color()], sp_cfg.description)
    else 
        self.Special.text = Language.Medal.SpecialSoloDark..Language.Medal.SpecialNoneDark
    end 

    for i = 1,4 do self.AttrObjList[i]:SetActive(false) end 
    for k, v in ipairs(data:BaseAttr() or {}) do
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

    self.frame_eff_id = sp_cfg ~= nil and data:Color() + 10010844 or 10010844
    self.scan_eff_id = sp_cfg ~= nil and MedalConfig.BuildScanEffect[data:Color()] or MedalConfig.BuildScanEffect[ItemColor.None]

    self.BtnSkipObj:SetActive(false)

    MedalData.Instance:PlayMedalMarquee(data)
end

function MedalBuildMedalJumpInfo:OnEnable()
    self.is_complete = false
    UIWFlushPanel.OnEnable(self)
    -- TimeHelper:AddDelayTimer(function()
    --     self.effect_Scan = self.EffectTool:Play(tostring(self.scan_eff_id))
    -- end,0)
end

function MedalBuildMedalJumpInfo:OnScanComplete()
    self.effect_ = self.EffectTool:Play(tostring(self.frame_eff_id))
    self.is_complete = true
end

function MedalBuildMedalJumpInfo:EffectShut()
    if self.effect_ then 
        self.EffectTool:Stop(self.effect_)
    end 

    if self.effect_Scan then 
        self.EffectTool:Stop(self.effect_Scan)
    end 
end

MedalBuildLevelCell = DeclareMono("MedalBuildLevelCell", UIWidgetBaseItem)
function MedalBuildLevelCell:MedalBuildLevelCell( )

end

function MedalBuildLevelCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.Name.text = data.name --data.level_min..Language.Medal.BuildLevel
    self.Named.text = data.name --data.level_min..Language.Medal.BuildLevel

    self.Interactor.Interactable = RoleData.Instance:GetRoleLevel() >= data.level_min

    self:careData(BagData.Instance.item_grid, BindTool.Bind(self.CheckRedPoint,self))
    self:careData(MedalData.Instance.medal_redtips, BindTool.Bind(self.CheckRedPoint,self))

    UH.SpriteName(self.ProfIcon,MedalConfig.MedalProfIcon[data.job_type])
end

function MedalBuildLevelCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

function MedalBuildLevelCell:CheckRedPoint()
    local list = MedalData.Instance:GetMedalRedTips()

    for k,v in pairs(list) do 
        if v == self.data.seq then 
            if self.red_point == nil then
                self.red_point = UiRedPoint.New(self.RedPoint, Vector3.New(0,0,0))
            end
            local num = self:RedNum()
            self.red_point:SetNum(num)
            return 
        end 
    end 

    if self.red_point ~= nil then
        self.red_point:SetNum(0)
    end 
end

function MedalBuildLevelCell:RedNum()
    return Item.GetNum(CommonItemId.XianFen) >= self.data.one_num and 1 or 0
end

function MedalBuildLevelCell:OnDestroy()
    self:uncareData()
end

MedalJumpCell = DeclareMono("MedalJumpCell", UIWidgetBaseItem)
function MedalJumpCell:MedalJumpCell()

end

--10010842
--10010843
--10010844
--10010845
--10010861

function MedalJumpCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local sp_cfg = nil

    if data.param.sp_id > 0 then 
        sp_cfg = BagData.Instance:GetEquipEffectCfg(data.param.sp_id)
    end 
    
    local eff_id = sp_cfg ~= nil and data:Color() + 10010840 - 1 or 10010861
    self.drag_effect = sp_cfg ~= nil and MedalConfig.BuildDragEffect[data:Color() ] or MedalConfig.BuildDragEffect[ItemColor.None]
    if eff_id > 0 then 
        self.effect_ = self.EffectTool:Play(tostring(eff_id) )
    end 

    local medal = Item.Create({item_id = data.item_id,param = {sp_id = data.param.sp_id }})
    -- UH.SetIcon(self.icon, medal:IconId())

    self.sp_eff:SetActive( medal.param.sp_id > 0)
    if medal.param.sp_id > 0 then 
        local cfg = Cfg.MedalEffectById(medal.param.sp_id)
        self.sp_eff:GetComponent(typeof(UnityUI.Text)).text = cfg.name
        self.sp_eff:GetComponent(typeof(UnityUI.Outline)).effectColor = Medal_Color_Outline[data:Color() ]
        self.sp_eff:GetComponent(typeof(UnityUI.Shadow)).effectColor = Medal_Color_Shadow[data:Color() ]
    else 
        self.sp_eff:GetComponent(typeof(UnityUI.Text)).text = ""
    end 

    -- self.icon_obj:SetActive(false)
end

function MedalJumpCell:OnClickEffect()
    if self.data == nil then return end 
    self:ChangeEffect()

    -- self.info_view_obj:SetActive(true)
    self.info_view:SetData(self.data)
    -- self.icon_obj:SetActive(true)
    if self.FlyEffect then 
        self.FlyEffect:Play(tostring(self.drag_effect))   

        self.GroupBlock:SetActive(true)
    else
        self.info_view_obj:SetActive(true)
    end
end

function MedalJumpCell:ChangeEffect(_type)
    if self.effect_ then 
       self.EffectTool:Stop(self.effect_)
    end 
end