

FightFBViewYMZ = FightFBViewYMZ or DeclareMono("FightFBViewYMZ", UIWFlushPanel)

function FightFBViewYMZ:FightFBViewYMZ()
	self.data = FightCtrl.Instance:Data()
    self.team_data = TeamCtrl.Instance:Data()
    self.language = Language.Fight.YMZ

	self.data_cares = {
		{data = self.data.fight_fb_ymz, func = self.FlushType, keys = {"type"}},
		{data = self.data.fight_fb_ymz, func = self.FlushPage, keys = {"page"}, init_call = false},
		{data = self.data.fight_fb_ymz, func = self.FlushItemSel, keys = {"flush_sel"}},
		{data = self.data.fight_fb_ymz, func = self.FlushInfo, keys = {"info"}},
		{data = self.data.fight_fb_ymz, func = self.FlushRecordNum, keys = {"normal_fb_record_list"}, init_call = false},
		{data = self.data.fight_crystal_sweep, func = self.FlushJingYingTimes},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushCosume, init_call = false},
	}

    GuideManager.Instance:RegisterGetGuideUi("FightFBYMZSeq5", function ()
        return self.ShowList and self.ShowList.index_item[1], function ()
            self.ShowList.index_item[1]:OnClickItem()
		end
    end)

    self:ChekcSimularIphone()

    self.flush_layout = {}
end

function FightFBViewYMZ:Awake()
    UIWFlushPanel.Awake(self)

    self:uncareData()
    local list = self.data.ymz_type_list
    
    for i = 1, self.TypeLists:Length() do
        table.sort(list[i], function (a, b)
            if not a or not b then return a end
            local an = BagData.Instance:GetNumByItemId(a.expend_item) > 0 and 1 or 0
            local bn = BagData.Instance:GetNumByItemId(b.expend_item) > 0 and 1 or 0
			if an == bn then
				return a.sort < b.sort
			end
			return an > bn
        end)
        self.TypeLists[i]:SetData(list[i])
    end

    self.LeftBtns:SetActive(FlyUpData.Instance:IsFinish())
end

function FightFBViewYMZ:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    
    Runner.Instance:RemoveRunUntilTrue(self.move_handle)
    self:uncareData()
    TimeHelper:CancelTimer(self.timer_dt)
end

function FightFBViewYMZ:IsLoaded()
    return true
end

function FightFBViewYMZ:ChekcSimularIphone()
    if IS_IPHONEX or EditorSettings:SimularIphoneX() then
        self.PanelRect:SetSizeWithCurrentAnchors(0, self.PanelRect.rect.width - 100)
    end
end

function FightFBViewYMZ:uncareData()
    if nil == self.data.ymz_care_handles then
        self.data.ymz_care_handles = {}
        return
    end

    for handle, data in pairs(self.data.ymz_care_handles) do
		data:Uncare(handle)
	end
    self.data.ymz_care_handles = {}
end

function FightFBViewYMZ:FlushType()
    self.data:FightFBYMZAutoSel()
    local fb_type = self.data.fight_fb_ymz.type or 1
    self:FlushBgLayouts(fb_type)
    self.data:FightFBEnergyShow(1 == fb_type or 4 == fb_type)
    self.TypeList[fb_type].isOn = true
    self:FlushPage()
    local num = self.data:FightFBYMZHaveNumAll()
    self.HaveNumAll.text = string.format(self.language.HaveNumAll, math.min(num, 99), num > 99 and "+" or "")
    self.BtnCostInter.Interactable = num > 0
end

function FightFBViewYMZ:FlushPage()
    local fb_type = self.data.fight_fb_ymz.type
    local fb_page = self.data.fight_fb_ymz.page
    self.ShowList:SetData(self.data.ymz_page_list[fb_type] or {})
end

