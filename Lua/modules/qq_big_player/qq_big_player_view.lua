
QQBigPlayerView = QQBigPlayerView or DeclareView("QQBigPlayerView", "qq_big_player/qq_big_player_view", Mod.QQBigPlayer.Main)
VIEW_DECLARE_LEVEL(QQBigPlayerView,ViewLevel.L1)

--潜力咖 		加赠、周礼包、游券、绝版特权礼包、		体验卡、生日
--准玩咖		加赠、周礼包、游券、绝版特权礼包、		体验卡、生日
--大玩咖		加赠、周礼包、游券、绝版特权礼包、客服、体验卡、生日
--超级大玩咖	加赠、周礼包、游券、绝版特权礼包、客服、		生日、共享
local showTypeEnum = {
	"ChongZhiJiaZeng",		--充值加赠			1
	"ZhuanShuZhouLi",		--专属周礼			2
	"YouQuan",				--抵扣游券			3
	"reward_item_1",		--绝版特权礼包		4
	"GM",					--专属客服			5
	"TeQuanTiYanZi",		--高级特权体验卡	6
	"birthday",				--生日礼包			7
	"ChongZhiGongXiangZi",	--充值返利共享		8
}

local indexHasType = {
	[1] = "1|1|1|1|0|1|1|0",
	[2] = "1|1|1|1|0|1|1|0",
	[3] = "1|1|1|1|1|1|1|0",
	[4] = "1|1|1|1|1|0|1|1",
}

function QQBigPlayerView:QQBigPlayerView()
	self.data = QQBigPlayerData.Instance
	self.language = Language.QQBigPlayer
	self.runner_get_handle = TimeHelper:AddRunTimer(function ()
		LoginCtrl.Instance:GetQQPCPlayerLevel()
	end,30)
end

function QQBigPlayerView:CloseCallback()
	TimeHelper:CancelTimer(self.runner_get_handle)
	LoginCtrl.Instance:GetQQPCPlayerLevel()
end

function QQBigPlayerView:OnClickClose()
	ViewMgr:CloseView(QQBigPlayerView)
end

function QQBigPlayerView:OnClickActiveBtn()
	ViewMgr:OpenViewByKey(33003)
end

function QQBigPlayerView:OnClickTips()
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = self.language.NoticeTip})
end

-------------------------
QQBigPlayerPanel = QQBigPlayerPanel or DeclareMono("QQBigPlayerPanel", UIWFlushPanel)
function QQBigPlayerPanel:QQBigPlayerPanel()
	self.data_cares = {
		{data = QQBigPlayerData.Instance.qq_big_player_info, func = self.SetPlayerInfo, keys = {"score"}}
	}
	self.data = QQBigPlayerData.Instance
	self.data.cur_select_level = QQBigPlayerData.Instance.qq_big_player_info.level == 0 and 3 or QQBigPlayerData.Instance.qq_big_player_info.level
end

function QQBigPlayerPanel:OnEnable()
	local idx = QQBigPlayerData.LevelToIndex(self.data.qq_big_player_info.level)
	self.Toggles[idx].isOn = true
end

function QQBigPlayerPanel:SetPlayerInfo()
	self:SetShowList()
	UH.SetText(self.Score, self.data.qq_big_player_info.score)
	local cur_select_index = QQBigPlayerData.LevelToIndex(self.data.cur_select_level)
	local info_level_index = QQBigPlayerData.LevelToIndex(self.data.qq_big_player_info.level)
	local lock = false
	if self.data.qq_big_player_info.level == 0 then
		lock = true
	else
		lock = cur_select_index > info_level_index
	end
	-- UH.SpriteName(self.LockState, lock and "_LocWeiJiHuoTeQuan" or "_LocJiHuoTeQuan")
	--LogError("cur_select_level",self.data.cur_select_level)
	local str = lock and Format(Language.QQBigPlayer.LevelNotActive, Language.QQBigPlayer.levelName[self.data.cur_select_level]) or Language.QQBigPlayer.LevelActive
	UH.SetText(self.LockStateTxt, str)
	self.UnlockBtnObj:SetActive(lock)
end

function QQBigPlayerPanel:SetShowList()
	local data_list = {}
	local level_types = string.split(indexHasType[QQBigPlayerData.LevelToIndex(self.data.cur_select_level)],"|")
	local level_cfg = Config.dawanka_auto.basis_configuration[QQBigPlayerData.LevelToIndex(self.data.cur_select_level)]
	for i,v in ipairs(showTypeEnum) do
		if level_types[i] == "1" then
			local t = {show_type = i, str_key = v}
			if i == 2 or i == 4 then
				local items_cfg = i == 2 and level_cfg.reward_item_2 or level_cfg.reward_item_1
				t.item_list = {}
				for _, v in pairs(items_cfg) do
					local item = Item.Init(v.item_id, v.num, v.is_bind)
					item.param = {click_show_type = i}
					table.insert(t.item_list, item)
				end
			end
			table.insert(data_list, t)
		end
	end
	-- LogError("data_list",data_list)
	self.ShowList:SetData(data_list)
