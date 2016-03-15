#!/bin/bash

git add --all .
git commit -m 'update'
#git remote add origin https://github.com/talbert48/milliq.git
git remote add origin https://github.com/gabrielmagill/milliq.git
git remote -v
git push origin master

