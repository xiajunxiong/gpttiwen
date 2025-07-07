--FullScreen 普通全屏界面

--NotFullRect 全屏界面 不强制缩进 可以唯一控制缩放
--FullScreenRect 全屏界面 需要缩进的Panel 可以唯一控制缩放和缩进

--FullScreenBg 需要适配苹果的背景 强制缩进
--FullScreenBgArray 需要适配苹果的背景列表 强制缩进

--等待列表 OpenWait
BaseView = BaseView or DeclareMono("BaseView")

BaseView.FlushAll = "_FlushAll"

local audio_open = {
    [ViewLevel.L1] = CommonAudio.ViewOpenL1
}

local audio_close = {
    [ViewLevel.L1] = CommonAudio.ViewCloseL1
}

-------FlushGroup====UIWFlushPanelGroup
-------OpenWait======UIWSubstitute[]
-------Anim======Animator(nil)
-------FullScreen===Bool
-------AudioOpen====String
-------AudioClose==String
function DeclareView(view_name, res_name, mod_key, layer)
    local view_class = DeclareMono(view_name, BaseView)
    view_class.res_path = string.format("ui/views/%s.prefab", res_name)
    if layer == nil then
        layer = ViewLayer.Normal
    end
    view_class.layer = layer
    view_class.block_mask = ViewMask.BgBlockNoAnim
    view_class.mod_key = mod_key
    if mod_key then
        if type(mod_key) == "number" then
            if mod_key > 0 then
                Mod.RegisterView(mod_key, view_class)
            end
        elseif type(mod_key) == "table" then
            Mod.ReigsterModkeyToModTab(mod_key)
            Mod.RegisterView(mod_key, view_class)
        end
    end
    return view_class
end

local perfect_w = 1425
local perfect_h = 750
local win_per_w = 1334
local win_per_h = 750

local perfect_aspect = perfect_w / perfect_h
local win_per_aspect = win_per_w / win_per_h
local scale_per_1 = perfect_aspect / win_per_aspect

local opened_views = {}

local scale_cache = nil

function BaseView.GetWindowScale()
    if IS_EDITOR or scale_cache == nil then
        local screenSize = Vector2.New(UnityEngine.Screen.width, UnityEngine.Screen.height)
        local aspect = screenSize.x / screenSize.y
        if aspect >= perfect_aspect then
            scale_cache = 1
        elseif aspect <= win_per_aspect then
            scale_cache = scale_per_1
        else
            scale_cache = perfect_aspect / aspect
        end
    end
    return scale_cache
end

function BaseView.RefreshAllReslution()
    scale_cache = nil
	for view, _ in pairs(opened_views) do
		view:RefreshReslution()
    end
end

function BaseView:BaseView()
    self.canvas = nil --UnityEngine.Canvas
    self.is_show = false
    self.show_hide_eve = nil
    self.onhided = nil
    self.tmp_block_open_anim = false            --是否临时在打开的时候不显示打开动画，设置为true后只会有一次作用，一般在LoadCallback或者view的构造函数里面赋值
end

function BaseView:performAllWait(complete_func)
    Runner.Instance:RunUntilTrue(
        function()
            if self.OpenWait ~= nil then
                for i = 1, self.OpenWait:Length() do
                    local wait = self.OpenWait[i]
                    if  (self.gameObject:GetParent().activeSelf == false) or --剧情等情况下会关掉根canvas，这个时候wait对象的完成判断暂停
                        (wait:IsActiveAndEnabled() and not wait:IsLoaded()) 
                        then 
                        return false
                    end
                end
            end
            complete_func()
            return true
        end
    )
end

function BaseView:SetVisible(visible)
    self.canvas.enabled = visible
end

function BaseView:waitForShow(complete_func)
    self.canvas.enabled = true
    if self.Anim ~= nil then
        if self.show_hide_eve == nil then
            self.Anim:Rebind()
            self.show_hide_eve =
                self.Anim:ListenEvent(
                "view_state",
                function(eve_param, anim_st_info)
                    self.is_show = eve_param == "showed" and true or false
                    if self.is_show == false and self.onhided ~= nil then
                        local hi = self.onhided
                        self.onhided = nil
                        hi()
                    end
                end
            )
        end
        self.Anim:SetBool(APH("is_show"), true)
        if self.tmp_block_open_anim then
            self.tmp_block_open_anim = false
            self.Anim:Play("showing",-1,1)
            self.is_show = true
            complete_func()
        else
            Runner.Instance:RunUntilTrue(
                function()
                    if self.is_show then
                        complete_func()
                    end
                    return self.is_show
                end
            )
        end
    else
        self.is_show = true
        complete_func()
    end
end

