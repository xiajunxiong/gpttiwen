MapExploreData = MapExploreData or BaseClass()
MapExploreEffect = {
    [1016] = SceneEffectConfig.EffectList.BoxMapExploreEffect1,
    [1017] = SceneEffectConfig.EffectList.BoxMapExploreEffect2,
    [1018] = SceneEffectConfig.EffectList.BoxMapExploreEffect3,
    [1019] = SceneEffectConfig.EffectList.BoxMapExploreEffect4,
    [1027] = SceneEffectConfig.EffectList.BoxMapExploreEffect5,
}
function MapExploreData:__init()
	if MapExploreData.Instance ~= nil then
		Debuger.LogError("[MapExploreData] attempt to create singleton twice!")
		return
	end	
    MapExploreData.Instance = self

    self.explore_data = SmartData.New({map = {},reward_show = -1,mark_show = -1})
    self.explore_reward_data = SmartData.New({list = {},flush = 0})
    self.radus = Config.map_explore_auto.other[1].npc_radius
    self:MapConfig()
    self.effect_list = {}
end 

function MapExploreData:MapConfig()
    self.scene_list = {}
    for k,v in pairs(Config.map_explore_auto.npc_xy) do 
        if self.scene_list[v.scene_id] == nil then 
            self.scene_list[v.scene_id] = {}
        end 

        table.insert(self.scene_list[v.scene_id], v)
    end 

    self.explore_reward_list = {}
    for k,v in pairs(Config.map_explore_auto.explore_reward) do 
        self.explore_reward_list[v.scene_id] = v
    end

    self.anecdote_scene_list = {}
    for k,v in pairs(Config.anecdotes_tasklist_auto.task_match_map) do 
        if self.anecdote_scene_list[v.scene_id] == nil then 
            self.anecdote_scene_list[v.scene_id] = {}
        end 

        table.insert(self.anecdote_scene_list[v.scene_id],v)
    end 
    -- for k,v in pairs(AnecdoteType) do 
    --     for i,j in pairs(Config.anecdotes_tasklist_auto.task_list) do 
    --         if j.pretaskid1 == 0 and j.stranger_task_type == v then 
    --             local accept_npc = j.accept_npc
    --             local commit_npcs = Split(j.commit_npc,"|")
    --             if Config.npc_dynamic_auto.npc_dynamic_cfg[j.accept_npc] then 
    --                 local scene_id = Config.npc_dynamic_auto.npc_dynamic_cfg[j.accept_npc].scene_id
    --                 if self.anecdote_scene_list[scene_id] == nil then 
    --                     self.anecdote_scene_list[scene_id] = {}
    --                 end 
    --                 table.insert(self.anecdote_scene_list[scene_id],v)
    --                 break
    --             end 
    --             for npc_index,npc_id in pairs(commit_npcs) do 
    --                 if Config.npc_dynamic_auto.npc_dynamic_cfg[tonumber(npc_id)] then 
    --                     local scene_id = Config.npc_dynamic_auto.npc_dynamic_cfg[tonumber(npc_id)].scene_id
    --                     if self.anecdote_scene_list[scene_id] == nil then 
    --                         self.anecdote_scene_list[scene_id] = {}
    --                     end 
    --                     table.insert(self.anecdote_scene_list[scene_id],v)
    --                     break
    --                 end 
    --             end 
    --         end 
    --     end 
    -- end 

    self.challenge_scene_list = {}
    for k,v in pairs(Config.anecdotes_tasklist_auto.challenge_anecdote) do 
        if v.special_type == 0 then 
            local scene_id = v.scene_id
            if self.challenge_scene_list[scene_id] == nil then 
                self.challenge_scene_list[scene_id] = {}
            end 
    
            table.insert(self.challenge_scene_list[scene_id],{special_type = v.special_type,monster_seq = v.monster_seq})
        else 
            if self.anecdote_scene_list[v.scene_id] == nil then 
                self.anecdote_scene_list[v.scene_id] = {}
            end 
    
            local vo = {
                is_special = true,
                special_type = v.special_type,
                monster_seq =  v.monster_seq,
            }
            table.insert(self.anecdote_scene_list[v.scene_id],vo)
        end
    end 
end

function MapExploreData:OnGameSceneLoaded(scene_id)
    if self:CancelExplore(scene_id) then    
        return 
    end 

    self.range_invoke_timer = TimeHelper:AddRunTimer(function()
        self:CheckBoxCatch()
    end, 0.5)
end

