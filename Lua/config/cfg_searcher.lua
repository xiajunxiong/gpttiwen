Cfg = Cfg or {}

--[[
Cfg.KeyGet(auto,error_tag) 
主键直接获取 参数只有一个数字 [无额外内存消耗，检索速度最快，适合有大量配置的公用表]

Cfg.CacheGet(auto,key_name1)
预先将所有配置按指定key存入缓存，运行时直接用下标取出 [需要一定时间装载，有一定内存开销，检索速度最快，适合常用的非主键检索配置]

Cfg.CacheGet2Key(auto,key_name1,key_name2)
预先将所有配置按指定key1,key2存入缓存，运行时直接用下标取出 [需要一定时间装载，有一定内存开销，检索速度最快，适合常用的非主键检索配置]

Cfg.CacheGet2KeyList(auto,key_name1,key_name2)
预先将所有配置按指定key1,key2存入缓存，运行时直接用下标取出 [需要一定时间装载，有一定内存开销，检索速度最快，适合常用的非主键检索配置]

Cfg.RealTimeSearch(auto,key_name1)
实时查找，找到的配置会被cache下来，下次直接用key返回值 [无装载时间，只有必要的内存开销，检索速度较慢，适合不常用的检索配置]

Cfg.RealTimeSearch2Key(auto,key_name1,key_name2)
实时查找，找到的配置会被cache下来，下次直接用key返回值 [无装载时间，只有必要的内存开销，检索速度较慢，适合不常用的检索配置]

Cfg.Register(func_name,searcher)
注册监所方案func_name 调用检索的函数名 seacher 外部创建的searcher
]]

--主键直接获取 参数只有一个数字 [无额外内存消耗，检索速度最快，适合有大量配置的公用表]
function Cfg.KeyGet(auto,error_tag, extra_auto)
	local re = {}
	re.auto = auto
	if error_tag ~= nil then
		re.error_tag = error_tag
	end
	if extra_auto then
		re.extra = extra_auto
	end
	re.SearchFunc = function (key,miss_report)
		if miss_report == nil then
			miss_report = true
		end
		local c = re.auto[key]
		if c ~= nil then
			return c
		end
		if re.extra then
			c = re.extra[key]
			if c ~= nil then
				return c
			end
		end
		if miss_report then
			Debuger.LogErrorSD("%s NotExist ERROR key=%s,miss_report=%s",re.error_tag,tostring(key),tostring(miss_report))
		end
		return nil
	end
	return re
end

--预先将所有配置按指定key存入缓存，运行时直接用下标取出 [需要一定时间装载，有一定内存开销，检索速度最快，适合常用的非主键检索配置]
function Cfg.CacheGet(auto,key_name1)
	local re = {}
	re.auto = auto
	re.cache = {}

	for _,v in pairs(re.auto) do
		re.cache[v[key_name1]] = v
	end

	re.SearchFunc = function (key1,miss_report)
		if miss_report == nil then
			miss_report = true
		end
		local c = re.cache[key1]
		if c ~= nil then 
			return c
		end
		if miss_report then
			Debuger.LogErrorSD("%s NotExist ERROR key=%s",re.error_tag,tostring(key1))
		end
		return nil
	end

	return re
end

--预先将所有配置按指定key1,key2存入缓存，运行时直接用下标取出 [需要一定时间装载，有一定内存开销，检索速度最快，适合常用的非主键检索配置]
function Cfg.CacheGet2Key(auto,key_name1,key_name2)
	local re = {}
	re.auto = auto
	re.cache = {}
	for _,v in pairs(re.auto) do
		local key1_val = v[key_name1]
		local key2_val = v[key_name2]
		local key1_cache = re.cache[key1_val]
		if key1_cache == nil then
			key1_cache = {}
			re.cache[key1_val] = key1_cache
		end
		key1_cache[key2_val] = v
	end

	re.SearchFunc = function (key1,key2,miss_report)
		if miss_report == nil then
			miss_report = true
		end		
		local key1_ca = re.cache[key1]
		if key1_ca ~= nil then
			local c = key1_ca[key2]
			if c ~= nil then
				return c
			end
		end
		if miss_report then
			Debuger.LogErrorSD("%s NotExist ERROR key1=%s,key2=%s",re.error_tag,tostring(key1),tostring(key2))
		end
		return nil
	end

	return re
