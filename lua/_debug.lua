-- Set require("_debug") as the first instruction in your script.
-- For debugging lua script with ZeroBrane Studio: https://studio.zerobrane.com/
-- Adjust paths to your ZeroBrane Studio installation.
local ZBS = ";C:/Portable_App/ZeroBraneStudio"
package.path = package.path .. ZBS .. "/lualibs/mobdebug/?.lua"
package.path = package.path .. ZBS .. "/lualibs/?.lua"
package.cpath = package.cpath .. ZBS .. "/bin/clibs/?.dll"
require("mobdebug").start()