function BaseView:RefreshReslution()
    if self.gameObject == nil then
        return
    end
    if IS_IPHONEX or EditorSettings:SimularIphoneX() then
        if not self.cache or not self.seted_reslution_rect then
            if not self.NotFullRect and not self.FullScreenRect then
                local rectTransform = self.gameObject:GetComponent(typeof(UnityEngine.RectTransform))
                rectTransform:SetSizeWithCurrentAnchors(0, rectTransform.rect.width - 100)
            end
            if self.FullScreenBg then
                self.FullScreenBg:SetLocalScale(Vector3.New(1.1,1.1,1))
            end
            if self.FullScreenBgArray then
                for i=1, self.FullScreenBgArray:Length() do
                    self.FullScreenBgArray[i]:SetLocalScale(Vector3.New(1.1,1.1,1))
                end
            end
            if self.FullScreenRect then
                self.FullScreenRect:SetSizeWithCurrentAnchors(0, self.FullScreenRect.rect.width - 80)
            end
            self.seted_reslution_rect = true
        end
        if self.FullScreen or self.NotFullRect then
            ViewMgr:CheckReslutionData()
        end
    elseif not self.FullScreen and not self.NotFullRect and not self.FullScreenRect then
        local sc = BaseView.GetWindowScale()
        self.gameObject:SetLocalScale(Vector3.New(sc, sc, 1))
    end
end

function BaseView:StartLoading(callback, data)
    self:RefreshReslution()
    self:performAllWait(
        function()
            self:playOpenAudio()
            self:LoadCallback(data)
            --first flush
            self:Flush()
            self:performAllWait(
                function()
                    callback()
                    opened_views[self] = true
                end
            )
        end
    )
end
function BaseView:SelectTabbar(modKey)
    local mod = FunOpen.Instance:FindMod(modKey)
    local _, param_t = Mod.ParseKey(modKey)
    -- LogError(mod, key , modKey ,param_t)
    if self.Board and self.Board.Tabbar then
        self.Board.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(mod, self.Board.Tabbar, param_t and param_t.open_param or 1))
    end
end

function BaseView:playOpenAudio()
    if self.AudioOpen == "" then
        return
    end --设置不播放打开音乐
    local audio_name = (self.AudioOpen ~= nil) and self.AudioOpen or audio_open[self:GetType().view_level]
    if audio_name ~= nil then
        AudioMgr:PlayEffect(AudioType.UI, audio_name)
    end
end

function BaseView:playCloseAudio()
    if self.AudioClose == "" then
        return
    end --设置不播放关闭音乐
    local audio_name = (self.AudioClose ~= nil) and self.AudioClose or audio_close[self:GetType().view_level]
    if audio_name ~= nil then
        AudioMgr:PlayEffect(AudioType.UI, audio_name)
    end
end

function BaseView:StartOpening(callback)
    self:waitForShow(
        function()
            self:OpenCallback()
            callback()
        end
    )
end

function BaseView:StartClosing(callback)
    self:playCloseAudio()
    self.onhided = function()
        -- self:CloseCallback()
        callback()
        opened_views[self] = nil
    end
    if self.Anim ~= nil then
        -- Runner.Instance:RunUntilTrue(function()
        -- 		local hided = not self.is_show
        -- 		if hided then
        -- 			onhided()
        -- 		end
        -- 		return hided
        -- 	end)
        self.Anim:SetBool(APH("is_show"), false)
    else
        self.is_show = false
        self.onhided()
        self.onhided = nil
    end
end

function BaseView:Flush(flush_tab)
    if self.FlushGroup ~= nil then
        if flush_tab == nil or flush_tab[BaseView.FlushAll] == true then
            self.FlushGroup:Flush()
        else
            for k, v in pairs(flush_tab) do
                self.FlushGroup:Flush(k)
            end
        end
    end
    if self.OnFlush ~= nil then
        if self.wait_on_fl == nil then
            self.wait_on_fl =
                self:performAllWait(
                function()
                    self:OnFlush()
                    self.wait_on_fl = nil
                end
            )
        end
    end
end

--------------------重载函数-------------------------
function BaseView:LoadCallback()
end

function BaseView:OpenCallback()
end

function BaseView:CloseCallback()
end

------Event Router-------

function _UI_EVENT_ROUTER_THREE_PARAM(send_data, arg1, arg2, arg3)
    local reciever_tab = send_data.ReceiverTable
    local func = reciever_tab[send_data.ReceiverFunc]
    if func == nil then
        Debuger.LogError("_UI_EVENT_ROUTER_THREE_PARAM:Func not exist=[%s]", tostring(send_data.ReceiverFunc))
    else
        func(reciever_tab, arg1, arg2, arg3)
    end
end

function _UI_EVENT_ROUTER_TWO_PARAM(send_data, arg1, arg2)
    _UI_EVENT_ROUTER_THREE_PARAM(send_data, arg1, arg2, nil)
end

function _UI_EVENT_ROUTER_ONE_PARAM(send_data, arg1)
    _UI_EVENT_ROUTER_THREE_PARAM(send_data, arg1, nil, nil)
end

function _UI_EVENT_ROUTER(send_data, arg1)
    _UI_EVENT_ROUTER_THREE_PARAM(send_data, nil, nil, nil)
end