end

--预先将所有配置按指定key1,key2存入缓存，运行时直接用下标取出 [需要一定时间装载，有一定内存开销，检索速度最快，适合常用的非主键检索配置]
function Cfg.CacheGet2KeyList(auto,key_name1,key_name2)
	local re = {}
	re.auto = auto
	re.cache = {}
	for _,v in pairs(re.auto) do
		local key1_val = v[key_name1]
		local key2_val = v[key_name2]
		local key1_cache = re.cache[key1_val]
		if key1_cache == nil then
			key1_cache = {}
			re.cache[key1_val] = key1_cache
		end
		if key1_cache[key2_val] == nil then
			key1_cache[key2_val] = {}
		end
		table.insert(key1_cache[key2_val], v)
	end

	re.SearchFunc = function (key1,key2,miss_report)
		if miss_report == nil then
			miss_report = true
		end
		local key1_ca = re.cache[key1]
		if key1_ca ~= nil then
			local c = key1_ca[key2]
			if c ~= nil then
				return c
			end
		end
		if miss_report then
			Debuger.LogErrorSD("%s NotExist ERROR key1=%s,key2=%s",re.error_tag,tostring(key1),tostring(key2))
		end
		return nil
	end
	return re
end

--实时查找，找到的配置会被cache下来，下次直接用key返回值 [无装载时间，只有必要的内存开销，检索速度较慢，适合不常用的检索配置]
function Cfg.RealTimeSearch(auto,key_name1)
	local re = {}
	re.auto = auto
	re.cache = {}
	re.SearchFunc = function (key1, miss_report)
		if miss_report == nil then
			miss_report = true
		end
		local c = re.cache[key1]
		if c == nil then
			--缓存里没有循环查找
			for _,v in pairs(re.auto) do
				local n = v[key_name1]
				if n == key1 then
					re.cache[key1] = v
					return v
				end
			end
		else
			return c
		end
		if miss_report then
			Debuger.LogErrorSD("%s NotExist ERROR key1=%s",re.error_tag,tostring(key1))
		end
		return nil
	end
	return re
end

--实时查找，找到的配置会被cache下来，下次直接用key返回值 [无装载时间，只有必要的内存开销，检索速度较慢，适合不常用的检索配置]
function Cfg.RealTimeSearch2Key(auto,key_name1,key_name2)
	local re = {}
	re.auto = auto
	re.cache = {}
	re.SearchFunc = function (key1,key2,miss_report)
		if miss_report == nil then
			miss_report = true
		end
		local key1_ca = re.cache[key1]
		if key1_ca == nil then
			for _,v in pairs(re.auto) do
				if v[key_name1] == key1 and v[key_name2] == key2 then
					key1_ca = {}
					key1_ca[key2] = v
					re.cache[key1] = key1_ca
					return v
				end
			end
		else
			local c = key1_ca[key2]
			if c == nil then
				for _,v in pairs(re.auto) do
					if v[key_name1] == key1 and v[key_name2] == key2 then
						key1_ca[key2] = v
						return v
					end
				end
			else
				return c
			end
		end
		if miss_report then
			Debuger.LogErrorSD("%s NotExist ERROR key1=%s,key2=%s",re.error_tag,tostring(key1),tostring(key2))
		end
		return nil
	end

	return re
end

