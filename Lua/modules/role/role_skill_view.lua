RoleViewSkill = RoleViewSkill or DeclareMono("RoleViewSkill",UIWFlushPanel)
-- SkillName = Text
-- ButtonNormalGreen = GameObject
-- BtnLearn = GameObject
-- LearnCost = Text
-- LeftSkillIconList = UIWidgetList
-- LeftSkillToggleGroup = ToggleGroup
-- CanLearnObj = GameObject
-- CanLearnTxt = Text
-- RedBg = GameObject
-- GreenBg = GameObject
-- SkillInfoList = UIWidgetList
function RoleViewSkill:RoleViewSkill()
	self.data_cares = {
		{data = RoleCtrl.Instance.skill_data.smart_data, func = self.FlushLeftSkillIconList},
		{data = RoleCtrl.Instance.skill_data.change_data, func = self.FlushLeftSkillIconList,keys = {"change_level","common_use_change","skill_page"}},
		{data = RoleCtrl.Instance.skill_data.change_data, func = self.FlushRightSkillInfoList,keys = {"click_skill_id","change","change_level"}, init_call = false},
		{data = RoleCtrl.Instance.skill_data.change_data, func = self.onDataFlush,keys = {"click_skill_id","change_level","common_use_change"}},
		{data = RoleCtrl.Instance.skill_data.change_data, func = self.ChangeRightType ,keys = {"rightType"}},
		{data = RoleSkillData.Instance.current_train_data, func = self.FlushLeftSkillIconList, keys = {"level"}},
		{data = RoleData.Instance:GetCurrencyData(), keys = {CurrencyType.CoinBind}, func = self.FlushLeftSkillIconList, init_call = false},
	}

	self.cur_show_skill_id = 0
	self.cur_show_skill_lv = 0

	GuideManager.Instance:RegisterGetGuideUi("LSSecondSkill", function ()
		if self.LeftSkillIconList.item_list[1] ~= nil then
			local item_list = self.LeftSkillIconList.item_list[1].grid_par.item_list
			for i,v in ipairs(item_list) do
				if v.InteractorHandler.Interactable == false 
				and v:GetData().skill_id == GuideData.Instance:LearnSkillId() then
					return self.LeftSkillIconList.item_list[1].grid_par.item_list[i]
				end
			end
			--保险
			for i,v in ipairs(item_list) do
				if v.InteractorHandler.Interactable == false then
					return self.LeftSkillIconList.item_list[1].grid_par.item_list[i]
				end
			end
		end
		return nil
	end)

	GuideManager.Instance:RegisterGetGuideUi("LSSpecialSkill", function ()
		if self.LeftSkillIconList.item_list[1] ~= nil then
			local item_list = self.LeftSkillIconList.item_list[1].grid_par.item_list
			for i,v in ipairs(item_list) do
				if v:GetData().skill_id == GuideData.Instance:GetSkillId() then
					return self.LeftSkillIconList.item_list[1].grid_par.item_list[i]
				end
			end
		end
		return nil
	end)

	self.limitType = RoleSkillLimitType.None
	SkillData.Instance:PopSkillUnlock()		--防止自动学技能后显示解锁动画
end
function RoleViewSkill:onClickOpenSort()
	ViewMgr:OpenView(RoleSkillSortView)
end

function RoleViewSkill:onClickOpenDing()
	ViewMgr:OpenViewByKey(Mod.GlassTripod.Main)
end

function RoleViewSkill:OnDestroy()
	GuideManager.Instance:UnRegisterGetGuideUi("LSSecondSkill")
	RoleCtrl.Instance.skill_data:SetClickSkillId(0)
	SkillData.Instance.cur_click_list = 1
	SkillData.Instance:SetSkillPage(1)
	RoleCtrl.Instance.skill_data:SetRightType(1)
	RoleSkillData.Instance.cur_select_cost_type.typeIndex = 1
	if self.learn_runner ~= nil then
		TimeHelper:CancelTimer(self.learn_runner)
		self.learn_runner = nil
	end
	if self.flow_delay ~= nil then
		TimeHelper:CancelTimer(self.flow_delay)
		self.flow_delay = nil
	end
	TimeHelper:CancelTimer(self.ht_jump)
	-- RemindCtrl.Instance:Remind():UnRegister(Mod.Role.RoleSkill, self.train_remind)
	UIWFlushPanel.OnDestroy(self)
	if self.anim_event ~= nil then
		self.anim_event = nil
	end
end

function RoleViewSkill:ChangeRightType()
	self.RightType1:SetActive(RoleCtrl.Instance.skill_data.change_data.rightType == 1)
	self.RightType2:SetActive(RoleCtrl.Instance.skill_data.change_data.rightType == 2)
	self.RightType3:SetActive(RoleCtrl.Instance.skill_data.change_data.rightType == 3)
	self.Type2Bg:SetActive(RoleCtrl.Instance.skill_data.change_data.rightType == 2)
	self.Type3Bg:SetActive(RoleCtrl.Instance.skill_data.change_data.rightType == 3)
end

--生活技能提示文字刷新
function RoleViewSkill:NoticeStrFlush(skill_id, skill_level)
	local config = Cfg.SkillLife(skill_id, skill_level)
	if config ~= nil then
		local data = SkillData.Instance:GetLifeSkillDataFromServer(skill_id)
		local levelNeedExp = config.up_level_need_exp
		local value = 0
		if skill_level == data.skill_level then
			value = data.skill_exp
		end
		self.RedBg:SetActive(false)
		self.GreenBg:SetActive(true)
		if value == levelNeedExp then
			self:SetFullText(skill_level)
		elseif value > 0 and value < levelNeedExp then
			UH.SetText(self.CanLearnTxt, Language.Role.RoleViewSkillCanLearn)
			-- self.skill_proj_txtoj:SetActive(true)
		elseif value == 0 then
			UH.SetText(self.CanLearnTxt, Language.Role.NeedPreSkill)
			self.GreenBg:SetActive(false)
			self.RedBg:SetActive(true)
		end
	end
end

--刷新右侧技能信息列表
function RoleViewSkill:FlushRightSkillInfoList(value)
	local skillCareerCfg = Cfg.CareerSkillList(RoleData.Instance:GetRoleProf(), SkillData.Instance.change_data.click_skill_id)
	if skillCareerCfg == nil then
		LogError("skillCareerCfg is nil ",RoleData.Instance:GetRoleProf(), SkillData.Instance.change_data.click_skill_id)
		return
	end
	local curRoleSkillData = {}
	local is_passive_skill = false
	if SkillData.Instance.tab == 1 then
		curRoleSkillData = RoleCtrl.Instance.skill_data:getSkillDataById(SkillData.Instance.change_data.click_skill_id) or {}
		if curRoleSkillData.index2 == 0 then
			is_passive_skill = true
		end
	elseif SkillData.Instance.tab == 2 then
		curRoleSkillData = SkillData.Instance:GetLifeSkillDataFromServer(SkillData.Instance.change_data.click_skill_id) or {}
	end
	if is_passive_skill then
		local skillDescCfg = Cfg.SkillDescPartnerAndRole(curRoleSkillData.skill_id, curRoleSkillData.skill_level)
		if skillDescCfg and skillDescCfg.description then
			UH.SetText(self.PassiveDesc, skillDescCfg.description)
		end
		self.PassiveSkillObj:SetActive(true)
		self.SkillInfoList:SetObjActive(false)
		self.ObjCanJinJie:SetActive(false)
		self.MaxLevelShow:SetActive(false)
	else
		local data_list = {}
		self.ObjNotZeroLevel:SetActive(true)
		self.ObjBtnLearn:SetActive(false)
		if TableIsEmpty(curRoleSkillData) then
			self.ObjNotZeroLevel:SetActive(false)
			self.ObjBtnLearn:SetActive(true)
			-- 没学，就只放一级，置灰
			local data = {}
			data.skill_id = skillCareerCfg.skill_id
			data.skill_level = 1
			data.curRoleSkillData = curRoleSkillData
			data.maxLevel = skillCareerCfg.skill_level_limit
			table.insert(data_list, data)
		elseif curRoleSkillData.skill_level == skillCareerCfg.skill_level_limit then
			-- 满级,就只放满级
			local data = {}
			data.skill_id = skillCareerCfg.skill_id
			data.skill_level = skillCareerCfg.skill_level_limit
			data.curRoleSkillData = curRoleSkillData
			data.maxLevel = skillCareerCfg.skill_level_limit
			table.insert(data_list, data)
		else
			-- 放当前技能和下一级技能，下一级置灰
			for i = curRoleSkillData.skill_level, curRoleSkillData.skill_level + 1 do
				local data = {}
				data.skill_id = skillCareerCfg.skill_id
				data.skill_level = i
				data.curRoleSkillData = curRoleSkillData
				data.maxLevel = skillCareerCfg.skill_level_limit
				table.insert(data_list, data)
			end
		end
		-- for i=1,skillCareerCfg.skill_level_limit do
		-- 	local data = {}
		-- 	data.skill_id = skillCareerCfg.skill_id
		-- 	data.skill_level = i
		-- 	data.curRoleSkillData = curRoleSkillData
		-- 	data.maxLevel = skillCareerCfg.skill_level_limit
		-- 	table.insert(data_list, data)
		-- end
		self.PassiveSkillObj:SetActive(false)
		self.SkillInfoList:SetObjActive(true)
		self.SkillInfoList:SetData(data_list)
		self.ObjCanJinJie:SetActive(curRoleSkillData.skill_level ~= skillCareerCfg.skill_level_limit)
		self.MaxLevelShow:SetActive(curRoleSkillData.skill_level == skillCareerCfg.skill_level_limit)
		if TableIsEmpty(curRoleSkillData) then
			local is_limit, limitType, limitStr = SkillData.Instance:GetSkillLimit(RoleCtrl.Instance.skill_data:GetClickSkillId())
			if is_limit and limitType == RoleSkillLimitType.RoleLevel then
				UH.SetText(self.TxtJinJieTip, Format(Language.Role.SkillLearnTip, limitStr))
				self.LearnHandler.Interactable = false
				self.LearnRemind:SetNum(0)
			else
				UH.SetText(self.TxtJinJieTip, "")
				self.LearnHandler.Interactable = true
				self.LearnRemind:SetNum(1)
			end
		else
			local next_advance_cfg = RolePromoteData.GetRoleProfNextConfig()
			if not TableIsEmpty(next_advance_cfg) then
				UH.SetText(self.TxtJinJieTip, Format(Language.Role.JinJieTip, next_advance_cfg.max_skill_level))
			else
				UH.SetText(self.TxtJinJieTip, "")
			end
		end
	end

	self.AllToggle.isOn = RoleSkillData.Instance:ToggleAllLearn() == 0
	local need_coin = 0
	local advance_cfg = RolePromoteData.GetRoleProfConfig()
	-- 等于0就是勾选，默认勾选
	if RoleSkillData.Instance:ToggleAllLearn() == 0 then
		local is_can_up = false
		-- 勾了的话需要判断所有主动技能是否可以升级
		for k, v in pairs(RoleData.Instance.ret_data[1].list) do
			local now_skill = RoleCtrl.Instance.skill_data:getSkillDataById(v.skill_id) or {}
			if now_skill.index2 ~= 0 and RoleCtrl.Instance.skill_data:getSkillDataById(v.skill_id) then
				local is_limit, limitType, limitStr = SkillData.Instance:GetSkillLimit(v.skill_id)
				if not is_limit then
					is_can_up = true
					-- 需要计算所有已学习技能能升到最高级时消耗的铜币
					need_coin = need_coin + SkillData.Instance:GetSkillLvUpCoin(now_skill.skill_id, now_skill.skill_level, advance_cfg.max_skill_level)
				end
			end
		end
		self.IHandleAll.Interactable = is_can_up
		self.ObjConsume:SetActive(is_can_up)
	else
		if TableIsEmpty(curRoleSkillData) then
			self.IHandleAll.Interactable = false
			self.ObjConsume:SetActive(false)
		else
			if curRoleSkillData.index2 ~= 0 then
				local is_limit, limitType, limitStr = SkillData.Instance:GetSkillLimit(curRoleSkillData.skill_id)
				-- 需要计算该技能能升到最高级时消耗的铜币
				need_coin = SkillData.Instance:GetSkillLvUpCoin(curRoleSkillData.skill_id, curRoleSkillData.skill_level, advance_cfg.max_skill_level)
				self.IHandleAll.Interactable = not is_limit
				self.ObjConsume:SetActive(not is_limit)
			else
				self.IHandleAll.Interactable = false
				self.ObjConsume:SetActive(false)
			end
		end
	end
	UH.SetText(self.TxtCoin, need_coin)
	-- 现在只会创建两个，不需要滑动了
	-- if value == nil or value.click_skill_id ~= nil or value.change_level ~= nil or SkillData.Instance.role_skill_info_list_slide then
	-- 	local autoIndex = curRoleSkillData.skill_level or 1
	-- 	autoIndex = autoIndex == 0 and 1 or autoIndex
	-- 	self.SkillInfoList:AutoSlideRealy(autoIndex,false)
	-- end
