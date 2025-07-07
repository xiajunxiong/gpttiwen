TowerAdventureProfSelectView = TowerAdventureProfSelectView or DeclareView("TowerAdventureProfSelectView", "mini_game/tower_adventure_prof_select")
VIEW_DECLARE_LEVEL(TowerAdventureProfSelectView, ViewLevel.Ls)
VIEW_DECLARE_MASK(TowerAdventureProfSelectView, ViewMask.None)
function TowerAdventureProfSelectView:TowerAdventureProfSelectView()
    -- 小游戏创角界面,共用
    self.Data = NeoCharaChangeData.Instance
    self.SelectId = 1
    LoginCtrl.Instance:SendRandomNameReq()
    self.ht = RoleCreateData.Instance.create_view_data:Care(BindTool.Bind(self.GetRandName, self))
    self.ht2 = self.Data.prof_view_data:Care(BindTool.Bind(self.FlushSkillPanel, self), "is_show_skill_tips")
    self.ObjBlock:SetActive(false)
end

function TowerAdventureProfSelectView:LoadCallback()
    if self.cur_ui_obj == nil then
        self.cur_ui_obj = UIChDrawer.New()
        self.CurAppear:SetShowData({
            ui_obj = self.cur_ui_obj,
            view = self,
        })
    end

    if self.next_ui_obj == nil then
        self.next_ui_obj = UIChDrawer.New()
        self.NextAppear:SetShowData({
            ui_obj = self.next_ui_obj,
            view = self,
        })
    end
    self:SetRandInfo()
    self:FlushAll()
    self:FlushSelect(1)
end

function TowerAdventureProfSelectView:GetRandName()
    self.rand_name = RoleCreateData.Instance.create_view_data.role_name
end

function TowerAdventureProfSelectView:SetRandInfo()
    self.rand_info = {}
    self.rand_info.avartar_info = MiniGameData.Instance:CreateRandAvartarInfo()
    self.rand_info.jod_info = MiniGameData.Instance:CreateRandJobInfo()
end

function TowerAdventureProfSelectView:OnClickSureChara()
    MiniGameData.Instance:SetRandInfo(self.rand_info.avartar_info, self.rand_info.jod_info[self.SelectId])
    -- 只允许点击一次
    self.ObjBlock:SetActive(true)
    self:CreateRole()
end

function TowerAdventureProfSelectView:CreateRole()
    RoleCreateData.Instance.create_view_data.appear_id = self.rand_info.avartar_info.avartar_type
    RoleCreateData.Instance.create_view_data.color_index = 0
    RoleCreateData.Instance.create_view_data.prof_id = self.rand_info.jod_info[self.SelectId].job_id
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
	local role_name = self.rand_name
	local value, error_code = RoleCreateData.Instance:CheckRoleName(role_name)
	if value then
		LoginCtrl.Instance:RequestCreateRole(function(err_code)
                if err_code >= RoleCreateResult.Success then
                    -- 创角成功
					RoleCreateData.Instance:ResetModelRotateInfo()
					local user_id = LoginData.Instance:GetPlatformName()
					local role_name = GameUtil.EncodeB64(RoleCreateData.Instance.role_name.name)
					--角色创建申请结果埋点
					MainProber:Step(MainProber.STEP_CREATE_ROLE_END, user_id, LoginData.Instance:GetLastLoginServer(), role_name, LoginData.Instance:GetNewCreateRoles().id) 
                    -- PublicPopupCtrl.Instance:Center(string.format("RecvCreateRole SUC:%s",tostring(err_code)))
                    -- 角色登录
					-- LoginCtrl.Instance:RequestRoleLogin(LoginData.Instance:GetNewCreateRoles().id)
					-- 创角成功后清除封测充值金额
					LoginData.Instance:ResetFCRecharge()
                    LoginData.Instance:NewCharaCreateComplete()

                    self:CreateSuccess()
                    -- 重置是否需要玩小游戏标志
	                LoginData.Instance:SetIsPlayMiniGame(false)
				else
					if err_code == -2 then
						PublicPopupCtrl.Instance:Center(ErrorText[49])
					else
						PublicPopupCtrl.Instance:Center(string.format("RecvCreateRole ERROR:%s", tostring(err_code)))
                    end
                    -- 失败的话关闭遮罩
                    self.ObjBlock:SetActive(false)
				end
			end)
	else
        PublicPopupCtrl.Instance:Center(error_code)
        -- 失败的话关闭遮罩
        self.ObjBlock:SetActive(false)
	end

	self.enter_timer = TimeHelper:AddDelayTimer(function()
		-- 5秒钟以后还在试图创建角色
		self.enter_timer = nil
		if LoginData.Instance:LoginState() == LoginStateType.RoleCreateRequesting then 
			LoginCtrl.GMDisconnect()
		end 
    end, 5)
