
IllusionInfo = IllusionInfo or BaseClass()

function IllusionInfo:__init(param_t)
	self.info = param_t.info
	self.id = self.info.id
	self.name = self.info.surface_name
	self.type = self.info.type
	self.suit_id = self.info.suit_id
	self.active_stuff_id = self.info.active_stuff_id
	self.res_id = self.info.res_id
	self.level = 0
	self.end_time = 0
	self.red_point = false
	self.show_level = param_t.info.show_level	--显示等级，角色大于等于这个等级时才会显示这个实装
	self:IllusionInfo()
end

function IllusionInfo:IllusionInfo()
end

function IllusionInfo:IllusionVo(vo)
	self.vo = vo
	self.level = vo.level
	self.end_time = vo.end_time
end

function IllusionInfo:Reset()
	self.vo = nil
	self.level = 0
	self.end_time = 0
	self.red_point = false
end

