FILE_REQ("modules/scene/vo_manager")
FILE_REQ("modules/scene/base_scene")
FILE_REQ("modules/scene/scene_data")
FILE_REQ("modules/scene/scene_obj_config")
FILE_REQ("modules/scene/scene_obj_data")
FILE_REQ("modules/scene/scene_obj_holder")
FILE_REQ("modules/scene/scene_logic")
FILE_REQ("modules/scene/game_scene_mode")
FILE_REQ("modules/scene/effect/scene_effect")

FILE_REQ("modules/scene/follow_data")
FILE_REQ("modules/scene/chs_follow")

FILE_REQ("modules/scene/sceneobj/scene_obj")
FILE_REQ("modules/scene/sceneobj/scene_ch")
FILE_REQ("modules/scene/sceneobj/main_ch")
FILE_REQ("modules/scene/sceneobj/attach_obj")
FILE_REQ("modules/scene/scene_requre_list")

SceneCtrl = SceneCtrl or BaseClass(BaseCtrl)

function SceneCtrl:__init()
	if SceneCtrl.Instance ~= nil then
		Debuger.LogError("[SceneCtrl] attempt to create singleton twice!")
		return
	end	
	SceneCtrl.Instance = self
	self.cur_scene = nil
	self.data = SceneData.New()
	self.obj_data = SceneObjData.New()
	self.vo_manager = VoManager.New()
	self.objs = SceneObjHolder.New()
	self.logic = SceneLogic.New()
	self.effect = SceneEffect.New()
	self.follow_data = FollowData.New()
	self:registerProcs()
	Game.SceneTouchHelper.AddLayer(GameObjLayer.ColliderScene,GameObjLayer.ColliderRole)
	self.wait_loading_view = false
	self.next_scene = nil
	self.change_scene_delay_handle = nil
	self.cache_scene_type = nil
	self.cache_show_loading = false
	self.is_first_enter = 0
	self.chs_follow_queue = ChsFollow.New()
end

function SceneCtrl:__delete()
	self:unregisterProcs()
	self.chs_follow_queue:DeleteMe()
	self.chs_follow_queue = nil
	if SceneCtrl.Instance == self then
		SceneCtrl.Instance = nil
	end
end
function SceneCtrl:GotoScene(scene_type,show_loading, old_scene_id)
	local change_cache_scene = function ()
		self.cache_scene_type = scene_type
		if self.cache_show_loading == false then
			self.cache_show_loading = show_loading	
		end
		if self.change_scene_delay_handle == nil then
			self.change_scene_delay_handle = TimeHelper:AddDelayFrameTimer(function()
					self.change_scene_delay_handle = nil
					self:peroformGotoScene(self.cache_scene_type,self.cache_show_loading)
					self.cache_scene_type = nil
					self.cache_show_loading = false	
				end,2)
		end
	end
	--判断是否要播放转场特效
	if old_scene_id ~= nil and not show_loading then
		local need_show_eff = self.data:GetNeedChangeSceneEffect(old_scene_id)
		if not need_show_eff then
			change_cache_scene()
		else
			self.delay_change_scene = TimeHelper:AddDelayTimer(function ()
				change_cache_scene()
			end,1.5)
		end
	else
		change_cache_scene()
	end
end


function SceneCtrl:peroformGotoScene(scene_type,show_loading)
	self.next_scene = scene_type.New(self.data:SceneId())
	if self.wait_loading_view == false then
		if show_loading then
			--show loading open loading view
			-- ViewMgr:OpenView(LoadingView)
			LoadingCtrl.Instance:AutoPercent(function()
				local judge_scene = self:CurScene() ~= nil and self:CurScene() or self:NextScene()
				-- LogError("scene===",judge_scene.scene_id, judge_scene:LoadingComplete())
				return judge_scene and judge_scene:LoadingComplete()
			end)
			self.wait_loading_view = true
			Runner.Instance:RunUntilTrue(function ()
				if ViewMgr:IsOpen(LoadingView) then
					self.wait_loading_view = false
					self:gotoSceneInternal(show_loading)
					return true
				else
					return false
				end
			end
			)
		else
			self:gotoSceneInternal(show_loading)
		end
	end
end

function SceneCtrl:gotoSceneInternal(show_loading)
	PublicPopupCtrl.Instance:HideWait()
	if self.cur_scene ~= nil then
		self.cur_scene:Dispose(show_loading)
	end
	self.cur_scene = self.next_scene
	self.next_scene = nil
	if self.cur_scene ~= nil then
		local c_s = self.cur_scene
		c_s:StartLoading()
		Runner.Instance:RunUntilTrue(function ()
			if c_s:Disposed() then
				return true
			end
			if c_s:LoadingComplete() == true then
				c_s:OnLoadingComplete()
				-- ViewMgr:CloseView(LoadingView)
				return true
			else
				return false
			end
		end
		)
	else
		-- ViewMgr:CloseView(LoadingView)
	end
end

function SceneCtrl:NextScene()
	return self.next_scene
end

function SceneCtrl:CurScene()
	return self.cur_scene
end

function SceneCtrl:Objs()
	return self.objs
end
function SceneCtrl:Logic()
	return self.logic
end
function SceneCtrl:VoMgr()
	return self.vo_manager
end
function SceneCtrl:AddNpc(vo,is_not_notice)
	self.vo_manager:AddNpc(vo,is_not_notice)
end

function SceneCtrl:DeleteNpc(vo,is_not_notice)
	self.vo_manager:DeleteNpc(vo,is_not_notice)
end

function SceneCtrl:BlockNpc(npc_seq)
	self.vo_manager:BlockNpc(npc_seq)
end

function SceneCtrl:UnBlockNpc(npc_seq)
	self.vo_manager:UnBlockNpc(npc_seq)
end

function SceneCtrl:OnUnloadGameLogic()
	self.data:SetSceneId(0)
	self.logic:ClearMoveCache(true)
	SceneMgr.data.loaded_scene_id = 0
	ViewMgr:FlushView(MainView,MainViewPanel:GetTypeName())
end

FILE_REQ("modules/scene/scene_ctrl_protocol")