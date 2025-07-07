
FILE_REQ("modules/story/story_config")
FILE_REQ("modules/story/story_info")
FILE_REQ("modules/story/story_data")
FILE_REQ("modules/story/story_options_data")

StoryCtrl = StoryCtrl or BaseClass(BaseCtrl)

function StoryCtrl:__init()
    if StoryCtrl.Instance ~= nil then
        Debuger.LogError("[StoryCtrl] attempt to create singleton twice!")
        return
    end
    StoryCtrl.Instance = self
    
    self.data = StoryData.New()

    self:RegisterAllProtocols()
end
function StoryCtrl:OnInit()
    
end
function StoryCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSTaskChosedReq)

	self:RegisterProtocol(SCMainTaskChosedTaskList, "OnMainTaskChosedTaskList")
end

function StoryCtrl:OnUnloadGameLogic()
    for k, v in pairs(self.data.story_pre_plays) do
        v:storyEnd()
    end
    self.data.story_pre_plays = {}
end

function StoryCtrl:OnMainTaskChosedTaskList(protocol)
    self.data:SetMainTaskChosedTaskList(protocol)
end

function StoryCtrl:SendTaskChosedReq(req_type, param1, param2)
    local protocol = GetProtocol(CSTaskChosedReq)
	protocol.req_type = req_type or 0
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
    protocol.param3 = 0
	SendProtocol(protocol)
end

-- function StoryCtrl:SendTaskChosedReqInfo()
--     self:SendTaskChosedReq(StoryConfig.OptionReqType.info)
-- end

-- function StoryCtrl:SendTaskChosedReqRecord(button_group, button_id)
--     self:SendTaskChosedReq(StoryConfig.OptionReqType.record, button_group, button_id)
-- end

function StoryCtrl:PlayStoryByMonsterGroupId(monster_group_id, trigger_type)
    return self.data:StartStoryByMgi(monster_group_id, trigger_type)
end

function StoryCtrl:VCamSetting(vir_cam)
    local monster_group_id = BattleData.Instance:BaseData().monster_group_id
    if 1011 == monster_group_id then
        local len = vir_cam.m_Lens
        len.FieldOfView = 35
        vir_cam.m_Lens = len
    end
end

function StoryCtrl:PlayStoryByStoryId(id)
    return self.data:PlayStoryByStoryId(id)
end

function StoryCtrl:EnterBattle()
    local monster_group_id = BattleData.Instance:BaseData().monster_group_id
    if 1002 == monster_group_id --[[ and BattleData.Instance:BaseData().cur_round == 1 ]] then
        self.data:RecordStoryPrePlay(101, self.data:StartStoryById(101))
        -- self.data:RecordStoryPrePlay(102, self.data:StartStoryById(102))
        -- self.data:RecordStoryPrePlay(103, self.data:StartStoryById(103))
        -- self.data:RecordStoryPrePlay(104, self.data:StartStoryById(104))
    end
    -- if 1001 == monster_group_id --[[ and BattleData.Instance:BaseData().cur_round == 1 ]] then
    --     self.data:RecordStoryPrePlay(107, self.data:StartStoryById(107))
    -- end
end


----TEMP_CODE 解决剧情战斗开始时镜头会闪一帧到正式画面的问题

--开始load战斗资源的通知，此处根据BattleData load需要初始播放的剧情资源
function StoryCtrl:OnBattleBeginLoading(func)
    local monster_group_id = BattleData.Instance:BaseData().monster_group_id
    self.battle_story_infos = {}
    if 1001 == monster_group_id --[[ and BattleData.Instance:BaseData().cur_round == 1 ]] then
        LoginData.Instance:NewCharaCreateComplete()
        self.battle_story_infos[106] = self.data:StartStoryById(106)
        -- self.battle_story_infos[107] = self.data:StartStoryById(107)
    end
    if not BattleData.Instance:IsVideo() and StoryConfig.SpecialStoryM2I[monster_group_id] --[[ and BattleData.Instance:BaseData().cur_round == 1 ]] then
        local story_id = StoryConfig.SpecialStoryM2I[monster_group_id]
        self.battle_story_infos[story_id] = self.data:StartStoryById(story_id)
        -- self.battle_story_infos[107] = self.data:StartStoryById(107)
    end
    if next(self.battle_story_infos) then
        Runner.Instance:RunUntilTrue(function ()
            for _, info in pairs(self.battle_story_infos) do
                if StoryConfig.State.Loading == info:State() then
                    return false
                end
            end
            func()
            return true
        end)
    else
        func()
    end
end

--loading结束关闭loading遮罩的通知函数，此处直接播放初始剧情
function StoryCtrl:OnBattleLoadingComplete()
    for id, info in pairs(self.battle_story_infos) do
        if StoryConfig.TriggerType.BattleStartNew == info:TriggerType() then
            info:Play()
            self.battle_story_infos[id] = nil
        end
    end
    local monster_group_id = BattleData.Instance:BaseData().monster_group_id
    if 1001 == monster_group_id --[[ and BattleData.Instance:BaseData().cur_round == 1 ]] then
        self.data:RecordStoryPrePlay(107, self.data:StartStoryById(107))
    end
end
------------------------------------

function StoryCtrl:PlayStoryByTaskId(task_id)
   return self.data:StartStoryByTaskId(task_id)
end

function StoryCtrl:PlayStoryByHHShowScene(scene_id)
    return self.data:StartStoryByHHShowScene(scene_id)
 end

function StoryCtrl:IsBattleStory()
    local monster_group_id = BattleData.Instance:BaseData().monster_group_id
    return nil ~= Cfg.StoryByTypeParam1(StoryConfig.StoryType.show_boss, monster_group_id, false)
 end
 
function StoryCtrl:CreateSoldier()
    local soldier_list = self.data:SoldierCfg()
    if soldier_list then
        for k, v in pairs(soldier_list) do
            v.obj_type = SceneObjType.Soldier
            v.obj_id = SceneObjHolder.CalcClientObjId(v.scene_id, v.obj_type, k)
            v.alway_show = true
            VoMgr:AddGCObj(v)
        end
    end
end

function StoryCtrl:TestOptions(id)
    MusicGameCtrl.Instance:MusicGame(1, true)
end
 