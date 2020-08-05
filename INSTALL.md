# Install and run

All binaries of the *Server* are located in `./bin`.

The binary of the *Client* is located in `./abclient/bin` and has the name
`fw`, `fw_d` (Debug), `fw.exe` (Windows) or `fw_d.exe` (Windows Debug).

## Server

1. Build the server, see [BUILD.md](BUILD.md). After that all server executables are located in the `bin` directory.
2. On Windows copy the *OpenSSL* and *PostgreSQL* client libraries to the `bin` directory.
    * `libeay32.dll` (OpenSSL)
    * `ssleay32.dll` (OpenSSL)
    * `libiconv-2.dll` (PostgreSQL)
    * `libintl-9.dll` (PostgreSQL)
    * `libpq.dll` (PostgreSQL)
    * `libwinpthread-1.dll` (PostgreSQL)
3. [Setup a PosgreSQL server](https://wiki.archlinux.org/index.php/PostgreSQL)
4. Create a `db_private.lua` file in `bin/config` directory to enter the DB configuration, example:
~~~lua
-- Postgres
db_host = "database.host"
db_port = 5432
db_user = "user"
db_pass = "password"
~~~
5. Create a database with the name `forgottenwars`.
~~~sh
$ createdb forgottenwars
~~~
6. Run `./bin/dbtool -a update` to create the database structure. Then run `./bin/dbtool -a updateskills` to update the Skills table. If the DB is empty, `dbtool` emits *one* error, you can ignore this.
7. Download server assets `data` from [OneDrive](https://1drv.ms/f/s!Ajy_fJI3BLBobOAOXZ47wtBgdBg) and put them into the `bin/data` directory. Don't overwrite files that are in the git repository.
8. Run the `./bin/keygen` tool to create the DH server keys.
9. Run `openssl req -x509 -newkey rsa:4096 -sha256 -days 3650 -nodes -keyout server.key -out server.crt` in the `bin` directory to create self-signed keys and certificate for the HTTPS file server.
10. Run `run.bat` or `./run` in the root directory, which runs all required services in the correct order.
11. You may want to create an account key to be able to create an account. To do so, run the `dbtool`:
~~~sh
$ ./bin/dbtool -a genacckey
~~~

To stop the servers in the correct order, you can use `./stop` in the root directory.

### Running

If you are behind a router and want that your friend can connect to your server running on your hardware, you must open the following ports:

1. TCP/UDP 2748: Login server
2. TCP 2749: Game server
3. TCP 8081: File server

This assumes you have the default configuration. You can change the ports in the respective configuration files:

1. `bin/ablogin.lua`: `login_port`
2. `bin/abserv.lua`: `base_port` or `game_port`
3. `bin/abfile.lua`: `file_port`

### Known issues

The Server should run on Windows and Linux.

## Client

1. Build it, see [BUILD.md](BUILD.md). The client executable is located in the `abclient/bin` directory and has the name `fw` or `fw.exe` on Windows.
2. Download client assets `client_data.zip` from [OneDrive](https://1drv.ms/f/s!Ajy_fJI3BLBobOAOXZ47wtBgdBg) and put them into the clients `abclient/bin` directory (don't replace existing files). There should also be the Urho3D asset files, this directory should look like:
~~~plain
bin
  - AbData
  - Autoload
  - CoreData
  - Data
  - SoundData
~~~
3. On Windows copy the required libraries to the `abclient/bin` directory:
    * `d3dcompiler_47.dll`
    * `libeay32.dll` (OpenSSL)
    * `ssleay32.dll` (OpenSSL)
4. Copy the configuration file for the Game client from `abclient/Setup/config.xml` to `abclient/bin/config.xml` and enter the Host and Port of the Login server:
~~~xml
<!-- Example when your run the server and client on the same machine -->
<config>
  <parameter name="LoginHost" type="string" value="localhost" />
  <parameter name="LoginPort" type="int" value="2748" />
</config>
~~~
5. Run `fw.exe` in `abclient/bin`. `$ cd abclient/bin && ./fw`.
6. Create an account using the UUID of your previously created account key. To find out what GUID was created, run the `dbtool`:
~~~sh
$ ./Bin/dbtool -a acckeys
~~~
This lists all account keys, pick one.

### Known issues

Nothing special so far.