end

-- 点击升级所有已激活技能toggle
function RoleViewSkill:OnAllToggleClick(value)
	RoleSkillData.Instance:ToggleAllLearn(value and 0 or 1)
	self:FlushRightSkillInfoList()
end

-- 点击角色技能一键升级
function RoleViewSkill:OnAllLearnClick()
	if self.IHandleAll.Interactable then
		-- 判断是否已经学习了，没学习的话只会升级该技能，已学习的就按照那个toggle来
		local curRoleSkillData = RoleCtrl.Instance.skill_data:getSkillDataById(SkillData.Instance.change_data.click_skill_id) or {}
		if TableIsEmpty(curRoleSkillData) then
			RoleCtrl.Instance:SendSkillReq(1, RoleCtrl.Instance.skill_data:GetClickSkillId())
		else
			RoleCtrl.Instance:SendSkillReq(5, RoleCtrl.Instance.skill_data:GetClickSkillId(), RoleSkillData.Instance:ToggleAllLearn() == 0 and 1 or 0)
		end
	else
		PublicPopupCtrl.Instance:Center(Language.Role.NeedJinShengTip)
	end
end

--刷新公共数据
function RoleViewSkill:onDataFlush()
	--刷新界面信息及状态
	UH.SetText(self.CanLearnTxt, "")
	self.RedBg:SetActive(false)
	self.GreenBg:SetActive(true)
	local skill_id = RoleCtrl.Instance.skill_data:GetClickSkillId()
	if skill_id == 0 then
		return
	end
	local curRoleSkillData = RoleCtrl.Instance.skill_data:getSkillDataById(SkillData.Instance.change_data.click_skill_id) or {}
	local skill_config = nil
	local skill_data = RoleCtrl.Instance.skill_data:getSkillDataById(skill_id) or {}
	local skill_level = curRoleSkillData.skill_level or 1
	local next_skill_config = nil

	--非生活技能标签时
	if SkillData.Instance.tab ~= 2 then
		self.Notyet:SetActive(false)
		-- self.ButtonNormalGreen:SetActive(true)
		local skillInfoCfgFunc = SkillData.idToTable(skill_id)
		skill_config = skillInfoCfgFunc(skill_id, skill_level)
		if skill_config == nil then
			return
		end
		--是否被限制了
		local is_limit, limitType, limitStr = SkillData.Instance:GetSkillLimit(RoleCtrl.Instance.skill_data:GetClickSkillId())
		if is_limit then
			if limitType == RoleSkillLimitType.RoleLevel or limitType == RoleSkillLimitType.PassiveSkill then
				self.CanLearnObj:SetActive(false)
			else
				self.CanLearnObj:SetActive(true)
				UH.SetText(self.CanLearnTxt, limitStr)
				self.limitType = limitType and limitType or RoleSkillLimitType.None
				self.RedBg:SetActive(true)
			end
			self.BtnLearn:SetActive(false)
			self.UpgradeTipObj:SetActive(false)
		else
			next_skill_config = skillInfoCfgFunc(skill_id, skill_level + 1)
			self.CanLearnObj:SetActive(false)
			-- self.BtnLearn:SetActive(true)
			self.UpgradeTipObj:SetActive(next(curRoleSkillData) ~= nil)
		end
		if limitType == RoleSkillLimitType.SkillFull then
			self:SetFullText(skill_level)
		end
	else
		self.limitType = RoleSkillLimitType.None
		--生活技能标签页时
		--self.ButtonNormalGreen:SetActive(false)
		self.BtnLearn:SetActive(false)
		self.UpgradeTipObj:SetActive(false)
		skill_config = Cfg.SkillDescLife(skill_id, 1)
		local data = SkillData.Instance:GetLifeSkillDataFromServer(skill_id)
		self:NoticeStrFlush(skill_id, data.skill_level)
		if data.skill_level == 0 and data.skill_exp == 0 then
			self.Notyet:SetActive(true)
		else
			self.Notyet:SetActive(false)
		end

		if self:SetFullText(data.skill_level) then
			-- self.CanLearnObj:SetActive(false)
		else
			local life_skill_cfg = Cfg.SkillDescLife(skill_id, data.skill_level)
			UH.SetText(self.CanLearnTxt, life_skill_cfg and life_skill_cfg.tips or "")
		end
		self.CanLearnObj:SetActive(false)
	end
	if skill_config.skill_name then
		UH.SetText(self.SkillName, skill_config.skill_name)
		self.cur_skill_name_page1 = skill_config.skill_name
	else
		local skill_desc = Cfg.SkillDescPartnerAndRole(skill_id, skill_level)
		if skill_desc and skill_desc.skill_name then
			UH.SetText(self.SkillName, skill_desc.skill_name)
			self.cur_skill_name_page1 = skill_desc.skill_name
		end
	end
	-- have_exp = have_exp or 0
	if skill_config.bind_coin_consume ~= nil and next_skill_config and next_skill_config.bind_coin_consume then
		-- self.LearnCost, next_skill_config.bind_coin_consume
		if curRoleSkillData.skill_level then
			UH.SetText(self.LearnCost, next_skill_config.bind_coin_consume)
			self.bind_coin_consume = next_skill_config.bind_coin_consume
		else
			UH.SetText(self.LearnCost, skill_config.bind_coin_consume)
			self.bind_coin_consume = skill_config.bind_coin_consume
		end
	end

	for k, v in pairs(self.LeftSkillIconList.item_list) do
		v:FlushMask()
	end
end

-- 去晋阶界面
function RoleViewSkill:GoToJinJie()
	ViewMgr:OpenView(RolePromoteView)
	ViewMgr:CloseView(RoleView)
end

--设置当前等级已满级的状态显示
function RoleViewSkill:SetFullText(compare_level)
	local maxLevel =  Cfg.CareerSkillList(RoleData.Instance:GetRoleProf(), RoleCtrl.Instance.skill_data:GetClickSkillId()).skill_level_limit
	if compare_level == maxLevel then
		UH.SetText(self.CanLearnTxt, Language.Role.RoleViewSkillFull)
		self.GreenBg:SetActive(true)
		return true
	else
		UH.SetText(self.CanLearnTxt, "")
		self.GreenBg:SetActive(false)
		return false
	end
end

function RoleViewSkill:OnClickRed()
	if self.limitType == RoleSkillLimitType.CareerLevel then
		ViewMgr:OpenView(RolePromoteView)
	end
end

function RoleViewSkill:OnLearnPressDown()
	if SkillData.Instance.tab == 2 then return end
	
	local clicked_skillid = RoleCtrl.Instance.skill_data:GetClickSkillId()
	local skill_data = RoleCtrl.Instance.skill_data:getSkillDataById(clicked_skillid)
	if self.learn_runner ~= nil then
		TimeHelper:CancelTimer(self.learn_runner)
		self.learn_runner = nil
	end
	local is_limit, limitType, limitStr = SkillData.Instance:GetSkillLimit(RoleCtrl.Instance.skill_data:GetClickSkillId())
	if not is_limit then
		local function learn_func()
			-- 策划说职业技能不检测铜币
			-- if RoleData.Instance:GetCurrency(CurrencyType.CoinBind) < self.bind_coin_consume then
			-- 	-- PublicPopupCtrl.Instance:Center(Language.Role.PopupTips7)
			-- 	MallCtrl.IsNotCurrency(CurrencyType.CoinBind,self.bind_coin_consume)
			-- 	TimeHelper:CancelTimer(self.learn_runner)
			-- 	self.learn_runner = nil
			-- 	return
			-- end
			SkillData.Instance.role_skill_info_list_slide = true
			RoleCtrl.Instance:SendSkillReq(1, RoleCtrl.Instance.skill_data:GetClickSkillId())
		end
		if skill_data ~= nil then
			local skill_level_cfg = SkillData.GetConfig(clicked_skillid, skill_data.skill_level + 1)
			local times = (skill_level_cfg.up_level_need_exp - skill_data.skill_exp) / skill_level_cfg.add_exp - 1
			local bind_coin_num = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
			times = bind_coin_num / skill_level_cfg.bind_coin_consume >= times and times or math.floor(bind_coin_num / skill_level_cfg.bind_coin_consume) + 1
			if GuideManager.Instance:IsGuide() then
				times = 1
			end
			--根据次数判断
			if times <= 10 then
				self.learn_runner = TimeHelper:AddRunTimer(function ()
					learn_func()
				end,0.3, times)
			else
				self.learn_runner = TimeHelper:AddCountDownTT(function () --update_func
					learn_func()
				end,
				function ()
					self.learn_runner = TimeHelper:AddRunTimer(function () --complete_func
						learn_func()
					end,0.1, times - 11)
				end, 3, 0.3) --一共执行3秒，每0.3秒一次
			end
		else
			learn_func()
		end
	elseif limitType == RoleSkillLimitType.CareerLevel then
		PublicPopupCtrl.Instance:Center(limitStr)
	elseif limitType == RoleSkillLimitType.RoleLevel then
		PublicPopupCtrl.Instance:Center(string.format(Language.Role.RoleLevelCanLearn, limitStr))
	end
