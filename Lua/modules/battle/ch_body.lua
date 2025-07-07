


local anim_eve_key = {
	["idle"] = ChAnimType.Idle,
	["run"] = ChAnimType.Run,
	["death"] = ChAnimType.Death,
	["behit"] = ChAnimType.Behit,

	["attack"] = ChAnimType.Attack,
	["defence"] = ChAnimType.Defence,
	["battle_run"] = ChAnimType.BattleRun,
	["counter_attack"] = ChAnimType.CounterAttack,
	["fall_down"] = ChAnimType.FallDown,

	["weapon_show"] = ChAnimType.WeaponShow,

	["run2"] = ChAnimType.Run2
}


ChBody = ChBody or BaseClass()

-- ChBody.State = {
-- 	Loading,
-- 	Loaded,
-- }


function ChBody:__init(parent)
	self.state = 0
	self.is_loading = true
	self.obj = nil
	self.parent_obj = parent
	self.animator = nil

	self.cur_anim = ChAnimType.Idle

	self.on_anim_hit = nil
end

function ChBody:__delete()
	if self.obj ~= nil then
		UnityGameObj.Destroy(self.obj)
		self.obj = nil
	end
end

function ChBody:Flush(res_path)
	self.is_loading = true
	if res_path == nil then
		res_path = "actor/monster/test.prefab"
	end
	ResMgr:LoadGameObject(res_path,function (obj)
	self.obj = obj
	self.obj:SetParent(self.parent_obj,false)
	self.animator = self.obj:GetComponent(typeof(UnityAnim))
	if self.animator ~= nil then
		self.animator:ListenEvent("enter",BindTool.Bind(self.OnEnterAnimatorState,self))
		self.animator:ListenEvent("hit",BindTool.Bind(self.OnAnimatorHit,self))
	end
		self.is_loading = false
	end,true,true)
end

function ChBody:IsLoading()
	return self.is_loading
end

function ChBody:PlayAnim(anim)
	if anim == ChAnimType.Idle then
		self.animator:SetInteger(APH("state"),0)
	elseif anim == ChAnimType.Run then
		self.animator:SetInteger(APH("state"),1)
	elseif anim == ChAnimType.BattleRun then
		self.animator:SetInteger(APH("state"),2)
	elseif anim == ChAnimType.Death then
		self.animator:SetInteger(APH("state"),8)
	else
		self.animator:SetTrigger(APH(anim))
		self.animator:SetBool(APH("to_unloop"),true)
	end
end
function ChBody:AnimClipsEmpty(name)
	self.animator:AnimClipsEmpty(name)
end

function ChBody:OnEnterAnimatorState(key,anim_st_info)
	self.cur_anim = anim_eve_key[key]
end

function ChBody:WaitAnimEnd(anim,func)
	local handle = nil
	handle = self.animator:ListenEvent("end",function(key,anim_st_info) 
			if anim_eve_key[key] == anim then
				func()
				self.animator:UnlistenEvent("end",handle)
			end
		end)
end

function ChBody:SetOnAnimHit(func)
	self.on_anim_hit = func
end


function ChBody:OnAnimatorHit(key,anim_st_info)
	if self.on_anim_hit ~= nil then
		self.on_anim_hit()
	end
end


function ChBody:CurAnim()
	return self.cur_anim
end