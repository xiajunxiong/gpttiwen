
FightMirageTerritoryLevelView = FightMirageTerritoryLevelView or DeclareView("FightMirageTerritoryLevelView", "fight_mirage_territory/mirage_territory_level")
VIEW_DECLARE_LEVEL(FightMirageTerritoryLevelView,ViewLevel.L1)

function FightMirageTerritoryLevelView:FightMirageTerritoryLevelView()
	self.Data = FightMirageTerritoryData.Instance

	self.btn_cache = {}
	self.btn_effect = {}
	self.is_click_btn = true
	self.Data.on_sweep = false
	--self.Data:SetCurLevelRange()
	--self.Data.cur_layer_data.sel_level = self.Data.cur_layer_data.pass_level % 10
end

function FightMirageTerritoryLevelView:LoadCallback()
	self.handle = self.Data.cur_layer_data:Care(BindTool.Bind(self.FlushPanel, self))
	self.clear_handle = self.Data.certain_dispatch:Care(BindTool.Bind(self.SweepAnim, self))
	self.MirageReward:SetObjActive(false)
	local data = self.Data.cur_area_data
	UH.SetText(self.Name,data.area_name)
	UH.SetText(self.ProfTxt1,Language.MirageTerritory.Profession[data.job1])
	UH.SetText(self.ProfTxt2,Language.MirageTerritory.Profession[data.job2])

	UH.SpriteName(self.ProfIcon1,RoleData.GetProfSp(data.job1))
	UH.SpriteName(self.ProfIcon2,RoleData.GetProfSp(data.job2))

	--self.ClearList:SetData(DataHelper.FormatItemList(data.reward_list or {})) 屏蔽

	-- for i = 1,self.AreaBgs:Length() do
	-- 	self.AreaBgs[i]:SetActive(i == data.seq + 1)
	-- end
	--请求下发布阵消息
	FightMirageTerritoryCtrl.Instance:SendMirageTerritoryInfoReq(MirageTerritoryConfig.ReqType.Formation,self.Data.cur_area_data.seq)
	--self.Data:SetCurLevelRange()

	
	self:FlushPanel()
	-- if self.Data.is_rotate then
	-- 	self.Data:AddAreaLayer(-10)
	-- 	self:OnClickRotateUp()
	-- 	self.Data.is_rotate = false
	-- end
end

function FightMirageTerritoryLevelView:FlushPos()
	if self.Data.cur_layer_data.layer < 0 then
		return
	end
	local page_index = math.floor((self.Data.cur_layer_data.layer + 10) / 10)
	--LogError("取值",page_index)
	if page_index > 5 then 
		page_index = 5
	end 
	for i = 1, self.LinesGourp:Length() do
		self.LinesGourp[i]:SetActive(i == page_index)
	end
	for i = 1 , self.LevelItems:Length() do
		local pos_data = FightMirageTerritoryData.GetPosData(page_index, i).ItemPos
		UH.LocalPos(self.LevelItems[i].ItemRect, Vector3.New(pos_data.x,pos_data.y,0))
	end
	for i = 1, self.DotRects:Length()do
		local pos_data = FightMirageTerritoryData.GetPosData(page_index, i).DotPos
		UH.LocalPos(self.DotRects[i], Vector3.New(pos_data.x,pos_data.y,0))
	end
end

function FightMirageTerritoryLevelView:CloseCallback()
	TimeHelper:CancelTimer(self.sweep_cache)
	self.Data.cur_layer_data.sel_level = 0
    self.Data.cur_layer_data:Uncare(self.handle)
	self.Data.certain_dispatch:Uncare(self.clear_handle)

	for i,v in pairs(self.btn_cache) do
		if self.btn_cache[i] then
			TimeHelper:CancelTimer(self.btn_cache[i])
			self.btn_cache[i] = nil
		end
	end
	if not ViewMgr:IsOpen(FightMirageTerritoryView) then
		ViewMgr:OpenView(FightMirageTerritoryView)
	end
end

