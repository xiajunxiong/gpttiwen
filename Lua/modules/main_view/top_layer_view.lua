TopLayerView = TopLayerView or DeclareView("TopLayerView", "top_layer_view", 0, ViewLayer.Top - 1)
VIEW_DECLARE_LEVEL(TopLayerView, ViewLevel.Lt)
VIEW_DECLARE_MASK(TopLayerView, ViewMask.None)

function TopLayerView:TopLayerView()
end

function TopLayerView:LoadCallback()
    -- body
end

function TopLayerView:SetVisible(visible)
    if ViewMgr:IsOpen(TopLayerView) then
        self.TabMarQuee:SetActive(visible)
    end
end

FloatingTextPanel = FloatingTextPanel or DeclareMono("FloatingTextPanel", UIWFlushPanel)
-- Item = GameObject
-- ItemParent = GameObject
-- ItemMono = FloatingItem
-- IconParent = GameObject
-- IconMono = FloatingIcon
-- IconItem = GameObject
-- IconTarPos = RectTransform
function FloatingTextPanel:FloatingTextPanel()
    self.floating_text_data = FloatingTextData.Instance
    -- 循环飘动 只要将SetSiblingIndex 设置为1
    -- 用于存储飘动的字
    self.cache_count = 6        --缓存上限，保持缓存的是收到的最新的
    self.floating_text_cache = {}   --信息缓存池，上限位self.cache_count
    self.label_can_timer = true
    self.sibling_index = 1
    self.data_cares = {
        {data = self.floating_text_data.floating_data, func = self.FloatingText, init_call = false}
    }
end

function FloatingTextPanel:FloatingText(keys)
    -- 存储
    local float_text = self.floating_text_data:GetFloatText()   --add进来的各种飘字信息（全部），放到信息缓存池
    for k, v in pairs(float_text) do
        if #self.floating_text_cache >= self.cache_count then
            table.remove(self.floating_text_cache, 1)
        end
        table.insert(self.floating_text_cache, v)
    end
    if self.label_can_timer then
        self.label_can_timer = false
        self:FloatingTime()
    end
end

function FloatingTextPanel:FloatingTime()
    if table.nums(self.floating_text_cache) > 0 then
        self:ShowFloatingText(table.remove(self.floating_text_cache, 1))
        TimeHelper:AddDelayTimer(BindTool.Bind(self.FloatingTime, self), 0.1)
    else
        self.label_can_timer = true
    end
end

function FloatingTextPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    --清理不执行
    self.floating_text_cache = {}
    self.sibling_index = 1
end

function FloatingTextPanel:ShowFloatingText(str)
    local clone = nil
    local obj_mono = nil
    
    local item_cache = self.floating_text_data:GetItemCache()--飘字物体储存池（生成的飘字结束后放入此）
    local float_quene = self.floating_text_data:GetFloatQuene()
    
    --储存池有就拿，没有就创建（由于播放动画限制，实际创建出来的会大于6个，）
    local item_cache_num = table.nums(item_cache)
    if item_cache_num > 0 then
        obj_mono = table.remove(item_cache, 1)
        obj_mono:ClearCache()
    else
        local clone = self.Item:Clone(self.ItemParent)
        clone:SetActive(true)
        clone.name = self.sibling_index
        obj_mono = clone:GetLuaComponent(self.ItemMono:GetTypeName())
    end
    obj_mono.Anim:ResetTrigger(APH("Fade"))         --创建或从池子取出后，重置一下trigger
    table.insert(float_quene, obj_mono)
    --当前正在飘的文字（超过上限时按序渐出）
    local num = table.nums(float_quene)
    if num > self.cache_count then
        local mono = table.remove(float_quene, 1)
        mono:SetFade()
        -- mono.Anim:SetTrigger(APH("Fade"))
    end
    local isTable = type(str) == "table"
    if isTable and not str.is_virtual and not str.is_rich then
        local icon_mono = nil
        local icon_cache = self.floating_text_data:GetFloatIconCache()
        if table.nums(icon_cache) > 0 then
            icon_mono = table.remove(icon_cache, 1)
            icon_mono.IconObj:SetParent(obj_mono.IconObj)
        else
            local icon_clone = self.IconItem:Clone(obj_mono.IconObj)
            icon_clone.name = self.sibling_index
            icon_mono = icon_clone:GetLuaComponent(self.IconMono:GetTypeName())
        end
        icon_mono.Anim:SetTrigger(APH("Reset"))
        icon_mono.IconObj.transform.localPosition = Vector3.zero
        obj_mono.floating_icon = icon_mono
        -- table.insert(float_icon_quene, icon_mono)
        icon_mono:SetData(str, self.IconParent, self.IconTarPos.localPosition)
    end
    self.sibling_index = self.sibling_index + 1
    obj_mono:SetData(str, self.sibling_index, self.IconParent, isTable and str.fly_icon)
    