function FightFBViewYMZ:FlushInfo()
    local info = self.data.fight_fb_ymz.info
    if not next(info) then return end
    self.data.fight_fb_ymz.type = info.ymz_type
    self.data.fight_fb_ymz.page = info.ymz_page
    self.RewardListE:SetData(info.rewards_e)
    self.RewardListM:SetData(info.rewards_m)
    self.RewardListI:SetData(info.rewards_i)
    self.ExpNum.text = self.data:GetFightCrystalExpNum()
    -- self.ExpObj:SetActive(1 == info.ymz_type)
    self.ExpObj:SetActive(false)
    self.EquipDesc.text = info.equip_desc
    self.Monsters:SetData(info.monsters)
    self.Monsters2:SetData(info.monsters)
    for i = 1, self.UpTypes:Length() do
        self.UpTypes[i]:SetActive(info.up_types[i] or false)
    end
    if info.crystal > 0 then
        self.EleAttr:SetData(EleCrystal.GetElement(info.crystal))
    end
	self.EleAttr:SetObjActive(info.crystal > 0)
    self.CrystalObj:SetActive(info.ymz_type > 1)
    self.BtnSweepInter.Interactable = self.data:IsFightCrystalPassFb(info.scene_seq) and RoleData.Instance:GetSumScore() >= info.recommend_score
    self:FlushCosume()
    self:FlushRecordNum()
    self:FlushBtnInfo()
end

function FightFBViewYMZ:FlushCosume()
	local info = self.data.fight_fb_ymz.info
    if not next(info) then return end
    local fb_type = self.data.fight_fb_ymz.type
    local have_num = BagData.Instance:GetNumByItemId(info.expend_item)
    -- self.CusumeI:SetActive(have_num > 0 or 2 == fb_type)
    -- self.CusumeE:SetActive(0 == have_num and 1 == fb_type)
    self.CusumeI:SetActive(false)
    self.CusumeE:SetActive(true)
    self.CusumeINum.text = string.format(self.language.ItemCosume, have_num > 0 and "02cf7a" or "ff3e3e", have_num)
    self.CusumeENum.text = self.data:GetCrystalFbSweepUsePower()
    UH.SetIcon(self.CusumeIIcon, info.consume:IconId())

    local num = self.data:FightFBYMZHaveNumAll()
    self.HaveNumAll.text = string.format(self.language.HaveNumAll, math.min(num, 99), num > 99 and "+" or "")
    self.BtnCostInter.Interactable = num > 0
end

function FightFBViewYMZ:FlushRecordNum()
    local info = self.data.fight_fb_ymz.info
    if not next(info) then return end
    local fb_type = self.data.fight_fb_ymz.type
    -- if 1 == fb_type then
        local pass_time_max = self.data:GetCrystalFbPassTimeMax()
        local num = pass_time_max + (self.data.fight_fb_ymz.normal_fb_record_bt_list[info.scene_seq] or 0) - (self.data.fight_fb_ymz.normal_fb_record_list[info.scene_seq] or 0)
        self.RecordNum.text = string.format(self.language.RecordNum, RichTextHelper.ColorStr(math.max(num, 0), num > 0 and COLORSTR.Green7 or COLORSTR.Red10))
    -- end
    -- self.RecordNumObj:SetActive(1 == fb_type)
    self.RecordNumObj:SetActive(true)
    local extra_buy_times = self.data:GetCrystalFbExtraBuyTimes()
    self.BtnBuyObj:SetActive(extra_buy_times > 0)
end

function FightFBViewYMZ:FlushJingYingTimes()
    self.JingYingTimes.SpriteName = "DaoJiShiShuZi" .. self.data.fight_crystal_sweep.new_jingying_play_times_remain
end

