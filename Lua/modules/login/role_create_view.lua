
RoleCreateView = RoleCreateView or DeclareView("RoleCreateView", "login/role_create", 0,  ViewLayer.Normal - 1)

VIEW_DECLARE_MASK(RoleCreateView,ViewMask.Block)

function RoleCreateView:RoleCreateView()
	self.data = RoleCreateData.Instance
end

function RoleCreateView:LoadCallback()
	MainProber:Step(MainProber.STEP_ROLECREATE_VIEWLOAD_END, MainProber.user_id)
end

function RoleCreateView:CloseCallback()
	LoginData.Instance:SetLoginBackType(RELOGIN_TYPE.WELCOME)
end

RoleCreateViewFlushPanel = RoleCreateViewFlushPanel or DeclareMono("RoleCreateViewFlushPanel",UIWFlushPanel)

function RoleCreateViewFlushPanel:RoleCreateViewFlushPanel()
    if self.create_ui_obj == nil then
        self.create_ui_obj = UIChDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.create_ui_obj,
            view = self
        })
    end 

	self.data = RoleCreateData.Instance
	self.data_cares = {
		{data = self.data.create_view_data,func = self.FlushCreateModel ,keys = {"appear_id","prof_id","color_index","advance_time"},init_call = false},
		{data = self.data.create_view_data,func = self.FlushViewShowProf, keys = {"is_show_prof"}},
		{data = self.data.create_view_data,func = self.FlushViewShowRight, keys = {"is_show_right"}, init_call = false},
		{data = self.data.create_view_data,func = self.FlushChara,keys = {"appear_id"},init_call = false},
		{data = self.data.create_view_data,func = self.FlushCharaDetail,keys = {"prof_id","advance_time"},init_call = false},
		{data = self.data.create_view_data,func = self.FlushSkillPanel,keys = {"is_show_skill_tips"}},
		{data = self.data.create_view_data,func = self.FlushRoleName,keys = {"role_name"}, init_call = false},
		{data = self.data.create_director_data,func = self.FlushDirector,keys = {"appear_id",init_call = false}},
		{data = self.data.create_view_data,func = self.ShowProfShow,keys = {"prof_id"},init_call = false},
		{data = self.data.model_rotate,func = self.FlushModelRotate},
	}

	local chara_list = self.data:GetRoleCharaList()
	self.CharaList:SetData(chara_list)

	self.lock_timer = TimeHelper:AddDelayTimer(function()
		self.lock_timer = nil
		self.LockObj:SetActive(true)
    end, 1)

    self.SkillList:SetCompleteCallBack(function()
		self.SkillList:ForceJumpHorizontal(0)
    end)

	self.anim_handle = self.ProfShowAnim:ListenEvent("ProfShowEnd", function ()
		self.CharaSelectObj:SetActive(false)
		self.ProfSelectObj:SetActive(true)
	end)

	self.mute_prof_voice = false
end

function RoleCreateViewFlushPanel:OnDestroy()
	self.data:ClearCreateData()
	self.data:ClearDirectorData()
	UIWFlushPanel.OnDestroy(self)

	if self.enter_timer ~= nil then
        TimeHelper:CancelTimer(self.enter_timer)
        self.enter_timer = nil
	end
	
	if self.lock_timer ~= nil then
        TimeHelper:CancelTimer(self.lock_timer)
        self.lock_timer = nil
    end

	self.ProfShowAnim:UnlistenEvent("ProfShowEnd", self.anim_handle)
end

function RoleCreateViewFlushPanel:FlushCreateModel()
	local role_appearance = self.data:GetCreateAppearance()
	if role_appearance == nil then return end
	self.create_ui_obj:DelWeapon()
	self.create_ui_obj:SetData(role_appearance)

	if not self.is_cancel_show then 
		self.create_ui_obj:PlayAnim(ChAnimType.WeaponShow)
	else 
		self.is_cancel_show = false
		self.create_ui_obj:PlayAnim(ChAnimType.Idle)
	end 
	-- self:ShowProfShow()

	self.ShowText.text = Language.RoleCreate.ShowText[role_appearance.advance_time]
end