--实时查找，第一次查找会重排一个新表缓存起来，都是引用配置的表，不会占用过多内存
--注意>>配置里的key不能重复
function Cfg.RealTimeSearch3Key(auto,key_name1,key_name2,key_name3)
	local re = {}
	re.auto = auto
	re.cache = {}
	re.SearchFunc = function (key1,key2,key3,miss_report)
		if miss_report == nil then
			miss_report = true
		end
		if key1 and key2 and key3 then
			if TableIsEmpty(re.cache) then
				for i,v in pairs(re.auto) do
					local key_1 = v[key_name1]
					local key_2 = v[key_name2]
					local key_3 = v[key_name3]
					if not re.cache[key_1] then
						re.cache[key_1] = {}
					end
					if not re.cache[key_1][key_2] then
						re.cache[key_1][key_2] = {}
					end
					re.cache[key_1][key_2][key_3] = v
				end
			end
			if re.cache[key1] and re.cache[key1][key2] and re.cache[key1][key2][key3] then
				local cfg = re.cache[key1][key2][key3]
				return cfg
			end
		end
		if miss_report then
			Debuger.LogErrorSD("%s NotExist ERROR key1=%s,key2=%s key3=%s",re.error_tag,tostring(key1),tostring(key2),tostring(key3))
		end
		return nil
	end

	return re
end


--实时查找，找到的配置会被cache下来，下次直接用key返回值，key_name1和key_nam2为或关系，任意键值等于传入值都会匹配成功 [无装载时间，只有必要的内存开销，检索速度较慢，适合不常用的检索配置]
function Cfg.RealTimeSearchOR(auto,key_name1,key_name2)
	local re = {}
	re.auto = auto
	re.cache = {}
	re.SearchFunc = function (key1, miss_report)
		if miss_report == nil then
			miss_report = true
		end
		local c = re.cache[key1]
		if c == nil then
			--缓存里没有循环查找
			for _,v in pairs(re.auto) do
				if v[key_name1] == key1 or v[key_name2] == key1  then
					re.cache[key1] = v
					return v
				end
			end
		else
			return c
		end
		if miss_report then
			Debuger.LogErrorSD("%s NotExist ERROR key1=%s",re.error_tag,tostring(key1))
		end
		return nil
	end
	return re
end

