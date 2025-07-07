EndlessTowerView = EndlessTowerView or DeclareView("EndlessTowerView", "endless_tower/endless_tower_view",Mod.Experience.EndlessTower)

VIEW_DECLARE_LEVEL(EndlessTowerView,ViewLevel.Lu)
VIEW_DECLARE_MASK(EndlessTowerView,ViewMask.None)

function EndlessTowerView:EndlessTowerView()
    self.data = EndlessTowerData.Instance
    self.care_handle = self.data.anims_data:Care(BindTool.Bind(self.FlushEnabled,self))
    self.rank_cross_handle = SocietyData.Instance.rank_cross_data:Care(BindTool.Bind(self.FlushRankView,self))
    self.rank_all_handle = SocietyData.Instance.rank_all_data:Care(BindTool.Bind(self.FlushRankInfoView,self))
    self.AutoImg:SetActive(PrefsInt(PrefKeys.EndlessTowerAuto())==1)
    self:InitEndlessTowerView()
end

function EndlessTowerView:LoadCallback(param_t)
    self:FlushEnabled()
    self:InitModelScene()
    self:InitQualitySettings()
    self:FlushndlessTowerView()
    self:FlushRankView()
end

function EndlessTowerView:InitEndlessTowerView()
    SocietyCtrl.Instance:SendCrossGetPersonAllRankList(PERSON_RANK_TYPE.ENDLESS_TOWER)
    EndlessTowerCtrl.Instance:SendEndlessTowerReq(0)
    MainOtherCtrl.Instance:CloseKeyUse()
end

function EndlessTowerView:FlushEnabled()
    UH.SetEnabled(self.Anim,self.data:GetOutBattle()==0)
end

function EndlessTowerView:FlushndlessTowerView()
    local pass_level = self.data:GetPassLevel()
    local data = self.data:GetPassLevelData(pass_level) or self.data:EndPassLevelData()
    UH.SetText(self.Level,Format(Language.DeepDarkFantasy.LayerShow,data.seq))
    local reward_list = self.data:GetRewardData(data.reward_id)
    self.Grid:SetData(DataHelper.FormatItemList(reward_list))
    self:SetVideoView(data)
    self.data:SetSelectData(data)
    UH.SetInter(self.BtnInter,self.data:GetPassLevelData() ~= nil)
    self.Complete:SetActive(self.data:GetPassLevelData() == nil)
end

function EndlessTowerView:FlushRankView()
    local rank_list = self.data:GetRankCrossList()
    for i=1,self.RankList:Length() do
        self.RankList[i]:SetData(rank_list[i])
    end
end

function EndlessTowerView:FlushRankInfoView()
    self.data:InitRankInfo()
end

function EndlessTowerView:OpenCallback()
	ActFormationCtrl.Instance:SendGetFormationInfo(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_ENDLESS_TOWER)
    self:SendGetPersonRankList()
end

function EndlessTowerView:SendGetPersonRankList()
    if LoginData.Instance:IsOnCrossSever() then
        SocietyCtrl.Instance:SendCrossGetPersonRankList(PERSON_RANK_TYPE.ENDLESS_TOWER,{})
    else
        SocietyCtrl.Instance:SendGetPersonRankList(PERSON_RANK_TYPE.ENDLESS_TOWER)
    end
end

function EndlessTowerView:OnClickClose()
    if EndlessTowerCtrl.CloseFunc then
        EndlessTowerCtrl.CloseFunc()
        EndlessTowerCtrl.CloseFunc = nil
    end
    ViewMgr:CloseView(EndlessTowerView)
end

function EndlessTowerView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(176)
end

function EndlessTowerView:OnClickReward()
    ViewMgr:OpenView(EndlessTowerRankView,{open_param = 2})
end

function EndlessTowerView:OnClickFormation()
    if EndlessTowerCtrl.IsInGoNextIng == true then
        return
    end
    if self.data:GetPassLevelData() == nil then
        PublicPopupCtrl.Instance:Center(Language.EndlessTower.EndPassTip)
        return
    end
    ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_ENDLESS_TOWER)
end

function EndlessTowerView:OnClickRank()
    ViewMgr:OpenView(EndlessTowerRankView)
end

