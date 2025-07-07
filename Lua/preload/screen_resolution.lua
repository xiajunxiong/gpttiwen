local Screen = UnityEngine.Screen
local physicalWidth = Screen.width
--AXX.ScreenToLua.GetResolutionWidth()
local physicalHeight = Screen.height
--AXX.ScreenToLua.GetResolutionHeight()

local screen_resolution = {
    screen_scale = 1,
    window_scale = 1,
    draw_screen_width = physicalWidth,
    draw_scrren_height = physicalHeight
}

local m_handle_win_w = 1124 --弹框界面最大的操作分辨率宽
local m_handle_win_h = 660 --弹框界面最大的操作分辨率高
local m_win_w_min = 1334
local m_win_w_max = 1334
local m_win_h_min = 750
local m_win_h_max = 750

function screen_resolution:getScreenScale()
    local screen_scale = 1
    local w
    local h
    -- if IS_EDITOR == true then
    -- 	 w = Screen.width
    -- 	 h = Screen.height
    -- else
    w = self.draw_screen_width
    h = self.draw_scrren_height
    -- end

    local ui_w = w
    local ui_h = h

    ui_w = math.min(m_win_w_max, ui_w)
    ui_w = math.max(m_win_w_min, ui_w)
    ui_h = math.min(m_win_h_min, ui_h)
    ui_h = math.max(m_win_h_min, ui_h)

    local sw = ui_w / w
    local sh = ui_h / h

    return math.max(sw, sh)

    -- Debug.LogErrorFormat("GetScreenScale===w={0},h={1}", w, h);

    -- if w < m_win_w_min or h < m_win_h_min then
    -- 	local sw = m_win_w_min / w;
    -- 	local sh = m_win_h_min / h;
    -- 	screen_scale = math.max(sw, sh);
    -- elseif w > m_win_w_max or h > m_win_h_max then
    -- 	local sw = m_win_w_max / w;
    -- 	local sh = m_win_h_max / h;
    -- 	screen_scale = math.min(sw, sh);
    -- end

    -- return screen_scale;
end

--弹框的缩放，界面尽量铺大;
function screen_resolution:getWindowScale()
    local resi = 1
    local camScale = self.screen_scale
    if camScale > 1 then
        local sc_h
        local sc_w

        -- if IS_EDITOR == true then
        -- 	sc_h = Screen.height / camScale;
        -- 	sc_w = Screen.width / camScale;
        -- else
        sc_h = m_handle_win_h / camScale
        sc_w = m_handle_win_w / camScale
        -- end

        local sh = self.draw_scrren_height / sc_h
        local sw = self.draw_screen_width / sc_w
        if sh < sw then
            resi = sh
        else
            resi = sw
        end
    end
    -- print("!!!!!!!!!!!!!!!!GetWindowScale return=" .. tostring(resi))
    return resi
end

function screen_resolution:GetScreenScale()
    return self.screen_scale
end

function screen_resolution:GetWindowScale()
    return self.window_scale
end

function screen_resolution:SetMaxScreenResolution(width, height)
    local max_w = width
    --1920
    local max_h = height
    --1080

    -- print("!!!!!!!!!!!!!!" .. tostring(Screen.currentResolution))
    local now_w = physicalWidth
    --AXX.ScreenToLua.GetResolutionWidth()
    local now_h = physicalHeight
    --AXX.ScreenToLua.GetResolutionHeight()

    local draw_width_holder = self.draw_screen_width
    local draw_height_holder = self.draw_scrren_height
    self.draw_screen_width = now_w
    self.draw_scrren_height = now_h

    if IS_EDITOR == false then
        local sc_w = 1
        local sc_h = 1

        if now_w > max_w then
            sc_w = max_w / now_w
        end

        if now_h > max_h then
            sc_h = max_h / now_h
        end

        local screen_sc

        if sc_w > sc_h then
            screen_sc = sc_h
        else
            screen_sc = sc_w
        end

        self.draw_screen_width = math.floor(now_w * screen_sc)
        self.draw_scrren_height = math.floor(now_h * screen_sc)

        if self.draw_screen_width ~= draw_width_holder or self.draw_scrren_height ~= draw_height_holder then
            Screen.SetResolution(self.draw_screen_width, self.draw_scrren_height, true)
        -- print(string.format("set Screen.SetResolution!!w=%d,h=%d",self.draw_screen_width,self.draw_scrren_height))
        end
    end

    -- self.screen_scale = self:getScreenScale()
	-- self.window_scale = self:getWindowScale()
	-- 本帧结束时分辨率才会被真正设置
    TimeHelper:AddDelayFrameTimer(
        function()
            BaseView.RefreshAllReslution()
        end,
        1
    )
    -- panelMgr:SetScreenScale(self.screen_scale)
    -- Crystal.MeshUI.Scale = 1 / self.screen_scale
    -- print(string.format("Screen Real Size:%d,%d---Set Screen Resolution:%d,%d---now physic screen size=%d,%d,screen_scale=%.2f,window_scale=%.2f",
    -- 	now_w,now_h,self.draw_screen_width,self.draw_scrren_height,physicalWidth,physicalHeight,self.screen_scale,self.window_scale))
end

function screen_resolution:RestoreScreenResolution()
    if physicalWidth == self.draw_screen_width and physicalHeight == self.draw_scrren_height then   --已经是当前分辨率了，不需要重复设置
        return
    end
	Screen.SetResolution(physicalWidth, physicalHeight, true)
	self.draw_screen_width = physicalWidth
	self.draw_scrren_height = physicalHeight
	TimeHelper:AddDelayFrameTimer(
        function()
            BaseView.RefreshAllReslution()
        end,
        1
    )
end

function screen_resolution:GetPhysicalHW()
    return physicalHeight, physicalWidth
end

return screen_resolution