function FightMirageTerritoryLevelView:OnClickReward()
	self.MirageReward:SetObjActive(true)
end

function FightMirageTerritoryLevelView:OnClickFormation()
	ViewMgr:OpenView(FormationPartnerView)
end

function FightMirageTerritoryLevelView:FlushPanel()
	local data = self.Data:GetClearReward()[self.Data.cur_layer_data.layer + self.Data.cur_layer_data.sel_level]
	--LogError("选择的index",self.Data.cur_layer_data.sel_level)
	self:SetVideoView(data)
	
	--属性
    if data.element == 0 then
		UH.SpriteName(self.ElementSp, "HunDunShuXing")
	else
		UH.SpriteName(self.ElementSp,ElementSpriteName[MirageTerritoryConfig.ElementType[data.element]])
	end

	self:FlushPos()
	self.DispatchObj:SetActive(self.Data.dispatch_data.pass_area == self.Data.cur_area_data.seq 
		and self.Data.dispatch_data.pass_seq == (self.Data.cur_layer_data.layer + self.Data.cur_layer_data.sel_level)
		and self.Data:GetDispatchTimes() <= 0
	)
	--self.ClearTipObj:SetActive(self.Data.cur_layer_data.is_clear) 屏蔽
	if self.Data.cur_layer_data.pass_level > 0 then
		UH.SetText(self.DescTxt,string.format(Language.MirageTerritory.ClearTip,DataHelper.GetDaXie(self.Data.cur_layer_data.pass_level)))
	else
		UH.SetText(self.DescTxt, Language.MirageTerritory.CanClearTip)
	end
	
	self.PassMark:SetActive(self.Data.cur_layer_data.layer + self.Data.cur_layer_data.sel_level <= self.Data.cur_layer_data.pass_level)

	UH.SetText(self.TabTxt,string.format(Language.MirageTerritory.RankType,DataHelper.GetDaXie(data.num),data.name))
	self.RewardList:SetData(DataHelper.FormatItemList(self.Data:GetRewardData(data.reward_id)))
	self.DispatchList:SetData(DataHelper.FormatItemList(self.Data:GetPartnerDispatchShow(data.reward_id)))
	UH.SetInter(self.InterClear,self.Data.cur_layer_data.is_clear)

	for i = 1 , self.LevelItems:Length() do
		self.LevelItems[i]:SetData(self.Data:GetClearReward()[self.Data.cur_layer_data.layer + i - 1])
	end

	self:FlushDot()

	local state = 1
	if self.Data.cur_layer_data.layer + self.Data.cur_layer_data.sel_level < self.Data.cur_layer_data.pass_level + 1 then
		state = 2
	end
	if self.Data.cur_layer_data.layer + self.Data.cur_layer_data.sel_level > self.Data.cur_layer_data.pass_level + 1 then
		state = 3
	end
	for i = 1,self.ChallengeImages:Length() do
		self.ChallengeImages[i]:SetActive(i == state)
	end

	self:OnClickLevel(self.Data.cur_layer_data.sel_level)
	self.BtnDown:SetActive(self.Data.cur_layer_data.layer < self.Data.cur_area_data.all_num - 10)
	self.BtnUp:SetActive(self.Data.cur_layer_data.layer > 1)

	UH.SetInter(self.InterChallenge,self.Data.cur_layer_data.layer + self.Data.cur_layer_data.sel_level <= self.Data.cur_layer_data.pass_level + 1)
end

function FightMirageTerritoryLevelView:FlushDot(force_show)
	for i = 1 , self.DotLocks:Length() do
		-- self.DotLocks[i]:SetActive(force_show or not self.Data:GetDotState(i))
		self.DotNotLocks[i]:SetActive(force_show or self.Data:GetDotState(i))
	end
end

function FightMirageTerritoryLevelView:OnClickClear()
	if not self.Data.cur_layer_data.is_clear then
		PublicPopupCtrl.Instance:Center(Language.MirageTerritory.NotClearTips[
			self.Data.dispatch_data.all_num > self.Data.dispatch_data.num and 2 or 1
		])
		return
	end
	ViewMgr:OpenView(PartnerDispatchView)
