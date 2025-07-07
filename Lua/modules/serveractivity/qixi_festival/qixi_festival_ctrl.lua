VIEW_REQ("modules/serveractivity/qixi_festival/qixi_festival_view")
-- 七夕活动
QixiFestivalCtrl = QixiFestivalCtrl or BaseClass(BaseCtrl)

function QixiFestivalCtrl:__init()
    if QixiFestivalCtrl.Instance ~= nil then
        Debuger.LogError("[QixiFestivalCtrl] attempt to create singleton twice!")
        return
    end
    QixiFestivalCtrl.Instance = self
    self.data = QixiFestivalData.New()
end

function QixiFestivalCtrl:OnInit()
    ActivityRandData.Instance:Register(Mod.QixiFestival.Main,function()
		return self.data:IsOpen()
    end)
end

function QixiFestivalCtrl:__delete()
    if QixiFestivalCtrl.Instance == self then
        QixiFestivalCtrl.Instance = nil
    end
end

-----------------QixiFestivalData-----------------
QixiFestivalData = QixiFestivalData or BaseClass()

function QixiFestivalData:__init()
    if QixiFestivalData.Instance ~= nil then
        Debuger.LogError("[QixiFestivalData] attempt to create singleton twice!")
        return
    end
    QixiFestivalData.Instance = self
    self.config = Config.activity_main_auto.charm_tanabata
end

function QixiFestivalData:__delete()
    QixiFestivalData.Instance = nil
end

function QixiFestivalData:IsOpen()
    for k,v in pairs(self.config) do
		if ActivityData.IsOpen(v.act_type) then
			return true
		end
	end
	if ViewMgr:IsOpen(QixiFestivalView) then
		ViewMgr:CloseView(QixiFestivalView)
	end
	return false
end

function QixiFestivalData:Config(seq)
    return self.config[seq] or {}
end