function EndlessTowerView:OnClickEnter()
    if EndlessTowerCtrl.IsInGoNextIng == true then
        return
    end
    if self.data:GetPassLevelData() == nil then
        PublicPopupCtrl.Instance:Center(Language.EndlessTower.EndPassTip)
        return
    end
    local pet_num = self.data:GetPetNum()
    if ActFormationData.Instance:GetHasEmptyAssist(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_ENDLESS_TOWER) then
		PublicPopupCtrl.Instance:DialogTips({
			content = Format(Language.EndlessTower.PetErrorDialog,pet_num,pet_num),
			cancel = {
                name = Language.CourageChallenge.SettingTip,
                func = function()
					PublicPopupCtrl.Instance:CloseDialogTips()
					self:OnClickFormation()
				end
			},
			confirm = {
                name = Language.CourageChallenge.FinishBtnName[1],
                func = function()
					PublicPopupCtrl.Instance:CloseDialogTips()
                    if self.data:OperaState() == EndlessTowerType.GoNext then
                        self.data:OperaState(EndlessTowerType.GoNextBattle)
                        EndlessTowerCtrl.IsInGoNextIng = true
                    else
                        self.data:OperaState(EndlessTowerType.GoBattle)
                    end
				end
			},
		})
    else
        if self.data:OperaState() == EndlessTowerType.GoNext then
            self.data:OperaState(EndlessTowerType.GoNextBattle)
            EndlessTowerCtrl.IsInGoNextIng = true
        else
            self.data:OperaState(EndlessTowerType.GoBattle)
        end
	end
end

--===============================录像相关===============================
function EndlessTowerView:SetVideoView(data)
    local value = data.monster_group or 0
	local video_config = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_ENDLESS_TOWER,value)
	if video_config then
		if self.handle_care then
            BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        end
        self.handle_care = BattleVideo.Instance:SimpleInfo():Care(BindTool.Bind(self.FlushGrayView, self, video_config))
        self:FlushGrayView(video_config)
        BattleCtrl.BattleReportSimpleInfo(video_config.monster_group_id, 0)
	end
end

function EndlessTowerView:FlushGrayView(video_config)
	self.ViewGray.enabled = BattleVideo.Instance:GetSimpleInfo(video_config.monster_group_id, 0) == 0
end

function EndlessTowerView:OnClickPlayVideo()
	local data = self.data:GetSelectData()
    local value = data.monster_group or 0
    local video_config = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_ENDLESS_TOWER,value)
	if video_config ~= nil then
		BattleVideo.Instance:PlayVideoCfg(video_config)
        BattleCtrl.BattleReportDetailInfo(video_config.monster_group_id)
    else
        PublicPopupCtrl.Instance:Center(Language.DreamNotes.NotHasPlayVideo)
	end
end

--===============================场景相关===============================
function EndlessTowerView:InitModelScene()
    ResMgr:LoadGameObject("actor/other/WJZT_2.prefab", function (obj)
        self.obj = obj
        self.obj.transform.localPosition = Vector3.New(1000,1000,1000)
        self.obj:SetParent(UnityGameObj.Find("ResidentObjectManager"))
    end, true, false)
end

function EndlessTowerView:InitQualitySettings()
    self:OnQualitySettings()
    EndlessTowerCtrl.CloseFunc = function()
        if self.obj then
            UnityGameObj.Destroy(self.obj)
            self.obj = nil
        end
        self:CancelQualitySettings()
    end
    self.scene_care_handle = SceneMgr:CareLoadedSceneIdChange(BindTool.Bind(self.OnQualitySettings,self))
end

function EndlessTowerView:OnQualitySettings()
    self.fog = UnityEngine.RenderSettings.fog
    self.ambientLight = UnityEngine.RenderSettings.ambientLight
    self.pixelLightCount = UnityEngine.QualitySettings.pixelLightCount
    self.anisotropicFiltering = UnityEngine.QualitySettings.anisotropicFiltering
    self.shadows = UnityEngine.QualitySettings.shadows
    self.shadowResolution = UnityEngine.QualitySettings.shadowResolution
    self.shadowCascades = UnityEngine.QualitySettings.shadowCascades
    self.shadowDistance = UnityEngine.QualitySettings.shadowDistance
    UnityEngine.RenderSettings.fog = false
    UnityEngine.RenderSettings.ambientLight = COLORS.CloudArena
    UnityEngine.QualitySettings.pixelLightCount = 20
    UnityEngine.QualitySettings.anisotropicFiltering = 2
    UnityEngine.QualitySettings.shadows = 3
    UnityEngine.QualitySettings.shadowDistance = 100
    UnityEngine.QualitySettings.shadowResolution = 2
    UnityEngine.QualitySettings.shadowCascades = 4
end

function EndlessTowerView:CancelQualitySettings()
    UnityEngine.RenderSettings.fog = self.fog
    UnityEngine.RenderSettings.ambientLight = self.ambientLight
    UnityEngine.QualitySettings.pixelLightCount = self.pixelLightCount
    UnityEngine.QualitySettings.anisotropicFiltering = self.anisotropicFiltering
    UnityEngine.QualitySettings.shadows = self.shadows
    UnityEngine.QualitySettings.shadowResolution = self.shadowResolution
    UnityEngine.QualitySettings.shadowCascades = self.shadowCascades
    UnityEngine.QualitySettings.shadowDistance = self.shadowDistance