function FightFBViewYMZ:FlushItemSel()
	local info = self.data.fight_fb_ymz.info
    if not next(info) then return end
    if FightFBViewYMZ.not_flush_sel then
        FightFBViewYMZ.not_flush_sel = nil
        return
    end
    local complete_func = function ()
        local index = self.ShowList:GetByData(info:Val())
        local to_x = - (index < 2 and 0 or (700 + 500 * (index - 3)))
        to_x = math.max(to_x, -self.ContentPos.sizeDelta.x)
        Runner.Instance:RemoveRunUntilTrue(self.move_handle)
        if self.data.fight_fb_ymz.flush_sel.is_move then
            local to_x_per = (to_x - self.ContentPos.anchoredPosition.x) / 10
            self.move_handle = Runner.Instance:RunUntilTrue(function ()
                if math.abs(to_x - self.ContentPos.anchoredPosition.x) > math.max(math.abs(to_x_per), 10) then
                    self.ContentPos.anchoredPosition = Vector2.New(self.ContentPos.anchoredPosition.x + to_x_per, self.ContentPos.anchoredPosition.y)
                    return false
                else
                    self.ContentPos.anchoredPosition = Vector2.New(to_x, self.ContentPos.anchoredPosition.y)
                    return true
                end
            end)
        else
            if 0 == self.ContentPos.sizeDelta.x then
                TimeHelper:AddDelayFrameTimer(function ()
                    local index = self.ShowList:GetByData(info:Val())
                    local to_x = - (index < 2 and 0 or (700 + 500 * (index - 3)))
                    to_x = math.max(to_x, -self.ContentPos.sizeDelta.x)
                    self.ContentPos.anchoredPosition = Vector2.New(to_x, self.ContentPos.anchoredPosition.y)   
                end)
            else
                self.ContentPos.anchoredPosition = Vector2.New(to_x, self.ContentPos.anchoredPosition.y)
            end
        end
    end
    complete_func()
end

function FightFBViewYMZ:FlushBtnInfo()
    local fb_type = self.data.fight_fb_ymz.type
    local special = self.InfoToggle.isOn and (1 == fb_type)
	-- self.BtnInfoPos.anchoredPosition = Vector2.New(-30, special and -55 or -100)
    self.InfoLayout.padding.top = special and -30 or 10
end

function FightFBViewYMZ:OnToggleType(value)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
	self.data.fight_fb_ymz.type = value

    local complete_func = function (value)
		self.ShowOrder.enabled = value
	    self.ShowCanvas.enabled = value
	end
    complete_func(false)

    local delay_frame = 1
    Runner.Instance:RunUntilTrue(
        function()
			if delay_frame > 0 then
				delay_frame = delay_frame - 1
				return false
			end
            complete_func(true)
            return true
        end
    )
end

function FightFBViewYMZ:OnPressDownItem()
	self.page_start_x = UnityEngine.Input.mousePosition.x
end

function FightFBViewYMZ:OnPressUpItem()
    local fb_type = self.data.fight_fb_ymz.type
    local add_page = UnityEngine.Input.mousePosition.x - self.page_start_x > 0 and -1 or 1
    local to_page = self.data.fight_fb_ymz.page + add_page
    if self.data.ymz_page_list[fb_type][to_page] then
        self.data.fight_fb_ymz.page = to_page
    end
end

function FightFBViewYMZ:OnClickTeam()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	local info = self.data.fight_fb_ymz.info
	local target = self.team_data:GetTeamGoalWithId(info.team_target)
	if LoginData.Instance:IsOnCrossSever() and TeamConfig.CrossType.not_cross == target.is_cross then
		ViewMgr:CloseView(FightFBView)
		PublicPopupCtrl.Instance:Center(Language.Team.MatchCrossError)
		return
	end
	if RoleData.Instance:GetRoleLevel() < target.low_level then
		PublicPopupCtrl.Instance:Center(Language.Team.MatchLevelError)
		return
	end
    local do_func = function ()
        if self.team_data:InTeam() then
            TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
            TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
            ViewMgr:OpenView(TeamView)
        else
            ViewMgr:CloseView(FightFBView)
            self.team_data.cur_platform_target_id = info.team_target
            TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
            PublicPopupCtrl.Instance:Center(Language.Team.MatchTips)
        end
    end
    if 0 == self.data.fight_crystal_sweep.new_jingying_play_times_remain then
        PublicPopupCtrl.Instance:DialogTips{
            content = Language.Fight.YMZ.JingYingNumTips,
            confirm = {
                func = function ()
                    Call(do_func)
                    ViewMgr:CloseView(DialogTipsView)
                end
            },
            tnr = DTTodayNotRemind.FightJYTips,
        }
    else
        Call(do_func)
    end
