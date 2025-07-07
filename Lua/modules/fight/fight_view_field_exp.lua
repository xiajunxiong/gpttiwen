
FightViewFieldExp = FightViewFieldExp or DeclareMono("FightViewFieldExp", UIWFlushPanel)

function FightViewFieldExp:FightViewFieldExp()
	self.data = FightCtrl.Instance:Data()
	self.bag_data = BagData.Instance
	self.language = Language.Fight
	self.limitExp = RoleData.Instance:GetRoleLevel() * FightData.GUA_JI_LIMIT_PARAM
	self:FlushExpInfo()
	self.data_cares = {
		{data = self.data.cur_sel_field, func = self.FlushDetail, init_call = false},
		{data = self.data.meet_monster_flag, func = self.FlushMeetMonster},
		{data = self.data.cur_sel_skills , func = self.FlushCurSkills},
		-- {data = self.bag_data:GetColumnGrids(ItemColumnType.Item), func = self.FlushClockIn},
		{data = self.data.exp_crystal_info, func = self.FlushClockIn},
		-- {data = self.data.patrol_task_info, func = self.FlushPatrolTask},
		-- {data = RoleData.Instance:GetBaseData(), func = self.FlushPatrolTask, keys = {"level"}, init_call = false},
		{data = self.data.sc_auto_seal_info, func = self.FlushAutoSeal, init_call = false},
		{data = self.data.today_add_guaji_exp, func = self.FlushExpInfo, init_call = false},
	}
	TaskCtrl.Instance:SendTaskPatrol(0)
	GuideManager.Instance:RegisterGetGuideUi(
        "AutoSealBtn",
        function()
           return self.AutoSealToggle,function ()
                self.AutoSealToggle.isOn = true
				self.data.is_guide_seal = true
		   end
        end
    )
end

--每日挂机经验值改变
function FightViewFieldExp:FlushExpInfo()
	local exp=  self.data.today_add_guaji_exp.exp
	local total_exp = Config.sundries_auto.other[1].guaji_points
	local value= exp/total_exp--self.limitExp
	self.ProPld:SetProgress(value)
	self.ProPld:SetText("<color=#"..COLORSTR.Red9 ..">"..exp.."</color>" .."/"..total_exp)--self.limitExp )
	local per=math.floor(value*100) 
	local color = value < 0.7 and COLORSTR.Red13 or (math.floor( value) >= 1 and COLORSTR.Red10 or COLORSTR.Red16)
	UH.SetText(self.TextPldPer, "<color=#"..color..">"..per.."%".."</color>")
end

function FightViewFieldExp:Awake()
	UIWFlushPanel.Awake(self)

	self.timer_dt = TimeHelper:AddDelayTimer(function ()
		FightViewFieldExp.SkillShowed = true
		self.SkillList:SetObjActive(true)
	end, FightViewFieldExp.SkillShowed and 0.4 or 0.7)
	UH.SetText(self.AutoTipText, Config.language_cfg_auto.textdesc[353].desc)
end

function FightViewFieldExp:OnDestroy()
	UIWFlushPanel.OnDestroy(self)

	TimeHelper:CancelTimer(self.timer_dt)
end

function FightViewFieldExp:OnClickAutoTips()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[354].desc})
end

function FightViewFieldExp:FlushDetail()
	local cur_sel_field = self.data.cur_sel_field
	-- self.Title.text = cur_sel_field._scene.name
	self.Monsters:SetData(cur_sel_field.monsters)
	self.Rewards:SetData(cur_sel_field.rewards)
	self.BgMapSp:SetByPath(string.format(FightConfig.PngPath, cur_sel_field.bg_map))
	self:FlushAutoSeal()
end

function FightViewFieldExp:FlushMeetMonster()
	self.MeetMonster.isOn = self.data.meet_monster_flag.flag ~= 0
end

function FightViewFieldExp:FlushClockIn()
	-- local list = self.bag_data:GetColumnGrids(ItemColumnType.Item)
	-- local item = list[0]
	-- if item then
	-- 	if item.item_type == ItemType.ExpCrystal then
	-- 		value = item.param.is_turn_on == 1
	-- 	end
	-- end
	local value = self.data.exp_crystal_info.is_open
	self.ClockInImage:SetActive(value)
	self.ClockInText.text = string.format(self.language.ClockIn, value and COLORSTR.Yellow5 or COLORSTR.Yellow14)
	local exp_crystal_count = self.data.exp_crystal_info.count
	self.ExpCrystalNum.text = string.format(self.language.DoubleEXP, exp_crystal_count) .. ((1080 == exp_crystal_count) and RichTextHelper.ColorStr(self.language.DoubleEXPFull, COLORSTR.Red7) or "")
	-- self.FullObj:SetActive(1080 == exp_crystal_count)
end

