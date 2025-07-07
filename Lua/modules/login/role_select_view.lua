RoleSelectView = RoleSelectView or DeclareView("RoleSelectView", "login/role_select", 0, ViewLayer.Top - 3)
---RoleList====UIWidgetList
VIEW_DECLARE_MASK(RoleSelectView,ViewMask.Block)

function RoleSelectView:RoleSelectView()
end

function RoleSelectView:LoadCallback()
    LuaMonoBehaviour:Awake()
    self.RoleList:SetData( LoginData.Instance:GetLoginRoles() )
    self.RoleList:ClickItem( LoginData.Instance:GetLastLoginIndex()+1)

    LoginData.Instance:SetLoginState(LoginStateType.RoleSelecting)
    PublicPopupCtrl.Instance:HideWait()

    self.doorAnim:SetTrigger(APH("Open"))
    RoleCreateData.Instance:LockUpDirector(true)

    ViewMgr:CloseView(NpcDialogView)
end

function RoleSelectView:OnClickRoleTest()
    if tostring(InstallPackageInfo:AgentId()) == "ml1" then
        -- ViewMgr:OpenView(PoetryView)
    end
end

function RoleSelectView:OpenCallback()
    -- PreloadViewStateMachine.Instance:StopAll()
    ViewMgr:CloseView(WelcomeView)

    self.Effecttool:Play(10010016)
end

function RoleSelectView:Close()
    ViewMgr:CloseView(RoleSelectView)
end

RoleSelectPanel = RoleSelectPanel or DeclareMono("RoleSelectPanel", UIWFlushPanel)
---RoleShow====ModelUIShow


function RoleSelectPanel:RoleSelectPanel()
    self.select_data = RoleSelectData.Instance    
    self.data_cares = { 
        {data = self.select_data.role_select_data,func = self.FlushRoleModel,keys = {"cur_select_role_data"},}
    }

    if self.cur_ui_obj == nil then
        self.cur_ui_obj = UIChDrawer.New()
        self.RoleShow:SetShowData({
            ui_obj = self.cur_ui_obj,
            view = self
        })
    end 
    self.try_timer = nil
end 

function RoleSelectPanel:FlushRoleModel()
    local role_data = self.select_data:GetSelectRoleData()
    if role_data.id == nil then return end 
    
    local appearance = {
        weapon_id = role_data.weapon_id ~=nil and role_data.weapon_id or 0,
        color = role_data.color,
        advance_time = RoleData.ProfIdToProfStage(role_data.profession or 0),
        avatar_type = role_data.avatar_type,
        fashion_body_id = IllusionData.Instance:FashionIdToResId(role_data.surface_list[RoleSelectSurface.Body]),
        fashion_weapon_id = IllusionData.Instance:FashionIdToResId(role_data.surface_list[RoleSelectSurface.Arm]),
        fly_flag = role_data.fly_flag
    }
    local limit_prof = Equip.GetLimitProf(role_data.weapon_id)
    if limit_prof ~= RoleData.Instance:GetRoleProfType(role_data.profession) then
        role_data.weapon_id = 0
        appearance.weapon_id = 0
    end

    self.cur_ui_obj:SetData(appearance)
end

-- function Role( ... )
--     -- body
-- end

function RoleSelectPanel:OnClickEnter()
    -- LoginCtrl.Instance:RequestRoleLogin()
    AudioMgr:PlayEffect(AudioType.Scene, "enter_game")

    local forbid_time = RoleSelectData.Instance:GetSelectRoleData().forbid_time

    if forbid_time ~= nil and forbid_time > 0 and forbid_time > os.time() then
        LoginData.Instance:SetForbidTime(RoleSelectData.Instance:GetSelectRoleData().forbid_time)
        ViewMgr:OpenView(LoginBanDialog)
        return 
    end
    local index = RoleSelectData.Instance:GetSelectRoleData().index
    if index == nil then
        return
    end
    LoginData.Instance:SetLastLoginIndex(index - 1)
    LoginCtrl.Instance:RequestRoleLogin(RoleSelectData.Instance:GetSelectRoleData().id)

    if self.try_timer == nil then 
        TimeHelper:CancelTimer(self.try_timer)
        self.try_timer = nil
    end 

    self.try_timer = TimeHelper:AddDelayTimer(function()
        -- 5秒钟以后还在试图登录角色
        self.try_timer = nil
        if LoginData.Instance:LoginState() == LoginStateType.RoleLogining then 
            local account_cache = LoginData.Instance:getAccountCache()
            local param_t = {
                server_view_id = LoginData.Instance.view_sel_server_id.server_id,
                last_account = account_cache.__last_account,
                account_info = account_cache[account_cache.__last_account],
            }

            LogError("login fail! check your role data ",param_t,RoleSelectData.Instance:GetSelectRoleData())
			LoginCtrl.GMDisconnect()
		end 
    end, 5)

    LoginData.Instance:SetLastRoleData()