end

function FightMirageTerritoryLevelView:OnClickChallenge()
	if self.Data.Instance.cur_layer_data.layer + self.Data.cur_layer_data.sel_level <= self.Data.cur_layer_data.pass_level then
		PublicPopupCtrl.Instance:Center(Language.MirageTerritory.PassTip)
		return 
	end
	if self.Data:IsNeedFormation() then
		self:OnClickFormation()
		return
	end

	if self.Data:IsCanFormation() then
		--提示确认框
		PublicPopupCtrl.Instance:DialogTips{
			content = Language.MirageTerritory.GoToFormationTip,
			confirm = {
				name = Language.MirageTerritory.Confirm,
				func = function()
					FightMirageTerritoryData.Instance:GoToChallenge()
					ViewMgr:CloseView(DialogTipsView)
				end
			},
			cancel = {
				name = Language.MirageTerritory.Cancel,
				func = function()
					self:OnClickFormation()
					ViewMgr:CloseView(DialogTipsView)
				end
			}
		}
		return
	end
	FightMirageTerritoryData.Instance:GoToChallenge()
end

function FightMirageTerritoryLevelView:OnClickLevel(value)
	if self.Data.cur_layer_data.layer + value > self.Data.cur_area_data.all_num or self.is_click_btn == false or self.is_stop_anim == false or self.Data.on_sweep then
		return
	end
	self:CloseBtnEffect()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
	self.Data.cur_layer_data.sel_level = value
	self.btn_effect[value] = self.EffectTool:Play(3165040, self.EffectList[value + 1])
end

function FightMirageTerritoryLevelView:CloseBtnEffect()
	for i,v in pairs(self.btn_effect) do
		if self.btn_effect[i] then
			-- self.EffectList[i + 1]:Stop(self.btn_effect[i])
			self.EffectTool:Stop(self.btn_effect[i])
			self.btn_effect[i] = nil
		end
	end
end

--===============================录像相关===============================
function FightMirageTerritoryLevelView:SetVideoView(data)
    local value = data.monster_group or 0
	local video_config = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY,value)
	if video_config then
        BattleCtrl.BattleReportSimpleInfo(video_config.monster_group_id, 0)
	end
end

function FightMirageTerritoryLevelView:OnClickPlay()
	local data = self.Data:GetClearReward()[self.Data.cur_layer_data.layer + self.Data.cur_layer_data.sel_level]
	local value = data.monster_group or 0
	local video_config = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY,value)
	if video_config ~= nil then
		BattleVideo.Instance:PlayVideoCfg(video_config)
		BattleCtrl.BattleReportDetailInfo(video_config.monster_group_id)
	else
		PublicPopupCtrl.Instance:Center(Language.MirageTerritory.NotHasPlayVideo)
	end
end

function FightMirageTerritoryLevelView:OnClickRotateDown()
	if FightMirageTerritoryData.Instance.cur_layer_data.layer == FightMirageTerritoryData.Instance.cur_area_data.all_num - 10 then
		return
	end
	self.is_add_layer = true
	self.Data:AddAreaLayer(10)
	if self.Data.cur_layer_data.layer < self.Data.cur_layer_data.pass_level and self.Data.cur_layer_data.layer + 10 > self.Data.cur_layer_data.pass_level then
		if self.Data.cur_layer_data.pass_level >= self.Data.cur_area_data.all_num then
			self.Data.cur_layer_data.sel_level = self.Data.cur_layer_data.pass_level - self.Data.cur_layer_data.layer
		else
			self.Data.cur_layer_data.sel_level = self.Data.cur_layer_data.pass_level - self.Data.cur_layer_data.layer + 1
		end
	else
		self.Data.cur_layer_data.sel_level = 0
	end
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL1)
	self:CloseBtnEffect()
	-- self:FlushPanel()