function RoleCreateViewFlushPanel:ShowProfShow()
	if self.data.create_view_data.prof_id == 0 then return end 
	local role_appearance = self.data:GetCreateAppearance()
	if role_appearance == nil then return end 
	local audio = Cfg.AudioProf(role_appearance.avatar_type,RoleData.ProfIdToProfType(self.data.create_view_data.prof_id))
    AudioMgr:StopEffectHandle(self.handle_audio1)
    AudioMgr:StopEffectHandle(self.handle_audio2)
    -- self.handle_audio1 = AudioMgr:PlayEffectHandle(AudioType.UI, audio.show_audio)
	if not self.mute_prof_voice then 
        self.handle_audio2 = AudioMgr:PlayEffectHandle(AudioType.UI, audio.show_voice)
	else 
		self.mute_prof_voice = false
	end 
end

function RoleCreateViewFlushPanel:FlushViewShowProf()
	self:FlushViewShow()
	if not self.data.create_view_data.is_show_prof then
		self.ToggleGroup:SetAllTogglesOff()
	end
end

function RoleCreateViewFlushPanel:FlushViewShowRight()
	self:FlushViewShow()
end

function RoleCreateViewFlushPanel:FlushViewShow()
	if self.data.create_view_data.is_show_prof then
		self.ProfSelectObj:SetActive(true)
		self.ProfShowAnim:SetTrigger(APH("ProfShow"))
	else
		
		self.CharaSelectObj:SetActive(not self.data.create_view_data.is_show_prof)
		self.ProfSelectObj:SetActive(self.data.create_view_data.is_show_prof)
	end
	if self.data.create_view_data.is_show_right then
		MainProber:Step(MainProber.STEP_ROLECREATE_CONFIRM_SHOW, MainProber.user_id, self.data.create_view_data.appear_id)
	end
	self.RightObj:SetActive(self.data.create_view_data.is_show_right)
	self.mark_pos:SetActive(self.data.create_view_data.is_show_right)
	self.BtnReturnObj:SetActive(not self.data.create_view_data.is_show_right)
	self.tipsObj:SetActive(not self.data.create_view_data.is_show_right)

	if self.data.create_view_data.is_show_prof then 
		self.ProfSelectList:ClickItem(RoleCreateConfig.DefaultProf[self.data.create_view_data.appear_id])
		self.ColorDeafualt.isOn = true
		AudioMgr:StopEffectHandle(self.handle_audio)
	end
end

function RoleCreateViewFlushPanel:FlushModel()
	local audio = Cfg.AudioCreateRole(self.data.cur_sel_role.role_id)
	AudioMgr:StopEffectHandle(self.handle_audio)
	self.handle_audio = AudioMgr:PlayEffectHandle(AudioType.UI, audio.voice)
end

function RoleCreateViewFlushPanel:FlushChara()
	if self.data.create_view_data.appear_id == 0 then return end 

	local cur_role_data = self.data:GetCharaDataByAvatarType(self.data.create_view_data.appear_id)
	local cur_prof_list = self.data:GetRoleProfList(self.data.create_view_data.appear_id)
	local cur_color_list = self.data:GetRoleColorList(self.data.create_view_data.appear_id)
	local name_sp = RoleCreateConfig.RoleSpName[cur_role_data.role_id]

	self.NameSp.SpriteName = name_sp

	local desc_str = ""
	for i = 1,3 do
		if cur_role_data["role_desc_"..i] ~= "" then 
			desc_str = desc_str .. cur_role_data["role_desc_"..i] .."\t"
		end 
	end 

	self.CharaDesclbl.text = desc_str

	self.ProfShowList:SetData(cur_prof_list.prof_list)
	self.ProfSelectList:SetData(cur_prof_list.prof_list)

	local color_list = cur_color_list.color_list
	if color_list == nil then return end 
	for i = 1, self.Colors:Length() do
		self.Colors[i]:SetData(color_list[i])
	end
	self.Colors[1]:OnClickItem()
end

function RoleCreateViewFlushPanel:FlushCharaDetail( )
	if self.data.create_view_data.prof_id == 0 then return end 
	local prof_name = RoleData.GetProfSp(RoleData.ProfIdToProfType(self.data.create_view_data.prof_id), true)
	self.ProfNameSp.SpriteName = "_Loc_"..prof_name
	self.ProfDescSp.SpriteName = "_Loc_"..prof_name.."_Desc"

	local cur_color_list = self.data:GetRoleColorListDetail(self.data.create_view_data.appear_id,self.data.create_view_data.advance_time)

	if cur_color_list == nil then return end 
	for i = 1, self.Colors:Length() do
		self.Colors[i]:SetData(cur_color_list[i])
	end