end
-------------------------------------
FloatingIcon = FloatingIcon or DeclareMono("FloatingIcon", UIWidgetBaseItem)
-- Icon = FloatingIconCell
-- IconObj = GameObject
-- Anim = Animator
-- IconItemObj = GameObject
-- ZheZhaoTop = UIImageSpriteSet
function FloatingIcon:FloatingIcon()
    self.floating_text_data = FloatingTextData.Instance
    self.float_icon = nil
    self.icon_curve = nil
end

function FloatingIcon:SetData(str, iconParent, iconTarPos)
    self.Icon:ClearCache()
    if self.anim_event == nil then
        self.anim_event = self.Anim:ListenEvent("float_icon", function (eve_param)
            if eve_param == "scale_end" then
                -- self:FlyToBag()
            end
        end)
    end
    self.iconParent = iconParent
    self.iconTarPos = iconTarPos
    self.IconObj.name = str.item_id
    self.IconObj:SetActive(true)

    local item_data = Item.Create({item_id = 0})
    item_data.item_id = str.item_id
    self.Icon.data = item_data
    
    CellFlushs.SetIcon(self.Icon, str.icon_id and str.icon_id or item_data:IconId())
    CellFlushs.CheckSomething(self.Icon, item_data)
    local scale = self.Icon.ZheZhaoTop:GetObjActive() and Vector3.New(0.8, 0.8, 0.8) or Vector3.one
    self.IconItemObj:SetLocalScale(scale)
    -- self.Icon:SetDataByItemId(str.item_id)
end

function FloatingIcon:FlyToBag()
    local start_pos = self.IconObj.transform.localPosition
    self.icon_curve = Game.CurveParabola(start_pos, Vector3.New(1, 1, 0), self.iconTarPos, Vector3.New(1, -2, 0))--, 0.3)
    -- self.icon_curve = Game.CurveParabola(start_pos, self.iconTarPos, Vector3.New(512, 193, 0), 0.3)
    self.start_frame = Time.frameCount
    self.totalStep = 16
    self.calc_step = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.UpdatePos, self), 1, self.totalStep)
end

function FloatingIcon:ClearCache()
    self.has_delay = false
    self.item_mono = nil
    self.icon_curve = nil
    if self.calc_step ~= nil then
        TimeHelper:CancelTimer(self.calc_step)
        self.calc_step = nil
    end
    if self.item_fade_delay ~= nil then
        TimeHelper:CancelTimer(self.item_fade_delay)
        self.item_fade_delay = nil
    end

    self.IconObj:SetActive(false)
    UH.LocalPosG(self.IconObj, Vector3.zero)
    local icon_cache = self.floating_text_data:GetFloatIconCache()
    table.insert(icon_cache, self)
end

function FloatingIcon:UpdatePos()
    if self.icon_curve == nil then
        return
    end
    self.firstEle = 0.013
    self.addSpeed = 1.35
    --等比数列前N(self.totalStep)项和（要求接近1且大于1），首项为self.firstEle,比例是(self.addSpeed)
    --前一段用等差数列匀速运动，后一段用等差数列加速
    local time1 = (Time.frameCount - self.start_frame) / self.totalStep
    local time2 = self.firstEle * (1 - math.pow(self.addSpeed, Time.frameCount - self.start_frame)) / (1 - self.addSpeed)
    local time = math.max(time1, time2)
    local pos = time >= 1 and self.iconParent.transform.localPosition or self.icon_curve:GetPos(time)
    
    if time >= 1 then
        -- self:ClearCache()
        if not self.has_delay then
            TimeHelper:AddDelayTimer(BindTool.Bind(self.ClearCache, self), 0.1)
            self.has_delay = true
            BagCtrl.Instance:TryItemGetAudioEff()
        end
    else
        UH.LocalPosG(self.IconObj, pos)
    end