end

function EndlessTowerView:CloseCallback()
    if EndlessTowerCtrl.CloseFunc then
        EndlessTowerCtrl.CloseFunc()
        EndlessTowerCtrl.CloseFunc = nil
    end
    if UnityEngine.RenderSettings.fog ~= self.fog then
        self:CancelQualitySettings()
    end
    if self.obj then
        UnityGameObj.Destroy(self.obj)
    end
    if nil ~= self.scene_care_handle then
        SceneMgr:UncareLoadedSceneIdChange(self.scene_care_handle)
        self.scene_care_handle = nil
    end
    if self.handle_care then
        BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        self.handle_care = nil
    end
    if self.care_handle then
        self.data.anims_data:Uncare(self.care_handle)
        self.care_handle = nil
    end
    if self.rank_cross_handle then
        SocietyData.Instance.rank_cross_data:Uncare(self.rank_cross_handle)
        self.rank_cross_handle = nil
    end
    if self.rank_all_handle then
        SocietyData.Instance.rank_all_data:Uncare(self.rank_all_handle)
        self.rank_all_handle = nil
    end
    if ViewMgr:IsOpen(EndlessTowerBoard) then
        ViewMgr:CloseView(EndlessTowerBoard)
    end
    TimeHelper:CancelTimer(self.OpenTimer)
end

function EndlessTowerView:OnToggleAuto()
	local is_active = not GetActive(self.AutoImg)
	if is_active then
		PublicPopupCtrl.Instance:Center(Language.CourageChallenge.AutoFightTips)
	end
	self.AutoImg:SetActive(is_active)
	PrefsInt(PrefKeys.EndlessTowerAuto(),is_active and 1 or 0)
end
----------------------------EndlessTowerRankCall----------------------------
EndlessTowerRankCall = EndlessTowerRankCall or DeclareMono("EndlessTowerRankCall", UIWidgetBaseItem)

function EndlessTowerRankCall:SetData(data)
    if data and data.role then
        UH.SetText(self.Name,data.role.name)
        UH.SetText(self.Level,data.rank_value)
    else
        UH.SetText(self.Name,Language.TombRaider.WithoutPlayer)
        UH.SetText(self.Level,"--")
    end
end

EndlessTowerOpera = EndlessTowerOpera  or DeclareMono("EndlessTowerOpera",UIWFlushPanel)

function EndlessTowerOpera:EndlessTowerOpera()
    self.ModelGame = {}
    self.data = EndlessTowerData.Instance
    self:CreateModel(self.data:GetPassLevelData())
    self:OnListenEvent()
    self.data_cares = {
        {data = self.data.opera_data, func = self.FlushAnimStateView,keys = {"cur_state"},init_call = false},
    }
    self:FlushAnimStateView()
end

function EndlessTowerOpera:CreateModel(data)
    if data == nil then
        if self.ModelGame[1] == nil then 
            local appearance = CommonStruct.AppearanceParam(RoleData.Instance:GetApearance())
            self.ModelGame[1] = UIChDrawer.CreateByAppearance(appearance)
            self.ModelGame[1]:Enable()
            self.ModelGame[1]:GetRoot():SetParent(self.RolePos)
            self.ModelGame[1]:GetRoot():SetLocalPosition(Vector3.zero)
            self.ModelGame[1]:GetRoot():SetLocalEulerAngles(Vector3.zero)
            self.ModelGame[1]:GetRoot():SetLocalScale(Vector3.one)
        end
        return 
    end
    local monster_cfg = Cfg.MonsterById(data.boss_id)
    local res_path = DrawerHelper.GetNpcPath(ResHelper.MonsterResId(data.boss_id))
    if self.ModelGame[0] ~= nil then 
        self.ModelGame[0]:SetData(res_path)
    else
        self.ModelGame[0] = UIObjDrawer.CreateByPath(res_path)
        self.ModelGame[0]:Enable()
        self.ModelGame[0]:GetRoot():SetParent(self.BossPos)
        self.ModelGame[0]:GetRoot():SetLocalPosition(Vector3.zero)
        self.ModelGame[0]:GetRoot():SetLocalEulerAngles(Vector3.zero)
        self.ModelGame[0]:GetRoot():SetLocalScale(Vector3.one)
    end
    if self.ModelGame[1] == nil then 
        local appearance = CommonStruct.AppearanceParam(RoleData.Instance:GetApearance())
        self.ModelGame[1] = UIChDrawer.CreateByAppearance(appearance)
        self.ModelGame[1]:Enable()
        self.ModelGame[1]:GetRoot():SetParent(self.RolePos)
        self.ModelGame[1]:GetRoot():SetLocalPosition(Vector3.zero)
        self.ModelGame[1]:GetRoot():SetLocalEulerAngles(Vector3.zero)
        self.ModelGame[1]:GetRoot():SetLocalScale(Vector3.one)
    end

    if self.data:GetOutBattle()==0 then
        ViewMgr:OpenView(EndlessTowerBoard,{obj = self.ModelGame[0]:GetRoot(),config = monster_cfg})
    else
        self.ShowFunc = function ()
            ViewMgr:OpenView(EndlessTowerBoard,{obj = self.ModelGame[0]:GetRoot(),config = monster_cfg})
        end
    end
    local effect_path = Format([[effect/517505%s.prefab]],data.element)
    ResMgr:LoadGameObject(effect_path, function (obj)
        self.DoorGame = obj
        self.DoorGame:SetParent(self.DoorPos)
        self.DoorGame:SetLocalPosition(Vector3.zero)
        self.DoorGame:SetLocalEulerAngles(Vector3.zero)
        self.DoorGame:SetLocalScale(Vector3.one)
    end, true, false)
