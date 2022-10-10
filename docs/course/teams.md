# IMU Exercise: In-Person Class Information

For students taking the in-person class, the information provided here is the necessary setup information. There are 14
teams numbered from 1 through 14. Each team will have one hardware platform and may connect as described below.

## WiFi Setup

Students should connect through our WiFi network when participating in the class. This allows access to our class
hardware platforms.

> Note: internet is not available on the class network

| WiFi Network | Password    |
|--------------|-------------|
| fprime-class | fpclass2020 |


## Hardware Connection Information

Students should connect to the hardware through SSH. The SSH username and password are as follows.

| Username | Password |
|----------|----------|
| odroid   | odroid   |

The hostname to connect to is team-specific. Each team should look up their hardware by device number in the following
table and use the provided hostname to ssh.

```bash
ssh odroid@<team hostname>

# e.g. Team 1
ssh odroid@odroid1.lan
```

### Team Hostname Table

| WiFi Number | Hostname     | MAC Address       | WiFi Adapter Name |
|-------------|--------------|-------------------|-------------------|
| 0 / 15      | odroid0.lan  | 6c:5a:b0:81:51:99 | wlx6c5ab0815199   |
| 1           | odroid1.lan  | 6c:5a:b0:81:46:a9 | wlx6c5ab08146a9   |
| 2           | odroid2.lan  | 6c:5a:b0:81:3f:b2 | wlx6c5ab0813fb2   |
| 3           | odroid3.lan  | 6c:5a:b0:81:43:dd | wlx6c5ab08143dd   |
| 4           | odroid4.lan  | 6c:5a:b0:81:51:a0 | wlx6c5ab08151a0   |
| 5           | odroid5.lan  | 6c:5a:b0:81:55:74 | wlx6c5ab0815574   |
| 6           | odroid6.lan  | 6c:5a:b0:81:4b:88 | wlx6c5ab0814b88   |
| 7           | odroid7.lan  | 6c:5a:b0:38:42:37 | wlx6c5ab0384237   |
| 8           | odroid8.lan  | 6c:5a:b0:81:51:d3 | wlx6c5ab08151d3   |
| 9           | odroid9.lan  | 6c:5a:b0:81:4b:28 | wlx6c5ab0814b28   |
| 10          | odroid10.lan | 6c:5a:b0:81:51:86 | wlx6c5ab0815186   |
| 11          | odroid11.lan | 6c:5a:b0:81:4c:0b | wlx6c5ab0814c0b   |
| 12          | odroid12.lan | 6c:5a:b0:81:51:e0 | wlx6c5ab08151e0   |
| 13          | odroid13.lan | 6c:5a:b0:81:3f:b7 | wlx6c5ab0813fb7   |
| 14          | odroid14.lan | 6c:5a:b0:81:57:a4 | wlx6c5ab08157a4   |
