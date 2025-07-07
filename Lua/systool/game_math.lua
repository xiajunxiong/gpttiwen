GameMath = GameMath or {}

local random_seed_num = -2147483648

function GameMath.SetRandomSeed()
    random_seed_num = random_seed_num + 1
    math.randomseed(tostring(os.time() + random_seed_num):reverse():sub(1, 7))
end

--无参数 :[0-1]的浮点
--只有参数n:[1-n]的整数
--m和n都有:[n-m]的整数
function GameMath.Ramdon(n, m)
    GameMath.SetRandomSeed()
    if n == nil then
    	return  math.random()
    elseif m == nil then
    	return  math.random(n)
   	else
	    return math.random(n, m)
   	end
end

--获取整数n-m中的随机浮点数,m > n
function GameMath.FloatRamdon(n, m)
    GameMath.SetRandomSeed()
    local len = m - n
    local rnd_per = math.random()
    return (n + len * rnd_per)
end

-- 求距离公式
-- @param need_sqrt 是否需要开方运算（不开方可以减少不必要的运算）
function GameMath.GetDistance(p1x, p1y, p2x, p2y, need_sqrt)
    local x_step = p1x - p2x
    local y_step = p1y - p2y
    local val = x_step * x_step + y_step * y_step
    if need_sqrt then
        return math.sqrt(val)
    end
    return val
end

-- 求p1离p2距离d的坐标
function GameMath.DistancePos(p1x, p1y, p2x, p2y, d)
   local dx = p1x - p2x
   local dy = p1y - p2y
   local dis = GameMath.GetDistance(p1x, p1y, p2x, p2y, true)
   local x = p2x + (d / dis) * dx
   local y = p2y + (d / dis) * dy
   return Vector2.New(x,y)
end

function GameMath.GetDistanceByPoint(v2s, v2t, need_sqrt)
    return GameMath.GetDistance(v2s.x, v2s.y, v2t.x, v2t.y, need_sqrt)
end

--@G求距离Distance减少参数pos满足（x,y,z）
function GameMath.Distance(pos1,pos2,need_sqrt)
    return GameMath.GetDistance(pos1.x,pos1.y,pos2.x,pos2.y,need_sqrt)
end

--@G求两点之间中心坐标
function GameMath.CenterPos(pos1,pos2)
    return Vector3.New((pos1.x + pos2.x)/2,(pos1.y + pos2.y)/2)
end

--@G转换坐标y与z的转换
function GameMath.ChangeTrans(pos)
    return Vector3.New(pos.x,pos.z,pos.y)
end

--@G转换为向量
function GameMath.Vector3(pos)
    return Vector3.New(pos.x,pos.y,pos.z)
end

--@G可适用于Vector3转Vector2
function GameMath.Vector2(pos)
    return Vector2.New(pos.x,pos.y)
end

--@G得到一个负数或正数 -1 或 1
function GameMath.Negative(value)
    return value < 0 and -1 or 1
end

--@G Equals 判断两个坐标是否一致
function GameMath.Equals(v1,v2)
    return v1.x == v2.x and v1.y == v2.y and ((v1.z and v2.z) and v1.z == v2.z or true)
end

--根据服务端下发坐标计算目标场景的客户端坐标
function GameMath.PosToClint(x,y,mapid)
    local re_p = Vector3.New()
    local map_cfg = Config_scenelist[mapid]
	if map_cfg ~= nil then
		re_p.x = x / 5 + map_cfg.edge_min_x
        re_p.z = y / 5 + map_cfg.edge_min_z
        re_p.y = map_cfg.edge_min_y
    end
	return re_p
end