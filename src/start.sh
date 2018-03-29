#!/bin/bash

killall upload
spawn-fcgi -a 127.0.0.1 -p 8082 -f upload
sudo /usr/local/nginx/sbin/nginx -s reload