function MapExploreData:CancelExplore(scene_id)
    TimeHelper:CancelTimer(self.range_invoke_timer)
    self.explore_data.reward_show = -1

    if scene_id == nil or self.scene_list[scene_id] == nil then return true end 
    local complete = true
    for k,v in pairs(self.scene_list[scene_id]) do 
        if self.explore_data.map[v.seq] == 0 then 
            complete = false
        end 
    end 
    return complete
end

function MapExploreData:CanExplore(scene_id )
    return self.scene_list[scene_id ~= nil and scene_id or SceneData.Instance:SceneId()] ~= nil
end

function MapExploreData:IsCompleteExplore()
    for k,v in pairs(self.scene_list[SceneData.Instance:SceneId()] or {}) do 
        if self.explore_data.map[v.seq] == 0 then 
            return false
        end 
    end 

    return true
end


function MapExploreData:GetBoxMapExplore(open_check)
    if not open_check then 
        return self.scene_list[SceneData.Instance:SceneId()] or {}
    else
        local map_list = {}
        for k,v in pairs(self.scene_list[SceneData.Instance:SceneId()] or {}) do 
            if self.explore_data.map[v.seq] == 0 then 
                -- table.insert( map_list, v )
                map_list[k] = v
            else
                local obj_id = SceneObjHolder.CalcClientObjId(SceneData.Instance:SceneId(), SceneObjType.MapExploreBox, v.seq)
                VoMgr:DelGCObj(obj_id)
            end
        end

        return map_list
    end
end

function MapExploreData:GetBoxMapExploreCfg(seq,scene_id)
    local operate = self.scene_list[scene_id]
    if self.scene_list[scene_id] == nil then return end 

    for k,v in pairs(operate) do 
        if seq == v.seq then 
            return v
        end 
    end 
end

function MapExploreData:GetCurExploreProg(scene_id)
    local list = self.scene_list[scene_id ~= nil and scene_id or SceneData.Instance:SceneId()] or {}

    local cur_num = 0
    for k,v in pairs(list) do
        if self.explore_data.map[v.seq] == 1 then 
            cur_num = cur_num +1
        end 
    end 
    return cur_num,#list 
end

function MapExploreData:GetCurAnecdoteEventProg(scene_id)
    local list = self.anecdote_scene_list[scene_id ~= nil and scene_id or SceneData.Instance:SceneId()] or {}

    local cur_num = 0
    for k,v in pairs(list) do
        if v.is_special then 
            if AnecdoteData.Instance:TaskIsCompelete(v.special_type,v.monster_seq) == 1 then 
                cur_num = cur_num +1
            end 
        else 
            
            if AnecdoteData.Instance:IsHasFinish(v.stranger_task_type) then 
                cur_num = cur_num +1
            end 
        end 
    end 
    return cur_num,#list 
end

function MapExploreData:GetCurAnecdoteChallengeProg(scene_id)
    local list = self.challenge_scene_list[scene_id ~= nil and scene_id or SceneData.Instance:SceneId()] or {}
    local cur_num = 0
    for k,v in pairs(list) do
        if AnecdoteData.Instance:TaskIsCompelete(v.special_type,v.monster_seq) == 1 then 
            cur_num = cur_num +1
        end 
    end 
    return cur_num,#list 
end

function MapExploreData:GetExploreSceneRewardCfg(scene_id)
    return self.explore_reward_list[scene_id]
end

