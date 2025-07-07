
local gameLMB = Game.LuaMonoBehaviour
LuaMonoBehaviour = LuaMonoBehaviour or {}-- BaseClass()
LuaMonoBehaviour._type_name = "LuaMonoBehaviour"
-- function LuaMonoBehaviour:onMonoAwake(um)
-- 	self.unityMono = um
-- 	self._serialize_checked = {}	--if serialize data has checked in C#,value of key in the table is true
-- 	self._serialize_data = um.Data
-- 	-- self:Awake()
-- end
-- local luamono_orginal_keys = 
-- {
-- 	unityMono = true,
-- 	_serialize_data = true,
-- }

local un_awake_mono_map = {} --key:CSharp LuaMono,val:LuaMono

function LuaMonoBehaviour:Awake()

end

function LuaMonoBehaviour:Start()

end

function LuaMonoBehaviour:OnEnable()

end

function LuaMonoBehaviour:OnDisable()

end

function LuaMonoBehaviour:OnDestroy()

end

function LuaMonoBehaviour:IsEnabled()
	return self.unityMono.enabled
end

function LuaMonoBehaviour:IsActiveAndEnabled()
	if self.unityMono == nil then
		Debuger.LogError("LuaMonoBehaviour:IsActiveAndEnabled ERROR,unityMono is NULL,LuaMonoType=%s",self._type_name)
		return false
	end
	if self.unityMono:Equals(nil) then
		Debuger.LogError("LuaMonoBehaviour:IsActiveAndEnabled ERROR,unityMono Has Destroyed,LuaMonoType=%s",self._type_name)
		return false
	end
	return self.unityMono.isActiveAndEnabled or false
end

function LuaMonoBehaviour:SetEnable(enable)
	self.unityMono.enabled = enable
end


function LuaMonoBehaviour.Add(game_obj,type_tab)
	-- local unity_lua_mono = game_obj:AddComponent(typeof(Game.LuaMonoBehaviour))
	-- return unity_lua_mono:AwakeByLua(type_tab._type_name) 
	local unity_lua_mono = gameLMB.Create(typeof(gameLMB),type_tab._type_name)
	return unity_lua_mono ~= nil and unity_lua_mono.Table or nil
end

function LuaMonoBehaviour.Destroy(behaviour)
	UnityObj.Destroy(behaviour.unityMono)
end

function LuaMonoBehaviour.FindMono(type_tab)
	return gameLMB.FindMono(type_tab._type_name)
end

--仅用于开发的时候方便查报错使用，业务逻辑尽量不要用
function LuaMonoBehaviour:attachGameObjecName()
	-- UnityEngine.Debug.LogError(self.unityMono.gameObject.name,self.unityMono.gameObject)
	return self.unityMono.gameObject.name
end

function LuaMonoBehaviour:GetType()
	return self._type_tab
end

function LuaMonoBehaviour:GetTypeName()
	return self._type_tab._type_name
end

function LuaMonoBehaviour:SetObjActive(active)
	self.unityMono:SetObjActive(active)
end

function LuaMonoBehaviour:GetObjActive()
	return self.unityMono:GetObjActive()
end

function LuaMonoBehaviour:Destroyed()
	local re_val = rawget(self,"unityMono")
	if re_val ~= nil then
		if type(re_val) == "function" then
			rawset(self, "unityMono", nil)
			return true
		elseif re_val:Equals(nil) then
			rawset(self, "unityMono", nil)
			return true
		else
			return false
		end
	else
		return true
	end
end

function LuaMonoBehaviour:GetObjActiveInScene()
	return self.unityMono:GetObjActiveInScene()
end
------------Array Serialize data--------------

local function get_array_key(key,index)
	return string.format("_AY_%s_%d",key,index)
end

local function check_create_array_tab(ser_data,key)
	local arr_tab = nil
	local ar_key = get_array_key(key,1)
	local ar_1 = ser_data:GetValue(ar_key)
	if ar_1 ~= nil then		--is array
		arr_tab = {_key = key,_ser_data = ser_data,_len = nil,[1] = ar_1}
		setmetatable(arr_tab,LuaMonoArrayData_Meta)
	end
	return arr_tab
end

--metatable
LuaMonoArrayData_Meta = LuaMonoArrayData or {}

LuaMonoArrayData_Meta.__index = 
	function(array_tab,key) 
		local re_val = LuaMonoArrayData_Meta[key]
		if re_val == nil then
			if type(key) == "number" then
				local ar_key = get_array_key(array_tab._key,key)
				re_val = array_tab._ser_data:GetValue(ar_key)
				array_tab[key] = re_val
			end 
		end
		return re_val
	 end