end

function RoleCreateViewFlushPanel:ClickToggleOn()
	-- self.BtnToggleOn:SetActive(false)
	self.ToggleAnim:SetTrigger(APH("is_show"))
	self.SkillClickArea:SetActive(true)
	-- self.ProfToggle:SetActive(true)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
end 

function RoleCreateViewFlushPanel:ClickToggleOff()
	-- self.BtnToggleOn:SetActive(true)
	self.ToggleAnim:SetTrigger(APH("is_hide"))
	self.SkillClickArea:SetActive(false)
	-- self.ProfToggle:SetActive(false)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
end 

function RoleCreateViewFlushPanel:OnClickSkillArea()
    self:ClickToggleOff()
end 

function RoleCreateViewFlushPanel:FlushSkillPanel()
    self.SkillPanelObj:SetActive(self.data.create_view_data.is_show_skill_tips)   
    -- self.SkillTipsShow.text = self.data.create_view_data.skill_info_tips
end

function RoleCreateViewFlushPanel:CloseSkillPanel()
    RoleCreateData.Instance:SetProfViewSkill({is_show = false})
end 

function RoleCreateViewFlushPanel:FlushRoleName()
	self.InputField.text = self.data.create_view_data.role_name
end

function RoleCreateViewFlushPanel:FlushDirector()
	if self.data.create_director_data.appear_id == 0 then return end 

	local data_list = self.data:GetRoleCharaList()

	local pos_ = 0
	for k,v in pairs(data_list) do 
		if v.role_id == self.data.create_director_data.appear_id then 
			pos_ = k
			break
		end 
	end 
	if pos_ > 0 then self.CharaList:ClickItem(pos_) end 
end

function RoleCreateViewFlushPanel:OnClickPause()
	self.ToggleGroup:SetAllTogglesOff()
	self.PreModelToggle:SetAllTogglesOff()
	self.ColorToggle:SetAllTogglesOff()

	self.data:SetAdvanceTime(1)
	-- self.data:ResetAppearId()
	self.data:ClearDirectorData()
	SceneCtrl.Instance:CurScene():PauseCreate()
	
	AudioMgr:StopEffectHandle(self.handle_audio)
	
	self.data:SetIsShowProf(false)
	AudioMgr:StopEffectHandle(self.handle_audio1)
	AudioMgr:StopEffectHandle(self.handle_audio2)
	MainProber:Step(MainProber.STEP_ROLECREATE_CLICK_PAUSE, MainProber.user_id)
end

function RoleCreateViewFlushPanel:OnClickReturn()
	SceneCtrl.Instance:CurScene():StopCreate(false)
	-- NetworkCtrl.Instance:DisconnectLoginServer()

	local role_list = LoginData.Instance:GetLoginRoles()
	local flag_empty = true

	for k,v in pairs(role_list) do 
		if v.id > -1 then 
			flag_empty = false
			break
		end 
	end 

	-- LoginData.Instance:Clear()
	LoginData.Instance:SetLoginBackType(flag_empty and RELOGIN_TYPE.WELCOME or RELOGIN_TYPE.ROLE_SELECT)
	SceneCtrl.Instance:GotoScene(LoginScene)

	if flag_empty then 
	    LoginCtrl.Instance:unloadGameLogicBeforeRelogin()
	end 
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
	MainProber:Step(MainProber.STEP_ROLECREATE_CLICK_RETURN, MainProber.user_id)
	self.data:ResetAppearId()
	self.data:LockUpDirector(true)
end

function RoleCreateViewFlushPanel:OnClickUnLock()
	if self.data:IsDelayBlack() == nil or self.data:IsDelayBlack() == false then return end 
	self.data:UnLockDirector() 
	self.data:DelayBlack(false) 
	self.LockObj:SetActive(false)
end

function RoleCreateViewFlushPanel:OnClickBack()
	self.ProfToggleGroup:SetAllTogglesOff()
	self.PreModelToggle:SetAllTogglesOff()
	self.ColorToggle:SetAllTogglesOff()
	-- self.BtnToggleOn:SetActive(true)
	self.data:SetAdvanceTime(1)
	self.data:SetIsShowProf(false)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)

	AudioMgr:StopEffectHandle(self.handle_audio1)
	AudioMgr:StopEffectHandle(self.handle_audio2)
	self.data:SetProfId(0)
	self:OnClickColor(0)

