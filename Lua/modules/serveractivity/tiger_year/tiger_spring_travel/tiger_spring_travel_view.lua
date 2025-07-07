TigerSpringTravelView = TigerSpringTravelView or DeclareView("TigerSpringTravelView", "serveractivity/tiger_year/tiger_spring_travel",Mod.TigerYear.TigerSpringTravel)

VIEW_DECLARE_LEVEL(TigerSpringTravelView,ViewLevel.L0)

function TigerSpringTravelView:LoadCallback()
    --  请求信息
    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
        rand_activity_type = ACTIVITY_TYPE.RAND_TIGER_FIND_SPRING,
        opera_type = 0,
    })
end 

function TigerSpringTravelView:OnClickClose()
    ViewMgr:CloseView(TigerSpringTravelView)
end

TigerSpringTravelPanel = TigerSpringTravelPanel or DeclareMono("TigerSpringTravelPanel", UIWFlushPanel)
function TigerSpringTravelPanel:TigerSpringTravelPanel()
    self.ctrl = TigerSpringTravelCtrl.Instance
    self.data_cares = {
        {data = TigerSpringTravelCtrl.Instance.travel_data, func = self.FlushView}
    }

    self.act_timer:SetShowCallBack(function (t)
        local time_s = ""
        for k , v in pairs(t) do 
            if tonumber(v) > 0 and k ~= "s" then 
                time_s = time_s ..v..Language.HappyLink.time[k]
            end 
        end 
        return ColorStr(Language.FlowerTouch.ActTime,COLORSTR.Red2)..time_s
    end)
end

function TigerSpringTravelPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.act_timer:CreateActTime(ACTIVITY_TYPE.RAND_TIGER_FIND_SPRING)
end

-- 一般来讲这个界面是接近于静止的
function TigerSpringTravelPanel:FlushView()
    -- TigerSpringTravelCtrl.Instance:CacheSpringTravelCfg(3)
    local cache_cfg = self.ctrl:GetCacheData()

    -- 一般来讲活动开启 or 登录会进行信息刷新产生cache_cfg
    -- 若没有，理应直接取消本次刷新等待loadcallback的新信息下发
    if cache_cfg == nil then return end 

    self.cahce = cache_cfg
    --加载地图
    if self.map_pic:TextureIsNil() then
        self.map_pic:SetByPath(self.ctrl:GetMapMiniPath(cache_cfg.scene_id))
    end
    -- 放置图标的位置
    local cfg = SceneData.Instance:GetSceneCfg(cache_cfg.scene_id) or {}
    local c_pos =  Vector3.New(cache_cfg.x_1,cache_cfg.z_1,cache_cfg.y_1) -- GameMath.PosToClint(cache_cfg.x,cache_cfg.y,cfg.mapid)
    local pos =  MapCtrl.Instance:WorldToTransformUV(c_pos,self.MapTex,cache_cfg.scene_id)
    UH.LocalPos(self.mark_pos,pos)

    -- self.item_list
    local list = {}
    for k,v in pairs(cache_cfg.reward_item) do 
        local item = BagData.Instance:ItemBuild(v) 
        table.insert(list,item)
    end 
    self.item_list:SetData(list)

    self.got:SetActive(TigerSpringTravelCtrl.Instance.travel_data.is_receive== 2)
    self.BtnGetEff:SetActive(TigerSpringTravelCtrl.Instance.travel_data.is_receive== 1)
    self.BtnGet:SetActive(TigerSpringTravelCtrl.Instance.travel_data.is_receive ~= 2)
    self.BtnInter.Interactable = TigerSpringTravelCtrl.Instance.travel_data.is_receive== 1
    self.red_point = self.red_point or UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
    self.red_point:SetNum(TigerSpringTravelCtrl.Instance.travel_data.is_receive== 1 and 1 or 0)
end

function TigerSpringTravelPanel:OnClickMove()
    ViewMgr:CloseView(TigerSpringTravelView)

    ViewMgr:OpenView(MapView,{need_click = false})
    
    TigerYearTotalCtrl.Instance:AutoCloseTotal(Mod.TigerYear.TigerSpringTravel)
    -- SceneLogic.Instance:AutoToPos(self.cahce.scene_id,nil,nil,nil,true)
end

function TigerSpringTravelPanel:OnClickGet()
    ServerActivityCtrl.Instance:SendRandActivityOperaReq({
        rand_activity_type = ACTIVITY_TYPE.RAND_TIGER_FIND_SPRING,
        opera_type = 2,
    })
end

function TigerSpringTravelPanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[221].desc})
end

TigerSTravelItem = TigerSTravelItem or DeclareMono("TigerSTravelItem", UIWidgetBaseItem)
function TigerSTravelItem:SetData(data)
    self.call:SetData(data)
end


TigerTravelExplore = TigerTravelExplore or DeclareView("TigerTravelExplore", "serveractivity/tiger_year/tiger_travel_explore")
VIEW_DECLARE_LEVEL(TigerTravelExplore, ViewLevel.Lt)
VIEW_DECLARE_MASK(TigerTravelExplore, ViewMask.None)
function TigerTravelExplore:TigerTravelExplore()
    self.ctrl = TigerSpringTravelCtrl.Instance
    Runner.Instance:AddRunObj(self, 16)
end 

function TigerTravelExplore:Update()
    local rate = (self.ctrl.mark_timer) / (self.ctrl.mark_wait * 4)

    self.prog.fillAmount = rate

    -- 完成进度槽强行关闭界面
    if rate >=1 then 
        ViewMgr:CloseView(TigerTravelExplore)
    end 
end  

function TigerTravelExplore:CloseCallback()
    Runner.Instance:RemoveRunObj(self)
end