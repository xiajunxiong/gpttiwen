
MainTopEscorting = MainTopEscorting or DeclareMono("MainTopEscorting", UIWFlushPanel)

MainTopFightCrystal = MainTopFightCrystal or DeclareMono("MainTopFightCrystal", UIWFlushPanel)

function MainTopFightCrystal:MainTopFightCrystal()
    self.data = FightCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data:GetFightCrystalInfo(), func = self.FlushTopMenu},
	}
	self.language = Language.Fight.CrystalDungenTop
end

function MainTopFightCrystal:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushBtnDetail()
end

function MainTopFightCrystal:FlushTopMenu()
    local fight_crystal_info = self.data:GetFightCrystalInfo()
	local cur_seq_info = self.data:GetFightCrystalConfigBySeq(fight_crystal_info.level_seq)
    if nil == cur_seq_info then
        -- Debuger.LogError("Error level_seq " .. fight_crystal_info.level_seq)
        return
    end
    local last_layer = fight_crystal_info.layer == cur_seq_info.layer
    self.LayerState1:SetActive(not last_layer)
    self.LayerState2:SetActive(last_layer and not fight_crystal_info.is_finish)
    self.LayerState3:SetActive(last_layer and fight_crystal_info.is_finish)
    self.BtnDetailObj:SetActive(not last_layer)
    self.LayerNum.text = string.format(self.language.layer, fight_crystal_info.layer, cur_seq_info.layer)
    self:NumFlush(self.CoinNum, fight_crystal_info.coin_num_r, fight_crystal_info.coin_num_max, self.CoinEffect)
    self:NumFlush(self.BoxNum, fight_crystal_info.box_num_r, fight_crystal_info.box_num_max, self.BoxEffect)
    self:NumFlush(self.MonsterNum, fight_crystal_info.monster_num_r, fight_crystal_info.monster_num_max, self.MonsterEffect)
    local percent = 0
    if last_layer then
        percent = fight_crystal_info.is_finish and 1 or 0
    else
        percent = 0 == fight_crystal_info.num_max and 1 or (fight_crystal_info.num_r / fight_crystal_info.num_max)
    end
    self.Progress:SetProgress(percent)
    self.Progress:SetText(math.floor(percent * 100) .. "%")
    if percent == 1 and self.EffectTool.isActiveAndEnabled then 
        self.EffectTool:Play("10010347") 
    end

    local obj_list = self.data:GetFightCrystalObjList()
    self.DetailGrid:SetData(obj_list)
    if last_layer and self.DetailObj.activeSelf then self.DetailObj:SetActive(false) end
end

function MainTopFightCrystal:NumFlush(text, num, num_max, effect)
    local show_effect = false
    num_max = num_max or 0
    num = num or 0
	if 0 == num_max then
		text.text = self.language.empty
	else
        if num_max == num then
            text.text = RichTextHelper.ColorStr(string.format(self.language.num, num, num_max), COLORSTR.Green10)
        else
            text.text = string.format(self.language.num, num, num_max)
            show_effect = true
        end
	end
    effect:SetActive(show_effect)
end

function MainTopFightCrystal:OnClickGoNpc()
    local obj_list = self.data:GetCrystalFbObjInfo()
    for _, obj_info in pairs(obj_list) do
        if FightConfig.CrystalObjType.Npc == obj_info.obj_type then
            local pos = SceneCtrl.Instance:CurScene():PosToClint(obj_info.pos_x, obj_info.pos_y)
            SceneLogic.Instance:AutoToPos(nil, {x = pos.x, y = pos.z},function ()
                self.data:AutoToNpc()
            end, false, false)
            break
        end
    end
end

function MainTopFightCrystal:OnClickGoBoss()
    local obj_list = self.data:GetCrystalFbObjInfo()
    for _, obj_info in pairs(obj_list) do
        if FightConfig.CrystalObjType.Boss == obj_info.obj_type then
            local pos = SceneCtrl.Instance:CurScene():PosToClint(obj_info.pos_x, obj_info.pos_y)
            SceneLogic.Instance:AutoToPos(nil, {x = pos.x, y = pos.z},function ()
                self.data:AutoToNpc()
            end, false, false)
            break
        end
    end
end

function MainTopFightCrystal:OnClickDetail()
    self.DetailObj:SetActive(not self.DetailObj.activeSelf)
    self:FlushBtnDetail()
end