end

function RoleCreateViewFlushPanel:OnClickSureChara()
	-- self.ToggleGroup:SetAllTogglesOff()
	LoginCtrl.Instance:SendRandomNameReq()
	SceneCtrl.Instance:CurScene():StopCreate(true)
	self.data:SetIsShowProf(true)
	self.mute_prof_voice = true
	MainProber:Step(MainProber.STEP_ROLECREATE_CONFIRM_AVATAR, MainProber.user_id, self.data.create_view_data.appear_id)
end

function RoleCreateViewFlushPanel:OnClickRandom()
	LoginCtrl.Instance:SendRandomNameReq()
	AudioMgr:PlayEffect(AudioType.UI, "role_create_color")
end

function RoleCreateViewFlushPanel:OnClickEnter()
	AudioMgr:StopEffectHandle(self.handle_audio1)
	AudioMgr:StopEffectHandle(self.handle_audio2)
	
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)

	local value, error_code = self.data:CheckRoleName(self.InputField.text)
	if value then
		local function callback()
			LoginCtrl.Instance:RequestCreateRole(function(err_code)
				if err_code >= RoleCreateResult.Success then
					RoleCreateData.Instance:ResetModelRotateInfo()
					local user_id = LoginData.Instance:GetPlatformName()
					local role_name = GameUtil.EncodeB64(self.data.role_name.name)
					--角色创建申请结果埋点
					MainProber:Step(MainProber.STEP_CREATE_ROLE_END, user_id, LoginData.Instance:GetLastLoginServer(), role_name, LoginData.Instance:GetNewCreateRoles().id)
					-- PublicPopupCtrl.Instance:Center(string.format("RecvCreateRole SUC:%s",tostring(err_code)))
					LoginCtrl.Instance:RequestRoleLogin(LoginData.Instance:GetNewCreateRoles().id)
					-- 创角成功后清除封测充值金额
					LoginData.Instance:ResetFCRecharge()
					LoginData.Instance:NewCharaCreateComplete()
				else
					if err_code == -2 then
						PublicPopupCtrl.Instance:Center(ErrorText[49])
					else
						PublicPopupCtrl.Instance:Center(string.format("RecvCreateRole ERROR:%s",tostring(err_code)))
					end
				end
			end)
		end

		if IS_STANDALONE() then
			local function cb(str)
				if str ~= self.data.role_name.name then
					self.data.role_name.name = str
					self.data.name_str_id = 0
					PublicPopupCtrl.Instance:Center(ErrorText[66])
				else
					callback()
				end
				
			end
			ChatSubCustom.PCChatFilter(self.InputField.text, 1, cb)
		else
			callback()
		end
	else
		PublicPopupCtrl.Instance:Center(error_code)
	end

	
	self.enter_timer = TimeHelper:AddDelayTimer(function()
		-- 5秒钟以后还在试图创建角色
		self.enter_timer = nil
		if LoginData.Instance:LoginState() == LoginStateType.RoleCreateRequesting then 
			LoginCtrl.GMDisconnect()
		end 
    end, 5)
end

function RoleCreateViewFlushPanel:OnClickCharaItem(data)
	-- if self.data.create_director_data.appear_id == data.role_id then return end 

	LoginCtrl.Instance:ClickAvatarType(data.role_id)

	self.BtnReturnObj:SetActive(false)
	self.tipsObj:SetActive(false)
	
	local audio = Cfg.AudioCreateRole(self.data.create_view_data.appear_id)
	AudioMgr:StopEffectHandle(self.handle_audio)
	self.handle_audio = AudioMgr:PlayEffectHandle(AudioType.UI, audio.voice)
	MainProber:Step(MainProber.STEP_ROLECREATE_CLICK_AVATAR, MainProber.user_id, self.data.create_view_data.appear_id)
end

function RoleCreateViewFlushPanel:OnClickColor(index)
	self.data:SetColorIndex(index)

	self.is_cancel_show = true

	AudioMgr:PlayEffect(AudioType.UI, "role_create_color")
end