function MapExploreData:GetExploreSceneReward(scene_id)
    local prog_map,prog_map_total = self:GetCurExploreProg(scene_id)
    local prog_event,prog_event_total = self:GetCurAnecdoteEventProg(scene_id)
    local prog_challenge,prog_challenge_total = self:GetCurAnecdoteChallengeProg(scene_id)
    local prog_total = prog_map + prog_event + prog_challenge
    local prog_total_total = prog_map_total + prog_event_total + prog_challenge_total

    local list = {} 
    local cfg = self:GetExploreSceneRewardCfg(scene_id)
    if cfg == nil then 
        return list
    end 
    list[1] = {
        index = 0,
        title = Language.Map.TitleTotalReward,
        reward_list = BagData.Instance:ItemListBuild(cfg.all_reward),
        seq = cfg.seq,
        flag = self.explore_reward_data.list[scene_id] and self.explore_reward_data.list[scene_id][0] or 0,
        pro_str = prog_total.."/"..prog_total_total,
        pro_rate = prog_total/prog_total_total,
        pro_complete = prog_total == prog_total_total ,
        scene_id = scene_id,
    }

    local item_2 = {
        index = 1,
        title = Language.Map.TitleExploreReward,
        reward_list = BagData.Instance:ItemListBuild(cfg.point_reward),
        seq = cfg.seq,
        flag = self.explore_reward_data.list[scene_id] and self.explore_reward_data.list[scene_id][1] or 0,
        pro_str = prog_map.."/"..prog_map_total,
        pro_rate = prog_map/prog_map_total,
        pro_complete = prog_map == prog_map_total ,
        scene_id = scene_id,
    }

    if prog_map_total > 0 then 
        table.insert(list,item_2)
    end 
    
    local item_3 = {
        index = 2,
        title = Language.Map.TitleEventReward,
        reward_list = BagData.Instance:ItemListBuild(cfg.event_reward),
        seq = cfg.seq,
        flag = self.explore_reward_data.list[scene_id] and self.explore_reward_data.list[scene_id][2] or 0,
        pro_str = prog_event.."/"..prog_event_total,
        pro_rate = prog_event/prog_event_total,
        pro_complete = prog_event == prog_event_total ,
        scene_id = scene_id,
    }

    if prog_event_total > 0 then 
        table.insert(list,item_3)
    end 

    local item_4 = {
        index = 3,
        title = Language.Map.TitleChallengeReward,
        reward_list = BagData.Instance:ItemListBuild(cfg.challenge_reward),
        seq = cfg.seq,
        flag = self.explore_reward_data.list[scene_id] and self.explore_reward_data.list[scene_id][3] or 0,
        pro_str = prog_challenge.."/"..prog_challenge_total,
        pro_rate = prog_challenge/prog_challenge_total,
        pro_complete = prog_challenge == prog_challenge_total ,
        scene_id = scene_id,
    }

    if prog_challenge_total > 0 then 
        table.insert(list,item_4)
    end 

    return {list = list}
end

function MapExploreData:CheckBoxCatch()
    -- 战斗检查
    if MainViewData.Instance:IsBattle() then return end 
    -- 虚空检查
    if SceneCtrl.Instance:CurScene() == nil or SceneCtrl.Instance:CurScene().PosToServer == nil then return end 
    if GuideData.Instance.yinyang_eye_open == true then return end

    local list = self.scene_list[SceneData.Instance:SceneId()]
    for k,v in pairs(list or {}) do 
        if self.explore_data.map[v.seq] == 0 then  
            local cur_scene = SceneCtrl.Instance:CurScene()
			local main_x, main_y = cur_scene:PosToServer(MainPlayer:GetPos())
            local distance = GameMath.GetDistance(main_x, main_y, v.x, v.y, false)  
            local pos_clint = cur_scene:PosToClint(v.x, v.y)
            
            if distance < ( self.radus * self.radus) and (pos_clint.y - 5 <= MainPlayer:GetPos().y and pos_clint.y + 5 >= MainPlayer:GetPos().y) then 
                self.explore_data.reward_show = v.seq
                return 
		    end 
        end 
    end 

    self.explore_data.reward_show = -1
end

function MapExploreData:SetMapExplorationInfo(protocol)
    self.explore_data.map = protocol.m_map

    if self.explore_data.mark_show > -1 and 
        self.explore_data.map[self.explore_data.mark_show] == 1 then 
            ViewMgr:OpenView(MapExploreRewardView,self:GetRewardParamt(self.explore_data.mark_show))
            local obj = SceneCtrl.Instance:Objs().client_objs[self.cache_list[self.explore_data.mark_show]]
            if obj ~= nil then obj:OpenBox() end 
            local cfg = self:GetBoxMapExploreCfg(self.explore_data.mark_show,SceneData.Instance:SceneId())
            if cfg ~= nil then
                local npc_cfg = Cfg.NpcById(cfg.npc_id)
                if cfg.is_disappear == 1 and obj ~= nil then
                    VoMgr:DelGCObj(obj.vo.obj_id, true)
                    -- SceneCtrl.Instance:Objs():DeleteClientObj(obj.vo.obj_id)
                elseif MapExploreEffect[npc_cfg.res_id] ~= nil and obj ~= nil then 
                    local handle = SE:Play(MapExploreEffect[npc_cfg.res_id], obj:GetPos())
                    table.insert( self.effect_list, handle )
                end
            elseif IS_EDITOR then
                LogError("GetBoxMapExploreCfg is nil mark_show ",self.explore_data.mark_show,"scene_id",scene_id)
            else
                BuglyReport("GetBoxMapExploreCfg is nil mark_show ",self.explore_data.mark_show,"scene_id",scene_id)
            end
        self.explore_data.mark_show = -1
    end 