end

function RoleViewSkill:OnClickAutoLearn()
	local is_limit, limitType, limitStr = SkillData.Instance:GetSkillLimit(RoleCtrl.Instance.skill_data:GetClickSkillId())
	if not is_limit then
		local can_max_level, cost_coin = RoleCtrl.Instance.skill_data:GetProfSkillCanMaxLevel()
		if cost_coin == 0 then
			PublicPopupCtrl.Instance:AlertTip(Language.Mall.NotCurrency[2],function()
				ShoppingCtrl.Instance:OpenStoreView(0)
			end)
			return
		end
		local info = {
			content = string.format(Language.Role.RoleSkillAutoLearnCostCoin, can_max_level, cost_coin),
			item_id = HuoBi[CurrencyType.CoinBind],
			confirm = {
				name = string.format(Language.Role.RoleSkillAutoLearnBtnTip, cost_coin),
				func = function ()
					SkillData.Instance.role_skill_info_list_slide = true
					-- 这个已经不用了，目前代码逻辑也走不进来
					RoleCtrl.Instance:SendSkillReq(4, RoleCtrl.Instance.skill_data:GetClickSkillId(), can_max_level)
				end
			},
			tnr = DTTodayNotRemind.RoleProfSkillAutoLearn
		}
		RoleCtrl.Instance:OpenAutoLearnDialogTip(info)
	end
end

function RoleViewSkill:OnLearnPressUp()
	if self.learn_runner ~= nil then
		TimeHelper:CancelTimer(self.learn_runner)
		self.learn_runner = nil
	end
end

--学习技能按钮
function RoleViewSkill:onLearnClick()
	if SkillData.Instance.tab == 2 then return end

	local clicked_skillid = RoleCtrl.Instance.skill_data:GetClickSkillId()
	local skill_data = RoleCtrl.Instance.skill_data:getSkillDataById(clicked_skillid)

	local is_limit, limitType, limitStr = SkillData.Instance:GetSkillLimit(RoleCtrl.Instance.skill_data:GetClickSkillId())
	if not is_limit then
		SkillData.Instance.role_skill_info_list_slide = true
		RoleCtrl.Instance:SendSkillReq(1, RoleCtrl.Instance.skill_data:GetClickSkillId())
		RoleCtrl.Instance:SendSkillReq(5, RoleCtrl.Instance.skill_data:GetClickSkillId(), 0)
	elseif limitType == RoleSkillLimitType.CareerLevel then
		PublicPopupCtrl.Instance:Center(limitStr)
	elseif limitType == RoleSkillLimitType.RoleLevel then
		PublicPopupCtrl.Instance:Center(string.format(Language.Role.RoleLevelCanLearn, limitStr))
	end
end

--左侧技能列表的缓存配置
function RoleViewSkill:GetSkillConfigRetData(skill_id)
	local data = RoleData.Instance.ret_data
	for k, v in pairs(data)do
		for x, y in pairs(v.list)do
			if y.skill_id == skill_id then
				return y
			end
		end
	end
	return nil
end

--左侧技能列表标签页按钮（生活技能）
function RoleViewSkill:onLifeSkillClick()
	if SkillData.Instance.tab == 2 then return end
	SkillData.Instance:SetSkillPage(2)
	self.BtnSort:SetActive(false)
end
--左侧技能列表标签页按钮（战斗技能）
function RoleViewSkill:onBattleSkillClick()
	if SkillData.Instance.tab == 1 then return end
	SkillData.Instance:SetSkillPage(1)
	self.BtnSort:SetActive(true)
end

-------切换左边技能列表标签页刷新数据
function RoleViewSkill:FlushLeftSkillIconList(value)
	local tab = SkillData.Instance.tab
	if tab == 2 then
		SkillData.Instance.cur_list = false
		self.LeftSkillToggleGroup:SetAllTogglesOff()
		-- self.ButtonNormalGreen:SetActive(false)
		SkillData.Instance:SetRightType(1)
	end
	local profession = RoleData.Instance:GetRoleProf()
	if profession == 0 then
		return
	end
	
	if tab == 1 then
		local config = SkillData.Instance.careerProfSkillList[profession]
		local ret_data = {}
		for i=1,2 do
			if config[i - 1] ~= nil then
				ret_data[i] = {}
				ret_data[i].list = config[i - 1]
				ret_data[i].sort_type = i - 1
				ret_data[i].cur_list = i

				for k, v in pairs(ret_data[i].list)do
					local skill_data = RoleCtrl.Instance.skill_data:getSkillDataById(v.skill_id)
					--未学习的多加100，为防止重复相加判断小于200
					if skill_data == nil then
						if v.index2 < 200 then
							v.index2 = v.index2 + 100 + v.skill_learn_level
						end
					else
						--已经学习了的，未设为常用的加
						if v.index2 == 99 then
							v.index2 = v.index2 + v.skill_learn_level - skill_data.skill_level
						end
					end
				end
				local sort_func = function (a,b)
					if a.index2 < b.index2 then
						return true
					elseif a.index2 == b.index2 then
						return a.skill_id < b.skill_id
					else
						return false
					end
				end
				table.sort(ret_data[i].list,sort_func)
			end
		end
		
		--添加修炼技能的列表
		ret_data[3] = {}
		ret_data[3].list = RoleSkillData.Instance:GetShowConfig()
		ret_data[3].sort_type = 5
		
		ret_data[4] = {}
		ret_data[4].list = RoleSkillData.Instance:GetAssistConfig()
		ret_data[4].sort_type = 4

		RoleData.Instance.ret_data = ret_data
		--查询当前已经学习了的技能，并赋值是否设为常用（index2 用于排序）
		for k, v in pairs(SkillData.Instance.smart_data) do
			if v.index2 ~= nil and v.index2 >= 0 then
				local skill_data = self:GetSkillConfigRetData(v.skill_id)
				if skill_data ~= nil then
					skill_data.index2 = v.index2
				end
			end
		end
		
		self.LeftSkillIconList:SetData(ret_data)
		self:flowToSkillIcon()
		--有背包的天引琉璃跳转执行
		if RoleSkillData.Instance.use_glass_tripod then
			TimeHelper:CancelTimer(self.ht_jump)
			self.ht_jump = TimeHelper:AddDelayFrameTimer(function ()
				--先滑动技能列表
				self.LeftSkillIconList:AutoSlide(ret_data[3], false)
				--选中可升级的修炼技能
				local index = self.LeftSkillIconList:GetSelectedIndex(ret_data[3])
				local skill_content_item = self.LeftSkillIconList:GetItem(index)
				if skill_content_item then
					local skill_index = RoleSkillData.Instance:CanTrainIndex() or 1
					skill_content_item.grid_par:ClickItem(skill_index)
				end
				RoleSkillData.Instance.use_glass_tripod = false
			end,2)
		else
			-- 存在职业技能id跳转，否则默认选择第一个主动技能
			TimeHelper:CancelTimer(self.ht_jump)
			local function jump_to_skill(sel_index)
				self.ht_jump = TimeHelper:AddDelayFrameTimer(function ()
					local ret_data_index = self.LeftSkillIconList:GetSelectedIndex(ret_data[1])
					local skill_content_item = self.LeftSkillIconList:GetItem(ret_data_index)
					--先滑动技能列表
					self.LeftSkillIconList:AutoSlide(ret_data[1], false)
					if skill_content_item then
						if sel_index > 0 then
							skill_content_item.grid_par:ClickItem(sel_index)
						end
					end
					RoleSkillData.Instance:SetNeedJumpSkillId(nil)
				end, 2)
			end
			local index = 0
			if RoleSkillData.Instance:GetNeedJumpSkillId() then
				-- 查找选中技能
				for i, v in ipairs(ret_data[1].list) do
					if v.skill_id == RoleSkillData.Instance:GetNeedJumpSkillId() then
						index = i
						break
					end
				end
				jump_to_skill(index)
			elseif RoleCtrl.Instance.skill_data:GetClickSkillId() == 0 then
				-- 查找第一个主动技能
				for i, v in ipairs(ret_data[1].list) do
					local now_skill = RoleCtrl.Instance.skill_data:getSkillDataById(v.skill_id) or {}
					if now_skill.index2 ~= 0 then
						index = i
						break
					end
				end
				jump_to_skill(index)
			end
			
		end
	elseif tab == 2 then
		local ret_data = {}
		for k, v in pairs(SkillData.Instance.life_skill_list) do
			if v.sort_type == 2 then
				if ret_data[1] == nil then
					ret_data[1] = {}
					ret_data[1].list = {}
				end
				ret_data[1].cur_list = 1
				v.cur_list = 1
				ret_data[1].sort_type = 2
				table.insert(ret_data[1].list,v)
			elseif v.sort_type == 3 then
				if ret_data[2] == nil then
					ret_data[2] = {}
					ret_data[2].list = {}
				end
				ret_data[2].cur_list = 2
				v.cur_list = 2
				ret_data[2].sort_type = 3
				table.insert(ret_data[2].list,v)
			end
		end
		self.LeftSkillIconList:SetData(ret_data)
	end
end

function RoleViewSkill:flowToSkillIcon()
	local guide = GuideManager.Instance:Guide()
	if guide ~= nil and guide.id == 66 then
		self.flow_delay = TimeHelper:AddDelayTimer(function ()
			self.LeftSkillIconList:JumpVerticalIndex(2, self.LeftSkillIconList.item_list[1].Rect.sizeDelta.y)
			self.LeftSkillIconList.item_list[3].grid_par:ClickItem(1)
		end,0.5)
	end
end

-----------辅助技能修炼界面-----------------------------------------------
AssistInfoPanel = AssistInfoPanel or DeclareMono("AssistInfoPanel", UIWFlushPanel)
function AssistInfoPanel:AssistInfoPanel()
	self.data_cares = {
		{data = RoleSkillData.Instance.current_train_data, func = self.OtherChangeFlush, keys = {"level","exp","train_type"}},
		{data = RoleSkillData.Instance.cur_select_cost_type, func = self.FlushAssistButton,keys = {"typeIndex"}},
		-- {data = RoleSkillData.Instance.smart_data, func = self.OtherChangeFlush, keys = {"flush_promoted",}},
	}
end

function AssistInfoPanel:OtherChangeFlush()
	self:FlushAssistInfo()
end

--刷新右侧信息窗口
function AssistInfoPanel:FlushAssistInfo()
	local data = RoleSkillData.Instance:GetCurTrainData()
	if data == nil then
		LogError("flushLevelCost current train data = nil")
		return
	end

	self.cur_is_max = data.level >= #RoleSkillData.Instance:GetCharacterData(data.train_type)
	local character_data = RoleSkillData.Instance:GetCharacterLevelData(data.train_type, data.level)
	local character_data_next = not self.cur_is_max and RoleSkillData.Instance:GetCharacterLevelData(data.train_type, data.level + 1) or nil
	self.assist_character_data = character_data
	self.assist_character_data_next = character_data_next
	if self.cur_assist_data ~= nil and self.cur_assist_data.level ~= data.level and self.cur_assist_data.train_type == data.train_type then
		if self.anim_event == nil then
	        self.anim_event =
	            self.AssistInfoAnim:ListenEvent(
	            "LevenUp",
	            function(eve_param, anim_st_info)
	                if eve_param == "end" then
	                	self.EffectTool:Play("10010307")
	                	self:flushAssist()
	                end
	            end
	        )
	    end
	    self.EffectTool:Play("10010305")
	    self.EffectTool:Play("10010813")
	    self.AssistInfoAnim:SetTrigger(APH("LevelUp"))
	    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
		self:setNextShowInfo(data, character_data_next)
		self.cur_assist_data = {level = data.level, train_type = data.train_type, exp = data.exp}
		return
	else
		self.cur_assist_data = {level = data.level, train_type = data.train_type, exp = data.exp}
		self:flushAssist()
	end