end
-------------------------------------
FloatingIconCell = FloatingIconCell or DeclareMono("FloatingIconCell", UIWidgetBaseItem)
function FloatingIconCell:FloatingIconCell()
end

function FloatingIconCell:ClearCache()
    self.ZheZhaoTop:SetObjActive(false)
    self.JiaoBiao:SetActive(false)
end
------------------------------------
-- RichtextItem = RichtextItem or DeclareMono("RichtextItem", UIWidgetBaseItem)
-- function RichtextItem:SetData()
-------------------------------------
FloatingItem = FloatingItem or DeclareMono("FloatingItem", UIWidgetBaseItem)
-- FloatLen = 75
-- FloatWidth = 588
-- FloatingLabel = text
-- Item = GameObject
-- Anim = Animator
-- Icon2 = UIImageSpriteSet
-- FloatRect = RectTransform
-- IconTarPos = RectTransform
-- IconPos = RectTransform
-- IconObj = GameObject
function FloatingItem:FloatingItem()
    self.floating_text_data = FloatingTextData.Instance
end

function FloatingItem:SetData(str, index, iconParent, fly_icon)
    local len = 0
    local isTable = type(str) == "table"
    self.iconParent = iconParent
    self.FloatingLabel:SetObjActive(isTable and not str.is_rich)
    self.OtherText:SetObjActive(isTable and str.is_rich == true)
    if type(str) == "string" then
        self.FloatingLabel.text = str
        self.FloatingLabel:SetObjActive(true)
        self.IconObj:SetActive(false)
        self.Icon2.SpriteName = ""
        len = string.len(str)
    elseif isTable and str.is_rich then
        self.OtherText.text = str.desc
        self.Richtext.text = str.rich_desc
        self.IconObj:SetActive(false)
        self.Icon2.SpriteName = ""
    else
        if isTable then
            local value = type(str.icon_id) == "string" and string.sub(str.icon_id, 1, 1) == "s"
            self.IconObj:SetActive(true)
            self.FloatingLabel.text = str.desc
            self.Icon2.SpriteName = str.icon_id2 or ""
            len = string.len(str.desc)
        else
            self.FloatingLabel.text = tostring(str)
            self.FloatingLabel:SetObjActive(true)
        end
    end
    self.Item:SetSiblingIndex(index)
    self.Item:SetActive(true)
    local item_cache = self.floating_text_data:GetItemCache()
    self.floating_state = 0
    local float_quene = self.floating_text_data:GetFloatQuene()
    if self.anim_event == nil then
        self.anim_event =
            self.Anim:ListenEvent(
            "floating_state",
            function(eve_param, anim_st_info)
                if eve_param == "fade_end" then
                    self.Item:SetActive(false)
                    self.IconObj:SetActive(true)
                    table.insert(item_cache, self)
                    self.floating_state = 2
                    for k,v in pairs(float_quene) do
                        if v == self then
                            table.remove(float_quene,k)
                        end
                    end
                elseif eve_param == "up_end" then
                elseif eve_param == "fade_start" then
                    self.floating_state = 1
                end
            end
        )
    end
    if fly_icon then
        --0.8秒后icon变大飞向背包
        TimeHelper:AddDelayTimer(BindTool.Bind(self.IconFly, self), 0.8)
    end
end

function FloatingItem:SetFade()
    if self.floating_state ~= 1 and self.floaint_state ~= 2 then
        self.Anim:SetTrigger(APH("Fade"))
    end
end

function FloatingItem:IconFly()
    self.IconObj:SetActive(false)
    if self.floating_icon ~= nil then
        self.floating_icon.IconObj:SetParent(self.iconParent)
        self.floating_icon.Anim:SetTrigger(APH("ChangeScale"))
        self.floating_icon:FlyToBag()
        self.floating_icon = nil
    end
end

function FloatingItem:ClearCache()

    self.Anim:SetTrigger(APH("Reset"))
end

------------------------------------------------
TopLayerViewMarquee = TopLayerViewMarquee or DeclareMono("TopLayerViewMarquee", UIWFlushPanel)

function TopLayerViewMarquee:TopLayerViewMarquee()
    self.data = TopLayerMarqueeData.Instance
    self.data_cares = {
        {data = self.data.caption_switch, func = self.ShowRolling, init_call = false, keys = {"switch"}},
        {data = LoginData.Instance.base_data, func = self.FlushState, init_call = false, keys = {"login_state"}}
    }
    self.item_list = {}