end
function FightMirageTerritoryLevelView:OnClickRotateUp()
	if FightMirageTerritoryData.Instance.cur_layer_data.layer == 1 then
		return
	end
	self.is_add_layer = false
	self.Data:AddAreaLayer(-10)
	if self.Data.cur_layer_data.layer < self.Data.cur_layer_data.pass_level and self.Data.cur_layer_data.layer + 10 > self.Data.cur_layer_data.pass_level then
		if self.Data.cur_layer_data.pass_level >= self.Data.cur_area_data.all_num then
			self.Data.cur_layer_data.sel_level = self.Data.cur_layer_data.pass_level - self.Data.cur_layer_data.layer
		else
			self.Data.cur_layer_data.sel_level = self.Data.cur_layer_data.pass_level - self.Data.cur_layer_data.layer + 1
		end
	else
		self.Data.cur_layer_data.sel_level = 0
	end
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL1)
	self:CloseBtnEffect()
	-- self:FlushPanel()
end

function FightMirageTerritoryLevelView:OnClickCloseTip()
	self.ClearTipObj:SetActive(false)
end

function FightMirageTerritoryLevelView:OnClickClose()
	ViewMgr:CloseView(FightMirageTerritoryLevelView)
end

----------扫荡动画--------
function FightMirageTerritoryLevelView:SweepAnim()
	self.Data.on_sweep = true
	self.DotSweepAnim:Play()
	self.LineSweepAnim[math.floor((self.Data.cur_layer_data.layer + 10) / 10)]:Play()
	self:CloseBtnEffect()

	self:FlushDot(true)
	AudioMgr:PlayEffect(AudioType.UI, "yunwuzhuanchang")
	for i = 1,self.SweepObjs:Length() do
		self.SweepObjs[i]:SetActive(i >= 3)
	end

	for i = 1 , self.LevelItems:Length() do
		self.LevelItems[i]:SetData(false)
		self.LevelItems[i]:SetObjActive(false)
	end

	self.prog_val = 0
	UH.LocalPos(self.LevelRect,Vector3.New(220,47,0))
	self.sweep_cache = TimeHelper:AddRunTimer(BindTool.Bind(self.SweepProgress,self),0.03)
end

function FightMirageTerritoryLevelView:SweepProgress()
	self.prog_val = self.prog_val + 2.5
	local value = self.prog_val / 300
	if value >= 1 then
		value = 1
		self.Data.on_sweep = false
		if self.sweep_cache then
			TimeHelper:CancelTimer(self.sweep_cache)
			self.sweep_cache = nil
		end

		for i = 1,self.SweepObjs:Length() do
			self.SweepObjs[i]:SetActive(i < 3)
		end

		--for i = 1 , self.LevelItems:Length() do
		--	self.LevelItems[i]:SetObjActive(true)
		--end
		--self.Data:SetCurLevelRange()
		UH.LocalPos(self.LevelRect,Vector3.zero)
		FightMirageTerritoryData.Instance:SendPartnerDispatch()
	end
	self.Progress.value = value
	UH.SetText(self.ProgressTxt,(math.floor(value * 100)).."%")
end
----------------------------------------------------------
MirageTerritoryLevelBg = MirageTerritoryLevelBg or DeclareMono("MirageTerritoryLevelBg",UIWFlushPanel)
function MirageTerritoryLevelBg:MirageTerritoryLevelBg()
	self.Data = FightMirageTerritoryData.Instance
    self.data_cares = {
        {data = self.Data.cur_layer_data, func = self.FlushBgImg, keys = {"seq"}},
    }
	self.is_load = false
end

function MirageTerritoryLevelBg:FlushBgImg()
	for i = 1,self.AreaBgs:Length() do
		self.AreaBgs[i]:SetActive(i == self.Data.cur_layer_data.seq + 1)
	end
    local bg_path = string.format("ui/textures/mirage_territory/%sBeiJing.png", self.Data:GetBgName())
    ResMgr:LoadObject(bg_path,function(tex)
        if not self:Destroyed() then
            if self.RawImage then
				self.RawImage.enabled = true
                self.RawImage.texture = tex
                self.is_load = true
            end
        end
    end,false)