end

function AssistInfoPanel:setNextAssistLevelInfo(data , character_data_next)
	if not self.cur_is_max then
		self.NextAssistLevelInfoObj:SetActive(true)
		self.AssistArrow:SetActive(true)
		self.NextSkillInfoShowObj:SetActive(true)
		--下一级信息
		UH.SetText(self.NextAssistLevelText, Language.LvDot(data.level + 1))
		if character_data_next.attribute_value and data.train_type ~= 2 and data.train_type ~= 3 and data.train_type <= 1000 then
			UH.SetText(self.NextAssistLevelValue, tostring(character_data_next.attribute_value / 100).."%")
		else
			UH.SetText(self.NextAssistLevelValue, "")
		end
		UH.SetText(self.LimitLevelText, "")
	else
		self.NextAssistLevelInfoObj:SetActive(false)
		self.AssistArrow:SetActive(false)
		self.NextSkillInfoShowObj:SetActive(false)
		UH.SetText(self.LimitLevelText, Language.Role.XiulianMaxStr)
		UH.AnchoredPosition(self.LimitLevelTextRect, Vector3.New(276.6, -279.6, 0))
	end
end

function AssistInfoPanel:setCurAssistLevelInfo(data, character_data, character_data_next)
	if character_data.attribute_value then
		-- 2 3 修改
		if data.train_type == 2 or data.train_type == 3 then
			UH.SetText(self.CurAssistLevelValue, "")
			local value = RoleSkillData.Instance:getPromotedValue(data.train_type)
			local attr_up_str = data.level == 0 and "" or string.format(Language.Role.TrainAttr[data.train_type], tostring(value / 10) .. "%")
			if character_data.level_limit and character_data.level_limit ~= 0 then
				attr_up_str = attr_up_str .. string.format(Language.Role.TrainAttrMax, tostring(character_data.level_limit / 10))
			end
			UH.SetText(self.AttrUpText, attr_up_str)
		elseif data.train_type > 1000 then
			UH.SetText(self.CurAssistLevelValue, "")
			UH.SetText(self.AttrUpText, "")
		else
			UH.SetText(self.CurAssistLevelValue, tostring(character_data.attribute_value / 100).."%")
			UH.SetText(self.AttrUpText, "")
		end
	else
		UH.SetText(self.CurAssistLevelValue, "")
		UH.SetText(self.AttrUpText, "")
	end
	local desc_cfg = RoleSkillData.GetNameAndDesc(data.train_type)
	UH.SpriteName(self.AssistIconBg, "xiulianIcon"..tostring(RoleSkillData.RoleXiuLianIconIndex[data.train_type] or data.train_type))
	UH.SpriteName(self.AssistIcon,"xiulianSmallIcon"..tostring(data.train_type))

	if data.train_type == 2 or data.train_type == 3 then
		UH.SetText(self.CurAssistLevelDesc, character_data.attribute_value == 0 and Language.Role.TrainDesc[data.train_type] or string.format(desc_cfg.description, tostring(character_data.attribute_value / 10)))
		if character_data_next then
			UH.SetText(self.NextAssistLevelDesc, string.format(desc_cfg.description, tostring(character_data_next.attribute_value / 10)))
			UH.SetText(self.NextShowAssistLevelDesc, string.format(desc_cfg.description, tostring(character_data_next.attribute_value / 10)))
		end
	elseif data.train_type > 1000 then
		if type(character_data.attr_type) == "number" then
			UH.SetText(self.CurAssistLevelDesc, character_data.train_level <= 0 and Language.Role.TrainDesc[data.train_type] or string.format(desc_cfg.description, character_data.attribute_value))
			if character_data_next then
				UH.SetText(self.NextAssistLevelDesc,  string.format(desc_cfg.description, character_data_next.attribute_value))
				UH.SetText(self.NextShowAssistLevelDesc,  string.format(desc_cfg.description, character_data_next.attribute_value))
			end
	else
			UH.SetText(self.CurAssistLevelDesc, character_data.train_level <= 0 and Language.Role.TrainDesc[data.train_type] or string.format(desc_cfg.description, character_data.attribute_value / 10))
			if character_data_next then
				UH.SetText(self.NextAssistLevelDesc,  string.format(desc_cfg.description, character_data_next.attribute_value / 10))
				UH.SetText(self.NextShowAssistLevelDesc,  string.format(desc_cfg.description, character_data_next.attribute_value / 10))
			end
		end
	else
		UH.SetText(self.CurAssistLevelDesc, desc_cfg.description)
		UH.SetText(self.NextAssistLevelDesc, desc_cfg.description)
		UH.SetText(self.NextShowAssistLevelDesc, desc_cfg.description)
	end
	UH.SetText(self.SkillName, desc_cfg.train_name)
	self.cur_skill_name_page2 = desc_cfg.train_name
	UH.SetText(self.CurAssistLevelText, Language.LvDot(data.level))
	--有下一级就设置下一级的，没有就设置当前等级的
	self.curCostCoinNum = character_data_next and character_data_next.bind_coin_consume or character_data.bind_coin_consume
end

function AssistInfoPanel:flushAssist()
	local data = RoleSkillData.Instance:GetCurTrainData()
	if data == nil then
		LogError("flushLevelCost current train data = nil")
		return
	end
	--当前等级信息
	local descStr = RoleSkillData.GetNameAndDesc(self.cur_assist_data.train_type).description
	if self.assist_character_data ~= nil then
		self:setCurAssistLevelInfo(self.cur_assist_data, self.assist_character_data, self.assist_character_data_next)
	end
	--下一级等级信息
	self:setNextAssistLevelInfo(self.cur_assist_data, self.assist_character_data_next)

	local is_opend, limitStr = RoleSkillData.Instance:GetTrainTypeIsOpen(self.cur_assist_data.train_type)
	if self.cur_assist_data.train_type > 1000 then
		is_opend, limitStr = RoleSkillData.Instance:GetTrainCanLearn(self.cur_assist_data.train_type, 0)
		limitStr = limitStr ~= nil and string.format(ErrorText[90], limitStr) or ""
	elseif self.cur_assist_data.train_type > 999 then
		local pet_train_cfg = RoleSkillData.Instance:GetCharacterLevelData(self.cur_assist_data.train_type, 1)
		is_opend = RoleData.Instance:GetRoleLevel() >= pet_train_cfg.skill_learn_level
		limitStr = string.format(ErrorText[90],pet_train_cfg.skill_learn_level)
	end
	if not is_opend and not self.cur_is_max then
		-- self.NextAssistLevelInfoObj:SetActive(false)
		self.AssistBtnObj:SetActive(false)
		self.ToggleOnceObj:SetActive(false)
		self.ToggleTenObj:SetActive(false)
		UH.SetText(self.LimitLevelText, limitStr)
		if data.train_type == 2 or data.train_type == 3 or data.train_type > 1000 then
			UH.SetText(self.CurAssistLevelDesc, string.format(Language.Common.Xstr, COLORSTR.Gray3, Language.Role.TrainDesc[data.train_type]))
		else
			UH.SetText(self.CurAssistLevelDesc, string.format(Language.Common.Xstr, COLORSTR.Gray3, descStr))
		end
		UH.AnchoredPosition(self.LimitLevelTextRect, Vector3.New(276.6, -147, 0))
		for i=1,self.AssistInteractor:Length() do
			self.AssistInteractor[i].Interactable = false
		end
		self.AssistProg:SetProgress(0)
		self.AssistProg:SetText("")
		return
	else
		for i=1,self.AssistInteractor:Length() do
			self.AssistInteractor[i].Interactable = true
		end
	end
	self.AssistBtnObj:SetActive(not self.cur_is_max)
	self.ToggleOnceObj:SetActive(not self.cur_is_max)
	self.ToggleTenObj:SetActive(not self.cur_is_max)
	self.LimitLevelObj:SetActive(self.cur_is_max)

	--进度条信息
	self:setAssistProgress(self.cur_assist_data, self.assist_character_data, self.assist_character_data_next)
	--刷新按钮状态
	self:FlushAssistButton()
end

function AssistInfoPanel:setAssistProgress(data, character_data, character_data_next)
	local rate = 0
	local progress_text = ""
	if character_data_next ~= nil then
		if data.train_type <= 1000 then
		progress_text = string.format("%s/%s", tostring(data.exp) ,tostring(character_data_next.need_exp))
		rate = data.exp / character_data_next.need_exp
	else
			progress_text = string.format("%s/%s", tostring(data.exp) ,tostring(character_data_next.need_exp))
			rate = data.exp / character_data_next.need_exp
		end
	else
		progress_text = string.format("%d/%d",character_data.need_exp, character_data.need_exp)
		rate = 1
	end
	self.AssistProg:SetProgress(rate)
	self.AssistProg:SetText(progress_text)
end

function AssistInfoPanel:setNextShowInfo(data, character_data_next)
	if not self.cur_is_max then
		self.NextSkillInfoShowObj:SetActive(true)
		self.NextAssistLevelInfoObj:SetActive(true)
		self.AssistArrow:SetActive(true)
		--下一级信息
		if character_data_next.attribute_value and data.train_type ~= 2 and data.train_type ~= 3 and data.train_type <= 1000 then
			UH.SetText(self.NextShowAssistLevelValue, tostring(character_data_next.attribute_value / 100).."%")
		else
			UH.SetText(self.NextShowAssistLevelValue, "")
		end
		UH.SetText(self.NextShowAssistLevelText, Language.LvDot(data.level + 1))
		UH.SetText(self.LimitLevelText, "")
	else
		self.NextSkillInfoShowObj:SetActive(false)
	end
end

---------------右边修炼技能信息
--刷新修炼按钮状态
function AssistInfoPanel:FlushAssistButton()
	local costStr = ""
	if RoleSkillData.Instance.cur_select_cost_type.typeIndex == 1 then
		costStr = string.format(Language.Role.XiulianCostFormat, self.curCostCoinNum)
	elseif RoleSkillData.Instance.cur_select_cost_type.typeIndex == 2 then
		costStr = string.format(Language.Role.XiulianCostFormat, self.curCostCoinNum * 10)
	end
	if RoleSkillData.Instance.cur_select_cost_type.typeIndex ~= 3 then
		UH.SetText(self.CostCoinNumText, costStr)
	end
	-- self.CostDanObj:SetActive(RoleSkillData.Instance.cur_select_cost_type.typeIndex == 3)
	-- self.CoinIconObj:SetActive(RoleSkillData.Instance.cur_select_cost_type.typeIndex ~= 3)
	-- self.CostCoinNumObj:SetActive(RoleSkillData.Instance.cur_select_cost_type.typeIndex ~= 3)