end

function TopLayerViewMarquee:FlushState()
    if LoginStateType.None == LoginData.Instance.base_data.login_state then
        self.data:ClearCaptionList()
        for _, item in pairs(self.item_list) do
            self:Rolling(item, true)
        end
    end
end

function TopLayerViewMarquee:ShowRolling(caption_switch)
    if nil ~= caption_switch and false == caption_switch.switch then
        local t = {}
        if self.data:NextCaptionInfo(t) then
            self.ContentParent:SetActive(true)
            local item = self.data:GetContentItem(self.ContentItem, self.ContentParent)
            self.item_list[item.item_obj] = item
            item.item_text.text = t.content
            item.item_obj:SetActive(true)
            if self.data.show_special_effect == true then
                self.handle_effect = self.EffectTool:Play(2165005)
                self.data.show_special_effect = false
            end
            TimeHelper:AddDelayTimer(function ()
                item.item_pos.anchoredPosition = Vector2.New(item.item_pos.sizeDelta.x, 0)
                item.timer_rt = TimeHelper:AddRunTimer(BindTool.Bind(self.Rolling, self, item), TopLayerMarqueeSetting.timer_interval)
            end, 0.2)
        end
    end
end

function TopLayerViewMarquee:Rolling(item, is_stop)
    local x = item.item_pos.anchoredPosition.x - TopLayerMarqueeSetting.per_move_x * Time.deltaTime
    item.item_pos.anchoredPosition = Vector2.New(x, 0)
    if x < -TopLayerMarqueeSetting.distance and not item.is_next then
        item.is_next = true
        self:ShowRolling{switch = false}
    end
    if x < -TopLayerMarqueeSetting.bg_width or is_stop then
        item.is_next = false
        TimeHelper:CancelTimer(item.timer_rt)
        self.item_list[item.item_obj] = nil
        self.data:RecycleContentItem(item)
        if self.handle_effect then
            self.EffectTool:Stop(self.handle_effect)
            self.handle_effect = nil
        end
        self.data.rolling_item_num = self.data.rolling_item_num - 1
        if 0 == self.data.rolling_item_num then
            self.data.caption_switch.switch = false
            self.ContentParent:SetActive(false)
        end
    end
end

TopLayerViewActTips = TopLayerViewActTips or DeclareMono("TopLayerViewActTips", UIWFlushPanel)

function TopLayerViewActTips:TopLayerViewActTips()
    self.data = TopLayerActTipsData.Instance
    self.base_data = MainViewData.Instance:BaseData()
    self.language = Language.Activity.ActTips

    self.data_cares = {
        {data = self.data.cur_show_act, func = self.FlushActTips},
        {data = self.base_data, func = self.FlushActTips, keys = {"is_battle"}, init_call = false},
        {data = SceneData.Instance:Base(), func = self.FlushActTips, keys = {"scene_mode"}},
    }
    self.role_setting_change_handle = EventSys:Bind(GlobalEventKeys.RoleSettingChange, BindTool.Bind(self.RoleSettingChange, self))
end

function TopLayerViewActTips:RoleSettingChange(key,value)
    if key == ROLE_SETTING_TYPE.SET_SHOW_ACTIVITY_MSG then
        self.ActTipsObj:SetActive(value ~= 0)
        if value ~= 0 then
            self:FlushActTips()
        end
    end
end

function TopLayerViewActTips:Awake()
    UIWFlushPanel.Awake(self)
    self.ActTime:SetCallBack(function()
        self.data.cur_show_act_time.num = 0
        self:OnClickClose()
    end)
    local RemindTips = Language.Activity.RemindTips
    UH.SetText(self.CickText,RichTextHelper.LinkStr(RichTextHelper.ColorStr(RemindTips[1],COLORSTR.Yellow1),"_ul",COLORSTR.Yellow1))
    UH.SetText(self.MatchText,RichTextHelper.LinkStr(RichTextHelper.ColorStr(RemindTips[2],COLORSTR.Yellow1),"_ul",COLORSTR.Yellow1))
end

