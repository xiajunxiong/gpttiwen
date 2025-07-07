PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        ResMgr:LoadObjectOfType(
            "material/bianyuanfaliang_528.mat",
            typeof(UnityEngine.Material),
            function(obj)
                PreloadCtrl.shan = obj
                PreloadCtrl.UnregisterLoading()
            end
        )
    end
)
PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        ResMgr:LoadObjectOfType(
            "material/bianyuanfaliang_529.mat",
            typeof(UnityEngine.Material),
            function(obj)
                PreloadCtrl.e = obj
                PreloadCtrl.UnregisterLoading()
            end
        )
    end
)
YinYangObj = YinYangObj or BaseClass(SceneObj)
YinYangObj.box_res = {
    [1016] = true,
    [1017] = true,
    [1018] = true,
    [1019] = true,
    [1027] = true,
}
function YinYangObj:__init()
end 
function YinYangObj:__delete()
    -- if self.handle_effect and self.handle_effect.obj then
        SE:Release(self.handle_effect)
    -- end
    -- if self.handle_opened and self.handle_opened.obj then
        SE:Release(self.handle_opened)
    -- end
    self.handle_opened = nil
    self.handle_effect = nil
    if self.handle_care then
        GuideData.Instance.yinyang_eye_effect:Uncare(self.handle_care)
    end
    TimeHelper:CancelTimer(self.handle_delay_hide)
    -- if self.show_effect and self.show_effect.obj then
        SE:Release(self.show_effect)
    -- end
    -- if self.hide_effect and self.hide_effect.obj then
        SE:Release(self.hide_effect)
    -- end
end

function YinYangObj:InitEnd()
    --进行初始位置设置
    -- local explore_cfg = MapExploreData.Instance:GetYinYangObjCfg(self.vo.explore_seq,self.vo.scene_id)
    -- self:SetScenePos(self.vo.scene_id, explore_cfg.x,  explore_cfg.y, true)
    if self.vo.show_type == 1 then
        self:SetTouchAble(true)
    end
    -- care yinyang change
    self.handle_care = GuideData.Instance.yinyang_eye_effect:Care(BindTool.Bind(self.YinYangEyeChange, self))
end
function YinYangObj:FlushDrawer()
    local config = Cfg.NpcById(self.vo.npc_id, false)
    if config then
        local data = string.format(DrawerHelper.GetNpcPath(config.res_id))
        self.body:SetData(data)
        self.body:SetLoadedCallback(BindTool.Bind(self.LoadedCallback, self))
    end
end
function YinYangObj:LoadedCallback()
    if self.vo == nil then
        return
    end
    TimeHelper:CancelTimer(self.handle_delay_hide)
    self:SetVisible(true)
    self.body:PlayAnim(ChAnimType.Idle)
    self:SetScenePos(self.vo.scene_id, self.vo.x,  self.vo.y, true)
    --先判断善恶给不同的材质球特效
    if self.vo.NPC_type == 0 then
        if self.body and self.body.SetRendererMaterial then
            self.body:SetRendererMaterial(PreloadCtrl.shan, 1)
        end
    else
        if self.body and self.body.SetRendererMaterial then
            self.body:SetRendererMaterial(PreloadCtrl.e, 1)
        end
    end
    --npc类身上有淡蓝火焰
    if self.vo.show_type == 1 then
        SE:Release(self.handle_effect)
        --4171003
        self.handle_effect = SE:Play(SceneEffectConfig.EffectList.DanLanHuoYan,nil,nil,self.root_node)
    end
    if self.vo.is_always_appear == 0 then
        --普通状态下隐藏
        self:SetVisible(false)
    end
    --判断当前状态是否显示模型或者是否可交互
    self:YinYangEyeChange(true)
end
function YinYangObj:InteractionBehavior()
    if self.vo then
        YinYangData.Instance.explore_data.vo = self.vo
        -- LogDG("点击阴阳对象进行互动", self.vo)
        if ViewMgr:IsOpen(Compass) then
            ViewMgr:CloseView(Compass)
        end
        if self.vo.is_talk == 1 then
            YinYangData.Instance.explore_data.npc_vo = self.vo
            BehaviorCtrl.ExeByVo(self.vo)
        else
            YinYangCtrl.CSYinYangEyeOpReq(2, self.vo.seq)
        end
    end
end

function YinYangObj:YinYangEyeChange(ignore_effect)
    local opened = GuideData.Instance.yinyang_eye_open
    --出现的时候播放特效 4171004
    TimeHelper:CancelTimer(self.handle_delay_hide)
    if opened then
        self:SetVisible(true)
        if ignore_effect ~= true then
            self.show_effect = SE:Play(SceneEffectConfig.EffectList.YinYangObjShow, nil,nil, self.root_node)
        end
    else
        if self.vo.is_always_appear == 0 then
            TimeHelper:CancelTimer(self.handle_delay_hide)
            self.handle_delay_hide = TimeHelper:AddDelayTimer(function ()
                self:SetVisible(false)
            end, 2.1)
            -- self:SetVisible(false)
        end
        if ignore_effect ~= true then
            self.hide_effect = SE:Play(SceneEffectConfig.EffectList.YinYangObjHide, nil,nil, self.root_node)
        end
    end
end
function YinYangObj:CloseBox()
    self.body:PlayAnim(ChAnimType.Idle)
end

function YinYangObj:OpenBox()
    --播放特效
    SE:Release(self.handle_opened)
    --[[ if self.handle_opened and self.handle_opened.obj then
    end ]]
    self.body:PlayAnim(ChAnimType.BattleRun)
    local npc_cfg = NpcHelper.NpcCo(self.vo.id)
    if not YinYangObj.box_res[npc_cfg.res_id] then
        return
    end
    if self.vo.NPC_type == 1 then
        self.handle_opened = SE:Play(SceneEffectConfig.EffectList.YinYangBox1, nil,nil,self.root_node)
    else
        self.handle_opened = SE:Play(SceneEffectConfig.EffectList.YinYangBox2, nil,nil,self.root_node)
    end
end