end

-- 创角成功后
function TowerAdventureProfSelectView:CreateSuccess()
    local mini_game_type = LoginData.Instance:GetMiniGame()
    if mini_game_type == MINIGAME_TYPE.TOWER_ADVENTURE then
        ViewMgr:OpenView(TowerAdventureLevelView)
    elseif mini_game_type == MINIGAME_TYPE.COW_BOY then
        -- 角色登录
        LoginCtrl.Instance:RequestRoleLogin(LoginData.Instance:GetNewCreateRoles().id)
    end
    ViewMgr:CloseView(TowerAdventureProfSelectView)
end

function TowerAdventureProfSelectView:FlushSkillPanel()
    self.SkillPanel:SetActive(self.Data.prof_view_data.is_show_skill_tips)
    UH.SetText(self.TxtSkillTips, self.Data.prof_view_data.skill_info_tips)
end

function TowerAdventureProfSelectView:CloseSkillPanel()
    NeoCharaChangeData.Instance:SetProfViewSkill({is_show = false})
end 

function TowerAdventureProfSelectView:FlushAll()
    local get_appearance = function(index)
        return {
            weapon_id = RoleProfessionSimpleWeaponId[RoleData.ProfIdToProfType(self.rand_info.jod_info[index].job_id)] or 0,
            color = 0,
            prof_base = 0,
            avatar_type = self.rand_info.avartar_info.avartar_type or 1
        }
    end
    self.cur_ui_obj:SetData(get_appearance(1))
    self.next_ui_obj:SetData(get_appearance(2))

    local cur_prof = RoleData.ProfIdToProfType(self.rand_info.jod_info[1].job_id)
    local cur_desc_config = RoleData.Instance:GetProfessionDesc(self.rand_info.jod_info[1].job_id, true)
    UH.SetText(self.CurToggleTip1, cur_desc_config.weapon)
    UH.SetText(self.CurToggleTip2, cur_desc_config.armor_type)
    UH.SetText(self.CurToggleTip3, cur_desc_config.point)
    UH.SetText(self.CurToggleTip4, cur_desc_config.desc)

    local cur_skill_data = SkillData.GetProfSkillList(cur_prof)
    self.CurSkillList:SetData(cur_skill_data)
    self.CurSkillList:JumpHorizontalIndex(1, 1)
    UH.SetText(self.CurName, Language.Common.Prof[cur_prof])

    local next_prof = RoleData.ProfIdToProfType(self.rand_info.jod_info[2].job_id)
    local next_desc_config = RoleData.Instance:GetProfessionDesc(self.rand_info.jod_info[2].job_id, true)
    UH.SetText(self.NextToggleTip1, next_desc_config.weapon)
    UH.SetText(self.NextToggleTip2, next_desc_config.armor_type)
    UH.SetText(self.NextToggleTip3, next_desc_config.point)
    UH.SetText(self.NextToggleTip4, next_desc_config.desc)

    local next_skill_data = SkillData.GetProfSkillList(next_prof)
    self.NextSkillList:SetData(next_skill_data)
    self.NextSkillList:JumpHorizontalIndex(1, 1)
    UH.SetText(self.NextName, Language.Common.Prof[next_prof])
end

function TowerAdventureProfSelectView:FlushSelect(index)
    self.SelectId = index
    self.CurSelect:SetActive(index == 1)
end

function TowerAdventureProfSelectView:ClickCurToggleOn()
    self.CurBtnToggleOn:SetActive(false)
    self.CurProfToggleAnim:SetTrigger(APH("is_show"))
end

function TowerAdventureProfSelectView:ClickCurToggleOff()
    self.CurBtnToggleOn:SetActive(true)
    self.CurProfToggleAnim:SetTrigger(APH("is_hide"))
end

function TowerAdventureProfSelectView:ClickNextToggleOn()
    self.NextBtnToggleOn:SetActive(false)
    self.NextProfToggleAnim:SetTrigger(APH("is_show"))
end

function TowerAdventureProfSelectView:ClickNextToggleOff()
    self.NextBtnToggleOn:SetActive(true)
    self.NextProfToggleAnim:SetTrigger(APH("is_hide"))
end

function TowerAdventureProfSelectView:CloseCallback()
    if self.cur_ui_obj ~= nil then
		self.cur_ui_obj:DeleteMe()
	end
    self.cur_ui_obj = nil

    if self.next_ui_obj ~= nil then
		self.next_ui_obj:DeleteMe()
	end
    self.next_ui_obj = nil

    if self.enter_timer ~= nil then
        TimeHelper:CancelTimer(self.enter_timer)
        self.enter_timer = nil
	end
    RoleCreateData.Instance.create_view_data:Uncare(self.ht)
    self.Data.prof_view_data:Uncare(self.h2)
    self:CloseSkillPanel()
end