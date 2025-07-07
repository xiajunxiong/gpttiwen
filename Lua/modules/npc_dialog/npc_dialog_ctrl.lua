
FILE_REQ("modules/npc_dialog/npc_dialog_config")
FILE_REQ("modules/npc_dialog/npc_dialog_data")
VIEW_REQ("modules/npc_dialog/npc_dialog_view")
VIEW_REQ("modules/npc_dialog/npc_dialog_view_details")
VIEW_REQ("modules/npc_dialog/npc_dialog_sync")

NpcDialogCtrl = NpcDialogCtrl or BaseClass(BaseCtrl)

function NpcDialogCtrl:__init()
	if NpcDialogCtrl.Instance ~= nil then
		Debuger.LogError("[NpcDialogCtrl] attempt to create singleton twice!")
		return
	end	
	NpcDialogCtrl.Instance = self
	self.data = NpcDialogData.New()
	self.is_need_chck = false
	self:RegisterAllProtocols()
end

function NpcDialogCtrl:__delete()
	if NpcDialogCtrl.Instance == self then
		NpcDialogCtrl.Instance = nil
	end
end

function NpcDialogCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSRoleTalkToNpc)
end

function NpcDialogCtrl:SendRoleTalkToNpc()
	local protocol = GetProtocol(CSRoleTalkToNpc)
	SendProtocol(protocol)
end

-- param_t = {talk_id, beh_btn_list, talk_list, is_skip, not_block_close, block_dofunc}
-- beh_btn_list = {
-- 	[1] = NpcDialogBtnData,
-- 		..
-- 	}
-- talk_list = {						-- 自定义对话内容需要用到
-- 	[1] = {
-- 		talker = npc_id or 0 			-- 0 主角
-- 		talk_text = "balabala..."
-- 	}
-- 	[2] = {

-- 	}
-- 	..
-- }
-- is_skip -- 直接跳过到最后一句话(部分战斗任务没打过重新打需要跳过)
-- not_block_close      -- 为true时无法点击关闭界面，默认nil 不会执行end_func
-- not_block_close_func -- 为true时无法点击关闭界面，默认nil 会执行end_func
-- need_click_close -- 强行需要自己手动点击才会关闭对话界面
-- need_auto_typing -- 为true时自动跳过
-- block_dofunc 屏蔽npc对话界面的DoFunc调用（屏蔽执行任务）
-- block_flush 是否屏蔽刷新npc对话（小游戏界面会再界面上打开NPC对话窗口这时如果主线任务进行到与npc对话会导致对话被刷新为任务对话而不是小程序对话）

function NpcDialogCtrl:NpcDialogView(param_t,end_func, need_check, default_npc, need_click_close, block_flush)
	if self.block_flush then
		return
	end
	self.data:NpcDialogData(param_t)
	self.data:SetCustomEndFunc(end_func)
	self.data.default_npc = default_npc
	if need_click_close then
		self.data.need_click_close = need_click_close 
	else
		self.data.need_click_close = false
	end
	if not ViewMgr:IsOpen(NpcDialogView) then
		--判断是否有其他界面打开
		if need_check == true and ViewMgr:ExistOtherOpenView() then
			self.is_need_chck = true
			return
		end
		self.data.cahce_chck_data = param_t
		ViewMgr:OpenView(NpcDialogView)
	elseif self.block_flush ~= true and not block_flush then
		ViewMgr:FlushView(NpcDialogView)
	end
	self.block_flush = block_flush
	-- self.is_need_chck = false
end
function NpcDialogCtrl:ViewCloseCheck()
	if BattleData.Instance:IsServerFighting() then
		self.is_need_chck = false
		return
	end
	if GuideManager.Instance:IsGuide() then
		self.is_need_chck = false
		return
	end
	if not ViewMgr:ExistOtherOpenView() and self.is_need_chck then
		ViewMgr:OpenView(NpcDialogView)
		self.is_need_chck = false
	end
end

function NpcDialogCtrl:NpcInfo(npd_id)
	return self.data.npc_list_config[npd_id]
end

function NpcDialogCtrl:SwichModel(res_id)
	local view = LuaMonoBehaviour.FindMono(NpcDialogView)
	view.ui_obj_npc:SetData(DrawerHelper.GetNpcPath(res_id))
	view.ui_obj_npc:PlayAnim(ChAnimType.Show)
end

function NpcDialogCtrl:OnUnloadGameLogic()
	ViewMgr:CloseView(NpcDialogView)
end

function NpcDialogCtrl.TalkNpcDialog(talk_id,beh_btn_list)
	NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id,beh_btn_list = beh_btn_list})
end

function NpcDialogCtrl:TestOptions(id)
	NpcDialogCtrl.Instance:NpcDialogView({talk_id = id})
end