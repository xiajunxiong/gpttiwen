RoleSettingPanel = RoleSettingPanel or DeclareMono("RoleSettingPanel", UIWFlushPanel)
function RoleSettingPanel:RoleSettingPanel()
    self.is_loaded = false
    self.data = RoleSettingData.Instance
    self.care_handle = self.data.head_red_point:Care(BindTool.Bind(self.FlushChangeHeadNum,self))
    -- 添加审核版本控制按钮显示
    if IS_AUDIT_VERSION then
        if not GLOBAL_CONFIG:PkgInfo().is_youyan then
            for i = 1, self.ObjAuditNotShows:Length() do
                self.ObjAuditNotShows[i]:SetActive(false)
            end
        else
            for i = 1, self.ObjAuditNotShows:Length() >= 2 and 2 or self.ObjAuditNotShows:Length() do
                self.ObjAuditNotShows[i]:SetActive(false)
            end
        end
    end
    self.data_cares = {
        {data = RoleData.Instance.base_data, func = self.FlushAvatarCell, keys = {"appearance"}, init_call = true}
    }
end

function RoleSettingPanel:RoleSettingChange(key, value)
    if key == ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS then
        self.RoleCount:SetProgress(value / 100)
        self.RoleCount:SetText(value .. Language.RoleSetting.Role)
    end
end

function RoleSettingPanel:onFlush()
    self.NewMode:SetActive(GLOBAL_CONFIG:CanChangeMode())
    self.NewModeTex:SetActive(GLOBAL_CONFIG:CanChangeMode())

    for i = ROLE_SETTING_TYPE.SET_TOTAL_VOLUME, ROLE_SETTING_TYPE.SET_SOUND_VOLUME do
        local value = self.data:GetData(i)
        self["Volume" .. i]:SetKeyValueData(i, value)
    end

    local quality = self.data:GetData(ROLE_SETTING_TYPE.SET_PICTUER_QUALITY)
    self.SelectQuality[quality + 1].isOn = true

    local role_count = self.data:GetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS)
    self.RoleCount:SetProgress(role_count / 100)
    self:DragRoleCount()

    for i = ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER, ROLE_SETTING_TYPE.SET_AUTO_RESEARCH_TASK do
        local value = self.data:GetServerData(i)
        self["RoleSettingSwitch" .. i].isOn = value > 0
    end
    if IS_STANDALONE() then
        self.SwitchJoyToggleObj:SetActive(false)
    end

    local value = self.data:GetServerData(ROLE_SETTING_TYPE.SYSTEM_SET_INFO_SHIELDING_FLOWERS)
    self["RoleSettingSwitch" .. ROLE_SETTING_TYPE.SYSTEM_SET_INFO_SHIELDING_FLOWERS].isOn = value > 0

    -- 再增加一个就做成list
    self.isjoy.isOn = self.data:GetData(ROLE_SETTING_TYPE.SET_INFO_IS_JOY) > 0
    self.ispower.isOn = self.data:GetData(ROLE_SETTING_TYPE.SET_INFO_IS_POWER) == 0
    self.ToggleMode.isOn = self.data:GetData(ROLE_SETTING_TYPE.SYSTEM_SET_INFO_BATTLE_CONTROL_MODE) == 0
    self.ToggleNewMode.isOn = self.data:GetData(ROLE_SETTING_TYPE.SYSTEM_SET_INFO_BATTLE_CONTROL_MODE) == 1

    self.is_loaded = true
    local role_base_data = RoleData.Instance:GetBaseData()
    self.RoleName.text = role_base_data.name
    self.RoleID.text = "ID:" .. role_base_data.role_id
    local account_key = LoginData.Instance:GetAccountKey()
    local last_login_server = LoginData.Instance:getAccountCacheData(account_key).last_login_server
    local server_info = LoginData.Instance:GetServerInfoById(last_login_server)
    if server_info ~= nil then
        if IS_EDITOR then
            self.ServerName.text = string.format(Language.Login.ServerNameEditor, server_info.id, server_info.name)
        else
            self.ServerName.text = string.format(Language.Login.ServerName, server_info.name)
        end
    end
    self.role_setting_change_handle = EventSys:Bind(GlobalEventKeys.RoleSettingChange, BindTool.Bind(self.RoleSettingChange, self))
    self:FlushChangeHeadNum()
    local data = LoginData.Instance:GetUserProtocol()
    self.PrivateProtBtn:SetActive(not data.is_null)
    self.UserProtBtn:SetActive(not data.is_null)
end

function RoleSettingPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    RoleSettingCtrl.Instance:SaveRoleSettingInfo()
    self.data:DelayApply()
    if self.role_setting_change_handle then
        EventSys:UnBind(self.role_setting_change_handle)
        self.role_setting_change_handle = nil
    end
    self.data.head_red_point:Uncare(self.care_handle)
	self.care_handle = nil
end

function RoleSettingPanel:Close()
    ViewMgr:CloseView(RoleSettingPanel)
end

function RoleSettingPanel:ClickDown()
end

function RoleSettingPanel:DragRoleCount()
    local value = self.RoleCountSlider.value
    local value_int = math.modf(value * 100)
    self.RoleCount:SetProgress(value)
    self.RoleCount:SetText(value_int .. Language.RoleSetting.Role)
    self.data:SetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS, value_int)
end

function RoleSettingPanel:ClickChangeID()
    LoginCtrl.Instance:HintToReturnLogin(Language.RoleSetting.ChangeId, true)
    EventSys:Fire(GlobalEventKeys.TOUCH_BEGAN)
end

function RoleSettingPanel:ClickChangeServer()
    ViewMgr:CloseView(RoleSettingPanel)
    ViewMgr:OpenView(ServerView)
end

function RoleSettingPanel:ClickQuality(quality)
    self.data:SetData(ROLE_SETTING_TYPE.SET_PICTUER_QUALITY, quality)
end

function RoleSettingPanel:OnClickCDKey()
    ViewMgr:OpenView(CDKeyExchangeView)
end

function RoleSettingPanel:OnClickFeedBack()
    ViewMgr:OpenView(FeedBackView)
end
function RoleSettingPanel:OnClickMode()
    --[[ if BattleData.Instance:IsServerFighting() then
        -- PublicPopupCtrl.Instance:Center(Language.RoleSetting.InBattle)
        return
    end ]]
    self.data:SetData(ROLE_SETTING_TYPE.SYSTEM_SET_INFO_BATTLE_CONTROL_MODE, 0)
end
function RoleSettingPanel:OnClickNewMode()
    --[[ if BattleData.Instance:IsServerFighting() then
        -- PublicPopupCtrl.Instance:Center(Language.RoleSetting.InBattle)
        return
    end ]]
    self.data:SetData(ROLE_SETTING_TYPE.SYSTEM_SET_INFO_BATTLE_CONTROL_MODE, 1)
end

-- 切换角色
function RoleSettingPanel:OnClickChangeCharacter()
    self:CollectLastRoleData()
    ViewMgr:CloseView(RoleSettingPanel)
    LoginData.Instance:SaveLastLoginServer(LoginData.Instance:GetAccountKey(),LoginData.Instance:GetLastLoginServer() )
    LoginData.Instance:SetLoginBackType(RELOGIN_TYPE.ROLE_SELECT)
    LoginCtrl.Instance:unloadGameLogicBeforeRelogin()
    LoginCtrl.Instance:RequestServerAccountLogin(function(error_code)
        if error_code >= AccountLoginResult.Success then
            SceneCtrl.Instance:GotoScene(LoginScene)
        else
            PublicPopupCtrl.Instance:Center(string.format("RecvLoginToAccount ERROR:%s",tostring(error_code)))
        end
    end)   
    
end

function RoleSettingPanel:ToggleChange(key)
    local is_on = self["RoleSettingSwitch" .. key].isOn
    local value_int = is_on and 1 or 0
    self.data:SetData(key, value_int)
end

function RoleSettingPanel:JoyToggleChange(key)
    local is_on = self.isjoy.isOn
    local value_int = is_on and 1 or 0
    self.data:SetData(ROLE_SETTING_TYPE.SET_INFO_IS_JOY, value_int)
    self.data.joy_is_on.val = value_int
end

function RoleSettingPanel:PowerToggleChange(key)
    local is_on = self.ispower.isOn
    local value_int = is_on and 0 or 1
    self.data:SetData(ROLE_SETTING_TYPE.SET_INFO_IS_POWER, value_int)
    self.data.power_is_on.val = value_int
end

--更换职业
function RoleSettingPanel:OnClickChangeProf()
    local npc_config = VoMgr:NpcVoByNpcId(40015)
    SceneLogic.Instance:AutoToPos(npc_config[1].scene_id,nil,function()SceneLogic.Instance:AutoToNpc(40015)end,nil,true)
    ViewMgr:CloseView(RoleSettingPanel)
end

--更换形象
function RoleSettingPanel:OnClickChangeImage()
    local npc_config = VoMgr:NpcVoByNpcId(40132)
    SceneLogic.Instance:AutoToPos(npc_config[1].scene_id,nil,function()SceneLogic.Instance:AutoToNpc(40132)end,nil,true)
    ViewMgr:CloseView(RoleSettingPanel)