function TopLayerViewActTips:FlushActTips()
    if 0 == self.data.cur_show_act_time.num then
        self:OnClickClose()
        return
    end
    -- if RoleSettingData.IsShield(ROLE_SETTING_TYPE.SET_SHOW_ACTIVITY_MSG) then
    --     self.ActTipsObj:SetActive(false)       
    --     return
    -- end
    local cur_show_act = self.data.cur_show_act
    if not next(cur_show_act) then
        self:OnClickClose(false)
        return
    end
    if self.base_data.is_battle or SceneModeType.None ~= SceneData.Instance:GetMode() then
        self:OnClickClose(true)
        return
    end
    self.ActName.text = cur_show_act.act_name

    self.ActTipsObj:SetActive(true)

    self.ActTime:StampTime(self.data.cur_show_act_time.num,TimeType.Type_Special_12)

    self.match_func = function()
        TeamCtrl.Instance:TeamDetectByActType(cur_show_act.info.act_type,function()
        end)
    end

    local act_type = cur_show_act.info.act_type
    local status = ActivityData.Instance:GetActivityStatus(act_type)
    if status == ActStatusType.Standy then
        self.TipItem:SetActive(true)
        self.CickItem:SetActive(false)
        self.BtnMatch:SetActive(false)
    else
        self.TipItem:SetActive(false)
        self.CickItem:SetActive(true)
        self.BtnMatch:SetActive(cur_show_act.info.is_team == 1)
    end    
end

function TopLayerViewActTips:OnClickClose(not_empty,is_go)
    self.ActTipsObj:SetActive(false)
    if not_empty then return end
    if not next(self.data.cur_show_act) then return end
    if PublicPopupCtrl.Instance.stop_act == true then return end
    if not is_go and ActivityData.Instance:GetActNeedCloseTips(self.data.cur_show_act.act_type) then 
        ViewMgr:OpenView(ActCloseTipsView,{act_type = self.data.cur_show_act.act_type})
    end 
    PublicPopupCtrl.Instance:CloseActTips()
end

function TopLayerViewActTips:OnClickGo()
    if self.data.cur_show_act:OnClickHandle() then
        return
    end
    self:OnClickClose(nil,true)
end

function TopLayerViewActTips:OnClickMatch()
    if self.match_func then
        self.match_func()
    end
end

function TopLayerViewActTips:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.role_setting_change_handle then
		EventSys:UnBind(self.role_setting_change_handle)
	end
end

ActCloseTipsView = ActCloseTipsView or DeclareView("ActCloseTipsView", "main/act_close_tips")
function ActCloseTipsView:ActCloseTipsView()end

function ActCloseTipsView:LoadCallback(act_info)
    self.act_info = act_info
    local cfg = ActivityData.Instance:GetActActivityShow(self.act_info.act_type)
    UH.SetText(self.Content, cfg.reward_desc)

	local rewards = {}
    for _, item_id in pairs(string.split(cfg.reward_id, ":")) do
        table.insert(rewards, Item.Init(tonumber(item_id)))
    end
    self.item_list:SetData(rewards)
end 

function ActCloseTipsView:CheckToday()
    if self.TodayNotRemind.isOn then
        DialogTipsData.Instance:SetTodayRemind(DTNewTodayNotRemind.ActCloseTips)
    end 
end

function ActCloseTipsView:OnClickCancel()
    self:CheckToday()
    ViewMgr:CloseView(ActCloseTipsView)
end

function ActCloseTipsView:OnClickConfirm()
    self:CheckToday()
    local act_info = ActivityData.Instance:GetActivityInfo(self.act_info.act_type)
    act_info:OnClickHandle()
    ViewMgr:CloseView(ActCloseTipsView)
end


TopMapPanel = TopMapPanel or DeclareMono("TopMapPanel", UIWFlushPanel)
function TopMapPanel:TopMapPanel()
    self.scene_care_handle = SceneMgr:CareLoadedSceneIdChange(BindTool.Bind(self.OnLoadedSceneIdChange,self))
end

function TopMapPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    SceneMgr:UncareLoadedSceneIdChange(self.scene_care_handle)
end

function TopMapPanel:SetMapConfig()
    local name = SceneData.Instance:SceneName()
    if StringIsEmpty(name) then
        return
    end
    self.Anim.enabled = true
    self.Anim:Play()
    UH.SetText(self.Name, name)
end