--lv:
--窗口弹窗等级,决定窗口之间的互斥，3级弹框不会互斥
-- ViewLevel = {
-- 	L0 = 1,		--主界面，组队等全屏界面
-- 	L1 = 2,		--1级弹框，背包，人物等
-- }
function VIEW_DECLARE_LEVEL(view_tab, lv)
    view_tab.view_level = lv
end

--mask_type:
--窗口遮罩方式
-- ViewMask = {
-- 	None = 0,
-- 	Block = 1,				--只遮挡下层事件
-- 	BgBlock = 2,			--遮挡下层事件并显示半透明背景
-- 	BlockClose = 3,			--遮挡下层事件并点击关闭
-- 	BgBlockClose = 4,		--遮挡下层事件并显示半透明背景且点击关闭
-- 	BgBlockNoAnim = 5,		--遮挡下层事件并显示半透明背景(无蒙版动画)
-- 	BgBlockCloseNoAnim = 6,	--遮挡下层事件并显示半透明背景且点击关闭(无蒙版动画)
-- }
function VIEW_DECLARE_MASK(view_tab, mask_type)
    view_tab.block_mask = mask_type
end

--为缓存窗口
function VIEW_DECLARE_CACHE(view_tab)
    view_tab.cache = true
end
---------------------------
--为全屏界面不需要缩进窗口层背景但需要缩进sub而设计
--必要条件SubRect的锚点Anchors Min=(0,0) Max=(1,1)
--★★★★★★★★★★★★★★★★★★★★★★★★★★
--★建议把此LuaMonoBehaviour放入View的OpenWait      ★
--★如果放入OpenWait这必须把此LuaMonoBehaviour挂载到★
--★与View相同的GameObject上否则界面可能打不开      ★
--★★★★★★★★★★★★★★★★★★★★★★★★★★
--SubRects ======Array(RectTransform)
--IndentValue =======Array(string/或非string类型的值) 序号需要与SubRects中的对应 
-----string:左右侧缩进值通过减号连接 例50-50
-----非string类型的值：代表SubRects对应序号中的项在缩进时使用默认值即左右各50，建议使用int类型填写对应SubRects中的序号，比较直观不用一个一个数
IndentSubPanel = IndentSubPanel or DeclareMono("IndentSubPanel",UIWFlushPanel)
function IndentSubPanel:IndentSubPanel()
    self.indent_done = false
    self.seted_reslution_rect = false
end

function IndentSubPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if IS_IPHONEX or EditorSettings:SimularIphoneX() then
        if self.SubRects and not self.seted_reslution_rect then
            for i=1, self.SubRects:Length() do
                local values = {"50","50"}
                if self.IndentValue and self.IndentValue[i] and type(self.IndentValue[i]) == "string" then
                    values = string.split(self.IndentValue[i],"-")
                end
                self.SubRects[i].offsetMin = Vector2.New(tonumber(values[1]), 0)
                self.SubRects[i].offsetMax = Vector2.New(tonumber(values[2]), 0)
            end
            self.seted_reslution_rect = true
        end
    end
    self.indent_done = true
end

function IndentSubPanel:IsLoaded()
    return self.indent_done
end

--全屏背景适配问题处理方案
SetFullScreenReslution = SetFullScreenReslution or DeclareMono("SetFullScreenReslution",UIWFlushPanel)

function SetFullScreenReslution:SetFullScreenReslution()
    ViewMgr.reslution_data:Uncare(self.handle_care)
    if IS_IPHONEX or EditorSettings:SimularIphoneX() then
        self.handle_care = ViewMgr.reslution_data:Care(function()
            local canvas_width = ViewMgr:GetMainCanvasRect().rect.width
            local rectTransform = self.gameObject:GetComponent(typeof(UnityEngine.RectTransform))
            if self.gameObject:GetComponent(typeof(UnityEngine.UI.AspectRatioFitter)) == nil then
                rectTransform:SetSizeWithCurrentAnchors(0, canvas_width)
            else
                local scale = canvas_width / rectTransform.rect.width
                UH.LocalScale(rectTransform,Vector3.New(scale,scale,1))    
            end
            ViewMgr.reslution_data:Uncare(self.handle_care)
        end)
    end
end

--设置全屏适配边距
SetFullScreenSepartion = SetFullScreenSepartion or DeclareMono("SetFullScreenSepartion",UIWFlushPanel)

function SetFullScreenSepartion:SetFullScreenSepartion()
    ViewMgr.reslution_data:Uncare(self.handle_care)
    if IS_IPHONEX or EditorSettings:SimularIphoneX() then
        self.handle_care = ViewMgr.reslution_data:Care(function()
            local rectTransform = self.gameObject:GetComponent(typeof(UnityEngine.RectTransform))
            rectTransform.offsetMin = Vector2.New(self.OffsetMin or 0, 0)
            rectTransform.offsetMax = Vector2.New(-self.OffsetMax or 0, 0)
            ViewMgr.reslution_data:Uncare(self.handle_care)
        end)
    end
end