end

--脱离卡死
function RoleSettingPanel:OnClickBackMain()
    ViewMgr:CloseView(RoleSettingPanel)
    if MainPlayer then
        MainPlayer:StopMove()
    end
    SceneLogic.Instance:ClearMoveCache()
    RoleSettingCtrl.Instance:SendEscapeFromCurrentPosition()
    TeamCtrl.Instance:SendTemporarilyPartReq()
end

--改变头像
function RoleSettingPanel:OnClickChangeHead()
    ViewMgr:OpenView(RoleChangeHeadView)
    RoleSettingData.Instance:SetHeadRedPoint(false)
end
--自定义战术
function RoleSettingPanel:OnClickBattleStrategy()
    local block, notice_str = LoginData.Instance:BlockBattleStrategy()
    if block then
        PublicPopupCtrl.Instance:Center(notice_str)
        return
    end
    ViewMgr:OpenView(BattleStrategy)
end

--用户协议
function RoleSettingPanel:OnClickUserProt()
    local data = LoginData.Instance:GetUserProtocol()
    local split_str = RichTextHelper.ParseAllFunc(data.user_agree_url, true ,function(paramt)
        self.link_mark = self.link_mark or {}
        self.link_mark[paramt.mark] = paramt.address 
     end)
    if self.link_mark["1"] then
        UnityApp.OpenURL(self.link_mark["1"])
    end
end

--隐私协议
function RoleSettingPanel:OnClickPrivateProt()
    local data = LoginData.Instance:GetUserProtocol()
     local split_str = RichTextHelper.ParseAllFunc(data.privacy_url, true ,function(paramt)
        self.link_mark = self.link_mark or {}
        self.link_mark[paramt.mark] = paramt.address 
     end)
    if self.link_mark["2"] then
        UnityApp.OpenURL(self.link_mark["2"])
    end
end

function RoleSettingPanel:FlushChangeHeadNum()
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.RedPos, Vector3.zero) 
    end
    self.red_point:SetNum(self.data:GetHeadRedPointNum())
end


function RoleSettingPanel:CollectLastRoleData()
    local app_data = RoleData.Instance:GetApearance()
    
    local param_t = {}
    param_t.avatar_type = app_data.avatar_type
    param_t.name = RoleData.Instance:GetRoleName()
    param_t.level = RoleData.Instance:GetRoleLevel()
    param_t.profession = RoleData.Instance:GetRoleProfId()
    param_t.color = app_data.color
    param_t.surface_list = IllusionData.Instance.fashion_wear_list
    param_t.weapon_id = app_data.weapon_id

    LoginData.Instance:SetLastRoleData(param_t)
end

--=========================新拓展==================================================
function RoleSettingPanel:FlushAvatarCell()
    self.AvatarCell:SetData({
        avatar_id = RoleData.Instance:GetRoleAvatarId(),
        avatar_type = RoleData.Instance:GetRoleAvatarType(),
        avatar_quality = RoleData.Instance:GetRoleAvatarQuality(),
        image_type = AvatarImageType.Other
    })
end

RoleSettingVolume = RoleSettingVolume or DeclareMono("RoleSettingVolume")

function RoleSettingVolume:SetKeyValueData(key, value)
    self.key = key
    self.Progress:SetProgress((math.abs(value) == 
        RoleSettingData.VOLUME_ZERO and 0 or math.abs(value)) / 100)
    self:DragVolume()
    self.Toggle.isOn = value > 0
end

function RoleSettingVolume:DragVolume()
    local value = self.Slider.value
    local value_int = math.modf(value*100)
    self.Progress:SetProgress(value)
    self.Progress:SetText(value_int .. "%")
    local oldVal = RoleSettingData.Instance:GetData(self.key)
    if value_int == 0 then
        value_int = RoleSettingData.VOLUME_ZERO
    end
    if oldVal < 0 then  --如果原本是关着的，保持关着的表现
        value_int = -value_int;
    end
    RoleSettingData.Instance:SetData(self.key, value_int)
end

function RoleSettingVolume:ToggleChange()
    local is_on = self.Toggle.isOn
    self.Mark.localPosition = is_on and Vector2.New(21, 0) or Vector2.New(-21, 0)
    self.TipPos.localPosition = is_on and Vector2.New(-21, 0) or Vector2.New(21, 0)
    self.Tip.text = is_on and Language.RoleSetting.On or Language.RoleSetting.Off

    local value_int = RoleSettingData.Instance:GetData(self.key)
    value_int = is_on and math.abs(value_int) or -math.abs(value_int)
    RoleSettingData.Instance:SetData(self.key, value_int)
end
