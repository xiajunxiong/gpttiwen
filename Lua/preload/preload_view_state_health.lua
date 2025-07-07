PreloadViewStateHealth = PreloadViewStateHealth or DeclarePreloadView("preload/health")

local ShowState = {
    None = -1,
    Appare = 0,
    Keep = 1,
    Disappare = 2,
}
PreloadViewStateHealth.appare_time = 0.75   --渐变出现的时间
PreloadViewStateHealth.keep_time = 1.5   --出现后的总持续时间
PreloadViewStateHealth.disappare_time = 0.75   --渐变消失的时间


function PreloadViewStateHealth:__init()
    -- local bgVol = math.min(UnityPlayerPrefs.GetInt(PrefKeys.SettingVal(1),100),UnityPlayerPrefs.GetInt(PrefKeys.SettingVal(2),100))
    -- if bgVol >= 0 then
    --     Game.AudioManager.Singleton().BgVolume = bgVol/100
    --     Game.AudioManager.Singleton():PlayBg("audio/bg/test2.asset")
    -- end
    self.timer = nil
    self.canvas_group = nil
    self.show_st = ShowState.None
    self.frame_count = 0
end


function PreloadViewStateHealth:LoadCallback()
    self.canvas_group = self:Obj():GetComponent(typeof(UnityEngine.CanvasGroup))
    self.canvas_group.alpha = 0
    self.show_st = ShowState.Appare
    -- self.timer = Time.realtimeSinceStartup
    local splash_path = ChannelAgentConsts.SplashBgPath()
    if splash_path ~= nil then
        self.bg_raw_image = self:Obj().Find("bg"):GetComponent(typeof(UnityUI.RawImage))
        -- UnityEngine.Debug.LogError("splash_path "..splash_path)
        local tex = PreloadViewStateHealth.LoadStreamingTexture(splash_path)
        self.bg_raw_image.texture = tex
        -- self.bg_raw_image:SetNativeSize()
    end
end

function PreloadViewStateHealth.LoadStreamingTexture(streaming_path)
    local tex = nil
    tex = UnityEngine.Texture2D.New(2,2,UnityEngine.TextureFormat.RGBA32,false)
    local pic_data = Nirvana.StreamingAssets.ReadAllBytes(streaming_path)
    UnityEngine.ImageConversion.LoadImage(tex, pic_data)
    return tex
end

function PreloadViewStateHealth:Start(load_callback)
    PreloadViewState.Start(self,load_callback)
    PreloadData.Instance:SetProgSpeed(PreloadData.ProgSpeed.Slow)
end

function PreloadViewStateHealth:Update()
    PreloadViewState.Update(self)
    -- print("PreloadViewStateHealth.update")
    if self.frame_count < 1 then
        self.frame_count = self.frame_count + 1
        return
    end
    if self.timer == nil then
        self.timer = Time.realtimeSinceStartup
    end
    if self.show_st == ShowState.Appare then
        local alpha_val = (Time.realtimeSinceStartup - self.timer) / PreloadViewStateHealth.appare_time
        self.canvas_group.alpha = alpha_val
        if alpha_val >= 1 then
            PreloadData.Instance:SetProgSpeed(PreloadData.ProgSpeed.Fast)   ---中间定格画面，可以偷偷同步加载更多的东西
            self.show_st = ShowState.Keep
            self.timer = Time.realtimeSinceStartup
        end
    elseif self.show_st == ShowState.Keep then
        if Time.realtimeSinceStartup - self.timer > PreloadViewStateHealth.keep_time then
            PreloadData.Instance:SetProgSpeed(PreloadData.ProgSpeed.Slow)
            self.show_st = ShowState.Disappare
            self.timer = Time.realtimeSinceStartup
        end
    elseif self.show_st == ShowState.Disappare then
        local alpha_val = 1 - (Time.realtimeSinceStartup - self.timer) / PreloadViewStateHealth.disappare_time
        self.canvas_group.alpha = alpha_val
        if alpha_val <= 0  and  PreloadViewStateLoad.preload_obj ~= nil then
            self.show_st = ShowState.None
            self:Stop()
        end
    end
end

function PreloadViewStateHealth:Stop()
	PreloadViewState.Stop(self)
    if not IS_IOS() then
    	print("PreloadViewStateHealth:Stop")
    end
end

return PreloadViewStateHealth