end

--点击修炼按钮
function AssistInfoPanel:OnClickXiulianButton()
	local need_num = RoleSkillData.Instance.cur_select_cost_type.typeIndex == 1 and self.curCostCoinNum or self.curCostCoinNum * 10
	if not MallCtrl.IsNotCurrency(CurrencyType.CoinBind, need_num) then
		return
	end
	if RoleSkillData.Instance.current_train_data.train_type == 1000 then
		local train_cfg = RoleSkillData.Instance:GetCharacterData(RoleSkillData.Instance.current_train_data.train_type)
		local level_cfg = train_cfg[RoleSkillData.Instance:GetRolePetXiulianLevel() + 1] or {}
		local role_level = RoleData.Instance:GetRoleLevel()
		if level_cfg.skill_learn_level ~= nil and role_level < level_cfg.skill_learn_level then
			PublicPopupCtrl.Instance:Center(string.format(Language.Role.XiulianLevelNotEnough, level_cfg.skill_learn_level))
			return
		end
		RoleCtrl.Instance:sendCSRolePetTrainSkillReq(RoleSkillData.Instance.cur_select_cost_type.typeIndex == 1 and 1 or 10)
		return
	end
	local can, level = RoleSkillData.Instance:GetTrainCanLearn(self.assist_character_data.train_type, self.assist_character_data.train_level)
	if not can then
		if level then
		PublicPopupCtrl.Instance:Center(string.format(Language.Role.XiulianLevelNotEnough, level))
		end
		return
	end
	local req_type = 0
	if RoleSkillData.Instance.current_train_data.train_type > 2000 then
		req_type = 5
	elseif RoleSkillData.Instance.current_train_data.train_type > 1000 then
		req_type = 4
	end
	if RoleSkillData.Instance.cur_select_cost_type.typeIndex == 1 then
		RoleCtrl.Instance:SendCSTrainReq(req_type, RoleSkillData.Instance.current_train_data.train_type, 1)
	else
		RoleCtrl.Instance:SendCSTrainReq(req_type, RoleSkillData.Instance.current_train_data.train_type, 10)
	end
end

--选择修炼消耗
--金币1次
function AssistInfoPanel:OnSelectOnce()
	RoleSkillData.Instance.cur_select_cost_type.typeIndex = 1
end
--金币10次
function AssistInfoPanel:OnSelectTen()
	RoleSkillData.Instance.cur_select_cost_type.typeIndex = 2
end

----------------------------------------------------------
TrainInfoPanel = TrainInfoPanel or DeclareMono("TrainInfoPanel", UIWFlushPanel)
function TrainInfoPanel:TrainInfoPanel()
	self.data_cares = {
		{data = RoleSkillData.Instance.current_train_data, func = self.FlushTrainInfo, keys = {"train_type","level","exp"}},
		{data = RoleSkillData.Instance.cur_select_cost_type, func = self.FlushTrainButton,keys = {"typeIndex"}},
	}

	GuideManager.Instance:RegisterGetGuideUi("TrainBtnObj", function ()
		if self:IsActiveAndEnabled() then
			return self.TrainBtnObj
		end
	end)
end

--刷新右侧信息窗口
function TrainInfoPanel:FlushTrainInfo()
	local data = RoleSkillData.Instance:GetCurTrainData()
	if data == nil then
		LogError("flushLevelCost current train data = nil")
		return
	end
	self.ReddotToggle.isOn = RoleSkillData.Instance:GetReddotNoticeFlag(data.train_type)
	self.cur_is_max = data.level >= #RoleSkillData.Instance:GetCharacterData(data.train_type)
	local character_data = RoleSkillData.Instance:GetCharacterLevelData(data.train_type, data.level)
	local character_data_next = not self.cur_is_max and RoleSkillData.Instance:GetCharacterLevelData(data.train_type, data.level + 1) or nil
	self.train_character_data = character_data
	self.train_character_data_next = character_data_next
	if self.cur_train_data ~= nil 
		and self.cur_train_data.train_type == data.train_type
		and self.cur_train_data.level ~= data.level then
		if self.anim_event == nil then
	        self.anim_event =
	            self.TrainInfoAnim:ListenEvent(
	            "LevenUp",
	            function(eve_param, anim_st_info)
	                if eve_param == "end" then
	                	if self.delayShowUplevelEffect ~= nil then
	                		TimeHelper:CancelTimer(self.delayShowUplevelEffect)
	                	end
		                self.delayShowUplevelEffect = TimeHelper:AddDelayTimer(function ()
		                	self.EffectTool:Play("100108148")
		                	self:flushTrain()
		                end, 0.4)
	                end
	            end
	        )
	    end
	    self.TrainProg:AddToProgress(1)
	    self.TrainProg:SetText("")
	    self.TrainInfoAnim:SetTrigger(APH("LevelUp"))
	    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
		self:setNextShowInfo(data, character_data_next)
		self.cur_train_data = TableCopy(data)
	else
		local addTo = self.cur_train_data ~= nil and self.cur_train_data.train_type == data.train_type
		self.cur_train_data = TableCopy(data)
		self:flushTrain(addTo)
	end
end

function TrainInfoPanel:OnDestroy()
	if self.delayShowUplevelEffect ~= nil then
		TimeHelper:CancelTimer(self.delayShowUplevelEffect)
		self.delayShowUplevelEffect = nil
	end
	UIWFlushPanel.OnDestroy(self)
end

function TrainInfoPanel:flushTrain(addTo)
	local data = RoleSkillData.Instance:GetCurTrainData()
	if data == nil then
		LogError("flushLevelCost current train data = nil")
		return
	end
	--当前等级信息
	local descStr = RoleSkillData.GetNameAndDesc(self.cur_train_data.train_type).description
	if self.train_character_data ~= nil then
		self:setCurTrainLevelInfo(self.cur_train_data, self.train_character_data, self.train_character_data_next)
	end
	--下一级等级信息
	self:setNextTrainLevelInfo(self.cur_train_data, self.train_character_data_next)

	local is_opend, limitStr = RoleSkillData.Instance:GetTrainTypeIsOpen(self.cur_train_data.train_type)

	if not is_opend then
		-- self.NextAssistLevelInfoObj:SetActive(false)
		self.TrainBtnObj:SetActive(false)
		self.AutoTrainBtnObj:SetActive(false)
		UH.SetText(self.LimitLevelText, limitStr)
		UH.SetText(self.CurTrainLevelDesc, string.format(Language.Common.Xstr, COLORSTR.Gray3, descStr))
		UH.AnchoredPosition(self.LimitLevelTextRect, Vector3.New(276.6, -273, 0))
		for i=1,self.TrainInteractor:Length() do
			self.TrainInteractor[i].Interactable = false
		end
		self.TrainProg:SetProgress(0)
		self.TrainProg:SetText("")
		self.ReddotToggleObj:SetActive(false)
		return
	else
		for i=1,self.TrainInteractor:Length() do
			self.TrainInteractor[i].Interactable = true
		end
		self.ReddotToggleObj:SetActive(true)
	end
	self.TrainBtnObj:SetActive(not self.cur_is_max)
	self.AutoTrainBtnObj:SetActive(not self.cur_is_max)
	--进度条信息
	self:setTrainProgress(self.cur_train_data, self.train_character_data, self.train_character_data_next, addTo)
	--刷新按钮状态
	self:FlushTrainButton()
end

function TrainInfoPanel:setTrainProgress(data, character_data, character_data_next, addTo)
	local rate = 0
	-- local progress_text = ""
	if character_data_next ~= nil then
		progress_text = string.format("%s/%s", tostring(data.exp) ,tostring(character_data_next.need_exp))
		rate = data.exp / character_data_next.need_exp
	else
		progress_text = string.format("%d/%d",character_data.need_exp, character_data.need_exp)
		rate = 1
	end
	if addTo then
		self.TrainProg:AddToProgress(rate)
	else
		self.TrainProg:SetProgress(rate)
	end
	self.TrainProg:SetText(progress_text)
end

function TrainInfoPanel:setCurTrainLevelInfo(data, character_data, character_data_next)
	local attribute_value = ""

	if not StringIsEmpty(character_data.attr_name) then
		if data.level == 0 then
			attribute_value = ""
			UH.SetText(self.CurTrainLevelDesc, Language.Role.XiuXianCanIncrase..character_data.attr_name)
		else
			attribute_value = character_data.attribute_value and "+"..character_data.attribute_value or ""
			UH.SetText(self.CurTrainLevelDesc, Language.Role.trainSkillDesc..character_data.attr_name)
		end
		
		UH.SetText(self.NextTrainLevelDesc, Language.Role.trainSkillDesc..character_data.attr_name)
		UH.SetText(self.NextShowTrainLevelDesc, Language.Role.trainSkillDesc..character_data.attr_name)
		UH.SetText(self.SkillName, character_data.attr_name..Language.Role.Tab3)
		
	else
		local desc_cfg = RoleSkillData.GetNameAndDesc(data.train_type)
		UH.SetText(self.CurTrainLevelDesc, desc_cfg.description)
		UH.SetText(self.NextTrainLevelDesc, desc_cfg.description)
		UH.SetText(self.NextShowTrainLevelDesc, desc_cfg.description)
		UH.SetText(self.SkillName, desc_cfg.train_name)
		attribute_value = character_data.attribute_value and tostring(character_data.attribute_value / 100).."%" or ""
	end
	UH.SetText(self.CurTrainLevelValue, attribute_value)
	UH.SetText(self.CurTrainLevelText, Language.LvDot(data.level))
	--有下一级就设置下一级的，没有就设置当前等级的
	self.curCostCoinNum = character_data_next and character_data_next.train_ball_consume or character_data.train_ball_consume
end

function TrainInfoPanel:setNextTrainLevelInfo(data , character_data_next)
	if not self.cur_is_max then
		self.NextTrainLevelInfoObj:SetActive(true)
		self.TrainArrow:SetActive(true)
		self.NextSkillInfoShowObj:SetActive(true)
		--下一级信息
		UH.SetText(self.NextTrainLevelText, Language.LvDot(data.level + 1))
		if character_data_next.attribute_value then
			if not StringIsEmpty(character_data_next.attr_name) then
				UH.SetText(self.NextTrainLevelValue, "+"..character_data_next.attribute_value)
			else
				UH.SetText(self.NextTrainLevelValue, tostring(character_data_next.attribute_value / 100).."%")
			end
		else
			UH.SetText(self.NextTrainLevelValue)
		end
		UH.SetText(self.LimitLevelText)
	else
		self.NextTrainLevelInfoObj:SetActive(false)
		self.TrainArrow:SetActive(false)
		self.NextSkillInfoShowObj:SetActive(false)
		UH.SetText(self.LimitLevelText, Language.Role.XiulianMaxStr)
		UH.AnchoredPosition(self.LimitLevelTextRect, Vector3.New(276.6, -279.6, 0))
	end