function TopMapPanel:OnLoadedSceneIdChange()
    if SceneCtrl.Instance.is_first_enter == 1 then return end
    if SceneCtrl.Instance:CurScene():GetClassType() ~= GameScene then
		return
    end
    if ViewMgr:IsOpen(MapView) then
        return
    end
    self:SetMapConfig()
end

-----------------------UI点击特效-----------------------------------------
ClickEffectPanel = ClickEffectPanel or DeclareMono("ClickEffectPanel", UIWFlushPanel)
function ClickEffectPanel:ClickEffectPanel()
    -- 监听smdata
    self.data = PublicPopupData.Instance
    self.data_cares = {
        {data = self.data:ClickData(), func = self.PlayEffect, init_call = false},
        {data = self.data:ClickData(), func = self.PlayEffect, init_call = false}
    }
end
function ClickEffectPanel:PlayEffect()
    local pos = self.data:ClickData().position
    pos.x = pos.x - (UnityEngine.Screen.width / 2)
    pos.y = pos.y - (UnityEngine.Screen.height / 2)
    pos = self:ScreenToUI(pos)
    UH.LocalPos(self.EffectNode, pos)
    self.EffectTool:Play(10010263)
end

function ClickEffectPanel:ScreenToUI(point)
	point = point:Clone()
    local width = UnityEngine.Screen.width
    local height = UnityEngine.Screen.height
    local origin = 1425 / 750
    local current = width / height
    local cw = 0
    local ch = 0
    if current > origin then
        cw = 1425 * current / origin
        ch = 750
    else
        cw = 1425
        ch = 750 * origin / current
    end
    point.x = point.x * cw / width
    point.y = point.y * ch / height
    return point
end

-------------------------------------------------
ChangeSceneEffectPanel = ChangeSceneEffectPanel or DeclareMono("ChangeSceneEffectPanel",UIWFlushPanel)
function ChangeSceneEffectPanel:ChangeSceneEffectPanel()
    self.data_cares = {
        {data = SceneData.Instance.change_scene_effect, func = self.PlayChangeSceneEffect, keys = {"id"}, init_call = false},
        {data = SceneData.Instance:Base(),func = self.PlayOtherEffect, keys = {"scene_mode"},init_call = true},
        {data = BattleData.Instance:BaseData(), func = self.PlayOtherEffect, keys = {"battle_mode"}, init_call = false},
        {data = GuideData.Instance.yinyang_eye_effect, func = self.PlayYinYangEffect, keys = {"id"}, init_call = false},
        {data = GuideData.Instance.yinyang_eye_effect, func = self.FlushYinYangEye, keys = {"id2"}, init_call = true},
    }
end

function ChangeSceneEffectPanel:PlayChangeSceneEffect()
    if SceneData.Instance.change_scene_effect.id ~= 0 and self.effect_handle == nil then
        self.effect_handle = self.EffectTool:PlayByEffectId(SceneData.Instance.change_scene_effect.id, self.EffectParent, true)
    end
end
function ChangeSceneEffectPanel:PlayYinYangEffect()
    local id = GuideData.Instance.yinyang_eye_effect.id
    if id > 0 then
        self.yinyang_effect = self.EffectTool:PlayByEffectId(id, self.EffectParent, true)
        self.yingyang_scene_effect = SE:Play(SceneEffectConfig.EffectList.YinYanEye, nil, nil, SceneMgr:MainCamera().gameObject)
    else
        if self.yinyang_effect then
            self.EffectTool:Stop(self.yinyang_effect)
            self.yinyang_effect = nil
        end
        -- if self.yingyang_scene_effect then
            SE:Release(self.yingyang_scene_effect)
            self.yingyang_scene_effect = nil
        -- end
    end
end

function ChangeSceneEffectPanel:PlayEffectEnd()
    SceneData.Instance.change_scene_effect.id = 0
    if self.effect_handle then
        self.EffectTool:Stop(self.effect_handle)
    end
    self.effect_handle = nil
end

function ChangeSceneEffectPanel:PlayOtherEffect()
    self.Canvas.sortingOrder = ViewLayer.Buttom * 1000
    self.SceneEffect:SetActive(SceneData.Instance:GetMode() == SceneModeType.DreamNotes or BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_DREAM_NOTES_CHALLENGE)
end
function ChangeSceneEffectPanel:FlushYinYangEye()
    self.YinYangEffect:SetActive(GuideData.Instance.yinyang_eye_effect.id2 > 0)