end

function FightFBViewYMZ:OnClickFight()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    local info = self.data.fight_fb_ymz.info
	if RoleData.Instance:GetRoleLevel() < (info.level_min or 0) then
		PublicPopupCtrl.Instance:Center(ErrorInfo[100008])
		return
	end
    -- if (info.ymz_type or 1) > 1 then
    --     local do_func = function ()
    --         if BagData.Instance:GetNumByItemId(info.expend_item) >= 1 then
    --             ViewMgr:CloseView(FightFBView)
    --             if nil ~= info.click_handle then
    --                 info.click_handle()
    --             end
    --         else
    --             local item_cfg = Item.GetConfig(info.expend_item)
    --             PublicPopupCtrl.Instance:Center(string.format(Language.RichText.ShowStr.item_not_enough_fb4, ColorStr(item_cfg.name,QualityColorStr[item_cfg.color])))
    --         end
    --     end
    --     if 0 == self.data.fight_crystal_sweep.new_jingying_play_times_remain then
    --         PublicPopupCtrl.Instance:DialogTips{
    --             content = Language.Fight.YMZ.JingYingNumTips,
    --             confirm = {
    --                 func = function ()
    --                     Call(do_func)
    --                     ViewMgr:CloseView(DialogTipsView)
    --                 end
    --             },
    --             tnr = DTTodayNotRemind.FightJYTips,
    --         }
    --     else
    --         Call(do_func)
    --     end
    -- else
        if nil ~= info.click_handle then
            info.click_handle()
        end
    -- end
end

function FightFBViewYMZ:OnClickSweep()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    local info = self.data.fight_fb_ymz.info
	local value, tips = self.data:IsCanFightCrystalFbSweep(info)
	if not value then
		PublicPopupCtrl.Instance:Center(tips)
		return
	end
    if RoleData.Instance:GetSumScore() < info.recommend_score then
        PublicPopupCtrl.Instance:Center(string.format(self.language.NeedScoreSweepTip, info.recommend_score))
        return
    end
	FightCtrl.Instance:SendCrystalFbFastPassInfo(info.scene_seq, 1)
end

function FightFBViewYMZ:OnClickCost()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
    self.data:FightFBYMZTypeItemFlush()
    self.TypesObj:SetActive(true)
    self.TypesBlock:SetActive(true)
end

function FightFBViewYMZ:OnClickTypesBlock()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoClose)
    self.TypesObj:SetActive(false)
    self.TypesBlock:SetActive(false)
end

function FightFBViewYMZ:OnInfoToggle()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	self:FlushBtnInfo()
end

function FightFBViewYMZ:OnClickBuyTimes()
    local info = self.data.fight_fb_ymz.info
    local extra_buy_times = self.data:GetCrystalFbExtraBuyTimes()
    local num = extra_buy_times - (self.data.fight_fb_ymz.normal_fb_record_bt_list[info.scene_seq] or 0)
    if num > 0 then
        PublicPopupCtrl.Instance:AlertTip(string.format(self.language.BuyTimesTip, self.data:GetCrystalFbExtraBuyConsume(), math.max(num, 0)),function()
            FightCtrl.Instance:SendCrystalFbReq(3, info.scene_seq)
        end,CurrencyType.Gold)
    else
        FightCtrl.Instance:SendCrystalFbReq(3, info.scene_seq)
    end
end