end

function RoleSelectPanel:OnClickBack()

    NetworkCtrl.Instance:DisconnectLoginServer()
    LoginData.Instance:SetLoginState(LoginStateType.None)
	-- LoginData.Instance:Clear()

    ViewMgr:OpenView(WelcomeView)
    ViewMgr:CloseView(RoleSelectView)
    AudioMgr:PlayEffect(AudioType.UI, "role_select_back")

    LoginData.Instance:SetLastRoleData()
end

function RoleSelectPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.cur_ui_obj ~= nil then
		self.cur_ui_obj:DeleteMe()
	end
    self.cur_ui_obj = nil

    
    if self.try_timer == nil then 
        TimeHelper:CancelTimer(self.try_timer)
        self.try_timer = nil
    end 
end 

RoleSelectItem = RoleSelectItem or DeclareMono("RoleSelectItem", UIWidgetBaseItem)
---RoleHead====Image
---SelectEffect====Toggle
---RoleName====Text
---RoleLevel====Text
---RoleProf====Text
---RoleProfImage====Image

--function RoleSelectItem:RoleSelectItem()  
--end

function RoleSelectItem:SetData(data)
    self.data = data 

    if data.index == LoginData.Instance:GetLastLoginIndex()+1 then 
        self.SelectEffect.isOn = true 
        -- if LoginData.Instance:GetLastRoleData() then 
        --     self:DataChange()
        --     -- self.data = 
        -- end 
    end
    self.FlyBgSub:SetData(data)
    if data.id == -1 then 
        self.RoleHeadObj:SetActive(false)
        self.RoleProfObj:SetActive(false)
        UH.SetText(self.RoleName,Language.RoleSelect.CreateRole)
        UH.SetText(self.RoleLevel,"")
        return
    end
    --[[ self.FlyBg:SetObjActive(data.fly_flag > 0)
    UH.SpriteName(self.FlyBg, FlyUpData.RoleFlyQ[data.fly_flag]) ]]
    if IS_AUDIT_VERSION and GLOBAL_CONFIG:PkgInfo().is_youyan then
        local img_path, stream_has = ChannelAgentConsts.GetUIStremingAsset("Headp"..(self.data.avatar_type or 1))
        if stream_has then
            self.RoleHeadRawObj:SetActive(true)
            ChannelAgentConsts.SetRawImg(self.RoleHeadRaw,img_path)
            UH.SpriteName(self.RoleHead)
        else
            UH.SetAvatarp(self.RoleHead,(self.data.avatar_type or 1),data.avatar_id,data.avatar_quality)
        end
	else
        UH.SetAvatarp(self.RoleHead,(self.data.avatar_type or 1),data.avatar_id,data.avatar_quality)
    end
    UH.SetText(self.RoleName,(self.data.name or ""))
    if self.data.top_level and self.data.top_level > 0 then
        UH.SetText(self.RoleLevel, "")--string.format(Language.PeakRoad.Level2, self.data.top_level))
        self.PeakObj:SetActive(true)
    else
        UH.SetText(self.RoleLevel,Language.RoleSelect.Level..(self.data.level or 0))
        self.PeakObj:SetActive(false)
    end
    UH.SetText(self.RoleProf,RolePromoteData.GetProfName(self.data.profession or 0))
    UH.SpriteName(self.RoleProfImage, RoleData.GetProfSpByProfId(self.data.profession or 0,false))
end

function RoleSelectItem:Click()
    if self.data.id > 0 then 
        RoleSelectData.Instance:SetSelectRoleData(self.data)
    else
        -- ViewMgr:OpenView(StoryOptionsView)
        -- TimeHelper:AddDelayTimer(function()
            SceneCtrl.Instance:GotoScene(RoleCreateScene)
            LoginData.Instance:SetLoginState(LoginStateType.RoleCreating)
        -- end,0.2)

        self.timer = TimeHelper:AddDelayTimer(function() 
            RoleCreateData.Instance:DelayBlack(true) 
            self.timer = nil
        end,0.2)
    end 

    AudioMgr:PlayEffect(AudioType.UI, "role_select_click")
end 

function RoleSelectItem:DataChange()
    -- LogError("DataChange!!!")
    local last_data = LoginData.Instance:GetLastRoleData()
    for k,v in pairs(last_data) do 
        if self.data[k] ~= v then 
            self.data[k] = v
        end 
    end 
end