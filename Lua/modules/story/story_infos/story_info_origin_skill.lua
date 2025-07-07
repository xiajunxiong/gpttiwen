
StoryInfoOriginBattleSkill = StoryInfoOriginBattleSkill or BaseClass(StoryInfo)

function StoryInfoOriginBattleSkill:StoryInfo()
    self.trigger_type = StoryConfig.TriggerType.None
    self.group_id = 1001
end

function StoryInfoOriginBattleSkill:StoryReady()
    -- ViewMgr:OpenView(StoryShowEffectView)
    self.main_role_info = { is_ready = false }
end

function StoryInfoOriginBattleSkill:storyCheckResourcesEnd()
end

function StoryInfoOriginBattleSkill:Play()
    -- self.data.story_event:Set{id = self.info.id}
    BattleData.Instance.story_data.hide_ui = true
    StoryInfo.Play(self)
end

function StoryInfoOriginBattleSkill:StoryLoaded(obj)
    self.story_mono = obj:GetLuaComponent(StoryMonoItem:GetTypeName())
    self.story_mono:SetStoryInfo(self)
end

function StoryInfoOriginBattleSkill:StoryDyLoad(wait_until_get_resource, res_type, res_id)
    if res_type == "MainRoleAnim" then
        local prof = RoleData.Instance:GetRoleAvatarType()
        wait_until_get_resource(
            function()
                local ch = BattleCtrl.Instance.displayer:getCh(3) or BattleCtrl.Instance.displayer:getCh(8)
                if nil == ch then return end
                local obj = ch:GetSkin0()
                local animator = ch:GetSkin0Animator()
                local cullingMode = animator.cullingMode
                animator.cullingMode = "AlwaysAnimate"
                self.main_role_info = {
                    is_ready = true,
                    ch = ch,
                    obj = obj,
                    pos = obj.transform,
                    parent = ch:GetRoot(),
                    animator = animator,
                    cullingMode = cullingMode,
                }
                return obj
            end,
            prof
        )
    end
end

function StoryInfoOriginBattleSkill:StoryEvent(type, params)
    if StoryConfig.CustomEventType.SetParent == type and self.main_role_info.is_ready then
        self.main_role_info.pos:SetParent(self.story_mono.MainRolePos)
        self.main_role_info.obj:ResetTransform()
        self.main_role_info.obj:SetActive(true)
        self.main_role_info.ch:HideWeapon()
    end
end
function StoryInfoOriginBattleSkill:StoryEnd()
    -- ViewMgr:CloseView(StoryShowEffectView)
    BattleData.Instance.story_data.hide_ui = false
    if self.main_role_info.is_ready then
        self.main_role_info.pos:SetParent(self.main_role_info.parent.transform)
        self.main_role_info.obj:ResetTransform()
    end
    self.main_role_info = { is_ready = false }
end