function FightFBViewYMZ:FlushBgLayouts(type)
    type = type or 1
	if self.flush_layout[type] then return end
    self.flush_layout[type] = true
    self.BgLayouts[type].enabled = false
    self.BgLayouts[type].enabled = true
    TimeHelper:CancelTimer(self.timer_dt)
    self.timer_dt = TimeHelper:AddDelayFrameTimer(function ()
        self.BgLayouts[type].enabled = false
        self.BgLayouts[type].enabled = true
    end)
end


FightFBViewYMZItem = DeclareMono("FightFBViewYMZItem", UIWidgetBaseItem)

function FightFBViewYMZItem:FightFBViewYMZItem()
    self.fight_data = FightCtrl.Instance:Data()
    self.language = Language.Fight.YMZ.ItemShow
end

function FightFBViewYMZItem:OnDestroy()
    self:uncareData()
end

function FightFBViewYMZItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self:uncareData()
    self.care_handles = {}
    local is_empty = nil == data
    self.ItemObj:SetActive(not is_empty)
    if is_empty then return end
    self:careData(self.fight_data.fight_fb_ymz, BindTool.Bind(self.FlushShow, self), "info")
end

function FightFBViewYMZItem:FlushShow()
    local is_sel = self.data == self.fight_data.fight_fb_ymz.info:Val()
    local type = is_sel and 1 or 2
    local la = self.language[type]
    local co_bg = FightConfig.YMZItemBgs[type]
    self.Name.text = self.data.scene_name
    self.Level.text = string.format(la.level, self.data.level_min)
    self.Name.text = string.format(la.name, self.data.scene_name)
    self.Desc.text = la.desc
    self.DescLevel.text = string.format(la.desc_level, self.data.equip_level)
    self.BgSp1.SpriteName = co_bg.sp1
    self.BgSp2.SpriteName = co_bg.sp2
end

function FightFBViewYMZItem:OnClickItem()
	self.fight_data.fight_fb_ymz.info:Set(self.data)
end

FightFBViewYMZTypeItem = DeclareMono("FightFBViewYMZTypeItem", UIWidgetBaseItem)

function FightFBViewYMZTypeItem:FightFBViewYMZTypeItem()
    self.fight_data = FightCtrl.Instance:Data()
    self.language = Language.Fight.YMZ.TypeShow
end

function FightFBViewYMZTypeItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self:careData(self.fight_data.fight_fb_ymz, BindTool.Bind(self.FlushSel, self), "info")
    self:careData(RoleData.Instance.base_data, BindTool.Bind(self.FlushLock, self), "level")
    self:careData(self.fight_data.fight_fb_ymz_type_item_flush, BindTool.Bind(self.FlushNum, self))
    -- self:careData(BagData.Instance:GetColumnGrids(ItemColumnType.Item), BindTool.Bind(self.FlushNum, self))

    self.Name.text = self.data.scene_name
    self.Level.text = string.format(self.language.Level, self.data.level_min)
    -- self.Level.text = self.data.scene_type_1  .. "_" .. self.data.scene_seq
end

function FightFBViewYMZTypeItem:careData(data,func,...)
	self.fight_data.ymz_care_handles[data:Care(func,...)] = data
	func()
end

function FightFBViewYMZTypeItem:FlushSel()
    local is_sel = self.data == self.fight_data.fight_fb_ymz.info:Val()
    if self.is_sel == is_sel then return end
    self.is_sel = is_sel
    -- local type = is_sel and 1 or 2
    -- local co_bg = FightConfig.YMZTypeBgs[type]
    -- self.ItemSp.SpriteName = co_bg
    self.SelObj:SetActive(is_sel)
end

function FightFBViewYMZTypeItem:FlushLock()
    self.ItemInter.Interactable = RoleData.Instance:GetRoleLevel() >= self.data.level_min 
end

function FightFBViewYMZTypeItem:FlushNum()
    local num = BagData.Instance:GetNumByItemId(self.data.expend_item)
    self.HaveNum.text = string.format(self.language.HaveNum, math.min(num, 99), num > 99 and "+" or "")
    self.HaveNumObj:SetActive(num > 0)