function RoleCreateViewFlushPanel:OnClickProfItem(data)
	LoginCtrl.Instance:ClickAvatarTypeProf(data.prof_index)
	local flag_same = data.prof_id == self.data:GetProfId()
	self.data:SetProfId(data.prof_id)
	
	local desc_config = RoleData.Instance:GetProfessionDesc(data.prof_id,true)
    self.ToggleTip1.text = desc_config.weapon
    self.ToggleTip2.text = desc_config.armor_type
    self.ToggleTip3.text = desc_config.point
    self.ToggleTip4.text = desc_config.desc

    local skill_data = SkillData.GetProfSkillList(RoleData.ProfIdToProfType(data.prof_id))
	if not flag_same then self.SkillList:SetData(skill_data) end 
	self.ProfSimple.text = Language.RoleCreate.ProfSpecial..desc_config.desc
	AudioMgr:PlayEffect(AudioType.UI, "role_create_color")
	MainProber:Step(MainProber.STEP_ROLECREATE_CLICK_PROF, MainProber.user_id, self.data.create_view_data.appear_id, data.prof_id)
end

function RoleCreateViewFlushPanel:OnClickZongShi()
	if self.PreZongTog.isOn then 
		self.data:SetAdvanceTime(4)
	else
		self.data:SetAdvanceTime(1)
	end 

	self.is_cancel_show = true
end

function RoleCreateViewFlushPanel:OnClickChuanShuo()
	if self.PreChuanTog.isOn then 
		self.data:SetAdvanceTime(5)
	else
		self.data:SetAdvanceTime(1)
	end 

	self.is_cancel_show = true
end

function RoleCreateViewFlushPanel:FlushModelRotate()
	self.ModelRotate:SetActive(self.data.model_rotate.is_show)
end

function RoleCreateViewFlushPanel:OnPressDownModel()
	self.mouse_pos_x = nil
end

function RoleCreateViewFlushPanel:OnDragModel()
	local mri = self.data.model_rotate_info
	if not mri.ready then return end
	if mri.parent then
        local mouse_pos_x = UnityEngine.Input.mousePosition.x
        if self.mouse_pos_x and self.mouse_pos_x ~= mouse_pos_x then
            local obj_rot = mri.parent:GetLocalEulerAngles()
            obj_rot.y = obj_rot.y + self.mouse_pos_x - mouse_pos_x
			mri.parent:SetLocalEulerAngles(obj_rot)
        end
        self.mouse_pos_x = mouse_pos_x
    end
end

CreateCharaListItem = DeclareMono("CreateCharaListItem", UIWidgetBaseItem)

function CreateCharaListItem:NeoCharaListItem()

end
function CreateCharaListItem:SetData(data)
	if IS_AUDIT_VERSION and GLOBAL_CONFIG:PkgInfo().is_youyan then
		local img_path, stream_has = ChannelAgentConsts.GetUIStremingAsset("Headp"..data.role_id)
		if stream_has then
			ChannelAgentConsts.SetRawImg(self.NomalIconRawImg,img_path)
			ChannelAgentConsts.SetRawImg(self.SelectIconRawImg,img_path)
			self.NomalIconRawImgObj:SetActive(true)
			self.SelectIconRawImgObj:SetActive(true)
			UH.SpriteName(self.NomalIconImg)
			UH.SpriteName(self.SelectIconImg)
		else
			UH.SpriteName(self.NomalIconImg, "headp"..data.role_id)
			UH.SpriteName(self.SelectIconImg, "headp"..data.role_id)
		end
	else
		UH.SpriteName(self.NomalIconImg, "headp"..data.role_id)
		UH.SpriteName(self.SelectIconImg, "headp"..data.role_id)
	end
	UIWidgetBaseItem.SetData(self, data)
end

function CreateCharaListItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

RoleCreateViewColorItem = DeclareMono("RoleCreateViewColorItem", UIWidgetBaseItem)
function RoleCreateViewColorItem:RoleCreateViewColorItem()
	self.rc_data = RoleCreateData.Instance
end

function RoleCreateViewColorItem:SetData(data)
	UH.SpriteName(self.ColorPic, "PeiSe"..data.color_id)
	UIWidgetBaseItem.SetData(self, data)
end

