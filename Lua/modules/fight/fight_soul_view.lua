
FightSoulView = FightSoulView or DeclareView("FightSoulView", FightConfig.ResPath .. "fight_soul", Mod.Fight.Soul)

function FightSoulView:FightSoulView()
	self.data = FightCtrl.Instance:Data()
end

function FightSoulView:LoadCallback()
	-- self.Board:SetData(self:GetType(), Language.Fight.FightSoul, Vector2.New(1240, 610))
end

function FightSoulView:OnClickClose()
	ViewMgr:CloseView(FightSoulView)
end


FightSoulViewF = FightSoulViewF or DeclareMono("FightSoulViewF", UIWFlushPanel)

function FightSoulViewF:FightSoulViewF()
	self.data = FightCtrl.Instance:Data()
	self.language = Language.Fight.FightSoul

	self.data_cares = {
		{data = self.data.fight_soul_sm, func = self.FlushInfo, keys = {"info"}},
		{data = RoleData.Instance.base_data, func = self.FlushInfo, keys = {"level"}, init_call = false},
	}
	-- 心灵之境左侧第一个按钮
	GuideManager.Instance:RegisterGetGuideUi("OfflineRewardFightSoulOne",function ()
        return self.ShowList:GuideVertical(1)
	end)
	-- 心灵之境左侧第二个按钮
	GuideManager.Instance:RegisterGetGuideUi("OfflineRewardFightSoulTwo",function ()
        return self.ShowList:GuideVertical(2)
	end)
	-- 心灵之境挑战按钮
	GuideManager.Instance:RegisterGetGuideUi("OfflineRewardFightSoulFight",function ()
        return self.ObjFight
	end)
	-- 心灵之境关闭按钮
	GuideManager.Instance:RegisterGetGuideUi("OfflineRewardFightSoulClose",function ()
        return self.ObjClose
	end)
end

function FightSoulViewF:Awake()
	UIWFlushPanel.Awake(self)
	
	local show_list, click_item = self.data:GetFightSoulList()
	self.ShowList:SetData(show_list)
	self.ShowList:ClickItemData(click_item)
	self.ShowList:ForceJumpVertical(112 * (self.ShowList:GetByData((click_item)) - 1))

	self.Toggle.isOn = self.data.fight_soul_has_remind.value
end

function FightSoulViewF:FlushInfo()
	local info = self.data.fight_soul_sm.info
    if not next(info) then return end
	
	self.Name.text = info.monster_name
	self.Level.text = string.format(self.language.level, info.level)
	self.Cap.text = string.format("%s/%s", RichTextHelper.ColorStr(RoleData.Instance:GetSumScore(), RoleData.Instance:GetSumScore() < info.recommend_score and COLORSTR.Red5 or COLORSTR.Green9), info.recommend_score)
	self.Desc.text = info.txt
	self.BtnFightInter.Interactable = RoleData.Instance:GetRoleLevel() >= info.level
	self.GetedObj:SetActive(self.data.fight_soul_info.seq >= info.seq)

	local reward_list = self.data:GetFightSoulRewardShow(info)
	self.RewardList:SetData(reward_list)

	local res_path = DrawerHelper.GetNpcPath(info.monster_res)
    local uo = self.ModelShow:UiObj()
    if uo == nil then
        uo = UIObjDrawer.CreateByPath(res_path)
        self.ModelShow:SetShowData(
            {
                ui_obj = uo,
                view = FightRareDungeonView,
                -- load_callback = BindTool.Bind(self.ModelCallback, self),
            }
        )
    else
        uo:SetData(res_path) 
    end
end


function FightSoulViewF:OnClickTips()
	PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[165].desc)
end

function FightSoulViewF:OnClickFight()
	local info = self.data.fight_soul_sm.info
	if not next(info) then return end
	if info.seq ~= 1 and info.seq - 1 > self.data.fight_soul_info.seq then
		PublicPopupCtrl.Instance:Center(Language.Fight.SoulFightTip)
		return
	end
	if RoleLevel() >= info.level then
		FightCtrl.Instance:IsSingleFight(function()
			ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_REALM_BREAK_FIGHT)
			-- FightData.Instance:SoulStoryShow(info)
			-- RoleCtrl.Instance:SendOfflineBreakFight(info.seq)
		end)
	else
		PublicPopupCtrl.Instance:Center(Language.Fight.SoulFightTip2)
	end
end

function FightSoulViewF:OnToggleClick(value)
	self.data.fight_soul_has_remind.value = value
	PrefsInt(PrefKeys.FightSoulRemind(), value and 0 or 1)
	self.data.fight_soul_has_remind:Notify()
	self.Toggle.isOn = self.data.fight_soul_has_remind.value
end

FightSoulViewItem = DeclareMono("FightSoulViewItem", UIWidgetBaseItem)

function FightSoulViewItem:FightSoulViewItem()
	self.fight_data = FightCtrl.Instance:Data()
end

function FightSoulViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	self:uncareData()
	self.care_handles = {}
	self:careData(RoleData.Instance.base_data, BindTool.Bind(self.FlushShow, self), "level")
	self:careData(FightData.Instance.fight_soul_has_remind, BindTool.Bind(self.FlushRedPoint, self))

	self:FlushShow()
end

function FightSoulViewItem:OnDestroy()
    self:uncareData()
end

function FightSoulViewItem:FlushShow()
	local is_lock =  RoleData.Instance:GetRoleLevel() < self.data.level
	UH.SetIcon(self.IconSp, self.data.monster_icon, ICON_TYPE.ITEM)
	self.Name.text = self.data.customs_name
	self.ComObj:SetActive(self.fight_data.fight_soul_info.seq >= self.data.seq)
	self.ItemInter.Interactable = not is_lock
	self.LockObj:SetActive(is_lock)
	self:FlushRedPoint()
end

function FightSoulViewItem:FlushRedPoint()
	self.RedPoint:SetNum(0)
	-- 屏蔽红点
	-- if FightData.Instance.fight_soul_has_remind.value then
	-- 	local is_lock =  RoleData.Instance:GetRoleLevel() < self.data.level
	-- 	self.RedPoint:SetNum(((self.fight_data.fight_soul_info.seq + 1) == self.data.seq and not is_lock) and 1 or 0)
	-- else
	-- 	self.RedPoint:SetNum(0)
	-- end
end

function FightSoulViewItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function FightSoulViewItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected then
        self.fight_data.fight_soul_sm.info:Set(self.data)
    end
end

FightSoulViewRewardItem = DeclareMono("FightSoulViewRewardItem", UIWidgetBaseItem)

function FightSoulViewRewardItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	-- local prof_type = RoleData.Instance:ProfType()
	-- prof_type = prof_type > 0 and prof_type or 1
	-- if data.old_id then
	-- 	local co = Cfg.RewardJobBagByIdType(data.old_id, prof_type)
	-- 	data.item_id = co and co.new_id or data.item_id
	-- 	-- item.show_key = "workshop_equip"
	-- end
    self.Cell:SetData(data)
end
