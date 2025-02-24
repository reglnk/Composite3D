require "io"
require "os"

function string.split(self, delim, limit)
	local arr = {}
	local iter = 1
	while iter <= #self do
		if limit then
			if limit == 0 then break
			else limit = limit - 1 end
		end
		local st, en = self:find(delim, iter)
		if st and en ~= 0 then
			local part = self:sub(iter, st - 1);
			if #part ~= 0 then table.insert(arr, part) end
			iter = en + 1
		else break end
	end
	local part = self:sub(iter, #self);
	if #part ~= 0 then table.insert(arr, part) end
	return arr
end

function verbose(msg)
	print(msg)
end

function toshell(cmd)
	return "sh -c \"".. cmd:gsub("\\", "\\\\"):gsub("\"", "\\\"") .."\""
end

function shexec(cmd)
	return os.execute(toshell(cmd))
end

function shexecf(cmd, ...)
	return os.execute(toshell(string.format(cmd, ...)))
end

-- check if directory is present and not empty
function checkdir(path)
	local ls_p = io.popen(toshell("/bin/ls -A \"".. path .."\" 2>/dev/null", "r"));
	if not ls_p then
		error("use human's unix shell u fumb duck")
	end
	local outp = ls_p:read("*all");
	local _, __, ec = ls_p:close();
	if ec ~= nil and ec ~= 0 then
		return false
	end
	outp = outp:gsub("[ \n\t\r\n\v\f]+", "");
	return #outp > 0
end

function checkfile(path)
	local fp = io.open(path, "r");
	if not fp then return false end
	fp:close()
	return path
end

-- error is intended to stop GNU make
function message_error(...)
	-- error("\x1b[0;33m".. table.concat({...}) .."\x1b[0m")
	error(table.concat({...}))
end

function main_action(lact, acttab, outFile, verbose)
	local success = false
	for i, v in ipairs(lact) do
		verbose("trying method '".. v .. "'");
		local res = acttab[v]()
		if res then
			local fp = io.open(outFile, "wb")
			if not fp then error("failed to open outFile") end
			for k, kv in pairs(res) do
				fp:write(k .. "=" .. kv .. "\n")
			end
			fp:flush()
			fp:close()
			os.execute("sync")
			success = true
			break
		end
		-- if the build failed, it's stupid to immediately try again
		if res == false then
			message_error("deciding not to continue");
			break
		end
	end
	return success
end