function RoleCreateViewColorItem:OnClickItem()
	self.rc_data:SetColorIndex(self.data.color_index)
	-- AudioMgr:PlayEffect(AudioType.UI, "role_create_color")
end

CreateProfDescCell = DeclareMono("CreateProfDescCell", UIWidgetBaseItem)
function CreateProfDescCell:CreateProfDescCell()

end

function CreateProfDescCell:SetData(data)
	self.prof_name.text = FormatEnum.Prof[data.prof_id]
	UIWidgetBaseItem.SetData(self, data)

	local prof_type = RoleData.ProfIdToProfType(data.prof_id)
	local recommend_data = RoleCreateData.Instance:GetRecommendJob()

	self.Recommend:SetActive(false)
	for k,v in pairs(recommend_data or {}) do 
		if prof_type == v.prof_id then 
			self.Recommend:SetActive(true)
			break
		end  
	end 
end

CreateProfListItem = DeclareMono("CreateProfListItem",UIWidgetBaseItem)
function CreateProfListItem:CreateProfListItem()
	self.rc_data = RoleCreateData.Instance
end

function CreateProfListItem:SetData(data)
    local icon_name = RoleData.GetProfSp(RoleData.ProfIdToProfType(data.prof_id), true)
    local prof_name = FormatEnum.Prof[data.prof_id]
    self.SelectImg.SpriteName = icon_name
	self.NomalImg.SpriteName = icon_name
	
    self.SelectName.text = prof_name
    self.NomalName.text = prof_name
	UIWidgetBaseItem.SetData(self, data)


	local prof_type = RoleData.ProfIdToProfType(data.prof_id)
	local recommend_data = RoleCreateData.Instance:GetRecommendJob()

	self.Recommend:SetActive(false)
	for k,v in pairs(recommend_data or {}) do 
		if prof_type == v.prof_id then 
			self.Recommend:SetActive(true)
			break
		end  
	end 
end

function CreateProfListItem:Click()
    if self.ItemClick and self.ItemClick.gameObject.activeSelf then
        self.ItemClick:Trigger()
    end
end

CreateProfSkillItem = CreateProfSkillItem or DeclareMono("CreateProfSkillItem", UIWidgetBaseItem)

function CreateProfSkillItem:NeoProfSkillItem()
end

function CreateProfSkillItem:SetData(data)
    self.SkillName.text = data.skill_name
    UH.SetIcon(self.SkillIcon,data.icon_id,ICON_TYPE.SKILL)

	local num = DataHelper.GetStringWordNum(data.skill_name)
	self.Element.preferredWidth = (num < 4 and 4 or num ) * 24 + 20

    UIWidgetBaseItem.SetData(self, data)
end

function CreateProfSkillItem:Click()
    local desc_info = Cfg.GetChangeJobDesc(RoleData.ProfIdToProfType(RoleCreateData.Instance.create_view_data.prof_id),self.data.skill_id)
    if desc_info == nil then 
        LogError("empty desc!",RoleData.ProfIdToProfType(RoleCreateData.Instance.create_view_data.prof_id),self.data.skill_id)
        return 
    end 
    local data = {
        -- tips_str = string.format(Language.Role.RolePromoteSkillTips,desc_info.skill_name,desc_info.skill_level,desc_info.description),
		tips_str = string.format("%s%s","<color=#".. COLORSTR.Yellow15 .. ">"..desc_info.skill_name..":</color>",desc_info.description),
        is_show = true,
		skill_name = self.data.skill_name,
		skill_icon = self.data.icon_id,	
    }

    RoleCreateData.Instance:SetProfViewSkill(data)
end

CreateSkillShow = CreateSkillShow or DeclareMono("CreateSkillShow", UIWFlushPanel)
function CreateSkillShow:CreateSkillShow() 
	self.data = RoleCreateData.Instance
	self.data_cares = {
		{data = self.data.create_view_data,func = self.FlushSkillPanel,keys = {"is_show_skill_tips"}},
	}
end
function CreateSkillShow:FlushSkillPanel() 
	self.SkillContent.localPosition = Vector3.zero
    self.SkillDesc.text = self.data.create_view_data.skill_info_tips
	self.SkillName.text = self.data.create_view_data.skill_info_name
    UH.SetIcon(self.SkillIcon,self.data.create_view_data.skill_info_icon,ICON_TYPE.SKILL)
end