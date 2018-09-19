# README

## smb.conf

`smb.conf` 文件位于 `/etc/samba/smb.conf`，配置好的存储服务路径为 `/home/samba/anonymous_shares/`。

重启 `samba` 服务的命令：

```shell
sudo service smbd restart
```

权限设置如下：

```shell
haixiang@life-ubuntu-server  /home/samba  
total 4.0K
drwxr-xr-x 6 nobody nogroup 4.0K Jul  4 18:21 anonymous_shares
```

设置命令：

```shell
sudo chmod -R 0755 /home/samba/anonymous_shares
sudo chown -R nobody:nogroup /home/samba/anonymous_shares
```

---

## UDPCatch.sh

`UDPCatch.sh` 文件位于 `/root/`，使用 root 用户运行（因为抓包软件只能使用 root 用户运行）。