end

function EndlessTowerOpera:OnListenEvent()
    self.Anim:ListenEvent([[go_next]],
    function (eve_param,anim_info)
        if eve_param == [[exit]] then
            self.data:OperaState(EndlessTowerType.GoBattle)
        end  
    end)
    self.Anim:ListenEvent([[go_battle]],
    function (eve_param,anim_info)
        if eve_param == [[exit]] then
            if EndlessTowerCtrl.CloseFunc then
                EndlessTowerCtrl.CloseFunc()
                EndlessTowerCtrl.CloseFunc = nil
            end
            EndlessTowerCtrl.Instance:OnEnterEndlessTower()
        end 
    end)
end

function EndlessTowerOpera:FlushAnimStateView()
    self.Anim:SetInteger(APH("state"), self.data:OperaState())
    if self.data:OperaState() == EndlessTowerType.GoNext then
        EndlessTowerCtrl.IsInGoNextIng = true
        self.ShowTimer = Invoke(function()
            EndlessTowerCtrl.IsInGoNextIng = nil
            if self.ShowFunc then
                self.ShowFunc()
                self.ShowFunc = nil
            end
        end,2)
    end
end

function EndlessTowerOpera:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self,[[ModelGame]])
    self.data:Initialization()
    TimeHelper:CancelTimer(self.ShowTimer)
    EndlessTowerCtrl.IsInGoNextIng = nil
end

function EndlessTowerOpera:ModeMoveCheck(is_chara,state)
    local operate = self.ModelGame[is_chara]
    if operate then
        operate:PlayAnim(state == 1 and ChAnimType.Run or ChAnimType.Idle)
    end
end

function EndlessTowerOpera:ModeShowCheck(is_chara,state)
    local operate = self.ModelGame[is_chara]
    if operate and state == 1 then
        operate:PlayAnim(ChAnimType.Show)
    end
end

function EndlessTowerOpera:ModeDoorShowCheck(state)
end

EndlessTowerModelCheck = EndlessTowerModelCheck or DeclareMono("EndlessTowerModelCheck", UIWFlushPanel)
function EndlessTowerModelCheck:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if self.IsShow then
        self.Main:ModeShowCheck(self.IsCharacter, 1)
    else
        self.Main:ModeMoveCheck(self.IsCharacter, 1)
    end
end

function EndlessTowerModelCheck:OnDisable()
    UIWFlushPanel.OnDisable(self)
    if self.IsShow then
        self.Main:ModeShowCheck(self.IsCharacter, 0)
    else
        self.Main:ModeMoveCheck(self.IsCharacter, 0)
    end
end

EndlessTowerDoorShowCheck = EndlessTowerDoorShowCheck or DeclareMono("EndlessTowerDoorShowCheck",UIWFlushPanel)
function EndlessTowerDoorShowCheck:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.Main:ModeDoorShowCheck(1)
end 

function EndlessTowerDoorShowCheck:OnDisable()
    UIWFlushPanel.OnDisable(self)
    self.Main:ModeDoorShowCheck(0)
end

EndlessTowerBoard = EndlessTowerBoard or DeclareView("EndlessTowerBoard", "endless_tower/endless_tower_board")
VIEW_DECLARE_MASK(EndlessTowerBoard,ViewMask.None)
function EndlessTowerBoard:LoadCallback(param_t)
    if param_t.obj == nil then
        return
    end
    self.Follow.Target = param_t.obj.transform
    UH.SetText(self.Name,param_t.config.name)
    self.Element:SetData(MonsterData.GetElement(param_t.config), false, param_t.config.is_chaos == 1)
end