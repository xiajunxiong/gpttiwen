
FightSingleView = FightSingleView or DeclareView("FightSingleView", FightConfig.ResPath .. "fight_single", Mod.Fight.Team)
VIEW_DECLARE_LEVEL(FightSingleView,ViewLevel.L1)

function FightSingleView:FightSingleView()
	self.data = FightCtrl.Instance:Data()
	self.language = Language.Fight
end

function FightSingleView:LoadCallback()
	self.Board:SetData2(self:GetType(), self.language.FightTeam, self.data:GetFightBoardSingleTitle2())
end

FightSingleViewOffset = FightSingleViewOffset or DeclareMono("FightSingleViewOffset", UIWFlushPanel)

function FightSingleViewOffset:FightSingleViewOffset()
	self.data = FightCtrl.Instance:Data()
	self.language = Language.Team
end

function FightSingleViewOffset:Awake()
	UIWFlushPanel.Awake(self)

	self.remind_time = FightConfig.SINGLE_TOTAL_TIME
	self.timer_tt = TimeHelper:AddCountDownTT(BindTool.Bind(self.TimerCTUpdate, self), BindTool.Bind(self.TimerCTComplete, self), self.remind_time, 1)
	local beh_cfg = FightData.Instance:FightSingleBeh()
	if FB_TYPE.ADVANCED_FB == beh_cfg.param1 then  -- or FB_TYPE.MEDAL == beh_cfg.param1 then
		self.Consume:SetActive(false)
		self.DescObj:SetActive(true)
	elseif FB_TYPE.MEDAL == beh_cfg.param1 then
		local info = MedalTaskData.Instance:GetCurMedalDungeonInfo()
		if info == nil then  
			self.Consume:SetActive(false)
		else 
			local item = Item.New({item_id = info.consume_item})
			self.ConsumeCell:SetData(item)
			self.ConsumeTips1.text = string.format(Language.Fight.CrystalDungen.consume_tips1, 1, item:Name())
			self.ConsumeTips2.text = string.format(Language.Fight.CrystalDungen.consume_tips2, BagData.Instance:GetNumByItemId(info.consume_item))
		end 
	elseif FB_TYPE.CRYSTAL_FB == beh_cfg.param1 and 1 == (tonumber(beh_cfg.param3) or 0) then
		self.FightConsume:SetDataBySeq({seq = beh_cfg.param2})
		self.FightConsume:SetObjActive(true)
		self.Consume:SetActive(false)
	else
		local cur_sel_dungeon = self.data.cur_sel_dungeon
		self.ConsumeCell:SetData(cur_sel_dungeon.consume)
		self.ConsumeTips1.text = string.format(Language.Fight.CrystalDungen.consume_tips1, 1, cur_sel_dungeon.consume:Name())
		self.ConsumeTips2.text = string.format(Language.Fight.CrystalDungen.consume_tips2, BagData.Instance:GetNumByItemId(cur_sel_dungeon.consume.item_id))
	end
end

function FightSingleViewOffset:TimerCTUpdate()
	self.CountdownProgress:SetProgress(self.remind_time / FightConfig.SINGLE_TOTAL_TIME)
	self.CountdownNum.text = self.remind_time .. self.language.Second
	self.remind_time = self.remind_time - 1
end

function FightSingleViewOffset:TimerCTComplete()
	ViewMgr:CloseView(FightSingleView)
end

function FightSingleViewOffset:CloseView()
	ViewMgr:CloseView(FightSingleView)
end

function FightSingleViewOffset:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.timer_tt)
end

function FightSingleViewOffset:OnClickLeave()
	ViewMgr:CloseView(FightSingleView)
end

function FightSingleViewOffset:OnClickConfirm()
	local beh_cfg = FightData.Instance:FightSingleBeh()
	local param3 = tonumber(beh_cfg.param3) or 0
	if FB_TYPE.MEDAL == beh_cfg.param1 then
		local info = MedalTaskData.Instance:GetCurMedalDungeonInfo()
		if nil ~= info and 0 == BagData.Instance:GetNumByItemId(info.consume_item) then
			local item = Item.New({item_id = info.consume_item})
			PublicPopupCtrl.Instance:Center(string.format(Language.Fight.MedalConsumeTips, item:QuaName()))
			SceneLogic.Instance:AutoToNpc(Config.medaltask_scene_auto.other[1].npc_seq,nil,nil,true)
			ViewMgr:CloseView(FightSingleView)
			return
		end
	end
	if FB_TYPE.CRYSTAL_FB == beh_cfg.param1 then
		local co = FightData.Instance:GetFightCrystalConfigBySeq(beh_cfg.param2)
		param3 = BagData.Instance:GetNumByItemId(co.consume_item) > 0 and 0 or param3
	end
	SceneCtrl.Instance:RequestEnterFb(beh_cfg.param1,beh_cfg.param2,param3)
	ViewMgr:CloseView(FightSingleView)
end

FightSingleConsumeItem = FightSingleConsumeItem or DeclareMono("FightSingleConsumeItem")

function FightSingleConsumeItem:SetDataBySeq(data)
	local dungeon_info = FightData.Instance:GetDungeonInfoBySceneSeq(data.seq)
	if dungeon_info then
		local have_num = BagData.Instance:GetNumByItemId(dungeon_info.consume.item_id)
		if have_num > 0 then
			UH.SetIcon(self.ConsumeIcon, dungeon_info.consume:IconId())
			self.ConsumeName.text = dungeon_info.consume:Name()
			self.ConsumeVal.text = 1
		else
			UH.SpriteName(self.ConsumeIcon, FightConfig.TiLiZhi)
			self.ConsumeName.text = Language.Fight.Sweep.energy
			self.ConsumeVal.text= FightData.Instance:GetCrystalFbSweepUsePower()
		end
		self.FightReward:SetDataBySeq(data)
	end
end