end

function QQBigPlayerPanel:OnClickToggle(p1)
	self.data.cur_select_level = p1
	self:SetPlayerInfo()
end

--------------------------
QQBigPlayerListItem = QQBigPlayerListItem or DeclareMono("QQBigPlayerListItem",UIWidgetBaseItem)
function QQBigPlayerListItem:QQBigPlayerListItem()
	self.data_instance = QQBigPlayerData.Instance
end

function QQBigPlayerListItem:SetData(data)
	self.data = data
	UH.SpriteName(self.TitleImg, "_LocTiaoXingKuangBiaoTi"..data.show_type)
	--show_type 2/4有item_list
	if not TableIsEmpty(data.item_list) then
		self.IconObj:SetActive(false)
		UH.SpriteName(self.DescImg,"")
		self.ItemList:SetObjActive(true)
		self.ItemList:SetData(data.item_list)
	else
		local desc_img_name = "_LocDesc"..data.show_type
		if data.show_type == 1 or data.show_type == 3 or data.show_type == 5 then
			desc_img_name = desc_img_name .. "_" .. QQBigPlayerData.LevelToIndex(self.data_instance.cur_select_level)
		end
		UH.SpriteName(self.DescImg, desc_img_name)
		local icon_name = "TypeIcon"..data.show_type
		UH.SpriteName(self.TypeIcon, icon_name)
		self.IconObj:SetActive(true)
		self.ItemList:SetObjActive(false)
	end
end

function QQBigPlayerListItem:OnClickItem()
	if self.data_instance.qq_big_player_info.level <= 0 then
		PublicPopupCtrl.Instance:Center(Language.QQBigPlayer.LevelNotEnough)
		return
	end
	if self.data.show_type == 2 then
		QQBigPlayerCtrl.Instance:SendGetWeekGift()
	elseif self.data.show_type == 3 then
		UnityApp.OpenURL("qqgameprotocol:///openembedwebdialog Caption=游券礼包特权 Width=700 Height=500 New=1 Url=https://minigame.qq.com/dawanka2/#/yqlbtq")
	elseif self.data.show_type == 4 then
		QQBigPlayerCtrl.Instance:SendGetLevelGift()
	elseif self.data.show_type == 5 then
		UnityApp.OpenURL("qqgameprotocol:///openembedwebdialog Caption=专属服务特权 Width=700 Height=500 New=1 Url=https://minigame.qq.com/dawanka2/#/zsfwtq")
	elseif self.data.show_type == 6 then
		UnityApp.OpenURL("qqgameprotocol:///openembedwebdialog Caption=高阶玩咖特权 Width=700 Height=500 New=1 Url=https://minigame.qq.com/dawanka2/#/gjwktq")
	elseif self.data.show_type == 7 then
		UnityApp.OpenURL("qqgameprotocol:///openembedwebdialog Caption=生日专属特权 Width=700 Height=500 New=1 Url=https://minigame.qq.com/dawanka2/#/srzstq")
	elseif self.data.show_type == 8 then
		UnityApp.OpenURL("qqgameprotocol:///openembedwebdialog Caption=充值共享特权 Width=700 Height=500 New=1 Url=https://minigame.qq.com/dawanka2/#/czgxtq")
	end
	LogError("Clicked ShowType",self.data.show_type)
end
-----------------------------------
QQBigPlayerListEffectItemCell = QQBigPlayerListEffectItemCell or DeclareMono("QQBigPlayerListEffectItemCell", UIWidgetBaseItem)
function QQBigPlayerListEffectItemCell:QQBigPlayerListEffectItemCell()
	self.data_instance = QQBigPlayerData.Instance
end

function QQBigPlayerListEffectItemCell:SetData(data)
	self.data = data
	self.ItemCell:SetData(data)
	local show_eff = false
	--周礼包
	if data.param.click_show_type == 2 then
		if self.data_instance.cur_select_level == self.data_instance.qq_big_player_info.level then
			show_eff = self.data_instance:CanGetWeekGift()
		end

	--绝版特权礼包
	elseif data.param.click_show_type == 4 then
		show_eff = self.data_instance:CanGetCurLevelGift()
		--LogError("t",self.data_instance.week_gift_info.tq_data)
	end
	if show_eff then
		if self.eff_handle == nil then
			self.eff_handle = self.EffectTool:Play(3164034)
		end
	elseif self.eff_handle ~= nil then
		self.EffectTool:Stop(self.eff_handle)
		self.eff_handle = nil
	end
end