function MainTopFightCrystal:FlushBtnDetail()
    local is_show = self.DetailObj.activeSelf
    self.BtnDetailImg.SpriteName = is_show and "ShouQi" or "ZhanKai"
    self.BtnDetail.text = is_show and self.language.close or self.language.open
end

MainTopFightCrystalItem = DeclareMono("MainTopFightCrystalItem", UIWidgetBaseItem)

function MainTopFightCrystalItem:MainTopFightCrystalItem()
    self.fight_data = FightCtrl.Instance:Data()
    self.language = Language.Fight
end

function MainTopFightCrystalItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.IconSp.SpriteName = data.icon
    self.Name.text = self.language.MenuDetailNames[data.icon] or self.data.name or self.language.FightMap.IconNames[data.icon]
    self.IsGet.text = FightConfig.CrystalObjType.Monster == self.data.obj_type and self.language.MenuGetState.kill or self.language.MenuGetState.get
    self.BtnGoObj:SetActive(not data.is_get)
    self.IsGet:SetObjActive(data.is_get)
    self.Line1Obj:SetActive(data.show_line1)
    self.Line2Obj:SetActive(data.show_line2)
end

function MainTopFightCrystalItem:OnClickGo()
    if self.data.is_get then return end
    local pos = SceneCtrl.Instance:CurScene():PosToClint(self.data.pos_x, self.data.pos_y)
    SceneLogic.Instance:AutoToPos(nil, {x = pos.x, y = pos.z},function ()
        self.fight_data:AutoToNpc()
    end, false, false)
end

MainTopLuckyMaze = MainTopLuckyMaze or DeclareMono("MainTopLuckyMaze",UIWFlushPanel)
function MainTopLuckyMaze:MainTopLuckyMaze()
    self.data = LuckyMazeData.Instance
	self.data_cares = {
        {data = self.data:GetSceneRewardInfo(), func = self.FlushPanel,init_call = true},
	}
	self.language = Language.Fight.CrystalDungenTop
end

function MainTopLuckyMaze:FlushPanel()
    if SceneData.Instance:GetMode() ~= SceneModeType.LuckyMaze then
        return
    end
    self.LayerState1:SetActive(true)
    local reward_info = self.data:GetSceneRewardInfo() or {}
    UH.SetText(self.LayerNum,string.format(self.language.layer,reward_info.layer or self.language.empty,reward_info.max_layer or self.language.empty))
    self:NumFlush(self.CoinNum, reward_info.coin_num_r or 0, reward_info.coin_num_max or 0,self.CoinEffect)
    self:NumFlush(self.BoxNum, reward_info.box_num_r or 0, reward_info.box_num_max or 0,self.BoxEffect)
    self:NumFlush(self.MonsterNum, reward_info.monster_num_r or 0, reward_info.monster_num_max or 0,self.MonsterEffect)
end

function MainTopLuckyMaze:NumFlush(text, num, num_max, effect)
    local show_effect = false
    num_max = num_max or 0
    num = num or 0
	if 0 == num_max then
		text.text = self.language.empty
	else
        if num_max == num then
            text.text = RichTextHelper.ColorStr(string.format(self.language.num, num, num_max), COLORSTR.Green10)
        else
            text.text = string.format(self.language.num, num, num_max)
            show_effect = true
        end
	end
    effect:SetActive(show_effect)
end

MainTopWeddingFB = MainTopWeddingFB or DeclareMono("MainTopWeddingFB", UIWFlushPanel)

function MainTopWeddingFB:MainTopWeddingFB()
    self.data = MarryCtrl.Instance:Data()
    self.language = Language.Marry.WeddingFBMenu

    self.data_cares = {
		{data = self.data.wedding_scene_info, func = self.FlushInfo},
        {data = self.data.wedding_fb_event, func = self.FlushStart},
    }
end

function MainTopWeddingFB:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_ct)
end

function MainTopWeddingFB:OnDisable()
    UIWFlushPanel.OnDisable(self)
    TimeHelper:CancelTimer(self.timer_ct)
end