end

function TrainInfoPanel:setNextShowInfo(data, character_data_next)
	if not self.cur_is_max then
		self.NextSkillInfoShowObj:SetActive(true)
		self.NextTrainLevelInfoObj:SetActive(true)
		self.TrainArrow:SetActive(true)
		--下一级信息
		if character_data_next.attribute_value then
			if not StringIsEmpty(character_data_next.attr_name) then
				UH.SetText(self.NextShowTrainLevelValue, "+"..character_data_next.attribute_value)
			else
				UH.SetText(self.NextShowTrainLevelValue, tostring(character_data_next.attribute_value / 100).."%")
			end
		else
			UH.SetText(self.NextShowTrainLevelValue)
		end
		UH.SetText(self.NextShowTrainLevelText, Language.LvDot(data.level + 1))
		UH.SetText(self.LimitLevelText)
	else
		self.NextSkillInfoShowObj:SetActive(false)
	end
end

--刷新修炼按钮状态
function TrainInfoPanel:FlushTrainButton()
	UH.SetText(self.CostCoinNumText, tostring(self.curCostCoinNum))
end

function TrainInfoPanel:checkCanUpgrade()
	if Item.GetNum(self.train_character_data.item_id) < self.train_character_data.train_ball_consume then
		ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.train_character_data.item_id})})
		return false
	end
	local allowMaxTrainLevel, nextTrainNeedCfg = RoleSkillData.Instance:GetAllowMaxLevel(RoleData.Instance:GetRoleLevel(), RoleSkillData.Instance.current_train_data.train_type)
	if self.cur_train_data.level >= allowMaxTrainLevel and nextTrainNeedCfg then
		PublicPopupCtrl.Instance:Center(string.format(Language.Role.XiulianLevelNotEnough, nextTrainNeedCfg.character_level or ""))
		return false
	end
	return true
end

function TrainInfoPanel:OnClickAutoTrain()
	if not self:checkCanUpgrade() then
		return
	end
	local can_up_level, learn_times, use_num = RoleSkillData.Instance:GetTrainCanMaxLevel()
	if use_num == 0 then
		PublicPopupCtrl.Instance:Center(Language.Role.trainSkillNotEnoughItem)
		return
	end
	-- LogError("can_up_level",can_up_level,"learn_times",learn_times,"use_num", use_num)
	local info = {
		content = string.format(Language.Role.trainSkillCanLearnTo, can_up_level),
		item_id = HuoBi[CurrencyType.SkyGlass],
		confirm = {
			name = string.format(Language.Role.trainSkillBtnTip, use_num),
			func = function()
				RoleCtrl.Instance:SendCSTrainReq(0, RoleSkillData.Instance.current_train_data.train_type, learn_times)
			end
		},
		tnr = DTTodayNotRemind.RoleSkillTrainAutoLearn,
	}
	RoleCtrl.Instance:OpenAutoLearnDialogTip(info)
end

--点击修炼按钮
function TrainInfoPanel:OnClickXiulianButton()
	if not self:checkCanUpgrade() then
		return
	end
	-- self.EffectTool:Play(10010069)

	if RoleSkillData.Instance:TrainSkillCanAndNoActive() then
		RoleSkillData.Instance.smart_data.first_learn = true
	else
		RoleCtrl.Instance:SendCSTrainReq(0, RoleSkillData.Instance.current_train_data.train_type, 1)
	end

end

function TrainInfoPanel:OnClickToggle(toggle_val)
	RoleCtrl.Instance:SendCSTrainReq(3,RoleSkillData.Instance.current_train_data.train_type)
end
----------------------------------------------------------
--左侧技能类型单元格
--每个技能是子单元
---grid_par=====UIWidgetList
RoleViewSkillContentItem = RoleViewSkillContentItem or DeclareMono("RoleViewSkillContentItem",UIWidgetBaseItem)
function RoleViewSkillContentItem:SetData(data)
	if data.sort_type == 0 then
		UH.SetText(self.SkillTypeName, Language.Role.zhiyeSkill)
	elseif data.sort_type == 1 then
		UH.SetText(self.SkillTypeName, Language.Role.tongyongSkill)
	elseif data.sort_type == 2 then
		UH.SetText(self.SkillTypeName, Language.Role.zhuanjingSkill)
	elseif data.sort_type == 3 then
		UH.SetText(self.SkillTypeName, Language.Role.caijiSkill)
	elseif data.sort_type == 4 then
		UH.SetText(self.SkillTypeName, Language.Role.assistSkill)
	else
		self:ShowTrainSkillEffect()
		UH.SetText(self.SkillTypeName, Language.Role.xiulianSkill)
	end
	--table.sort(data.list, DataSorter.KeyUpperSorter("is_recommend"))
	if SkillData.Instance.tab == 1 then
		self.grid_par:SetData(data.list)
		--当前记录的选择是修炼技能时，不自动选择某一个技能
		if data.sort_type ~= 4 and data.sort_type ~= 5 and SkillData.Instance.change_data.rightType ~= 2 and SkillData.Instance.change_data.rightType ~= 3 then
			local function isHave(list,click_id)
				for k, v in pairs(list)do
					if v.skill_id == click_id then
						return v
					end
				end
				return nil
			end
		
			local click_id = RoleCtrl.Instance.skill_data:GetClickSkillId()
			local da = isHave(data.list,click_id)
			if click_id ~= 0 and da then
				self.grid_par:ClickItemData(da)
			elseif click_id ~= 0 and da == nil then
				SkillData.Instance.pre_click_id = click_id
				if SkillData.Instance.cur_click_list ==  data.cur_list and data.sort_type ~= 1 then
					self.grid_par:ClickItem(1)
				end
			else
				if data.cur_list == 1 then
					-- 职业技能这里不默认选中第一个
					-- self.grid_par:ClickItem(1)
				end
			end
		end
	elseif SkillData.Instance.tab == 2 then
		table.sort(data.list,DataSorter.KeyLowerSorter("index2"))
		self.grid_par:SetData(data.list)
		if data.cur_list == 1  then
			self.grid_par:ClickItem(1)
		end
	end
end

function RoleViewSkillContentItem:PlayFirstLearnEffect()
	if RoleSkillData.Instance.smart_data.first_learn then
		self.learn_eff_handle = self.EffectTool:Play(10010069)
	end
	self.ClickFilter:SetActive(RoleSkillData.Instance.smart_data.first_learn)
end

function RoleViewSkillContentItem:ShowTrainSkillEffect()
	self.ClickFilter:SetActive(RoleSkillData.Instance.smart_data.first_learn)
	self.FilterRect.sizeDelta = self.Rect.sizeDelta
	if RoleSkillData.Instance:TrainSkillCanAndNoActive() then
		if self.normal_eff_handle == nil then
			self.normal_eff_handle = self.EffectTool:Play(10010068)
		end
		if self.train_learn_care_handle == nil then
			self.train_learn_care_handle = RoleSkillData.Instance.smart_data:KeysCare({"first_learn"},BindTool.Bind1(self.PlayFirstLearnEffect, self))
		end
	end
end

function RoleViewSkillContentItem:OnEffectEnd()
	if self.learn_eff_handle then
		self.EffectTool:Stop(self.learn_eff_handle)
	end
	if self.normal_eff_handle then
		self.EffectTool:Stop(self.normal_eff_handle)
	end
	self.learn_eff_handle = nil
	self.normal_eff_handle = nil
	RoleCtrl.Instance:SendCSTrainReq(0, RoleSkillData.Instance.current_train_data.train_type, 1)
end

function RoleViewSkillContentItem:OnDestroy()
	if self.train_learn_care_handle ~= nil then
		RoleSkillData.Instance.smart_data:Uncare(self.train_learn_care_handle)
		self.train_learn_care_handle = nil
	end
	RoleSkillData.Instance.smart_data.first_learn = false
	self.normal_eff_handle = nil
	self.learn_eff_handle = nil
end

function RoleViewSkillContentItem:FlushMask()
	for k, v in pairs(self.grid_par.item_list) do
		v:FlushMask()
	end
end
--------------左侧技能图标-------------------------
RoleViewSkillGridItem = RoleViewSkillGridItem or DeclareMono("RoleViewSkillGridItem",UIWidgetBaseItem)
-------ClickEvent=====LuaUIEvent
-------RedPoint=======GameObject
function RoleViewSkillGridItem:RoleViewSkillGridItem()
	self.red_point = nil
	self.is_first = true
end

function RoleViewSkillGridItem:OnDestroy()
	if self.red_point ~= nil then
		self.red_point:DeleteMe()
		self.red_point = nil
	end
	if self.open_red_point ~= nil then
		self.open_red_point:DeleteMe()
		self.open_red_point = nil
	end
	if self.item_grid_care_handle ~= nil then
		BagData.Instance.item_grid:Uncare(self.item_grid_care_handle)
		self.item_grid_care_handle = nil
		RoleSkillData.Instance.train_red_dot_flag:Uncare(self.notice_flag_care_handle)
	end
	if self.prof_skill_notice_care_handle ~= nil then
		RoleSkillData.Instance.prof_skill_red_notice:Uncare(self.prof_skill_notice_care_handle)
		self.prof_skill_notice_care_handle = nil
	end
	UIWidgetBaseItem.OnDestroy(self)
end

