
HarmonyHeavenRecordView = HarmonyHeavenRecordView or DeclareView("HarmonyHeavenRecordView", HarmonyHeavenConfig.ResPath .. "harmony_heaven_record", Mod.HarmonyHeaven.Record)

VIEW_DECLARE_LEVEL(HarmonyHeavenRecordView,ViewLevel.L0)

function HarmonyHeavenRecordView:HarmonyHeavenRecordView()
	self.data = HarmonyHeavenCtrl.Instance:Data()
end

function HarmonyHeavenRecordView:LoadCallback()
end

function HarmonyHeavenRecordView:OnClickClose()
    ViewMgr:CloseView(HarmonyHeavenRecordView)
end

HarmonyHeavenRecordViewF = HarmonyHeavenRecordViewF or DeclareMono("HarmonyHeavenRecordViewF", UIWFlushPanel)

function HarmonyHeavenRecordViewF:HarmonyHeavenRecordViewF()
	self.data = HarmonyHeavenCtrl.Instance:Data()
	self.language = Language.HarmonyHeaven.Record

	self.data_cares = {
		{data = self.data.fb_info.team_info, func = self.FlushShow},
	}
end

function HarmonyHeavenRecordViewF:FlushShow()
	local info = self.data.fb_info.team_info
	local co = Cfg.PromevalParadiseBossFbId(info.fb_id)

	if co then
		local monsters = self.data:GetMonstersShow(co)
		self.NameShow.text = string.format(self.language.NameShow, co.fb_name, self.language.TypeShow[co.fb_type])
		self.RecordList:SetData(monsters)
	end

	for i = 1, self.ShowItemsM:Length() do
		self.ShowItemsM[i]:SetData(info.member_list[i])
	end
	for i = 1, self.ShowItemsI:Length() do
		self.ShowItemsI[i]:SetData(info.member_list[i])
	end
end


HarmonyHeavenRecordViewInfoItem = DeclareMono("HarmonyHeavenRecordViewInfoItem", UIWidgetBaseItem)

function HarmonyHeavenRecordViewInfoItem:HarmonyHeavenRecordViewInfoItem()
	self.hh_data = HarmonyHeavenCtrl.Instance:Data()
	self.language = Language.HarmonyHeaven.Record
end

function HarmonyHeavenRecordViewInfoItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	self.SeqShow.text = string.format(self.language.SeqShow, DataHelper.GetDaXie(data.index))
	self.NameShow.text = data.name
end

function HarmonyHeavenRecordViewInfoItem:OnClickPlay()
    local video = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_HARMONY_HEAVEN, self.data.monster_group_id)
    if video then
        BattleVideo.Instance:PlayVideoCfg(video)
        BattleCtrl.BattleReportDetailInfo(video.monster_group_id, 1)

		self.hh_data:SetHHVideoMapId(self.data.scene_id)
    end    
end


HarmonyHeavenRecordViewItemM = DeclareMono("HarmonyHeavenRecordViewItemM", UIWidgetBaseItem)

function HarmonyHeavenRecordViewItemM:HarmonyHeavenRecordViewItemM()
	self.language = Language.HarmonyHeaven.Record
end

function HarmonyHeavenRecordViewItemM:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	local is_empty = 0 == data.uid
	if not is_empty then
		if not self.ui_obj then
			self.ui_obj = UIChDrawer.New()
			self.ui_obj:SetData(data.appearance)
			self.ModelShow:SetShowData(
				{
					ui_obj = self.ui_obj,
					view = self,
					show_shadow = false,
				}
			)
		else
			self.ui_obj:SetData(data.appearance)
		end
	end
	self.ModelShow:SetObjActive(not is_empty)
end

HarmonyHeavenRecordViewItemI = DeclareMono("HarmonyHeavenRecordViewItemI", UIWidgetBaseItem)

function HarmonyHeavenRecordViewItemI:HarmonyHeavenRecordViewItemI()
	self.info_data = RoleInfoCtrl.Instance:Data()
	self.language = Language.HarmonyHeaven.Record
end

function HarmonyHeavenRecordViewItemI:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

	local is_empty = 0 == data.uid
	if not is_empty then
		local prof_sp = RoleData.GetProfSp(data.prof)
		self.ProfI.SpriteName = prof_sp
		self.Name.text = data.name
		self.Level.text = string.format(self.language.LevelShow, data.level)
	end
	self.InfoObj:SetActive(not is_empty)
	self.EmptyObj:SetActive(is_empty)
end

function HarmonyHeavenRecordViewItemI:OnClickInfo()
	local uid =self.data.uid
	self.info_data.view_equip_uid = uid
	RoleInfoCtrl.Instance:SendReferRoleEquipment(uid)
end