end

TopLayerPartnerBiography = TopLayerPartnerBiography or DeclareMono("TopLayerPartnerBiography", UIWFlushPanel)

function TopLayerPartnerBiography:TopLayerPartnerBiography()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Biography
    self.data_cares = {
        {data = self.data.biography_show, func = self.FlushPartnerBiographyShow, init_call = false},
    }

    self.anim_handle = self.ShowAnim:ListenEvent("view_state", function (eve_param)
        if eve_param == "hided" then
            self.ShowAnim:SetObjActive(false)
		end
	end)
end

function TopLayerPartnerBiography:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.ShowAnim:UnlistenEvent("view_state", self.anim_handle)
end

function TopLayerPartnerBiography:FlushPartnerBiographyShow()
    local bs = PartnerData.Instance.biography_show
    local is_show = 0 ~= bs.partner and 0 ~= bs.biography
    if is_show then
        self:FlushPartnerBiographyDetail(bs.partner, bs.biography)
        self.ShowAnim:SetObjActive(true)
    end
    self.ShowAnim:SetBool(APH("is_show"), is_show)
end

function TopLayerPartnerBiography:FlushPartnerBiographyDetail(partner, biography)
    if not (partner > 0) then return end
    local data = self.data:GetPartnerInfoById(partner)
    local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(data.id)
    biography = data.biographys[biography]
    UH.SetIcon(self.IconSp, PartnerData.IconId(data), ICON_TYPE.ITEM)
    PartnerInfo.SetQuality(self.Quality, info_quality.quality)
    self.Content.text = string.format(self.language.unlock_show, data.info.name, biography.name)
end

TopLayerBlack = TopLayerBlack or DeclareMono("TopLayerBlack", UIWFlushPanel)

function TopLayerBlack:TopLayerBlack()
    self.data = PublicPopupCtrl.Instance:Data()
    self.data_cares = {
        {data = self.data.top_layer_black, func = self.FlushBlackShow, init_call = false},
    }
end

function TopLayerBlack:FlushBlackShow()
    self.BlackObj:SetActive(self.data.top_layer_black.is_show)
end


SkillUIEffectPanel = SkillUIEffectPanel or DeclareMono("SkillUIEffectPanel",UIWFlushPanel)
function SkillUIEffectPanel:SkillUIEffectPanel()
    self.data_cares = {
        {data = BattleData.Instance:ViewInfo(), func = self.PlaySkillUiEffect, keys = {"skill_ui_effect_id"}, init_call = false}
    }
    self.effect_handles = {}
end

function SkillUIEffectPanel:PlaySkillUiEffect()
    local effect_id  = BattleData.Instance:ViewInfo().skill_ui_effect_id
    if effect_id ~= 0 then
        self.effect_handles[effect_id] = self.EffectTool:PlayByEffectId(effect_id,self.EffectParent,true)
    end
end
function SkillUIEffectPanel:PlayEffectEnd(handle, time, key)
    if self.effect_handles[handle.effId] then
        self.EffectTool:Stop(self.effect_handles[handle.effId])
        self.effect_handles[handle.effId] = nil
    end
end

TopLayerFormation = TopLayerFormation or DeclareMono("TopLayerFormation", UIWFlushPanel)

function TopLayerFormation:TopLayerFormation()
    self.data = FormationCtrl.Instance:Data()
    self.partner_data = PartnerCtrl.Instance:Data()
    self.pet_data = PetCtrl.Instance:Data()

    self.data_cares = {
        {data = self.data.top_layer_formation, func = self.FlushInfo, init_call = false},
    }

    for i = 1, self.EffectList:Length() do 
        self.EffectList[i]:SetData({mono = self,end_func = function ()
            self:EndEffect()
        end})
    end 
end

function TopLayerFormation:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_dt1)
    TimeHelper:CancelTimer(self.timer_dt2)
end