function RoleViewSkillGridItem:FlushMask()
	if self.data == nil then
		return
	end
	if self:Destroyed() then
		if self.red_point ~= nil then
			self.red_point:DeleteMe()
			self.red_point = nil
		end
		if self.open_red_point ~= nil then
			self.open_red_point:DeleteMe()
			self.open_red_point = nil
		end
		return
	end
	if self.data.skill_id ~= nil then
		local skill_data = RoleCtrl.Instance.skill_data:getSkillDataById(self.data.skill_id) or {}
		if SkillData.Instance.tab == 1 then
			local is_limit, limitType, limitStr = SkillData.Instance:GetSkillLimit(self.data.skill_id)
			if is_limit and limitType == RoleSkillLimitType.RoleLevel then
				UH.SetText(self.MaskText, string.format(Language.Role.RoleLevelCanLearn, limitStr))
				self.MaskBg:SetActive(true)
			else
				self.MaskBg:SetActive(false)
			end
			if not next(skill_data) then
				self.InteractorHandler.Interactable = false
			else
				self.InteractorHandler.Interactable = true
			end
		elseif SkillData.Instance.tab == 2 then
			local skill_data = SkillData.Instance:GetLifeSkillDataFromServer(self.data.skill_id)
			self.MaskBg:SetActive(false)
			if skill_data.skill_level == 0 and skill_data.skill_exp == 0 then
				self.InteractorHandler.Interactable = false
			else
				self.InteractorHandler.Interactable = true
			end
		end

	elseif self.data.train_type ~= nil then
		self.fun_open_level = 0
		if self.data.train_type > 1000 then
			self.fun_is_opend, self.fun_open_level = RoleSkillData.Instance:GetTrainCanLearn(self.data.train_type, 0)
		elseif self.data.train_type > 999 then
			local pet_train_cfg = RoleSkillData.Instance:GetCharacterLevelData(self.data.train_type, 1)
			self.fun_is_opend = RoleData.Instance:GetRoleLevel() >= pet_train_cfg.skill_learn_level
			self.fun_open_level = pet_train_cfg.skill_learn_level			
		else
			self.fun_is_opend = RoleSkillData.Instance:GetTrainTypeIsOpen(self.data.train_type)
			self.fun_open_level = RoleSkillData.Instance:GetTrainTypeOpenLevel(self.data.train_type)
		end
		if self.fun_is_opend then
			local notice_flag = RoleSkillData.Instance:GetReddotNoticeFlag(self.data.train_type)
			self.MaskBg:SetActive(false)
			self.InteractorHandler.Interactable = true
			if self.data.skill_type == 1 then
				if RoleSkillData.Instance:TrainSkillCanAndNoActive() then
					self.InteractorHandler.Interactable = false
				end
				if notice_flag then
					self:FlushRedPoint()
				end
				if self.item_grid_care_handle == nil then
					self.item_grid_care_handle = BagData.Instance.item_grid:Care(BindTool.Bind1(self.FlushRedPoint, self))
					self.notice_flag_care_handle = RoleSkillData.Instance.train_red_dot_flag:Care(BindTool.Bind1(self.FlushRedPoint, self))
				end
			else
			-- 	if not UnityPlayerPrefs.HasKey("RoleTrain"..tostring(self.fun_open_level)..role_id) then
			-- 		self:SetRedPoint(1)
			-- 	else
					self:SetRedPoint(0)
			-- 	end
			end
		else
			self.MaskBg:SetActive(true)
			UH.SetText(self.MaskText, string.format(Language.Role.RoleLevelCanLearn, self.fun_open_level))
			self.InteractorHandler.Interactable = false
		end
	end
end

function RoleViewSkillGridItem:FlushRedPoint()
	local notice_flag = RoleSkillData.Instance:GetReddotNoticeFlag(self.data.train_type)
	local item_num = Item.GetNum(Config.train_auto.other[1].item_id)
	if item_num > 0 and RoleSkillData.Instance:GetTrainTypeRemind(self.data.train_type, item_num) > 0 and notice_flag then
		self:SetRedPoint(1)
	else
		self:SetRedPoint(0)
	end
end

function RoleViewSkillGridItem:SetRedPoint(num)
	if self.open_red_point ~= nil or num > 0 then
		self.open_red_point = self.open_red_point or UiRedPoint.New(self.RedPoint)
		self.open_red_point:SetNum(num)
	end
end

function RoleViewSkillGridItem:SetData(data)
	-- local seted = self.data ~= nil and self.data.skill_id == data.skill_id
	self.data = data
	self.up_bg:SetActive(false)
	local skill_data = nil
	if data.skill_id then
		skill_data = RoleCtrl.Instance.skill_data:getSkillDataById(data.skill_id)
	else
		skill_data = RoleSkillData.Instance:getTrainDataByType(data.train_type)
	end
	
	if SkillData.Instance.tab == 2 then
		local skill_cfg = Cfg.SkillDescLife(self.data.skill_id, self.data.skill_level)
		if skill_cfg then
			UH.SetIcon(self.SkillIcon, skill_cfg.icon_id,ICON_TYPE.SKILL)
			UH.SetText(self.SkillName, skill_cfg.skill_name)
		end
		if self.red_point ~= nil then
			self.red_point:SetNum(0)
		end
	else
		if data.skill_id ~= nil then
			--技能名称及icon
			local config = Cfg.SkillDescPartnerAndRole(self.data.skill_id, 1)
			if config ~= nil then
				if IS_AUDIT_VERSION then
					local audit_skill_index = SkillData.GetAuditSkillIndex(self.data.skill_id)
					if audit_skill_index then
						local img_path, stream_has = ChannelAgentConsts.GetUIStremingAsset("RoleSkillIcon"..audit_skill_index)
						if stream_has then
							self.SkillIconRawObj:SetActive(true)
							UH.SetIcon(self.SkillIcon)
							ChannelAgentConsts.SetRawImg(self.SkillIconRaw, img_path)
						else
							self.SkillIconRawObj:SetActive(false)
							UH.SetIcon(self.SkillIcon, config.icon_id,ICON_TYPE.SKILL)
						end
					else
						self.SkillIconRawObj:SetActive(false)
						UH.SetIcon(self.SkillIcon, config.icon_id,ICON_TYPE.SKILL)
					end
				else
					self.SkillIconRawObj:SetActive(false)
					UH.SetIcon(self.SkillIcon, config.icon_id,ICON_TYPE.SKILL)
				end
				UH.SetText(self.SkillName, config.skill_name)
			end

			--红点
			local is_limit = SkillData.Instance:GetSkillLimit(data.skill_id)
			if not is_limit then-- or limitType = RoleSkillLimitType.RoleLevel then
				local coin_consume = 0
				if not TableIsEmpty(skill_data) then
					local next_skill_cfg = SkillData.GetConfig(skill_data.skill_id, skill_data.skill_level + 1) or {}
					-- 职业技能检测铜币是否足够升一级
					coin_consume = next_skill_cfg.coin_consume or 0
				end
				
				local has_coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind) or 0
				-- 铜币够升级
				if has_coin >= coin_consume then
					-- 职业技能激活红点
					if SkillData.Instance:getSkillDataById(self.data.skill_id) == nil then
						if self.red_point == nil then self.red_point = UiRedPoint.New(self.RedPoint) end
						self.red_point:SetNum(1)

					-- 职业技能红点已经提醒过了
					elseif RoleSkillData.Instance.prof_skill_red_notice.clicked == false and RoleSkillData.Instance.prof_skill_red_notice[data.skill_id] == nil then
						if self.red_point == nil then self.red_point = UiRedPoint.New(self.RedPoint) end
						self.red_point:SetNum(1)
						-- RoleSkillData.Instance.prof_skill_red_notice[data.skill_id] = 1
					elseif self.red_point ~= nil then
						self.red_point:SetNum(0)
					end
				elseif self.red_point ~= nil then
					self.red_point:SetNum(0)
				end
			elseif self.red_point ~= nil then
				self.red_point:SetNum(0)
			end
			-- if self.prof_skill_notice_care_handle == nil then
			-- 	self.prof_skill_notice_care_handle = RoleSkillData.Instance.prof_skill_red_notice:KeysCare({"clicked"}, function ()
			-- 		self:FlushRedNoticeCare()
			-- 	end)
			-- end
		elseif data.train_type ~= nil then
			--修炼及辅助技能
			local train_attr_name = RoleSkillData.Instance:GetTrainAttrName(data.train_type, RoleData.Instance:ProfType())
			UH.SetText(self.SkillName, train_attr_name ~= nil and train_attr_name..Language.Role.Tab3 or data.train_name)
			data.train_name = train_attr_name ~= nil and train_attr_name..Language.Role.Tab3 or data.train_name
			UH.SetIcon(self.SkillIcon, data.icon, ICON_TYPE.SKILL)
			if self.prof_skill_notice_care_handle ~= nil then
				RoleSkillData.Instance.prof_skill_red_notice:Uncare(self.prof_skill_notice_care_handle)
				self.prof_skill_notice_care_handle = nil
			end
		end
	end
	self:FlushMask()

	local lv
	if SkillData.Instance.tab == 2 then
		lv = self.data.skill_level or 0
	else
		if skill_data then
			if skill_data.skill_level then
				lv = skill_data.skill_level
			elseif skill_data.level then
				lv = skill_data.level
			else
				lv = 0
			end
		else
			lv = 0
		end
	end
	if SkillData.RoleViewShowPassiveSkill[data.skill_id] then
		UH.SetText(self.ItemLvText, "")
		self.PassiveObj:SetActive(true)
	else
		self.LevelBgRect.sizeDelta = Vector2.New(lv >= 10 and 77 or 70, 38)
		local max_skill_level = ""
		if data.train_type ~= nil then
			max_skill_level = RoleSkillData.Instance:GetAllowMaxLevel(RoleData.Instance:GetRoleLevel(), data.train_type)
			--辅助技能红点
			if lv < max_skill_level and data.skill_type == 0 then
				local has_coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind) or 0
				local assist_cfg_next = RoleSkillData.Instance:GetCharacterLevelData(data.train_type, lv + 1)

				local need_times = math.ceil((assist_cfg_next.need_exp - (skill_data and skill_data.exp or 0)) / assist_cfg_next.get_exp)
				need_times = need_times > 0 and need_times or 1
				-- 铜币够升级
				if assist_cfg_next and assist_cfg_next.bind_coin_consume * need_times <= has_coin then
					if self.red_point == nil then
						self.red_point = UiRedPoint.New(self.RedPoint)
					end
					self.red_point:SetNum(1)
				elseif self.red_point ~= nil then
					self.red_point:SetNum(0)
				end
			elseif self.red_point ~= nil then
				self.red_point:SetNum(0)
			end
			
		elseif skill_data then
			local advance_cfg = RolePromoteData.GetRoleProfConfig()
			for i= skill_data.skill_level, advance_cfg.max_skill_level do
				local skill_cfg = SkillData.GetConfig(data.skill_id, i)
				if skill_cfg and skill_cfg.skill_learn_level and skill_cfg.skill_learn_level > RoleData.Instance:GetRoleLevel() then
					max_skill_level = i - 1
					break
				elseif i == advance_cfg.max_skill_level then
					max_skill_level = i
				end
			end
			-- max_skill_level = advance_cfg.max_skill_level
		end
		-- 主动技能等级不能手动升级
		if data.train_type ~= nil then
			UH.SetText(self.ItemLvText, Format(Language.Common.Slash, lv, max_skill_level))
		else
			-- 主动技能
			UH.SetText(self.ItemLvText, Format(Language.Role.RoleLevelLv, lv))
			-- 技能等级改变播放特效
			if self.is_first then
				self.last_lv = lv
				self.is_first = false
			else
				if self.last_lv ~= lv then
					self:PlayEffect()
					self.last_lv = lv
				end
			end
		end
		self.PassiveObj:SetActive(false)
	end
	self.ItemLvBg:SetActive(lv > 0)
end

function RoleViewSkillGridItem:FlushRedNoticeCare()
	if self.data.skill_id ~= nil and self.data.train_type == nil and RoleSkillData.Instance.prof_skill_red_notice[self.data.skill_id] == 1 then
		if self.red_point ~= nil then
			self.red_point:SetNum(0)
		end
	end
end

-- 升级特效
function RoleViewSkillGridItem:PlayEffect()
    self:StopEffect()
    self.effect = self.EffectTool:Play(4165110)
end

function RoleViewSkillGridItem:StopEffect()
    if self.effect then
        self.EffectTool:Stop(self.effect)
        self.effect = nil
    end
end

function RoleViewSkillGridItem:Click()
	-- 屏蔽点击即消失的提示红点 11.26 by Mr. Hu
	-- if RoleSkillData.Instance.prof_skill_red_notice[self.data.skill_id] == 1 and not RoleSkillData.Instance.prof_skill_red_notice.clicked then
	-- 	RoleSkillData.Instance.prof_skill_red_notice.clicked = true
	-- 	SkillData.Instance.change_data:SetDirty("change")
	-- end
	self.ClickEvent:Trigger()
