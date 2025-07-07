
StoryInfoWedding = StoryInfoWedding or BaseClass(StoryInfo)

function StoryInfoWedding:StoryInfo()
end

function StoryInfoWedding:StartStory()
    self.state = StoryConfig.State.Loading
    self:storyReady()
    
    local is_succ, avatars = MarryData.Instance:GetMarryWeddingAvatars()
    self.role_objs = {}
    if not is_succ then
        self:storyEnd()
        return
    end
    self.role_avatars = avatars

    self.obj_parent = UnityGameObj.Find("ResidentObjectManager")
    for i = 1, #avatars do
        local res_id = MarryData.AvatarTypeToResId(avatars[i])
        GameObjPool:RegisterPool(DrawerHelper.GetRolePath(res_id), BindTool.Bind(self.onResLoaded, self, i), true)
    end
    return self
end

function StoryInfoWedding:onResLoaded(index)
    local res_id = MarryData.AvatarTypeToResId(self.role_avatars[index])
    self.role_objs[index] = GameObjPool:Get(DrawerHelper.GetRolePath(res_id), self.obj_parent)
    self:LoadStart()
end

function StoryInfoWedding:LoadStart()
    if 2 == #self.role_objs then
        -- 处理刚进副本就在拜堂阶段情况(等场景加载完)
        if MarryData.Instance:ConfigOtherSceneId() == SceneMgr:LoadedSceneId() then
            ResMgr:LoadGameObject(DrawerHelper.GetStoryPath(self.info.res_name), BindTool.Bind(self.storyLoaded, self), self.instantiate)
        else
            TimeHelper:AddDelayFrameTimer(function ()
                self:LoadStart()
            end)
        end
    end
end

function StoryInfoWedding:Play()
    ViewMgr:MainViewVisible(false)
    ViewMgr:OpenView(StoryMarryBlessView)
    SceneCtrl.Instance:Objs():ShieldingWithoutTypes(true, {[SceneObjType.WeddingBaoWeiXiuQiuMonster] = true})
    -- self.data.story_event:Set{id = self.info.id}
    -- BattleData.Instance.story_data.hide_ui = true
    StoryInfo.Play(self)
end

function StoryInfoWedding:StoryLoaded(obj)
    self.story_mono = obj:GetLuaComponent(StoryMonoItem:GetTypeName())
    self.story_mono:SetStoryInfo(self)
end

function StoryInfoWedding:StoryDyLoad(wait_until_get_resource, res_type, res_id)
    if res_type == "RoleAnim" then
        local prof = RoleData.Instance:GetRoleAvatarType()
        wait_until_get_resource(
            function()
                local obj = self.role_objs[res_id]
                if nil ~= obj then
                    local animator = obj:GetComponent(typeof(UnityAnim))
                    local cullingMode = animator.cullingMode
                    animator.cullingMode = "AlwaysAnimate"
                    obj.transform:SetParent(self.story_mono.RolePos[res_id])
                    obj:ResetTransform()
                end
                return obj
            end,
            self.role_avatars[res_id]
        )
    end
end

function StoryInfoWedding:StoryEvent(type, params)
end

function StoryInfoWedding:StoryEnd()
    for i = 1, #self.role_objs do
        local obj = self.role_objs[i]
        if obj ~= nil then
            GameObjPool:Release(obj)    
        end
    end
    self.role_objs = {}
    ViewMgr:CloseView(StoryMarryBlessView)
    ViewMgr:MainViewVisible(true)
    SceneCtrl.Instance:Objs():ShieldingWithoutTypes(false, {[SceneObjType.WeddingBaoWeiXiuQiuMonster] = true})
    SocietyData.Instance:PlayGiftEffect(2, 1)
end