function LuaMonoArrayData_Meta:Length() 
	if self._len == nil then
		self._len = self._ser_data:GetArrayCount(self._key)
	end
	return self._len
end

------------Global mono func----------------

local _class_types = {}
_class_types["LuaMonoBehaviour"] = LuaMonoBehaviour
setmetatable(LuaMonoBehaviour,{__index = function(mono_inst,key) return nil end})

function DeclareMono(type_name,super)
	local type_tab = {}
	type_tab._type_name = type_name
	type_tab._type_tab = type_tab
	if super == nil then
		super = LuaMonoBehaviour
	end

	type_tab._super = super

	setmetatable(type_tab,{__index = function(mono_inst,key) return super[key] end})

	_class_types[type_name] = type_tab

	return type_tab
end

local function createLuaMono(type_tab,unityMono)
	local new_mono = {}

	new_mono._type_tab = type_tab
	new_mono.unityMono = unityMono
	new_mono._serialize_checked = {}	--if serialize data has checked in C#,value of key in the table is true
	new_mono._serialize_data = unityMono.Data

	local type_meta_tab = getmetatable(type_tab)
	local type_meta_tab_index = type_meta_tab.__index
	setmetatable(new_mono,{__index = function(mono_tab_inst,key)  
		local re_val = rawget(type_tab,key)
		if re_val == nil then
			re_val = type_meta_tab_index(mono_tab_inst,key)
		end
		if re_val == nil 
			-- and luamono_orginal_keys[key] ~= true  
			and rawget(mono_tab_inst,"unityMono") ~= nil
			then
			--game obj
			if key == "gameObject" then				
				re_val = rawget(mono_tab_inst,"unityMono").gameObject
				rawset(mono_tab_inst,key,re_val)
				return re_val
			else
				local ser_da_chk = rawget(mono_tab_inst,"_serialize_checked")
				if ser_da_chk[key] == true then
					return nil
				else
					local csh_ser_data = rawget(mono_tab_inst,"_serialize_data")
					local ser_da = csh_ser_data:GetValue(key)
					ser_da_chk[key] = true		--record has get data already
					if ser_da == nil then		---check array
						ser_da = check_create_array_tab(csh_ser_data,key)
					end
					if ser_da ~= nil then
						rawset(mono_tab_inst,key,ser_da)
					end
					return ser_da
				end
			end
		else
			return re_val
		end
	end,
	}
		)


	-- new_mono:onMonoAwake(unityMono)

	--call constructor
	do
		local call_constructor
		call_constructor = function(type_t)
			if type_t._super ~= nil then
				call_constructor(type_t._super)
			end
			if type_t[type_t._type_name] then
				type_t[type_t._type_name](new_mono)
			end
		end

		call_constructor(type_tab)
	end
	return new_mono
end

function CreateLuaMonoByTypeName(type_name,unityMono)
	local type_tab = _class_types[type_name]
	if type_tab == nil then
		UnityEngine.Debug.LogError("CreateLuaMonoByTypeName ERROR,type not exist:" .. tostring(type_name))
		return nil
	end
	local new_mono = createLuaMono(type_tab,unityMono)
	-- if unityMono.isActiveAndEnabled == false then
		un_awake_mono_map[unityMono] = new_mono
	-- end
	return new_mono
end


function MonoTypeOf(type_name)
	return _class_types[type_name]
end

local rawpairs = pairs

function LuaMonoCheckAndCleanCache()
	local count = 0
	local recycle_count = 0
	for c_sh_mono,lua_mono in rawpairs(un_awake_mono_map) do
		if c_sh_mono:Equals(nil) then
			recycle_count = recycle_count + 1
			local ser_chk = rawget(lua_mono,"_serialize_checked")
			if ser_chk ~= nil then
				for k,v in rawpairs(ser_chk) do
					if v then
						local ser_val = rawget(lua_mono,k)
						if ser_val ~= nil then
							if type(ser_val) == "table" and getmetatable(ser_val) == LuaMonoArrayData_Meta then
								for ak,_ in rawpairs(ser_val) do
									if ser_val[ak] ~= nil then
										ser_val[ak] = nil
									end
								end
							end
							rawset(lua_mono,k,nil)
						end
					end
				end
			end
			rawset(lua_mono,"unityMono",nil)
			rawset(lua_mono,"gameObject",nil)
			rawset(lua_mono,"_serialize_data",nil)
			un_awake_mono_map[c_sh_mono] = nil
		else
			count = count + 1
		end
	end
	print(string.format("LuaMonoCheckAndCleanCache Exe,RecycleCount=%d,RemaindCount=%d",recycle_count,count))
end