end

function MirageTerritoryLevelBg:IsLoaded()
    return self.is_load
end

-----------------------------------
MirageTerritoryLevelLineGroupPanel = MirageTerritoryLevelLineGroupPanel or DeclareMono("MirageTerritoryLevelLineGroupPanel",UIWFlushPanel)
function MirageTerritoryLevelLineGroupPanel:MirageTerritoryLevelLineGroupPanel()
	self.Data = FightMirageTerritoryData.Instance
	self.data_cares = {
		{data = self.Data.cur_layer_data, func = self.FlushLines, keys = {"seq","layer"}}
	}
end

function MirageTerritoryLevelLineGroupPanel:FlushLines()
	for i = 1, self.Lines:Length() do
		self.Lines[i]:SetActive(self.Data:GetDotState(i))
	end
end

--------------扫荡奖励---------------
MirageTerritoryRewardPanel = MirageTerritoryRewardPanel or DeclareMono("MirageTerritoryRewardPanel", UIWFlushPanel)
function MirageTerritoryRewardPanel:MirageTerritoryRewardPanel()
	self.Data = FightMirageTerritoryData.Instance
    self.data_cares = {
        {data = self.Data.cur_layer_data,func = self.FlushLayer,init_call = true},
    }
	self.List:SetData(FightMirageTerritoryData.Instance:GetClearReward())
end
function MirageTerritoryRewardPanel:FlushLayer()

end
function MirageTerritoryRewardPanel:OnClickClose()
	self:SetObjActive(false)
end

--------------奖励---------------
MirageTerritoryRewardItem = MirageTerritoryRewardItem or DeclareMono("MirageTerritoryRewardItem", UIWidgetBaseItem)
function MirageTerritoryRewardItem:SetData(data)
	UH.SetText(self.Name,data.name)
	UH.SetText(self.Num,string.format(Language.MirageTerritory.ShowType,DataHelper.GetDaXie(data.num)))
	self.List:SetData(DataHelper.FormatItemList(FightMirageTerritoryData.Instance:GetSweepRewardData(data.reward_id)))
	UIWidgetBaseItem.SetData(self, data)
end

--------------关卡---------------
MirageTerritoryLevelItem = MirageTerritoryLevelItem or DeclareMono("MirageTerritoryLevelItem", UIWidgetBaseItem)
function MirageTerritoryLevelItem:SetData(data)
	self:SetObjActive(data ~= nil)
	if not data then
		if data ~= nil then
			self:FlushDot(data)
		end
		return
	end
	local info = FightMirageTerritoryData.Instance.cur_layer_data
    local state = 1 -- 1解锁 2选择 3 未解锁
	if info.pass_level + 1 < data.num then --未解锁
		state = 3
	end
	if info.layer + info.sel_level == data.num then --选中的关卡
		state = 2
	end

	self:FlushDot(info.pass_level + 1 < data.num)

	UH.SpriteName(self.IconSp,MirageTerritoryConfig.LevelSprite[state])
	UH.SetText(self.Name,string.format(Language.MirageTerritory.LevelState[state <= 2 and 1 or 2],data.name))
	self.Lock:SetActive(data.num > info.pass_level + 1)
	UIWidgetBaseItem.SetData(self, data)
end
function MirageTerritoryLevelItem:FlushDot(state)
	-- for i = 1,self.ChildsLock:Length() do
	-- 	self.ChildsLock[i]:SetActive(state)
	-- end	
	-- for i = 1,self.Childs:Length() do
	-- 	self.Childs[i]:SetActive(not state)
	-- end
end

function MirageTerritoryLevelItem:OnClickItem()
	if FightMirageTerritoryData.Instance.on_sweep then
		return
	end
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
	FightMirageTerritoryData.Instance.cur_layer_data.sel_level = self.data.num - FightMirageTerritoryData.Instance.cur_layer_data.layer
end