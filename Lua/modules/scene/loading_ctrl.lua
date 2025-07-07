FILE_REQ("modules/scene/loading_data")

VIEW_REQ("modules/scene/loading_view")

LoadingCtrl = LoadingCtrl or BaseClass(BaseCtrl)

function LoadingCtrl:__init()
    if LoadingCtrl.Instance ~= nil then
		Debuger.LogError("[LoadingCtrl] attempt to create singleton twice!")
		return
    end
    LoadingCtrl.Instance = self
    self.data = LoadingData.New()
    self.handle_time = nil
    self.min_auto_per_time = 0.7
end

function LoadingCtrl:__delete()
	if LoadingCtrl.Instance == self then
		LoadingCtrl.Instance = nil
    end
end

--percent (0,1]
--如果percent 为1话loading界面会被关闭，percent 大于0的话会自动打开loading界面
function LoadingCtrl:SetPercent(percent,desc)
    self.data:SetViewData(percent,desc)
end

--假进度条
--com_fun 返回true是表示加载完成
function LoadingCtrl:AutoPercent(com_func,desc)
    self:SetPercent(0,desc)
    if not ViewMgr:IsOpen(LoadingView) then
        PublicPopupCtrl.Instance:ShowWait()
        ViewMgr:OpenView(LoadingView)
    end
    if self.handle_time ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.handle_time)
    end
    local begin_load_time = Time.realtimeSinceStartup
    self.handle_time = Runner.Instance:RunUntilTrue(
        function()
            if ViewMgr:IsOpen(LoadingView) and
                (Time.realtimeSinceStartup - begin_load_time >= self.min_auto_per_time) and 
                (com_func() == true) then
                self:SetPercent(1,desc)
                self.handle_time = nil
                return true
            end
            local per  = self.data:Percent()
            per = per + 0.02
            if per < 1 then
                -- local desc = string.format("", math.floor(per * 100))
                self:SetPercent(per,desc)
            end
            return false
        end)
end

function LoadingCtrl:ManualLoading(on_update)
    if self.handle_time ~= nil then     --取消正在进行的自动loading
        Runner.Instance:RemoveRunUntilTrue(self.handle_time)
        self.handle_time = nil
    end
    local init_per,init_desc = on_update()
    self:SetPercent(init_per,init_desc)
    if not ViewMgr:IsOpen(LoadingView) then
        PublicPopupCtrl.Instance:ShowWait()
        ViewMgr:OpenView(LoadingView)
    end
    self.handle_time = Runner.Instance:RunUntilTrue(
        function() 
            local per,desc = on_update()
            self:SetPercent(per,desc)
            if per >= 1 then
                self.handle_time = nil
                return true
            end
            return false
        end)
end