-- function FightViewFieldExp:FlushPatrolTask()
-- 	local value = FunOpen.Instance:GetFunIsOpened(Mod.Fight.PatrolTask)
-- 	self.PatrolTaskObj:SetActive(value)
-- 	self.PatrolTaskOpen.text = value and "" or string.format(self.language.PatrolTaskOpenTips, FunOpen.Instance:GetFunOpenLevel(Mod.Fight.PatrolTask))
-- 	if not value then return end
-- 	local total = self.data:PatrolTaskTotal()
-- 	local info = self.data.patrol_task_info
-- 	self.TaskProgress.text = info.task_progress .. "/" .. total
-- 	-- self.BtnAcceptObj:SetActive(0 == info.today_flag and 0 == info.today_reward)
-- 	self.BtnGetObj:SetActive(1 == info.task_status)
-- 	self.TaskComplete:SetActive(2 == info.task_status)

-- 	self.red_point_task = self.red_point_task or UiRedPoint.New(self.BtnGetObj, Vector3.New(55,15,0))
-- 	self.red_point_task:SetNum(1 == info.task_status and 1 or 0)
-- end

function FightViewFieldExp:FlushAutoSeal()
	local pet_list = self.data:GetCurSceneCanSealPets()
	if pet_list == nil or next(pet_list) == nil then
		self.AutoSealToggle.gameObject:SetActive(false)
		self.AutoProgressObj:SetActive(false)
		return
	end
	if self.data:IsSetAutoSealPet() then
		if self.data.cur_sel_field.scene_id == self.data:GetSealSceneId() then
			if self.AutoSealToggle.isOn == false then
				self.AutoSealToggle.gameObject:SetActive(false)
				self.AutoProgressObj:SetActive(true)
			end
			return
		else
			self.AutoSealToggle.gameObject:SetActive(true)
			self.AutoProgressObj:SetActive(false)
			self.AutoSealToggle.isOn = false
			return
		end
	else
		self.AutoSealToggle.gameObject:SetActive(true)
		self.AutoProgressObj:SetActive(false)
	end 
    self.AutoSealToggle.isOn = self.data:GetAutoSealFlag()
end

function FightViewFieldExp:OnClickPatrolGetReward()
	TaskCtrl.Instance:SendTaskPatrol(2)
end

function FightViewFieldExp:FlushCurSkills()
	self.SkillList:SetData(self.data.cur_sel_skills)
end

function FightViewFieldExp:OnClickMeetMonster()
	local flag = self.MeetMonster.isOn and 1 or 0
	RoleSettingCtrl.Instance:SetEncounterMineMonsterFlag(flag)
end

function FightViewFieldExp:OnClickClockIn()
	-- local list = self.bag_data:GetColumnGrids(ItemColumnType.Item)
	-- local item = list[0]
	-- if item then
	-- 	if item.item_type == ItemType.ExpCrystal then
	-- 		BagCtrl.Instance:SendUseItem(
	-- 			{
	-- 				column = ItemColumnType.Item,
	-- 				index = item.index,
	-- 				num = 1,
	-- 			}
	-- 		)
	-- 	end
	-- end
	FightCtrl.Instance:SendExpCrystalOp()
end

function FightViewFieldExp:OnClickFight()
	if RoleData.Instance:GetRoleLevel() < self.data.cur_sel_field.level_min then
		PublicPopupCtrl.Instance:Center(ErrorInfo[506])
		return
	end
	if nil ~= self.data.cur_sel_field.click_handle then
		self.data.cur_sel_field.click_handle()
		if self.data:IsSetAutoSealPet() then
			self.data:SetSealing(true)
		end
	end
end

function FightViewFieldExp:OnClickTips()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[24].desc})
end

function FightViewFieldExp:OnClickDesc()
	-- ViewMgr:OpenView(GuaJiPlDescView)
	PublicPopupCtrl.Instance:HelpTip(418)
end


--勾选自动封印宠物事件
function FightViewFieldExp:OnAutoSealClick()
	--组队不生效
	if TeamData.Instance:InTeam() then
		PublicPopupCtrl.Instance:Center(Language.Fight.Seal.TeamTip)
		self.data:SetAutoSealFlag(false)
		self.AutoSealToggle.isOn = false
		return
	end
	if self.data:IsSetAutoSealPet() then
		if self.data.cur_sel_field.scene_id ~= self.data:GetSealSceneId() then
			local scene_id = self.data:GetSealSceneId()
			local param_t = {
		        content = string.format(Language.Fight.Seal.SelectTip,MapData.Instance:GetMapShowData(scene_id).name),
		        confirm = {
		            func =  function()
		                ViewMgr:CloseView(DialogTipsView)
		                FightCtrl.Instance:ClearAutoSealData()
		                self.data:SetAutoSealFlag(true)
						ViewMgr:OpenView(FightAutoSealSetView)
		            end 
		        },
		        cancel = {
		        	func =  function()
		                ViewMgr:CloseView(DialogTipsView)
		                self.data:SetAutoSealFlag(false)
						self.AutoSealToggle.isOn = false
		            end 
		        }
		    }
		    PublicPopupCtrl.Instance:DialogTips(param_t)
		end	
		return
	end
	self.data:SetAutoSealFlag(true)
	ViewMgr:OpenView(FightAutoSealSetView)
