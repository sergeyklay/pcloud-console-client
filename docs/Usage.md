## Maintaining pCloud Console Client

### Run client

The usual way to run client is to type its name in a Unix shell prompt,
followed by pCloud account name as follows:

```sh
$ pcloudcc -u example@myemail.com -p
```

This will start up the process and give you control over it.

You can control its output (by default output is printed to the screen;
you can arrange to have output go into file `pcloud.log` by typing
`>pcloud.log` at the end of the command line), pause and restart the
process (by pressing <kbd>Ctrl</kbd> + <kbd>z</kbd> and typing `fg`), and
kill it (by pressing <kbd>Ctrl</kbd> + <kbd>c</kbd>).

Another way to terminate client is to send a signal eg
`killall -SIGTERM pcloudcc`. Signals with special meaning listened by
client are:

- `SIGINT`: issued if the user sends an interrupt signal
  (<kbd>Ctrl</kbd> + <kbd>c</kbd>)
- `SIGTERM`: sent by kill by default

Any other signals like `SIGQUIT` (<kbd>Ctrl</kbd> + <kbd>\\</kbd>),
`SIGKILL`, etc are handled by your OS. For more see `man 2 signal` and
`man 1 kill`.

To get a help run the client followed by `-h` option. This will ask client to
output short options description:

```
$ pcloudcc --help
           ________                __
    ____  / ____/ /___  __  ______/ /
   / __ \/ /   / / __ \/ / / / __  /
  / /_/ / /___/ / /_/ / /_/ / /_/ /
 / .___/\____/_/\____/\__,_/\__,_/
/_/

pCloud Console Client 3.0.0

Usage:
  pcloudcc [options] [--] [arguments]

Options:
      --dumpversion       Print the version of the client and don't do anything else
      --vernum            Print the version of the client as integer and quit
  -V, --version           Print client version information and quit
  -h, --help              Print this help message and quit
  -k, --commands-only     Daemon already started pass only commands
  -d, --daemonize         Daemonize the process
  -u, --username arg      pCloud account name
  -p, --password          Ask pCloud account password
  -c, --crypto            Ask crypto password
  -y, --passascrypto      Use user password as crypto password also
  -m, --mountpoint arg    Mount point where drive to be mounted
  -n, --newuser           Use if this is a new user to be registered
  -o, --commands          Parent stays alive and processes commands
  -s, --savepassword      Save password in database

```

Also, there are several commands that the running service can execute. Commands are passed using

```sh
$ pcloudcc -k
```

or  starting the daemon with `-o`.

Available commands are:
- `startcrypto <crypto pass>`: Start a crypto session using given password.
- `stopcrypto`: Stop a crypto session.
- `menu`, `m`: Print help menu.
- `quit`, `q`: Quit the current client (daemon stays alive).

#### Example usage scenario

1. Start the service manually
   ```sh
   $ pcloudcc -u example@myemail.com -p -s
   ```
2. Enter password and use `-s` switch to save the password.
3. Verify that file system starts and mounts normally. If you don't have
   existing user use `-n` switch to register new user:
   ```sh
   $ pcloudcc -u example@myemail.com -p -s -n
   ```
   Notice that a new user may take a while to mount. Please, be patient.
4. Start the daemon service:
   ```sh
   $ pcloudcc -u example@myemail.com -d
   ```
5. Verify the file system is mounted.
6. At that point you can test passing some commands.
   ```sh
   $ pcloudcc -u example@myemail.com -k
   ```
   Or starting the daemon with `-o`. Test unlocking and locking crypto if you
   have subscription for it.
7. Quit the client. Congratulations, your pCloud Console Client works properly.
   You can now add `pcloudcc -u example@myemail.com -d` command in you startup
   scripts  and thous mount the file system on startup.

**Note:** Stopping daemon will break pending background transfers.
Current version of `pcloudcc` doesn't support command to check if there are
pending transfers. Locally cached files are located under `~/.pcloud/Cache`
directory. When there is only one file `~/.pcloud/Cache/cached` (usually big sized)
this mean that all transfers are completed.

### Autostart on system boot

If you want client to start when your operating system boots, the
mechanism varies. It's probably easiet to just follow
[these instructions](https://www.howtogeek.com/228467/how-to-make-a-program-run-at-startup-on-any-computer/)
for setting up autostart. Alternatively, you can try following the instructions below.

#### macOS (launchd)

To use systemd integration the project should be built  with
`-DPCLOUD_WITH_LAUNCHD=ON` flag. `com.pcloud.pcloudcc@3.plist` is an "agent configuration file" for
macOS' launchd.  See the manual pages for `launchd`, `launchd.plist`, and `launchctl` for details.

You should place this file in: `~/Library/LaunchAgents`. The next required step is to set
`PCLOUD_USER_NAME` variable. You can put it for example in your `.bashrc/.zhrc` file as follows:

```sh
launchctl setenv PCLOUD_USER_NAME "example@myemail.com"
```

For more information about defining environment variables with `launchd`/`launchctl` see:

- https://apple.stackexchange.com/a/64917/267719
- https://stackoverflow.com/a/588442/1661465

After doing so, `pcloudcc` will start automatically  when you log in. Also, you can start and stop
it via `launchctl`.

If you have updated an _already running service_, for example:

```sh
$ cp pcloudcc /usr/local/bin/pcloudcc
```

you may need to restart it as follows:

```sh
$ launchctl kickstart -k gui/$(id -u)/com.pcloud.pcloudcc@3.plist
```

Remember to initialize you account **first** by running:

```sh
$ pcloudcc -u example@myemail.com -p -s
```

Note: you can always stop `pcloudcc` agent:

```sh
launchctl unload ~/Library/LaunchAgents/com.pcloud.pcloudcc@3.plist
```

edit plist, and re-run it again:

```sh
launchctl load ~/Library/LaunchAgents/com.pcloud.pcloudcc@3.plist
```

#### Linux (systemd)

To use systemd integration the project should be built  with
`-DPCLOUD_WITH_SYSTEMD=ON` flag.  You can autostart client on system boot as
follows:

As your own user enable it:

```sh
$ systemctl --user enable pcloudcc@<example@myemail.com>.service
```

Then, start it:

```sh
$ systemctl --user start pcloudcc@<example@myemail.com>.service
```

Remember to initialize you account **first** by running:

```sh
$ pcloudcc -u example@myemail.com -p -s
```

### EU region users

In order to connect to the EU region api server the environment variable
`PCLOUD_REGION_EU` has to be set, e.g. like this:

```sh
$ PCLOUD_REGION_EU=true pcloudcc -u example@myemail.com -p -s -n
```

Users with accounts in the EU region will get a login failed error
when connection to the US server and vice versa.
