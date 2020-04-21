
mkdir /opt/mds/logs
echo "
[Unit]
Description=Skynet mds API Service
Wants=network-online.target
After=network.target network-online.target

[Service]
Type=simple
WorkingDirectory=/opt/mds
ExecStart=/opt/mds/mds -conf /opt/mds/conf.yml -log_level INFO -verbose 2 -log_dir /opt/mds/logs
Restart=on-failure
UMask=0066
StandardOutput=null

[Install]
WantedBy=multi-user.target
" > skynet-mds.service

mv skynet-mds.service /usr/lib/systemd/system/

systemctl daemon-reload
systemctl enable skynet-mds.service
systemctl start skynet-mds.service
