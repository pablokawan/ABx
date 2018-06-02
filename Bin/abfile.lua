--------------------------------------------------------------------------------
-- General Settings ------------------------------------------------------------
--------------------------------------------------------------------------------

server_id = "0e9cf876-bb63-4926-a1a9-271bcf4a1c39"
location = "AT"

-- server_host = "stievie.mooo.com"
server_port = 8081

-- Used to calculate the load Byte/sec (100Mbit)
max_throughput = (100 * 1024 * 1024) / 8

server_key = "server.key"
server_cert = "server.crt"

-- Thread pool size
num_threads = 4
root_dir = "c:/Users/Stefan Ascher/Documents/Visual Studio 2015/Projects/ABx/Bin/file_root"

-- If true client must give Account UUID and password in HTTP header:
-- Auth: UUID:password
require_auth = false

-- Data server
data_host = "localhost"
data_port = 2770

require("config/abfile_admin")
