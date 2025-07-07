GiveSecretKeyView = GiveSecretKeyView or DeclareView("GiveSecretKeyView","serveractivity/give_secret_key_view",Mod.RandActivity.GiveSecretKey)
VIEW_DECLARE_LEVEL(GiveSecretKeyView,ViewLevel.L1)
VIEW_DECLARE_MASK(GiveSecretKeyView,ViewMask.Block)
function GiveSecretKeyView:GiveSecretKeyView()
	self.close_anima_speed = 12
end

function GiveSecretKeyView:OnCloseClick()
	if self.play_closeing then
		return
	end
	local icon_mono = ServerActivityData.Instance:GetGiveSecretKeyActIconMono()
	if icon_mono then
		self.BlockObj:SetActive(false)
		icon_mono:SetEffectState(false)
		self:PlayCloseAnimation(icon_mono.gameObject.transform)
		return
	end
	ViewMgr:CloseView(GiveSecretKeyView)
end

function GiveSecretKeyView:PlayCloseAnimation(icon_trans)
	self.play_closeing = true
	self.Animator.enabled = false
	self.gameObject.transform:SetParent(icon_trans)
	TimeHelper:CancelTimer(self.time_handle)
	self.panel_scale = 1
	self.panel_scale_lerp = self.panel_scale / self.close_anima_speed
	self.view_pos = self.gameObject:GetLocalPosition()
	self.view_pos_x_lerp = (self.view_pos.x + 30) / self.close_anima_speed
	self.view_pos_y_lerp = self.view_pos.y / self.close_anima_speed
	self.canvas_group_alpha = 1 / self.close_anima_speed
	self.time_handle = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.CloseAnimation,self),1,self.close_anima_speed)
end

function GiveSecretKeyView:CloseAnimation()
	self.panel_scale = self.panel_scale - self.panel_scale_lerp
	self.panel_scale = self.panel_scale <= 0.1 and 0.1 or self.panel_scale
	UH.LocalScale(self.PanelTrans,Vector3.New(self.panel_scale,self.panel_scale,self.panel_scale))
	self.view_pos.x = self.view_pos.x - self.view_pos_x_lerp
	self.view_pos.y = self.view_pos.y - self.view_pos_y_lerp
	self.gameObject:SetLocalPosition(self.view_pos)
	self.CanvasGroup.alpha = self.CanvasGroup.alpha - self.canvas_group_alpha

	self.close_anima_speed = self.close_anima_speed - 1
	if self.close_anima_speed <= 0 then
		ViewMgr:CloseView(GiveSecretKeyView)
		local icon_mono = ServerActivityData.Instance:GetGiveSecretKeyActIconMono()
		if icon_mono then
			icon_mono:SetEffectState(true)
		end
	end
end

function GiveSecretKeyView:CloseCallback()
	TimeHelper:CancelTimer(self.time_handle)
end

GiveSecretKeyPanel = GiveSecretKeyPanel or DeclareMono("GiveSecretKeyPanel",UIWFlushPanel)
function GiveSecretKeyPanel:GiveSecretKeyPanel()
	self.data = ServerActivityData.Instance
	self.data_cares = {
		{data = self.data.give_secret_key_data,func = self.FlushPanel,init_call = false,keys = {"info"}},
		{data = RoleData.Instance.base_data,func = self.FlushPanel,init_call = false,keys = {"level"}},
		{data = FightData.Instance.fight_crystal_sweep,func = self.FlushPanel,init_call = false},
	}
end

function GiveSecretKeyPanel:Awake()
	UIWFlushPanel.Awake(self)
	UH.SetText(self.KeyNum,self.data:GetGiveSecretKeyAllRewardNum())
	self:FlushPanel()
end

function GiveSecretKeyPanel:FlushPanel()
	local all_list = self.data:GetGiveSecretKeyCfg().target
	local top_list_data = {}
	local bottom_list_data = {}
	for i = 1,#all_list do
		if all_list[i].type == 1 then
			table.insert(top_list_data,all_list[i])
		elseif all_list[i].type == 2 then
			table.insert(bottom_list_data,all_list[i])
		end
	end
	self.TopList:SetData(top_list_data)
	self.BottomList:SetData(bottom_list_data)
end

function GiveSecretKeyPanel:OnOpenModulesClick()
	ViewMgr:OpenViewByKey(Mod.TreasureHunt.Main)
end


GiveSecretKeyItem = GiveSecretKeyItem or DeclareMono("GiveSecretKeyItem",UIWidgetBaseItem)
function GiveSecretKeyItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	self.Cell:SetData(Item.Create(data.item))
	if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(68, 80))
        self.red_point:SetNum(0)
    end
	self.flag = ServerActivityData.Instance:GetGiveSecretKeyRewardFlag(data)
	self.GetBtnObj:SetActive(self.flag == 1)
	self.MaskObj:SetActive(self.flag == -1)
	self.red_point:SetNum(self.flag > 0 and 1 or 0)
	self:SetTipText()
end

function GiveSecretKeyItem:SetTipText()
	local str = ""
	--self.Progress.gameObject:SetActive(self.data.type == 1)
	if self.data.type == 1 then
		str = string.format(Language.GiveSecretKey.Tip1,self.data.parameter)
		local lv = RoleData.Instance:GetRoleLevel()
		lv = lv > self.data.parameter and self.data.parameter or lv
		self.Progress:SetProgress(lv / self.data.parameter)
		self.Progress:SetText(string.format("%s/%s",lv,self.data.parameter))
	else
		local num = ServerActivityData.Instance:GetGiveSeretKeyIsFightCrystalPassFb(self.data.parameter) == true and 1 or 0
		self.Progress:SetProgress(num / 1)
		self.Progress:SetText(string.format("%s/%s",num,1))
		local scene_name = FightData.Instance:GetFBName(self.data.parameter)
		local str_split = Split(scene_name,"<")
		scene_name = str_split[1]
		if str_split[2] and string.find(str_split[2],Language.GiveSecretKey.Special) then
            str = string.format("%s%s",Language.GiveSecretKey.Special,scene_name)
		else
            str = string.format(Language.GiveSecretKey.Tip2,scene_name)
		end
	end
	UH.SetText(self.TipText,str)
end

function GiveSecretKeyItem:OnDestroy()
	if self.red_point ~= nil then
		self.red_point:DeleteMe()
		self.red_point = nil
	end
	UIWidgetBaseItem.OnDestroy(self)
end

function GiveSecretKeyItem:OnGetClick()
	if self.flag == 1 then
		if not BagData.Instance:TryFindEmptyInBag(self.data.item.item_id) then
			PublicPopupCtrl.Instance:Center(Language.Activity.BagFullTip)
			return
		end
		ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_GIVE_SECRET_KEY,1,self.data.seq)
	end
end

GiveSecretKeySub = GiveSecretKeySub or DeclareMono("GiveSecretKeySub",UIWFlushPanel)
function GiveSecretKeySub:OnEnable()
	UIWFlushPanel.OnEnable(self)
	ServerActivityData.Instance:SetGiveSecretKeyActIconMono(self)
end

function GiveSecretKeySub:OnDisable()
	ServerActivityData.Instance:SetGiveSecretKeyActIconMono(nil)
	self.effect:SetActive(false)
	UIWFlushPanel.OnDisable(self)
end

function GiveSecretKeySub:SetEffectState(state)
	self.effect:SetActive(state or false)
end