function TopLayerFormation:FlushInfo()
    local tlf = self.data.top_layer_formation
    if not tlf.is_show then return end
    for i = 1, self.EffectList:Length() do 
        self.EffectList[i]:AllHandleCancel()
    end
    self.fight_order_list = TableCopy(self.partner_data.fight_order_list:Val())
    self.partner = tlf.partner
    self.pet = tlf.pet
    self.data:ResetTopLayerFormation()
    self.partner_pos = 1
    for i = 1, #self.fight_order_list do
        if self.partner ~= self.fight_order_list[i] then
            self.partner_pos = self.partner_pos + 1
        end
    end
    self.partner_pos = math.min(4, self.partner_pos)

    self:FlushShow()

    self.ShowObj:SetActive(true)
    TimeHelper:CancelTimer(self.timer_dt1)
    self.timer_dt1 = TimeHelper:AddDelayTimer(function ()
        self.ShowObj:SetActive(false)
    end, 5)

    TimeHelper:CancelTimer(self.timer_dt2)
    self.timer_dt2 = TimeHelper:AddDelayTimer(BindTool.Bind(self.ShowEffect, self), 1)
end

function TopLayerFormation:FlushShow()
    UH.SetAvatar(self.RoleHead, RoleData.Instance:GetRoleAvatarType(),RoleData.Instance:GetRoleAvatarId(),RoleData.Instance:GetRoleAvatarQuality())
    local fol = self.fight_order_list
    local partner_list = {}
    if self.partner > 0 then
        for i = 1, #fol do
            if self.partner ~= fol[i] then
                table.insert(partner_list, fol[i])
            end
        end
    else
        partner_list = fol
    end
    for i = 1, self.PartnerCells:Length() do
        local partner_id = partner_list[i]
        self.PartnerCells[i]:SetObjActive(nil ~= partner_id)
        if nil ~= partner_id then
            self.PartnerCells[i]:SetData(self.partner_data:GetPartnerInfoById(partner_id))
        end
    end
    if self.partner > 0 then
        if self.pet_data:GetFightIndex() > -1 then
            self.PetCell:SetObjActive(true)
            self.PetCell:SetData(self.pet_data:GetFightPetInfo())
        end
    else
        self.PetCell:SetObjActive(self.pet > 0)
        if self.pet > 0 then
            self.PetCell:SetData(Pet.New{id = self.pet})
        end
    end
end

function TopLayerFormation:EndEffect()
    if self.partner > 0 then
        self.PartnerCells[self.partner_pos]:SetObjActive(true)
        self.PartnerCells[self.partner_pos]:SetData(self.partner_data:GetPartnerInfoById(self.partner))
    end
    if self.pet_data:GetFightIndex() > -1 then
        self.PetCell:SetObjActive(true)
        self.PetCell:SetData(self.pet_data:GetFightPetInfo())
    end
end

function TopLayerFormation:ShowEffect()
    if self.partner > 0 then
        self.EffectList[self.partner_pos]:Play(3161023)
    else
        self.EffectList[5]:Play(3161023)
    end
end

TopLayerFormationItemPartner = DeclareMono("TopLayerFormationItemPartner", UIWidgetBaseItem)

function TopLayerFormationItemPartner:TopLayerFormationItemPartner()
    self.partner_data = PartnerCtrl.Instance:Data()
end

function TopLayerFormationItemPartner:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetIcon(self.IconSp, PartnerData.IconId(self.data), ICON_TYPE.ITEM)
    local info_quality = self.partner_data:GetCurPartnerQualityInfo(self.data.id)
    PartnerInfo.SetQuality(self.Quality, info_quality.quality)
end

TopLayerFormationItemPet = DeclareMono("TopLayerFormationItemPet", UIWidgetBaseItem)

function TopLayerFormationItemPet:TopLayerFormationItemPet()
    self.pet_data = PetCtrl.Instance:Data()
end

function TopLayerFormationItemPet:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetIcon(self.IconSp, data:IconId())
    UH.SpriteName(self.Quality, data:QuaIcon())
end

TopLayerGiftEffect = TopLayerGiftEffect or DeclareMono("TopLayerGiftEffect", UIWFlushPanel)

function TopLayerGiftEffect:TopLayerGiftEffect()
    self.data = SocietyCtrl.Instance:Data()

    self.data_cares = {
        {data = self.data.gift_effect, func = self.FlushShow, init_call = false},
    }
end

function TopLayerGiftEffect:FlushShow()
    local ge = self.data.gift_effect
    if SocietyConfig.GiftEffect[ge.type] and SocietyConfig.GiftEffect[ge.type][ge.val] then
        local effects = SocietyConfig.GiftEffect[ge.type][ge.val]
        for _, effect in ipairs(effects) do
            self.EffectTool:Play(effect)
        end
    end
end

