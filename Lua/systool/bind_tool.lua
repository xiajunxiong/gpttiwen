BindTool = BindTool or {}

function BindTool.UnPack(param, count, i, ...)
	if i >= count then
		if i == count then
			return param[i], ...
		end
		return ...
	end
	return param[i], BindTool.UnPack(param, count, i + 1, ...)
end


function BindTool.Bind(func, ...)
	if type(func) ~= "function" then
		Debuger.LogErrorSD("BindTool.Bind error! type(func) ~= function")
		return function() end
	end

	local count = select('#', ...)
	local param = {...}

	if 0 == count then
		return function(...) return func(...) end
	elseif 1 == count then
		return function(...) return func(param[1], ...) end
	elseif 2 == count then
		return function(...) return func(param[1], param[2], ...) end
	end

	return function(...) return func(BindTool.UnPack(param, count, 1, ...)) end
end

function BindTool.Bind1(func, param1)
	if type(func) ~= "function" then
		Debuger.LogErrorSD("BindTool.Bind1 error! type(func) ~= function")
		return function() end
	end
	return function(...)
		return func(param1, ...)
	end
end

function BindTool.Bind2(func, param1, param2)
	if type(func) ~= "function" then
		Debuger.LogErrorSD("BindTool.Bind2 error! type(func) ~= function")
		return function() end
	end
	return function(...)
		return func(param1, param2, ...)
	end
end

function BindTool.Bind3(func, param1, param2, param3)
	if type(func) ~= "function" then
		Debuger.LogErrorSD("BindTool.Bind3 error! type(func) ~= function")
		return function() end
	end
	return function(...)
		return func(param1, param2, param3,...)
	end
end

function Call(func, ...)
	if func then
		return func(...)
	end
end

--@G New控件销毁
function Delete(self, key)
    if self[key] then
		if self[key].DeleteMe then
			self[key]:DeleteMe()
			self[key] = nil
		else
			for k,v in pairs(self[key]) do
				if v.DeleteMe then
					v:DeleteMe()
				end
			end
			self[key] = {}
		end
	end
end