end

function FightFBViewYMZTypeItem:OnClickItem()
    if self.is_sel then return end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	self.fight_data.fight_fb_ymz.info:Set(self.data)
    self.fight_data:FightFBYMZFlushSel(true)
end

FightFBViewYMZShowItem = DeclareMono("FightFBViewYMZShowItem", UIWidgetBaseItem)

function FightFBViewYMZShowItem:FightFBViewYMZShowItem()
    self.fight_data = FightCtrl.Instance:Data()
    self.language = Language.Fight.YMZ.ItemShow
end

function FightFBViewYMZShowItem:OnDestroy()
    self:uncareData()
end

function FightFBViewYMZShowItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self:uncareData()
    self.care_handles = {}
    local is_empty = nil == data
    self.ItemObj:SetActive(not is_empty)
    if is_empty then return end
    self.XiYouObj:SetActive(3 == data.scene_type_1)
    self.ChallengeObj:SetActive(self.data.scene_type_1 > 1 and RoleData.Instance:GetRoleLevel() >= self.data.level_min and BagData.Instance:GetNumByItemId(data.expend_item) > 0)
    self:careData(self.fight_data.fight_fb_ymz, BindTool.Bind(self.FlushShow, self), "info")
    self:careData(RoleData.Instance.base_data, BindTool.Bind(self.FlushLock, self), "level")
    self:careData(self.fight_data.fight_fb_ymz, BindTool.Bind(self.FlushSweepObj, self), "normal_fb_record_list")
end

function FightFBViewYMZShowItem:FlushShow()
    local is_sel = self.data == self.fight_data.fight_fb_ymz.info:Val()
    local type = is_sel and 1 or 2
    local la = self.language[type]
    local co_bg = FightConfig.YMZItemBgs[type]
    self.Name.text = self.data.scene_name
    self.Level.text = string.format(la.level, self.data.level_min)
    self.Name.text = string.format(la.name, self.data.scene_name)
    self.Desc.text = Format(la.desc, Language.Fight.YMZ.AccrssoryTip[self.data.is_feisheng])
    self.DescLevel.text = ""--string.format(la.desc_level, self.data.equip_level)
    self.BgSp1.SpriteName = co_bg.sp1
    self.BgSp2.SpriteName = co_bg.sp2
    self.PicSetter:SetByPath(string.format("fight/levels/%s_%s.png", self.data.scene_type_1, self.data.scene_seq))
    self.CapShow.text = string.format(self.language.CapShow, RichTextHelper.ColorStr(RoleData.Instance:GetSumScore(), RoleData.Instance:GetSumScore() >= self.data.recommend_score and COLORSTR.Green3 or COLORSTR.Red8), self.data.recommend_score)
end

function FightFBViewYMZShowItem:FlushSweepObj()
    local pass_time_max = self.fight_data:GetCrystalFbPassTimeMax()
    local num = pass_time_max + (self.fight_data.fight_fb_ymz.normal_fb_record_bt_list[self.data.scene_seq] or 0) - (self.fight_data.fight_fb_ymz.normal_fb_record_list[self.data.scene_seq] or 0)
    self.SweepObj:SetActive(1 == self.data.scene_type_1 and RoleData.Instance:GetSumScore() >= self.data.recommend_score and self.fight_data:IsFightCrystalPassFb(self.data.scene_seq) and num > 0)
end

function FightFBViewYMZShowItem:FlushLock()
    self.ItemInter.Interactable = RoleData.Instance:GetRoleLevel() >= self.data.level_min 
end

function FightFBViewYMZShowItem:OnClickItem()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	self.fight_data.fight_fb_ymz.info:Set(self.data)
    if self.ViewRect.rect.width - UnityEngine.Input.mousePosition.x <= 609 then
        self.fight_data:FightFBYMZFlushSel(true)
    end
end