end

--自动封印的勾选的勾勾取消事件
function FightViewFieldExp:OnCancelAutoSealClick()
	if not self.data:IsSetAutoSealPet() then
		return
	end
	local scene_id = self.data:GetSealSceneId()
	local param_t = {
        content = string.format(Language.Fight.Seal.SelectTip2,MapData.Instance:GetMapShowData(scene_id).name),
        confirm = {
            func =  function()
                ViewMgr:CloseView(DialogTipsView)
                FightCtrl.Instance:ClearAutoSealData()
                self.data:SetAutoSealFlag(false)
				self.AutoSealToggle.isOn = false
            end 
        },
        cancel = {
        	func =  function()
                ViewMgr:CloseView(DialogTipsView)
                self.data:SetAutoSealFlag(true)
				self.AutoSealToggle.isOn = true
            end 
        }
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)
end
function FightViewFieldExp:OnClickTip()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[333].desc})
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end
--自动封印查看进度事件
function FightViewFieldExp:OnLookAutoSealClick()
	ViewMgr:OpenView(FightAutoSealInfoView,{open_info_type = FightData.OPEN_AUTO_SEAL_INFO_TYPE.LOOK})
end

FightViewFieldExpItem = DeclareMono("FightViewFieldExpItem", UIWidgetBaseItem)

function FightViewFieldExpItem:FightViewFieldExpItem()
	self.fight_data = FightCtrl.Instance:Data()
end

function FightViewFieldExpItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Name.text = data._scene.name
	self.Level.text = string.format(Language.Fight.Level, data.level_min, data.level_max)
	self.CurObj:SetActive(data.value_cur)
	self.RecoObj:SetActive(data.value_reco)
	UH.SetIcon(self.MapIcon, data.map_id, ICON_TYPE.MAP)
	self.MaskObj:SetActive(RoleData.Instance:GetRoleLevel() < data.level_min)
end

function FightViewFieldExpItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function FightViewFieldExpItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
	if selected then
		self.fight_data.cur_sel_field:Set(self.data)
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
	end
end

FightViewFieldExpRewardItem = DeclareMono("FightViewFieldExpRewardItem", UIWidgetBaseItem)
function FightViewFieldExpRewardItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Cell:SetData(data)
	local rate = data.rate and tonumber(data.rate) or 0
	if rate == 0 then
		UH.SetText(self.Rate, "")
	else
		UH.SetText(self.Rate,Language.PetRefining.GaiLv.. tostring(rate).."%")
	end
	self.TraObj:SetActive(data.tradable)
end
FightViewFieldSkillItem = DeclareMono("FightViewFieldSkillItem", UIWidgetBaseItem)

function FightViewFieldSkillItem:FightViewFieldSkillItem()
	self.fight_data = FightCtrl.Instance:Data()
end

function FightViewFieldSkillItem:SetData(data)
	if nil == data or nil == data.detail then return end
	UIWidgetBaseItem.SetData(self, data)
	local detail = data.detail
	local seq = 1 == self.data.auto_seq and "first_" or "second_"
	local move_type = self.fight_data.skill_protocol[seq.. "move_type"]
	self.Name.text = detail.skill_name
	UH.SetIcon(self.IconSSp, detail.icon_id, ICON_TYPE.SKILL)
	local show_pet_icon = PetData.Instance:GetFightIndex() > -1
	self.IconTSp.SpriteName = (show_pet_icon and 2 == self.data.auto_seq) and FightConfig.IconSpNamePet or FightConfig.IconSpNameRole

	self.SkillCorner.text = detail.corner_icon or ""
	self.SkillCornerObj:SetActive(not StringIsEmpty(detail.corner_icon))
	self.AIObj:SetActive(BATTLE_MOVE_TYPE.AI_SKILL == move_type)

	if self.effect_handle ~= nil then
        self.EffectTool:Stop(self.effect_handle)
        self.effect_handle = nil
    end
    
    if VersionTag.HasTag(VersionTag.ClipAnima2D) and not StringIsEmpty(data.detail.asset_id) then
	    self.effect_handle = self.EffectTool:PlayByEffectId(tonumber(data.detail.asset_id), self.EffectParent)
	end
end

function FightViewFieldSkillItem:OnClickItem()
	self.fight_data.cur_auto_seq = self.data.auto_seq
	ViewMgr:OpenView(FightSkillView)
end