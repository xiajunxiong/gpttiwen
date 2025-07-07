
RoleInfoRoundView = RoleInfoRoundView or DeclareView("RoleInfoRoundView","role_info/role_info_round")
VIEW_DECLARE_MASK(RoleInfoRoundView,ViewMask.BlockClose)

function RoleInfoRoundView:RoleInfoRoundView()
	self.data = RoleInfoCtrl.Instance:Data()
	self.role_round_info = self.data:RoleRoundInfo()
	self.auto_setting = {
		half_info_pos_width = 168 / 2,
		info_pos_height = {
			base = 50,
			per_button = 58,
		}
	}
end

function RoleInfoRoundView:LoadCallback()
	self.input_t = {
		input_x = UnityEngine.Input.mousePosition.x / UnityEngine.Screen.width * self.ViewSize.rect.width,
		input_y = UnityEngine.Input.mousePosition.y / UnityEngine.Screen.height * self.ViewSize.rect.height,
	}
	self.Roles:SetData(self.role_round_info)
	local size_y = (#self.role_round_info > 5) and 400 or (40 + #self.role_round_info * 63)
	self.BgSize.sizeDelta = Vector2.New(300, size_y)
	self.ScrollRect.enabled = #self.role_round_info > 5
	self.Block.enabled = #self.role_round_info > 5
	self:AutoToPos(size_y)
end

function RoleInfoRoundView:AutoToPos(size_y)
	local pos_t = {
		input_x = self.input_t.input_x,
		input_y = self.input_t.input_y,
		view_w = self.ViewSize.rect.width,
		view_h = self.ViewSize.rect.height,
		x = 0,
		y = 0,
	}

	local info_h = size_y
	local y = pos_t.input_y - pos_t.view_h/2
	pos_t.y = info_h < pos_t.input_y and y or y + info_h - pos_t.input_y

	local plus_x = {
		right = self.auto_setting.half_info_pos_width,
		left = self.auto_setting.half_info_pos_width
	}                                                                                                
	if pos_t.input_x + plus_x.right > pos_t.view_w then
		pos_t.x = pos_t.input_x - (pos_t.input_x + plus_x.right - pos_t.view_w)
	elseif pos_t.input_x - plus_x.left < 0 then
		pos_t.x = pos_t.input_x - (pos_t.input_x - plus_x.left)
	else
		pos_t.x = pos_t.input_x
	end
	pos_t.x = pos_t.x - pos_t.view_w/2
	
	self.InfoPosition.localPosition = Vector2.New(pos_t.x, pos_t.y)
end

local special_names = {
	[73346] = Language.Marry.DeskName,
	[73625] = Language.Marry.DeskName,
	[73626] = Language.Marry.DeskName,
}

RoleInfoRoundViewItem = DeclareMono("RoleInfoRoundViewItem", UIWidgetBaseItem)

function RoleInfoRoundViewItem:RoleInfoRoundViewItem()
end

function RoleInfoRoundViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local vo = data
	local is_role = SceneObjType.Role == vo.obj_type
	local in_only_fight = SceneData.Instance:GetMode() == SCENE_TYPE.ONLY_FIGHT
	local name = vo.name
	if nil == name then
		local co = NpcHelper.NpcCo(vo.id)
		name = nil ~= co and co.name or ""
	end
	if StringIsEmpty(name) then
		if SceneObjType.Monster == vo.obj_type then
			local co = Cfg.MonsterById(vo.id)
			name = nil ~= co and co.name or ""
		elseif vo.id and special_names[vo.id] then
			name = special_names[vo.id]
		end
	end
	if in_only_fight and is_role then
		name = Language.ItemInfo.NoneName
	end
	self.Name.text = is_role and name or RichTextHelper.ColorStr(name, COLORSTR.Yellow1)
	self.NpcObj:SetActive(not is_role)
	local prof_type = RoleData.ProfIdToProfType(vo.profession or 0)
	self.ProfSp.SpriteName = RoleData.GetProfSp(prof_type)
end

function RoleInfoRoundViewItem:OnClick()
	local vo = self.data
	if SceneObjType.Role == vo.obj_type then
		SocietyCtrl.Instance:ClickToRoleVo(vo)
	else
		SceneCtrl.Instance:Logic():AutoToNpc(vo)
	end
	ViewMgr:CloseView(RoleInfoRoundView)
end

RoleInfoSwornDescView = RoleInfoSwornDescView or DeclareView("RoleInfoSwornDescView","role_info/role_info_sworn_desc")
VIEW_DECLARE_MASK(RoleInfoSwornDescView,ViewMask.BlockClose)

function RoleInfoSwornDescView:LoadCallback(param_t)
	UH.SetText(self.Title,param_t.title_name or "")
	UH.SetText(self.Desc,param_t.desc or "")
	UH.LocalPosG(self.Pos,param_t.pos or Vector3.zero)
	self.Rect.sizeDelta = param_t.rect or Vector2.New(208, 247)
end