end

function RoleViewSkillGridItem:onClick()
	if self.data.skill_id ~= nil then
		RoleCtrl.Instance.skill_data:SetClickSkillId(self.data.skill_id)
		SkillData.Instance:SetRightType(1)
		-- 屏蔽点击即消失的提示红点 11.26 by Mr. Hu
		-- if RoleSkillData.Instance.prof_skill_red_notice[self.data.skill_id] == 1 and not RoleSkillData.Instance.prof_skill_red_notice.clicked then
		-- 	RoleSkillData.Instance.prof_skill_red_notice.clicked = true
		-- 	SkillData.Instance.change_data:SetDirty("change")
		-- end
	elseif self.data.train_type ~= nil then
		RoleSkillData.Instance:SetCurTrainType(self.data.train_type)
		if self.data.skill_type == 0 then
			SkillData.Instance:SetRightType(2)
		else
			SkillData.Instance:SetRightType(3)
		end
		-- local role_id = RoleData.Instance:GetRoleId()
		-- UnityPlayerPrefs.SetInt("RoleTrain"..tostring(self.fun_open_level)..role_id, 0)
		-- self:SetRedPoint(0)
	end
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	SkillData.Instance.cur_click_list = self.data.cur_list
end

----------------------------------------------------------
--右侧技能信息列表格子（每个技能的所有等级的信息列表）飞升技能也是用了这个
RoleViewSkillInfoItem = RoleViewSkillInfoItem or DeclareMono("RoleViewSkillInfoItem",UIWidgetBaseItem)
-- LevelText = Text
-- ExpProgress = UIWProgressBarEx
-- ItemInterActor = InteractorHandler
-- ItemElement = LayoutElement
-- DescText = Text
-- CostManaText = Text
-- UseLimitText = Text
-- InfoItemBgSprite = UIImageSpriteSet
-- ArrowDown = GameObject
-- ExpProgressObj = GameObject
function RoleViewSkillInfoItem:RoleViewSkillInfoItem()
end

function RoleViewSkillInfoItem:SetData(data)
	local not_fade = self.data == nil or self.data.skill_id ~= data.skill_id or self.data.skill_level ~= data.skill_level
	local skillDescCfg = nil
	local skillInfoCfg = nil
	--拿低一级的升级所需经验
	local need_exp = 1
	if SkillData.Instance.tab == 2 then
		skillDescCfg = Cfg.SkillDescLife(data.skill_id, data.skill_level)
		skillInfoCfg = Cfg.SkillLife(data.skill_id, data.skill_level)
		local lifeSkillData =  SkillData.Instance:GetLifeSkillDataFromServer(data.skill_id)
		if lifeSkillData then
			if lifeSkillData.skill_exp > 1 or data.skill_level >= 1 then
				local preLevelConfig = Cfg.SkillLife(data.skill_id, data.skill_level >= 1 and data.skill_level - 1 or 0)
				if preLevelConfig ~= nil then
					need_exp = preLevelConfig.up_level_need_exp
				end
			end
			
			if lifeSkillData.skill_level == 0 then
				if lifeSkillData.skill_exp == 0 then
					self.Lock:SetActive(true)
					self.ProgressTextObj:SetActive(false)
				else
					self.Lock:SetActive(false)
					self.ProgressTextObj:SetActive(true)
				end
			else
				self.Lock:SetActive(false)
				self.ProgressTextObj:SetActive(true)
			end
		end
	elseif SkillData.Instance.tab == 1 then
		skillDescCfg = Cfg.SkillDescPartnerAndRole(data.skill_id, data.skill_level)
		local skillInfoCfgFunc = SkillData.idToTable(data.skill_id)
		skillInfoCfg = skillInfoCfgFunc(data.skill_id, data.skill_level)
		if data.skill_level > 1 then
			local preLevelConfig = skillInfoCfgFunc(data.skill_id, data.skill_level)
			if preLevelConfig ~= nil then
				need_exp = preLevelConfig.up_level_need_exp
			end
		end
		if not next(data.curRoleSkillData) then
			self.Lock:SetActive(true)
			self.ProgressTextObj:SetActive(false)
		else
			self.Lock:SetActive(false)
			self.ProgressTextObj:SetActive(true)
		end
	elseif SkillData.Instance.tab == 3 then
		skillDescCfg = SkillData.GetDecConfig(data.skill_id, data.skill_level) --统一技能描述接口 SkillData.GetDecConfig
		skillInfoCfg = SkillData.GetConfig(data.skill_id, data.skill_level)    --统一技能配置接口 SkillData.GetConfig
	end
	
	if skillDescCfg == nil or skillInfoCfg == nil then
		LogError("skillDescCfg or skillInfoCfg is nil of id = ",data.skill_id, " leve = ", data.skill_level, skillDescCfg == nil, skillInfoCfg == nil)
		return
	end

	UH.SetText(self.LevelText, string.format(Language.Common.LvDot, tonumber(data.skill_level)))
	UH.SetText(self.DescText, skillDescCfg.description)
	if skillInfoCfg.consume_mp and skillInfoCfg.consume_mp ~= 0 then
		UH.SetText(self.CostManaText, Language.Role.haolan..skillInfoCfg.consume_mp)
	else
		UH.SetText(self.CostManaText,"")
	end

	if skillInfoCfg.consume_sp and skillInfoCfg.consume_sp ~= 0 then
		UH.SetText(self.CostManaText, Language.Role.Anger..skillInfoCfg.consume_sp)
	else
		UH.SetText(self.CostManaText,"")
	end

	if self.RageList then
		if skillInfoCfg.consume_rage_point and skillInfoCfg.consume_rage_point > 0 then
			UH.AnchoredPosition(self.ManaRect,Vector2.New(-200,-85))
			self.RageList:SetObjActive(true)
			self.RageList:SetValue(skillInfoCfg.consume_rage_point)
		else
			self.RageList:SetObjActive(false)
			UH.AnchoredPosition(self.ManaRect,Vector2.New(-200,-96))
		end
	end
	UH.SetText(self.UseLimitText, skillDescCfg.use_limit == Language.Common.Wu and "" or skillDescCfg.use_limit)
	if next(data.curRoleSkillData) then
		UH.SpriteName(self.InfoItemBgSprite, data.skill_level ~= data.curRoleSkillData.skill_level and "activedBg" or "curSelectBg")
		self.ItemInterActor.Interactable = data.skill_level <= data.curRoleSkillData.skill_level
		if self.ArrowDown then
			self.ArrowDown:SetActive(data.skill_level == data.curRoleSkillData.skill_level and data.skill_level ~= data.maxLevel)
		end
		--新旧数据对比（用于判断升级后播放特效）
		if self.data and self.data.curRoleSkillData and next(self.data.curRoleSkillData) then
			-- self.data.curRoleSkillData.skill_level ~= data.curRoleSkillData.skill_level,
			-- self.data.skill_level == self.data.curRoleSkillData.skill_level)
			if self.data.curRoleSkillData.skill_id == data.curRoleSkillData.skill_id
				and self.data.curRoleSkillData.skill_level ~= data.curRoleSkillData.skill_level
				and	self.data.skill_level == data.curRoleSkillData.skill_level then
				-- 这里不飘字，不播特效
				-- self:PlayUpLevelEffect(skillDescCfg)
			end
		end
		--还需判断是否满足继续学习的条件
		local is_limit = false
		if SkillData.Instance.tab == 1 and data.limit_level == nil then
			is_limit = SkillData.Instance:GetSkillLimit(data.skill_id)
		end
		if data.skill_level == data.curRoleSkillData.skill_level + 1 and data.skill_level <= data.maxLevel and not is_limit then
			if data.curRoleSkillData.skill_exp then
				-- self.ItemElement.preferredHeight = 190
				-- self.ExpProgressObj:SetActive(true)
				if not_fade then
					self.ExpProgress:SetProgress(data.curRoleSkillData.skill_exp / need_exp)
				else
					self.ExpProgress:AddToProgress(data.curRoleSkillData.skill_exp / need_exp)
				end
				self.ExpProgress:SetText(data.curRoleSkillData.skill_exp.."/"..need_exp)
			else
				-- self.ExpProgressObj:SetActive(false)
				-- self.ItemElement.preferredHeight = 150
			end
		elseif data.skill_level == data.curRoleSkillData.skill_level then
			if data.skill_level == data.maxLevel then
				-- self.ItemElement.preferredHeight = 150
			else
				-- self.ItemElement.preferredHeight = 170
			end
			-- self.ExpProgressObj:SetActive(false)
		else
			-- self.ExpProgressObj:SetActive(false)
			-- self.ItemElement.preferredHeight = 150
		end
	else
		-- self.ExpProgressObj:SetActive(false)
		-- self.ItemElement.preferredHeight = 150
		UH.SpriteName(self.InfoItemBgSprite, "activedBg")
		self.ItemInterActor.Interactable = false
		-- self.ItemElement.preferredHeight = 150
		if self.ArrowDown then
			self.ArrowDown:SetActive(false)
		end
	end
	if data.skill_level == 1 then
		-- self.ItemElement.preferredHeight = self.ItemElement.preferredHeight + 10
		-- UH.AnchoredPosition(self.OffsetRect, Vector3.New(0,-85,0))
	end
	self.data = data
end

function RoleViewSkillInfoItem:PlayUpLevelEffect(skillDescCfg)
	PublicPopupCtrl.Instance:Center(string.format(Language.Role.SkillUptoLevel, skillDescCfg.skill_name, tostring(self.data.skill_level)))
	SkillData.Instance.role_skill_info_list_slide = false
	self.InfoItemEffectTool:Play(10010298)
	if self.data.skill_level > 1 then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
	end
end

-------------------------------------------------------------
RoleTrainSkillTipsView = RoleTrainSkillTipsView or DeclareView("RoleTrainSkillTipsView","role/role_train_skill_tips")
VIEW_DECLARE_MASK(RoleTrainSkillTipsView, ViewMask.BgBlockClose)
function RoleTrainSkillTipsView:RoleTrainSkillTipsView()
end

function RoleTrainSkillTipsView:LoadCallback(param)
	UH.SpriteName(self.CostNumImg, param.item_id)
	UH.SetText(self.Content, param.content)
	UH.SetText(self.ConfirmName, param.confirm.name)
	self.tnr = param.tnr
	self.confirm_func = param.confirm.func
	TimeHelper:AddDelayFrameTimer(function ()
		self.ContentSizeFitter.enabled = true
	end,2)
end

function RoleTrainSkillTipsView:OnClickConfirm()
	if nil ~= self.tnr then
        DialogTipsData.Instance.today_not_reminds[self.tnr..RoleData.Instance:GetRoleId()] = self.TodayNotRemind.isOn
    end
	if self.confirm_func ~= nil then
		self.confirm_func()
	end
	ViewMgr:CloseView(RoleTrainSkillTipsView)
end