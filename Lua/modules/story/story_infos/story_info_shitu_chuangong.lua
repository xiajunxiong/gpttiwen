
StoryInfoShiTuChuangGong = StoryInfoShiTuChuangGong or BaseClass(StoryInfo)

function StoryInfoShiTuChuangGong:StoryInfo()
end

function StoryInfoShiTuChuangGong:StartStory()
    self.state = StoryConfig.State.Loading
    self:storyReady()
    
    local is_succ, role_paths, role_avatars = SocietyData.Instance:GetShiTuChuanGongSceneInfo()
    self.role_objs = {}
    if not is_succ then
        self:storyEnd()
        return
    end
    self.role_paths = role_paths
    self.role_avatars = role_avatars

    self.obj_parent = UnityGameObj.Find("ResidentObjectManager")
    for i = 1, #role_paths do
        local role_path = role_paths[i]
        GameObjPool:RegisterPool(role_path, BindTool.Bind(self.onResLoaded, self, i), true)
    end
    return self
end

function StoryInfoShiTuChuangGong:onResLoaded(index)
    local role_path = self.role_paths[index]
    self.role_objs[index] = GameObjPool:Get(role_path, self.obj_parent)
    self:LoadStart()
end

function StoryInfoShiTuChuangGong:LoadStart()
    if 2 == #self.role_objs then
        -- 等场景加载完
        if SocietyData.Instance:ShiTuChuanGongConfigOtherSceneId() == SceneMgr:LoadedSceneId() then
            ResMgr:LoadGameObject(DrawerHelper.GetStoryPath(self.info.res_name), BindTool.Bind(self.storyLoaded, self), self.instantiate)
        else
            TimeHelper:AddDelayFrameTimer(function ()
                self:LoadStart()
            end)
        end
    end
end

function StoryInfoShiTuChuangGong:Play()
    ViewMgr:MainViewVisible(false)
    SceneCtrl.Instance:Objs():ShieldingWithoutTypes(true, {})
    -- BattleData.Instance.story_data.hide_ui = true
    StoryInfo.Play(self)
end

function StoryInfoShiTuChuangGong:StoryLoaded(obj)
    self.story_mono = obj:GetLuaComponent(StoryMonoItem:GetTypeName())
    self.story_mono:SetStoryInfo(self)
    Runner.Instance:AddRunObj(self, 3)
end

function StoryInfoShiTuChuangGong:StoryDyLoad(wait_until_get_resource, res_type, res_id)
    if res_type == "RoleAnim" then
        local prof = RoleData.Instance:GetRoleAvatarType()
        wait_until_get_resource(
            function()
                local obj = self.role_objs[res_id]
                local animator = obj:GetComponent(typeof(UnityAnim))
                local cullingMode = animator.cullingMode
                animator.cullingMode = "AlwaysAnimate"
                obj.transform:SetParent(self.story_mono.RolePos[res_id])
                obj:ResetTransform()
                return obj
            end,
            self.role_avatars[res_id]
        )
    end
end

function StoryInfoShiTuChuangGong:StoryEvent(type, params)
    if StoryConfig.CustomEventType.GameStart == type then
        -- MusicGameCtrl.Instance:MusicGame(1)
        SocietyCtrl.Instance:SendShiTuChuanGongFBReqGameReady()
    end
end

function StoryInfoShiTuChuangGong:StoryEnd()
    Runner.Instance:RemoveRunObj(self)
    for i = 1, #self.role_objs do
        local obj = self.role_objs[i]
        if obj ~= nil then
            GameObjPool:Release(obj)    
        end
    end
    self.role_objs = {}
    ViewMgr:MainViewVisible(true)
    SceneCtrl.Instance:Objs():ShieldingWithoutTypes(false, {})
    if TeamData.Instance:InTeam() and TeamData.Instance:IsLeader() then
        SceneCtrl.Instance:RequestLeaveFb()
    end
end

function StoryInfoShiTuChuangGong:Update()
    if SocietyData.Instance.shitu_chuangong_loop and self.story_mono.Director.time > 3.48 then
        self.story_mono.Director.time = 2.27
    end
end