end

function MapExploreData:SetMapExplorationRewardInfo(protocol)
    self.explore_reward_data.list = {}
    for k,v in pairs(protocol.map_exploration_reward_list) do 
        self.explore_reward_data.list[v.scene_id] = v.reward_flag
    end 
    self.explore_reward_data.flush = self.explore_reward_data.flush + 1
end

function MapExploreData:SetMapExplorationSingleRewardInfo(protocol)
    self.explore_reward_data.list[protocol.info.scene_id] = protocol.info.reward_flag
    self.explore_reward_data.flush = self.explore_reward_data.flush + 1
end

function MapExploreData:CreateBoxMapExplore()
    if not self:CanExplore() or self:IsCompleteExplore() then
        return
    end
    self:ClearCacheBoxMapExplore()
    local config_list = self:GetBoxMapExplore(true)
    for k,v in pairs(config_list) do
        local obj_id = SceneObjHolder.CalcClientObjId(v.scene_id, SceneObjType.MapExploreBox, v.seq)
        local npc_cfg = Cfg.NpcById(v.npc_id)
        local vo = CommonStruct.NpcVo(npc_cfg)
        vo.obj_id = obj_id
        vo.obj_type = SceneObjType.MapExploreBox
        vo.res_id = npc_cfg.res_id
        vo.explore_seq = v.seq
        vo.scene_id = v.scene_id
        vo.is_server = true
        vo.x = v.x
        vo.y = v.y
        VoMgr:AddGCObj(vo)
        self:CacheBoxMapExplore(v.seq, obj_id)
    end
end

function MapExploreData:GetRewardParamt(mark_show)
    local param_t = {}
    local list = self.scene_list[SceneData.Instance:SceneId()]
    param_t.total = #list
    param_t.cur_num = 0
    local mark_cfg = nil
    for k,v in pairs(list) do
        if self.explore_data.map[v.seq] == 1 then 
            param_t.cur_num = param_t.cur_num +1
        end 

        if mark_show == v.seq then 
            mark_cfg = v
        end 
    end 
    param_t.is_complete = param_t.cur_num == param_t.total
    
    local reward_list = {}
    if mark_cfg~= nil then
        for k,v in pairs(Config.map_explore_auto.reward[mark_cfg.reward_group_id].reward_item) do 
            local vo = Item.Create(v)
            table.insert(reward_list,vo)
        end 
    end 

    param_t.reward_list = reward_list

    return param_t
end 

function MapExploreData:SetMark(seq)
    if self.explore_data.map[seq] == 1 then 
        return 
    end 
    self.explore_data.mark_show = seq
end


function MapExploreData:ClearCacheBoxMapExplore()
    self.cache_list = {}
    for k,v in pairs(self.effect_list) do 
        SE:Stop(v)
    end 
    self.effect_list = {}
end

function MapExploreData:CacheBoxMapExplore(seq,obj_id)
    self.cache_list[seq] = obj_id
end 

function MapExploreData:GetMapExploreRed(scene_id)
    local list = self:GetExploreSceneReward(scene_id)
    
    for k,v in pairs(list.list or {} ) do 
        if v.flag == 0 and v.pro_complete then 
            return true
        end 
    end 
    return false
end

function MapExploreData:GetCurSceneProgDeatail(scene_id)
    local prog_map,prog_map_total = self:GetCurExploreProg(scene_id)
    local prog_event,prog_event_total = self:GetCurAnecdoteEventProg(scene_id)
    local prog_challenge,prog_challenge_total = self:GetCurAnecdoteChallengeProg(scene_id)
    local prog_total = prog_map + prog_event + prog_challenge
    local prog_total_total = prog_map_total + prog_event_total + prog_challenge_total
    return {
        prog_total = prog_total_total > 0 and string.format(Language.Map.Total,prog_total,prog_total_total) or "",
        prog_map = prog_map_total > 0 and string.format(Language.Map.MapExplore,prog_map,prog_map_total) or "",
        prog_event = prog_event_total > 0 and string.format(Language.Map.AnecdoteEvent,prog_event,prog_event_total) or "",
        prog_challenge = prog_challenge_total > 0 and string.format(Language.Map.AnecdoteChallenge,prog_challenge,prog_challenge_total) or "",
    }
end