VIEW_REQ("modules/reputation/reputation_donation_view")
VIEW_REQ("modules/reputation/reputation_batched_donation_view")
VIEW_REQ("modules/reputation/reputation_shop_view")
VIEW_REQ("modules/reputation/reputation_shop_help_view")

ReputationCtrl = ReputationCtrl or BaseClass(BaseCtrl)

function ReputationCtrl:__init()
	if ReputationCtrl.Instance ~= nil then
		Debuger.LogError("[ReputationCtrl] attempt to create singleton twice!")
		return
	end	
	ReputationCtrl.Instance = self
	self.data = PrestigeData.Instance
	self:RegisterAllProtocols()
	self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.OnRoleNoticeComplete, self))
end

function ReputationCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCShopRandListInfo, 'OnSCShopRandListInfo')
    self:RegisterProtocol(SCShopRandListSingleInfo, 'OnSCShopRandListSingleInfo')
end

function ReputationCtrl:OnSCShopRandListInfo(protocol)
	self.data:OnSCShopRandListInfo(protocol)
end

function ReputationCtrl:OnSCShopRandListSingleInfo(protocol)
    self.data:OnSCShopRandListSingleInfo(protocol)
end


--注册所有声望商人的商店逻辑 主城除外
function ReputationCtrl:OnInit()
	local list = Config.prestige_cfg_auto.village_list
	for index, value in ipairs(list) do
		BehaviorData.Instance:CustomNpcTalkBtnsId(value.npc_id, BindTool.Bind(self.SetupBehaviourBtn, self, value))
		BehaviorData.Instance:CustomNpcTalkBtnsId(value.juanzeng_npc, BindTool.Bind(self.SetupBehaviourBtn2, self, value))
	end
end

--监听伙伴觉醒任务特殊处理
function ReputationCtrl:OnRoleNoticeComplete()
	if UnityPlayerPrefs.GetInt("PartnerGuideIndex"  .. RoleData.Instance:GetRoleId()) == 0 and not TaskData.Instance:GetTaskIsCompleted(16051)then
		self.handle_completed = TaskData.Instance:CareCompletedTaskChange(BindTool.Bind(self.OnTaskCompletedChange, self))
	end
end

function ReputationCtrl:OnTaskCompletedChange()
	if TaskData.Instance:GetTaskIsCompleted(16050) then
		local role_id = RoleData.Instance:GetRoleId()
		if UnityPlayerPrefs.GetInt("PartnerGuideIndex"  .. role_id) == 0 then
			UnityPlayerPrefs.SetInt("PartnerGuideIndex" .. role_id,1)
			TaskData.shop_auto_close = true
			local config = Config.function_guide_auto.pet_skill[1]
			PrestigeData.Instance:SetVillageID(config.village_id)
			-- 这个指引位置检索需要村id正确获取含有该道具的列表才可行
			PrestigeData.GuideIndex = self.data:GetItemIndexById(config.moli_juexing)
			TaskData.Instance:UnCareAcceptedTaskChange(self.handle_completed)
		end
	end
end

function ReputationCtrl:OnUnloadGameLogic()
	self.data.donate_cache_num = {}
end

function ReputationCtrl:SetupBehaviourBtn(info, btn_list,obj_vo)
	if obj_vo and obj_vo.id == info.npc_id then
		self.data.village_id = info.village_id
		table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(Language.Reputation.Shop,
		function()
			ViewMgr:OpenView(ReputationShopView)
		end,true))
	end
end

function ReputationCtrl:SetupBehaviourBtn2(info, btn_list,obj_vo)
	if obj_vo and obj_vo.id == info.juanzeng_npc then
		self.data.village_id = info.village_id
		table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(Language.Reputation.Donation,
		function()
			PrestigeData.Instance:SetVillageID(info.village_id or 1)
			ViewMgr:OpenView(ReputationBatchedDonationView)
		end,true))
	end
end

function ReputationCtrl:SetSelectIndex(item_id,village_id)
	self.data:SetVillageID(village_id or 1)
	local index = self.data:GetItemIndexById(item_id)
	self.data:SetSelctItemIndex(index or 1)
end

--商店跳转到某个物品[item_id:物品id]
function ReputationCtrl:OpenSelectIndex(item_id,village_id)
	self:SetSelectIndex(item_id,village_id)
	ViewMgr:OpenViewByKey(Mod.Reputation.Main)
end

--任务打开跳转
function ReputationCtrl:Reputation(shop_id,item_id)
	PrestigeData.GeneralStore = shop_id
	self:SetSelectIndex(PrestigeData.StoreItemId or item_id)
	PrestigeData.StoreItemId = nil
	TaskData.shop_auto_close = item_id ~= 0
end

--指引打开跳转
function ReputationCtrl:GuideIndex(shop_id,item_id)
	PrestigeData.GeneralStore = shop_id
	PrestigeData.GuideIndex = self.data:GetItemIndexById(item_id)
	self:SetSelectIndex(item_id)
end

function ReputationCtrl:GuideStart(split,shop_id)
	local guide_id = tonumber(split[1]) or 0
	local role_id = RoleData.Instance:GetRoleId()
	if UnityPlayerPrefs.GetInt("reputation" .. guide_id .. role_id) == 0 then
		UnityPlayerPrefs.SetInt("reputation" .. guide_id .. role_id,guide_id)
		self:GuideIndex(shop_id,tonumber(split[2]))
		GuideManager.Instance:Start(guide_id)
		TaskData.shop_auto_close = true
	else
		self:Reputation(shop_id,tonumber(split[2]))
		TaskData.shop_auto_close = true
	end
end

function ReputationCtrl:SetOpenData(info,item)
	local config = PrestigeData.Instance:GetVillageByNpcId(info.npc)
	if item == nil or item.item_id == nil then
		item = ItemInfoCtrl.Instance:Data():GetItemInfoData()
	end
	if config ~= nil and item ~= nil then
		self:SetSelectIndex(item.item_id,config.village_id)
		if item.vo ~= nil then
			MallData.Instance:SetJumpItemNum(item.vo.jump_need_num)
		end
	end
	LinkMarkData.Instance:MarkIntroPlus("endtro_mod",Mod.Reputation.Main)
end

function ReputationCtrl.IsRequire(require_type,require_value)
    if require_type == 1 then
        return RoleData.Instance:GetRoleLevel() >= require_value,Format(Language.Reputation.RequireError[require_type],require_value)
    end
    return true
end

function ReputationCtrl:ClearCache()
	self.task_type = nil
end
function ReputationCtrl:SetSelectTaskCache(task_type)
	self.task_type = task_type
end
function ReputationCtrl:GetSelectTaskType()
	return self.task_type
end
