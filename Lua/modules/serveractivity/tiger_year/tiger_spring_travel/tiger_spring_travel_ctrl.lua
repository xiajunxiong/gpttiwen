VIEW_REQ("modules/serveractivity/tiger_year/tiger_spring_travel/tiger_spring_travel_view")

TigerSpringTravelCtrl = TigerSpringTravelCtrl or BaseClass(BaseCtrl)
function TigerSpringTravelCtrl:__init()
	if TigerSpringTravelCtrl.Instance ~= nil then
		Debuger.LogError("[TigerSpringTravelCtrl] attempt to create singleton twice!")
		return
	end	
    TigerSpringTravelCtrl.Instance = self
    self:RegisterAllProtocols()
    self.travel_data = SmartData.New({seq =-1,is_receive = 0})

    self.cache_cfg = nil
    self.radus = Config.yinhutanchun_auto.other[1].explore_radius
    self.mark_wait = Config.yinhutanchun_auto.other[1].watch_times
    self.mark_timer = 0 


    Remind.Instance:Register(Mod.TigerYear.TigerSpringTravel, self.travel_data, function()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_TIGER_FIND_SPRING) then
			return 0
		end
		return self:GetRedNum()
	end)
end 

function TigerSpringTravelCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCTigerFindSpringInfo, "OnTigerFindSpringInfo")
end 

function TigerSpringTravelCtrl:__delete()
    self:UnRegisterAllProtocols()
	TigerSpringTravelCtrl.Instance = nil
end

function TigerSpringTravelCtrl:OnTigerFindSpringInfo(protocol)
    -- LogError("!?@!!OnTigerFindSpringInfo!",protocol)
    self.travel_data.seq = protocol.seq
    self.travel_data.is_receive = protocol.is_receive
    
    self:CacheSpringTravelCfg(self.travel_data.seq)
end

function TigerSpringTravelCtrl:CacheSpringTravelCfg(seq)
    local act_cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_TIGER_FIND_SPRING, Config.yinhutanchun_auto.gift_configure, "seq")

    for k,v in pairs(act_cfg) do 
        if v.seq == seq then 
            self.cache_cfg = v
            break
        end 
    end 
end

function TigerSpringTravelCtrl:OnGameSceneLoaded(scene_id)
    if self:CancelTravel(scene_id) then    
        ViewMgr:CloseView(TigerTravelExplore)
        return 
    end 

    self.range_invoke_timer = TimeHelper:AddRunTimer(function()
        self:CheckTravel()
    end, 0.25)
end

function TigerSpringTravelCtrl:CancelTravel(scene_id)
    TimeHelper:CancelTimer(self.range_invoke_timer)
    
    return self.cache_cfg == nil or scene_id ~= self.cache_cfg.scene_id or MainPlayer == nil or (not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_TIGER_FIND_SPRING))
end

function TigerSpringTravelCtrl:CheckTravel()
    -- 战斗检查
    if MainViewData.Instance:IsBattle() then return end 
    -- 虚空检查
    if SceneCtrl.Instance:CurScene() == nil or SceneCtrl.Instance:CurScene().PosToServer == nil then return end 

    if MainPlayer == nil then return end 

    if self.cache_cfg == nil then return end 
    local cur_scene = SceneCtrl.Instance:CurScene()
    local main_x, main_y = cur_scene:PosToServer(MainPlayer:GetPos())
    local distance = GameMath.GetDistance(main_x, main_y, self.cache_cfg.x, self.cache_cfg.y, false)  
    local pos_clint = cur_scene:PosToClint(self.cache_cfg.x, self.cache_cfg.y)
    -- LogError("!?@!!",main_x,main_y,self.cache_cfg.x, self.cache_cfg.y)    
    -- LogError("!?@",self.radus * self.radus,distance)
    local flag =distance < ( self.radus * self.radus) and (pos_clint.y - 5 <= MainPlayer:GetPos().y and pos_clint.y + 5 >= MainPlayer:GetPos().y) 

    self:MarkTimer(flag)
end

function TigerSpringTravelCtrl:MarkTimer(inside)
    if self.travel_data.is_receive == 1 then 
        ViewMgr:CloseView(TigerTravelExplore)
        return 
    end 

    if not inside then 
        self.mark_timer = 0 
        return     
    end 

    if not ViewMgr:IsOpen(TigerTravelExplore) then 
		ViewMgr:OpenView(TigerTravelExplore)
	end 

    self.mark_timer = self.mark_timer + 1
    -- 毕竟是0.25秒检定一次，所以合计的计数是秒数X4
    if self.mark_timer > (self.mark_wait * 4) then 
        ServerActivityCtrl.Instance:SendRandActivityOperaReq({
            rand_activity_type = ACTIVITY_TYPE.RAND_TIGER_FIND_SPRING,
            opera_type = 1,
        })
    end 
end

function TigerSpringTravelCtrl:GetCacheData()
    return self.cache_cfg
end

--获取加载地图路径
function TigerSpringTravelCtrl:GetMapMiniPath(scene_id)
	local map_res = MapData.MapRes(scene_id)
    return Format("serveractivity/tiger_spring_travel/travel_%s.png", map_res)
end

function TigerSpringTravelCtrl:GetRedNum()
   return self.travel_data.is_receive == 1 and 1 or 0 
end