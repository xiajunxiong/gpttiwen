
TaskDCZYFBMenuView = TaskDCZYFBMenuView or DeclareMono("TaskDCZYFBMenuView", UIWFlushPanel)

function TaskDCZYFBMenuView:TaskDCZYFBMenuView()
	self.data = DuChuangZeiYingCtrl.Instance:Data()
	self.language = Language.Task.DCZYMenu
	self.data_cares = {
		{data = SceneData.Instance:Base(), func = self.FlushMenu, keys = {"scene_id"}}
	}
end

function TaskDCZYFBMenuView:FlushMenu()
	local co = Cfg.DuChuangZeiYingLayerBySceneId(SceneData.Instance:SceneId(), false)
	if nil == co then return end
	self.Progress.text = string.format(self.language.Progress, co.level, DuChuangZeiYingConfig.FB_LAYER_MAX)
	self.Goal.text = co.txt
end