Cfg.Config = {
--===============================公共======================================
	--通过npc_id获取npc_auto表中的npc配置
	["NpcById"] = Cfg.KeyGet(Config.npc_auto.npc_list,"NpcById"),
	["NpcBySeq"] = Cfg.KeyGet(Config.npc_dynamic_auto.npc_dynamic_cfg,"NpcBySeq"),
	--通过monster_id获取monster_auto表中的怪物配置
	["MonsterById"] = Cfg.KeyGet(Config.monster_auto.monster,"MonsterById", Config.monster2_auto.monster),
	["MonsterGroupById"] = Cfg.KeyGet(Config.monster_group_auto.monster_group,"MonsterGroupById"),
	["BuffByBuffID"] = Cfg.KeyGet(Config.buff_list_auto.buff_list,"BuffByBuffID"),

	["PetById"] = Cfg.KeyGet(Config.pet_cfg_auto.pet_info_2,"PetById"),
	["PetInfo2ById"] = Cfg.KeyGet(Config.pet_cfg_auto.pet_info_2, "PetInfo2ById"),
	["PetSkillPageById"] = Cfg.RealTimeSearch(Config.pet_cfg_auto.pet_gift_list, "param_1"),
	["PartnerById"] = Cfg.RealTimeSearch(Config.partner_cfg_auto.partner_info,"partner_id"),
	["MonsterGrow"] = Cfg.CacheGet(Config.tasklist_auto.monster_grow,"ordinary_monster"),
	
	["SkillSingle"] = Cfg.RealTimeSearch2Key(Config.single_skill_auto.skill_cfg,"skill_id","skill_level"),
	["SkillPower"] = Cfg.RealTimeSearch2Key(Config.power_skill_auto.skill_cfg,"skill_id","skill_level"),
	["SkillSuperPower"] = Cfg.RealTimeSearch2Key(Config.super_power_skill_auto.skill_cfg,"skill_id","skill_level"),
	["SkillRandom"] = Cfg.RealTimeSearch2Key(Config.random_skill_auto.skill_cfg,"skill_id","skill_level"),
	["SkillCombo"] = Cfg.RealTimeSearch2Key(Config.combo_skill_auto.skill_cfg,"skill_id","skill_level"),
	["SkillLine"] = Cfg.RealTimeSearch2Key(Config.line_skill_auto.skill_cfg,"skill_id","skill_level"),
	["SkillPassive"] = Cfg.RealTimeSearch(Config.passive_skill_auto.passive_cfg, "skill_id"),
	["SkillPassiveSpID"] = Cfg.RealTimeSearch(Config.passive_skill_auto.passive_cfg, "sp_id"),
	["SkillPassiveLevelSpID"] = Cfg.RealTimeSearch(Config.passive_skill_auto.passive_level_cfg, "sp_id"),
	["HeartSkillByID"] = Cfg.RealTimeSearch(Config.heart_skill_cfg_auto.heart_skill_base, "heart_skill_id"),
	["SkillArea"] = Cfg.RealTimeSearch(Config.area_skill_auto.skill_cfg, "skill_id"),

	["SkillPassiveByType"] = Cfg.RealTimeSearch2Key(Config.passive_skill_auto.passive_cfg, "skill_type","is_same_type"),
	["SkillLife"] = Cfg.RealTimeSearch2Key(Config.life_skill_auto.life_skill,"skill_id","skill_level"),
	["SkillDescPet"] = Cfg.RealTimeSearch2Key(Config.skill_description_auto.pet_skill_description,"skill_id","skill_level"),
	["SkillDescPartnerAndRole"] = Cfg.RealTimeSearch2Key(Config.skill_description_auto.partner_skill_description,"skill_id","skill_level"),
	["SkillDescLife"] = Cfg.RealTimeSearch2Key(Config.skill_description_auto.life_skill_desc, "skill_id","skill_level"),
	["SkillDescHeart"] = Cfg.RealTimeSearch2Key(Config.skill_description_auto.heart_skill_desc, "skill_id","skill_level"),
	["GetChangeJobDesc"] = Cfg.RealTimeSearch2Key(Config.skill_description_auto.change_job_desc,"job_type","skill_id"),
	["YouMingSkillDesc"] = Cfg.RealTimeSearch2Key(Config.skill_description_auto.dim_pet_skill_desc, "skill_id", "skill_level"),
	["CareerSkillList"] = Cfg.RealTimeSearch2Key(Config.career_advancements_auto.career_skill_list,"job_type","skill_id"),
	["CareerAdvancementsGrade"] = Cfg.RealTimeSearch2Key(Config.career_advancements_auto.career_advancements,"job_id","order"),
	["SpecialAddAttr"] = Cfg.RealTimeSearch(Config.special_effect_auto.spe_add_attr, "effect_id"),
	["SpecialEffect"] = Cfg.RealTimeSearch(Config.special_effect_auto.spe_list, "sp_id"),
	["RoleScoreLevel"] = Cfg.RealTimeSearch2Key(Config.score_cfg_auto.role_score,"job","level"),
	["PetScoreLevel"] = Cfg.RealTimeSearch(Config.score_cfg_auto.pet_score,"level"),

	["NpcButtonListByButtonId"] = Cfg.RealTimeSearch(Config.npc_auto.button_list, "button_id"),
	["ChatChannelInfoByChannelId"] = Cfg.RealTimeSearch(Config.chat_auto.channel_info, "channel_id"),

	["ItemModelGroup"] = Cfg.RealTimeSearch(Config.crystal_scene_auto.item_group,"item_id"),
	["SmeltAttrQua"] = Cfg.CacheGet2KeyList(Config.entry_cfg_auto.att_value_1,"level","att_type"),
	["SmeltAttrQuaJ"] = Cfg.CacheGet2KeyList(Config.entry_cfg_auto.att_value_2,"level","att_type"),
	["SmeltAttrQua2"] = Cfg.CacheGet2KeyList(Config.entry_cfg_auto.att_value_3,"level","att_type"),
	["SmeltAttrQuaJ2"] = Cfg.CacheGet2KeyList(Config.entry_cfg_auto.att_value_4,"level","att_type"),
	["SmeltAttrRandGroup"] = Cfg.CacheGet2KeyList(Config.entry_cfg_auto.rand_group,"att_group","att_type"),
	["SmeltAttrCellMax"] = Cfg.RealTimeSearch(Config.entry_cfg_auto.max_num,"level"),
	["SmeltAttrPri"] = Cfg.RealTimeSearch2Key(Config.entry_cfg_auto.priority,"prof","att_type"),

	["AudioLead"] = Cfg.RealTimeSearch2Key(Config.audio_auto.lead,"role_type","weapon_type"),
	["AudioNLead"] = Cfg.RealTimeSearch(Config.audio_auto.nlead,"res_id"),
	["AudioProf"] = Cfg.RealTimeSearch2Key(Config.audio_auto.profession,"role_type","profession_type"),
	["AudioCreateRole"] = Cfg.RealTimeSearch(Config.audio_auto.create_role,"role_type"),

	["BulletScreenById1"] = Cfg.RealTimeSearch(Config.bullet_screen_auto.bullet_screen_config, "bullet_screen_id"),
	["BulletScreenById2"] = Cfg.RealTimeSearch(Config.bullet_screen_auto.bullet_screen_content_config, "seq"),
	["BulletScreenById3"] = Cfg.RealTimeSearch(Config.bullet_screen_auto.bullet_screen_condition_config, "seq"),

	["MedalEffectById"] = Cfg.RealTimeSearch(Config.special_effect_auto.spe_list, "sp_id"),

	["RewardJobBagHasId"] = Cfg.CacheGet(Config.bag_cfg_auto.job_bag, "old_id"),
	["RewardJobBagByIdType"] = Cfg.RealTimeSearch2Key(Config.bag_cfg_auto.job_bag, "old_id", "job_type"),

	["FabaoUpgrade"] = Cfg.CacheGet2KeyList(Config.talisman_cfg_auto.talisman_upgrade_new,"id","level"),
	["FabaoAdvance"] = Cfg.CacheGet2KeyList(Config.talisman_cfg_auto.talisman_advance_new,"id","advanced_level"),
	["FabaoFettersId"] = Cfg.CacheGet(Config.talisman_cfg_auto.talisman_fetter,"id"),

	["StoreCfg"] = Cfg.CacheGet2KeyList(Config.shop_cfg_auto.department_store,"conver_type","item_id"),

	["ScaleScoreCfgByProf"] = Cfg.RealTimeSearch(Config.score_cfg_auto.scale_score, "prof"),

	["SpecialCompose"] = Cfg.RealTimeSearch(Config.compose_auto.compose_list, "stuff_id_1")
	-- ["CoolCardInfoById"] = Cfg.RealTimeSearch(Config.cool_summer_auto.sign_group,"sign_id"),
	-- ["CoolCardRewardByType"] = Cfg.RealTimeSearch(Config.cool_summer_auto.vitality_raward_group,"type"),
	-- ["CoolBreezeCoByType"] = Cfg.RealTimeSearch(Config.cool_summer_auto.cool_breeze,"type"),

--===============================挑战副本======================================
	--通过副本id和chapter_id查找chapter信息
	-- ["ChallengeFbChapter"] = Cfg.CreateCacheGet2Key(Config.challenge_fuben_auto.challenge_content,"fuben_id","chapter")

}

--注册监所方案
--func_name 调用检索的函数名
--seacher 外部创建的searcher
function Cfg.Register(func_name,searcher)
	if Cfg.Config[func_name] ~= nil then
		Debuger.LogErrorSD("Cfg.Register Same FuncName ERROR!func_name=%s",tostring(func_name))
		return
	end
	Cfg.Config[func_name] = searcher
	searcher.error_tag = func_name
end

function Cfg.UnRegister(func_name)
	Cfg.Config[func_name] = nil
end


local meta_tab = {}
meta_tab.__index = function (mono_inst,key)
	local auto_sercher = Cfg.Config[key]
	if auto_sercher ~= nil then
		return auto_sercher.SearchFunc
	end
end

setmetatable(Cfg,meta_tab)

for k,v in pairs(Cfg.Config) do
	if v.error_tag == nil then
		v.error_tag = k
	end
end