function MainTopWeddingFB:FlushInfo()
    TimeHelper:CancelTimer(self.timer_ct)
	local info = self.data.wedding_scene_info
    if info.init then return end
    local link_end_time = info.link_end_time
    local co_next = MarryData.Instance.config_wedding_party[info.link_seq + 1]
    if link_end_time > TimeHelper.GetServerTime() then
        self.ShowObj:SetActive(true)
        self.timer_ct = TimeHelper:AddCountDownCT(function ()
            local rt = link_end_time - TimeHelper.GetServerTime()
            if rt < 11 and co_next then
                if self.data.ShowRiteEffect and MarryConfig.FBEventType.rite == co_next.link_seq then
                    self.data.ShowRiteEffect = false
                    SocietyData.Instance:PlayGiftEffect(2, 0)
                end
                self.ShowLinkObj:SetActive(true)
                self.ShowLinkName.text = string.format(self.language.NextLinkName, co_next.wedding_link)
                self.ShowLinkTime.text = string.format(self.language.NextLinkTime, math.floor(rt))
            else
                self.ShowLinkObj:SetActive(false)
            end
            -- local ft = TimeHelper.FormatDHMS(link_end_time - TimeHelper.GetServerTime())
		    -- self.RemainTime.text = string.format(self.language.TopRemainTime, ft.min, ft.s)
        end, function ()
            self.ShowObj:SetActive(false)
            self.ShowLinkObj:SetActive(false)
        end, link_end_time)
    else
        self.ShowObj:SetActive(false)
    end
    -- local co = MarryData.Instance.config_wedding_party[info.link_seq]
    -- self.TypeName.text = co and co.wedding_link or ""
end

function MainTopWeddingFB:FlushStart()
	TimeHelper:CancelTimer(self.timer_dt)
	local type = self.data.wedding_fb_event.type
	if MarryConfig.WeddingStartImg[type] then
        SocietyData.Instance:PlayGiftEffect(2, 0)
		self.StartImg.SpriteName = MarryConfig.WeddingStartImg[type]
		self.StartObj:SetActive(true)
		self.timer_dt = TimeHelper:AddDelayTimer(function ()
			self.StartObj:SetActive(false)
		end, 3)
	else
		self.StartObj:SetActive(false)
	end
end

function MainTopWeddingFB:OnClickTips()
    local info = self.data.wedding_scene_info
    if info.init then return end
    local co = MarryData.Instance.config_wedding_party[info.link_seq]
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[co and co.link_des or 1].desc})
end


MainViewTopPanel = MainViewTopPanel or DeclareMono("MainViewTopPanel", UIWFlushPanel)

function MainViewTopPanel:MainViewTopPanel()
    self.data_cares = {
        {data = ChiefElectionData.Instance.primary_data, func = self.FlushChiefPrimaryView, keys = {"match_state"}},
        {data = SceneData.Instance:Base(), func = self.FlushChiefPrimaryTime, init_call = false, keys = {"scene_mode"}},
	}
end

function MainViewTopPanel:FlushChiefPrimaryView()
    if SceneData.Instance:GetMode() ~= SceneModeType.ChiefPrimary then
        return
    end
    local match_state = ChiefElectionData.Instance:GetMatchState()
    self.ChiefPrimaryGame:SetActive(match_state == 2)
    if match_state == 2 then
        self.ChiefPrimaryTime:SetCallBack(function()self.ChiefPrimaryGame:SetActive(false)end)
        self.ChiefPrimaryTime:StampTime(ChiefElectionData.Instance:GetNextMatchStateTime(),TimeType.Type_Special_2)
        ViewMgr:OpenView(ChiefRankView)
    end
end

function MainViewTopPanel:FlushChiefPrimaryTime()
    if SceneData.Instance:GetMode() == SceneModeType.ChiefPrimary then
        return
    end
    if not ActivityData.IsOpen(ActType.ChiefElection) then
        return
    end
    if ChiefElectionData.Instance:GetMatchState() == 0 then
        local act_info = ActivityData.Instance:GetActivityInfo(ActType.ChiefElection)
        PublicPopupCtrl.Instance:ActTips(act_info,ChiefElectionData.Instance:GetNextMatchStateTime())
    end
end

MainTopMarryParty = MainTopMarryParty or DeclareMono("MainTopMarryParty", UIWFlushPanel)

function MainTopMarryParty:MainTopMarryParty()
    self.data = MarryPartyCtrl.Instance:Data()
    self.language = Language.MarryParty.Menu

    self.data_cares = {
		{data = self.data.marry_party_scene_info, func = self.FlushInfo},
    }
end

function MainTopMarryParty:FlushInfo()
    TimeHelper:CancelTimer(self.timer_ct)
	local info = self.data.marry_party_scene_info
    if info.init then return end
    local co = self.data.config_game_link[info.link_seq]
    self.NameShow.text = co and co.wedding_link or 1
end

function MainTopMarryParty:OnClickTips()
    local info = self.data.marry_party_scene_info
    if info.init then return end
    local co = self.data.config_game_link[info.link_seq]
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[co and co.link_des or 1].desc})
end