require "os"
require "io"
require "gmake/util"

local actions, targetOS, buildProfile, outDir = ...

local outFile = outDir .."/glfw.cache"

-- == what can be done to obtain libs ==
-- 'clone' implies 'build'
-- 'build' will run cmake and build the library if needed, according to build profile
-- 'systemlib' will try to find systemwide installed library and use it
-- 'provided' will try to use prebuilt library by paths provided here
-- if nothing of above, only existing library will be used
local lact = actions:split(",")

-- where to locate or clone the library
local ldir = "external/glfw"

local linc = ldir .."/include"
local lbdir = ldir .."/build/".. targetOS .."/".. buildProfile

local message_verbose = function(...)
	print("[glfw.lua] ".. table.concat({...}))
end

-- ================

shexecf('mkdir -p "%s"', outDir);

local function tryProvided()
	if not checkdir(ldir) then
		message_verbose("not found at [".. ldir .."]")
		return
	end
	if not checkdir(linc) then
		message_verbose("includes not found at [".. linc .."]")
		return
	end
	local bobj = checkdir(lbdir);
	if not bobj then
		message_verbose("not all or no build objects found at [".. lbdir .."]")
		return
	end
	-- TODO find static library if glfw's built so
	local ilib
	if targetOS ~= "Windows" then
		ilib = lbdir .. "/src/libglfw.so";
	else
		ilib = lbdir .. "/src/libglfw.dll.a";
	end
	if not checkfile(ilib) then
		message_verbose("library not found at [".. ilib .."]");
		return
	end
	return {
		glfw_DIR = ldir,
		glfw_INC = linc,
		glfw_BDIR = lbdir,
		glfw_LIB = ilib
	}
end

local function trySystemUnix()
	local res = {glfw_INC = "/usr/include"};
	if not checkdir(res.glfw_INC .. "/GLFW") then
		message_verbose("includes not found at [".. res.glfw_INC .."]")
		return
	end
	for ld in pairs {"/usr/lib", "/usr/local/lib", "/usr/lib64", "/usr/local/lib64"} do
		if checkfile(ld .."/libglfw.so") then
			res.glfw_LIB = ld .."/libglfw.so";
		end
	end
	if not res.glfw_LIB then
		return
	end
	return res
end

local function trySystemWindows()
	local res = {glfw_DIR = "C:/Program Files (x86)/glfw"};
	if not checkdir(res.glfw_DIR) then
		message_verbose("not found at [".. res.glfw_DIR .."]")
		return
	end
	-- todo
	return res
end

local function tryBuild()
	if not checkdir(ldir) then
		message_verbose("not found at [".. ldir .."]")
		return
	end
	if not checkdir(linc) then
		message_verbose("includes not found at [".. linc .."]")
		return
	end
	shexecf('mkdir -p "%s"', lbdir);
	local fmt = 'cmake -S "%s" -B "%s" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_EXAMPLES=OFF';
	if targetOS == "Windows" then fmt = fmt .. ' -G "MinGW Makefiles"' end
	shexecf(fmt, ldir, lbdir);
	shexecf('gmake -C "%s" -j4', lbdir);
	-- TODO find static library if glfw's built so
	local ilib
	if targetOS ~= "Windows" then
		ilib = lbdir .. "/src/libglfw.so";
	else
		ilib = lbdir .. "/src/libglfw.dll.a";
	end
	if not checkfile(ilib) then
		message_verbose("library not found at [".. ilib .."] after a build attempt");
		return false
	end
	return {
		glfw_DIR = ldir,
		glfw_INC = linc,
		glfw_BDIR = lbdir,
		glfw_LIB = ilib
	}
end

local function tryClone()
	if not checkdir(ldir) then
		message_verbose("not found at [".. ldir .."]. cloning...")
		shexecf('git clone https://github.com/glfw/glfw "%s"', ldir)
	end
	shexecf('git -C "%s" checkout dc46d3f8129712e42856c20e99a604a3b08ad581', ldir) -- 3.3.10
	return tryBuild()
end

local acttab = {
	["provided"] = tryProvided,
	["systemlib"] = trySystemUnix,
	["build"] = tryBuild,
	["clone"] = tryClone,
	["0"] = function() end
};

local success = main_action(lact, acttab, outFile, message_verbose);
if success then
	return
end

message_error("no one of methods worked");
