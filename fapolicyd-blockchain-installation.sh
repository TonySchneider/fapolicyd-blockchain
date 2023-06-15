#!/bin/bash

# First, register your username:
subscription-manager register --username tonyschneider05

# Install essential packages:
dnf install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
dnf install -y rpm-libs
yum install -y rpm-devel

# Packages from specific repositories are required:

# For CentOS 8:
dnf --enablerepo=powertools install -y uthash-devel file-devel lmdb-devel

# For Enterprise RHEL 9:
# Download necessary packages:
wget https://rhel.pkgs.org/9/epel-x86_64/uthash-devel-2.3.0-1.el9.noarch.rpm.html
wget https://mirror.stream.centos.org/9-stream/CRB/x86_64/os/Packages/file-devel-5.39-12.el9.x86_64.rpm
wget https://mirror.stream.centos.org/9-stream/CRB/x86_64/os/Packages/lmdb-devel-0.9.29-3.el9.x86_64.rpm
wget https://dl.fedoraproject.org/pub/epel/9/Everything/x86_64/Packages/c/cjson-devel-1.7.14-5.el9.x86_64.rpm
wget https://dl.fedoraproject.org/pub/epel/9/Everything/x86_64/Packages/c/cjson-1.7.14-5.el9.x86_64.rpm

# Install the downloaded packages:
yum install -y uthash-devel-2.3.0-1.el9.noarch.rpm
yum install -y file-devel-5.39-12.el9.x86_64.rpm
yum install -y lmdb-devel-0.9.29-3.el9.x86_64.rpm
yum install -y cjson-devel-1.7.14-5.el9.x86_64.rpm
dnf install -y cjson-1.7.14-5.el9.x86_64.rpm

# Install other necessary packages:
yum install -y systemd-devel
yum install -y epel-release
dnf install -y libcap-ng libcap-ng-devel
dnf install -y libseccomp-devel
dnf install -y libcurl-devel
dnf install -y jansson-devel
yum install -y json-c-devel
yum install -y cJSON-devel

# Clone fapolicyd from its repository and navigate to the cloned directory:
git clone https://github.com/TonySchneider/fapolicyd-blockchain
cd fapolicyd

# Build the executable:
./autogen.sh
./configure --with-audit --disable-shared
make

# Run the compiled project in debug mode:
# ./src/fapolicyd —debug
