/*
 *
 * mlcc - Machine Learning Container Creator
 * Copyright (C) 2017 Red Hat Inc
 *  - Bill Gray (bgray@redhat.com)
 *
 * mlcc is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; version 2.1.
 *
 * mlcc is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should find a copy of v2.1 of the GNU Lesser General Public License
 * somewhere on your Linux system; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 *
 * */ 


//
// To build for GUI:  yum install gnome-devel-docs gtk+ gtk3-devel gtk3-devel-docs gtk+-devel gtk-doc libcanberra-gtk3
//
// Compile with:  gcc -std=gnu99 -g -Wall -o mlcc mlcc.c
// For GUI with:  gcc -std=gnu99 -g -Wall -o mlcc mlcc.c -DGUI `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`
//


#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>


int gui = 0;
int debug = 0;
int quiet = 0;
int verbose = 0;
int num_cpus = 0;
int interactive = 0;
char *prog_name = NULL;
char *list_delimiters = " \t,";
char *output_file_name = NULL;
char *title_string = "Machine Learning Container Creator";
char *version_string = "20180606c";


void display_version_and_exit() {
    printf("%s version: %s: %s\n", prog_name, version_string, __DATE__);
    exit(EXIT_SUCCESS);
}


void display_usage_and_exit() {
    fprintf(stderr, "-d to turn on debugging output\n");
    fprintf(stderr, "-G to use the GUI selection interface\n");
    fprintf(stderr, "-h to see this usage help message\n");
    fprintf(stderr, "-i <pkg>,<pkg>... to generate a dockerfile with specified pkgs\n");
    fprintf(stderr, "-I to use the interactive selection interface\n");
    fprintf(stderr, "-l to see a display of all pkg names\n");
    fprintf(stderr, "-o <output file name> to set output file name\n");
    fprintf(stderr, "-q to turn on quiet mode\n");
    fprintf(stderr, "-t <text> to set the GUI window title string\n");
    fprintf(stderr, "-V to show the %s code version\n", prog_name);
    fprintf(stderr, "-v to turn on verbose mode\n");
    exit(EXIT_FAILURE);
}


#define BS(...) #__VA_ARGS__
#define XBS(foo) BS(foo)

#define ACCEL_LO_PO_NUM_START  100
#define ACCEL_HI_PO_NUM_LIMIT  200
#define PYTHON_LO_PO_NUM_START 200
#define PYTHON_HI_PO_NUM_LIMIT 300
#define MISC_LO_PO_NUM_START   500
#define MAX_PO_NUM_LIMIT     10000


struct pkg_data {
    int po_num;
    int include;
    char *label;
    char *desc;
    char *frag;
} pkgs[] = {


//
// NOTE: all the mutually exclusive sets of packages must have po_nums in the
// same century group and have (po_num < 500 == MISC_LO_PO_NUM_START).
// See clear_all_selections_in_po_num_group().
//



//
//
// BASE OS Choices (0 <= po_num < 100 == ACCEL_LO_PO_NUM_START)
//
//

{ 10, 0, "RHEL7.2", "RHEL7.2 OS Container", BS( FROM registry.access.redhat.com/rhel7.2 ) },
{ 10, 0, "RHEL7.3", "RHEL7.3 OS Container", BS( FROM registry.access.redhat.com/rhel7.3 ) },
{ 10, 0, "RHEL7.4", "RHEL7.4 OS Container", BS( FROM registry.access.redhat.com/rhel7.4 ) },
{ 10, 0, "RHEL7.5", "RHEL7.5 OS Container", BS( FROM registry.access.redhat.com/rhel7.5 ) },

{ 10, 0, "Centos7", "Centos7 OS Container", BS( FROM centos:7 ) },

{ 10, 0, "Fedora25", "Fedora25 OS Container", BS( FROM fedora:25 ) },   // GCC v6.2
{ 10, 0, "Fedora26", "Fedora26 OS Container", BS( FROM fedora:26 ) },   // GCC v7.1
{ 10, 0, "Fedora27", "Fedora27 OS Container", BS( FROM fedora:27 ) },   // GCC v7.2
{ 10, 0, "Fedora28", "Fedora28 OS Container", BS( FROM fedora:28 ) },   // GCC v8.0.1


// sed -i 's/#baseurl/baseurl/;s/gpgcheck=1/gpgcheck=0/' /etc/yum.repos.d/epel.repo;
// echo 'http_caching=packages' >> /etc/yum.conf;
{ 20, 0, "RHEL7.2", "RHEL7.2 Repos", BS(
COPY MLCC_Repos/RHEL7.2/ /etc/yum.repos.d/
\nRUN
yum -y -v -t install "https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm";
yum clean all; yum -y update; cd /var/cache && /bin/rm -rf dnf yum
) },

{ 20, 0, "RHEL7.3", "RHEL7.3 Repos", BS(
COPY MLCC_Repos/RHEL7.3/ /etc/yum.repos.d/
\nRUN
yum -y -v -t install "https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm";
yum clean all; yum -y update; cd /var/cache && /bin/rm -rf dnf yum
) },

{ 20, 0, "RHEL7.4", "RHEL7.4 Repos", BS(
COPY MLCC_Repos/RHEL7.4/ /etc/yum.repos.d/
\nRUN
yum -y -v -t install "https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm";
yum clean all; yum -y update; cd /var/cache && /bin/rm -rf dnf yum
) },

{ 20, 0, "RHEL7.5", "RHEL7.5 Repos", BS(
COPY MLCC_Repos/RHEL7.5/ /etc/yum.repos.d/
\nRUN
yum -y -v -t install "https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm";
yum clean all; yum -y update; cd /var/cache && /bin/rm -rf dnf yum
) },

{ 20, 0, "Centos7", "Centos7 Repos", BS( RUN
yum -y -v -t --enablerepo=extras install epel-release;
yum clean all; yum -y update; cd /var/cache && /bin/rm -rf dnf yum
) },

{ 20, 0, "Fedora25", "Fedora25 Repos", BS( RUN
yum clean all; yum -y update; cd /var/cache && /bin/rm -rf dnf yum
) },

{ 20, 0, "Fedora26", "Fedora26 Repos", BS( RUN
yum clean all; yum -y update; cd /var/cache && /bin/rm -rf dnf yum
) },

{ 20, 0, "Fedora27", "Fedora27 Repos", BS( RUN
yum clean all; yum -y update; cd /var/cache && /bin/rm -rf dnf yum
) },

{ 20, 0, "Fedora28", "Fedora28 Repos", BS( RUN
yum clean all; yum -y update; cd /var/cache && /bin/rm -rf dnf yum
) },

// FIXME: specific version of cmake
{ 50, 0, "OS-Utils", "OS Utils", BS( RUN
echo -e '\
#!/bin/bash \n\
set -vx \n\
for (( TRY=1; TRY<=11; TRY++ )); do \n\
    yum -y -v install $@ \n\
    result=$? \n\
    for PKG in $@ ; do \n\
        yum list installed | grep "^$PKG" \n\
        (( result += $? )) \n\
    done \n\
    if (( $result == 0 )); then \n\
        /bin/rm -rf /var/cache/yum \n\
        /bin/rm -rf /var/cache/dnf \n\
        exit 0 \n\
    fi \n\
    sleep 10 \n\
done \n\
exit 1 \n'
>> /tmp/yum_install.sh;
chmod +x /tmp/yum_install.sh;
/tmp/yum_install.sh bzip2 findutils gcc gcc-c++ gcc-gfortran git gzip make patch pciutils unzip vim-enhanced wget xz zip;
cd /tmp && wget "https://cmake.org/files/v3.11/cmake-3.11.3.tar.gz" && tar -xf cmake*.gz;
cd /tmp/cmake-3.11.3 && ./bootstrap && make -j`getconf _NPROCESSORS_ONLN` && make install;
cd /tmp && /bin/rm -rf /tmp/cmake*;
cmake --version
) },


//
//
// Accelerator Choices (ACCEL_LO_PO_NUM_START == 100 <= po_num < 200 == ACCEL_HI_PO_NUM_LIMIT)
//
//

{ 150, 0, "CPU", "CPU Only", BS( RUN
echo -e '\
\n\
export PATH=/usr/local/bin:/usr/bin:${PATH} \n\
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH \n\
\n'
>> ~/.bashrc;
\nENV
PATH="/usr/local/bin:/usr/bin:${PATH:+:${PATH}}"
LD_LIBRARY_PATH="/usr/local/lib:${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
) },


// FIXME: specific CUDA, cuDNN and NCCL versions....
//
// There are some command line downloadable Nvidia package URLs like:
// "http://developer.download.nvidia.com/compute/redist/cudnn/v6.0/cudnn-8.0-linux-x64-v6.0.tgz"
// "http://developer.download.nvidia.com/compute/redist/cudnn/v7.0.3/cudnn-9.0-linux-x64-v7.tgz"
//
// V2 NCCL must be downloaded from "http://developer.nvidia.com/nccl"
// Obsolete way to build NCCL v1 from git repo:
// cd /tmp && git clone --depth 1 "https://github.com/NVIDIA/nccl.git";
// cd /tmp/nccl && make -j`getconf _NPROCESSORS_ONLN` install;
// /bin/rm -rf /tmp/nccl*;
//
{ 150, 0, "CUDA8.0", "NVIDIA CUDA v8", BS(
COPY MLCC_Repos/NVIDIA_PKGS/cudnn-8.0-linux-x64*.tgz MLCC_Repos/NVIDIA_PKGS/nccl*cuda8.0_x86_64.txz /tmp/
\nRUN
echo -e '\
exec > /etc/yum.repos.d/cuda.repo \n\
echo [cuda] \n\
echo name=cuda \n\
if [ -f /etc/fedora-release ]; then \n\
echo baseurl="http://developer.download.nvidia.com/compute/cuda/repos/fedora23/x86_64" \n\
else \n\
echo baseurl="http://developer.download.nvidia.com/compute/cuda/repos/rhel7/x86_64" \n\
fi \n\
echo enabled=1 \n\
echo gpgcheck=0 \n'
>> /tmp/Make_CUDA_Repo.sh;
sh /tmp/Make_CUDA_Repo.sh;

yum -y install cuda-8-0; cd /var/cache && /bin/rm -rf dnf yum;

cd /tmp && tar -xvf cudnn*.tgz;
cd /tmp/cuda && mv include/cudnn.h /usr/local/cuda/include && mv lib64/lib* /usr/local/cuda/lib64;
cd /tmp && /bin/rm -rf /tmp/cud*;

cd /tmp && tar -xvf nccl*.txz;
cd /tmp/nccl*x86_64 &&
mv NCCL-SLA.txt /usr/local/cuda &&
mv include/nccl.h /usr/local/cuda/include &&
mv lib/lib* /usr/local/cuda/lib64;
mkdir -p /usr/local/nccl && cd /usr/local/nccl &&
ln -s /usr/local/cuda/NCCL-SLA.txt NCCL-SLA.txt &&
ln -s /usr/local/cuda/include include &&
ln -s /usr/local/cuda/lib64 lib;
cd /tmp && /bin/rm -rf /tmp/nccl*;

ldconfig;

echo -e '\
\n\
export CUDA_HOME=/usr/local/cuda \n\
export CUDA_PATH=/usr/local/cuda \n\
export PATH=/usr/local/cuda/bin:/usr/local/bin:/usr/bin:${PATH} \n\
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:/usr/local/lib:$LD_LIBRARY_PATH \n\
\n'
>> ~/.bashrc;

\nENV
CUDA_VERSION="8.0"
CUDA_HOME="/usr/local/cuda"
CUDA_PATH="/usr/local/cuda"
CUDNN_VERSION="7.1"
NCCL_VERSION="2.2.12"
PATH="/usr/local/cuda/bin:/usr/local/bin:/usr/bin:${PATH:+:${PATH}}"
LD_LIBRARY_PATH="/usr/local/cuda/lib64:/usr/local/lib:${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"

) },

{ 150, 0, "CUDA9.0", "NVIDIA CUDA v9", BS(
COPY MLCC_Repos/NVIDIA_PKGS/cudnn-9.0-linux-x64*.tgz MLCC_Repos/NVIDIA_PKGS/nccl*cuda9.0_x86_64.txz /tmp/
\nRUN
echo -e '\
exec > /etc/yum.repos.d/cuda.repo \n\
echo [cuda] \n\
echo name=cuda \n\
if [ -f /etc/fedora-release ]; then \n\
echo baseurl="http://developer.download.nvidia.com/compute/cuda/repos/fedora25/x86_64" \n\
else \n\
echo baseurl="http://developer.download.nvidia.com/compute/cuda/repos/rhel7/x86_64" \n\
fi \n\
echo enabled=1 \n\
echo gpgcheck=0 \n'
>> /tmp/Make_CUDA_Repo.sh;
sh /tmp/Make_CUDA_Repo.sh;

yum -y install cuda-9-0; cd /var/cache && /bin/rm -rf dnf yum;

cd /tmp && tar -xvf cudnn*.tgz;
cd /tmp/cuda && mv include/cudnn.h /usr/local/cuda/include && mv lib64/lib* /usr/local/cuda/lib64;
cd /tmp && /bin/rm -rf /tmp/cud*;

cd /tmp && tar -xvf nccl*.txz;
cd /tmp/nccl*x86_64 &&
mv NCCL-SLA.txt /usr/local/cuda &&
mv include/nccl.h /usr/local/cuda/include &&
mv lib/lib* /usr/local/cuda/lib64;
mkdir -p /usr/local/nccl && cd /usr/local/nccl &&
ln -s /usr/local/cuda/NCCL-SLA.txt NCCL-SLA.txt &&
ln -s /usr/local/cuda/include include &&
ln -s /usr/local/cuda/lib64 lib;
cd /tmp && /bin/rm -rf /tmp/nccl*;

ldconfig;

echo -e '\
\n\
export CUDA_HOME=/usr/local/cuda \n\
export CUDA_PATH=/usr/local/cuda \n\
export PATH=/usr/local/cuda/bin:/usr/local/bin:/usr/bin:${PATH} \n\
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:/usr/local/lib:$LD_LIBRARY_PATH \n\
\n'
>> ~/.bashrc;

\nENV
CUDA_VERSION="9.0"
CUDA_HOME="/usr/local/cuda"
CUDA_PATH="/usr/local/cuda"
CUDNN_VERSION="7.1"
NCCL_VERSION="2.2.12"
PATH="/usr/local/cuda/bin:/usr/local/bin:/usr/bin:${PATH:+:${PATH}}"
LD_LIBRARY_PATH="/usr/local/cuda/lib64:/usr/local/lib:${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"

) },

{ 150, 0, "CUDA9.1", "NVIDIA CUDA v9.1", BS(
COPY MLCC_Repos/NVIDIA_PKGS/cudnn-9.1-linux-x64*.tgz MLCC_Repos/NVIDIA_PKGS/nccl*cuda9.1_x86_64.txz /tmp/
\nRUN
echo -e '\
exec > /etc/yum.repos.d/cuda.repo \n\
echo [cuda] \n\
echo name=cuda \n\
if [ -f /etc/fedora-release ]; then \n\
echo baseurl="http://developer.download.nvidia.com/compute/cuda/repos/fedora25/x86_64" \n\
else \n\
echo baseurl="http://developer.download.nvidia.com/compute/cuda/repos/rhel7/x86_64" \n\
fi \n\
echo enabled=1 \n\
echo gpgcheck=0 \n'
>> /tmp/Make_CUDA_Repo.sh;
sh /tmp/Make_CUDA_Repo.sh;

yum -y install cuda-9-1; cd /var/cache && /bin/rm -rf dnf yum;

cd /tmp && tar -xvf cudnn*.tgz;
cd /tmp/cuda && mv include/cudnn.h /usr/local/cuda/include && mv lib64/lib* /usr/local/cuda/lib64;
cd /tmp && /bin/rm -rf /tmp/cud*;

cd /tmp && tar -xvf nccl*.txz;
cd /tmp/nccl*x86_64 &&
mv NCCL-SLA.txt /usr/local/cuda &&
mv include/nccl.h /usr/local/cuda/include &&
mv lib/lib* /usr/local/cuda/lib64;
mkdir -p /usr/local/nccl && cd /usr/local/nccl &&
ln -s /usr/local/cuda/NCCL-SLA.txt NCCL-SLA.txt &&
ln -s /usr/local/cuda/include include &&
ln -s /usr/local/cuda/lib64 lib;
cd /tmp && /bin/rm -rf /tmp/nccl*;

ldconfig;

echo -e '\
\n\
export CUDA_HOME=/usr/local/cuda \n\
export CUDA_PATH=/usr/local/cuda \n\
export PATH=/usr/local/cuda/bin:/usr/local/bin:/usr/bin:${PATH} \n\
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:/usr/local/lib:$LD_LIBRARY_PATH \n\
\n'
>> ~/.bashrc;

\nENV
CUDA_VERSION="9.1"
CUDA_HOME="/usr/local/cuda"
CUDA_PATH="/usr/local/cuda"
CUDNN_VERSION="7.1"
NCCL_VERSION="2.1.15"
PATH="/usr/local/cuda/bin:/usr/local/bin:/usr/bin:${PATH:+:${PATH}}"
LD_LIBRARY_PATH="/usr/local/cuda/lib64:/usr/local/lib:${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"

) },

{ 150, 0, "CUDA9.2", "NVIDIA CUDA v9.2", BS(
COPY MLCC_Repos/NVIDIA_PKGS/cudnn-9.2-linux-x64*.tgz MLCC_Repos/NVIDIA_PKGS/nccl*cuda9.2_x86_64.txz /tmp/
\nRUN
echo -e '\
exec > /etc/yum.repos.d/cuda.repo \n\
echo [cuda] \n\
echo name=cuda \n\
if [ -f /etc/fedora-release ]; then \n\
echo baseurl="http://developer.download.nvidia.com/compute/cuda/repos/fedora27/x86_64" \n\
else \n\
echo baseurl="http://developer.download.nvidia.com/compute/cuda/repos/rhel7/x86_64" \n\
fi \n\
echo enabled=1 \n\
echo gpgcheck=0 \n'
>> /tmp/Make_CUDA_Repo.sh;
sh /tmp/Make_CUDA_Repo.sh;

yum -y install cuda-9-2; cd /var/cache && /bin/rm -rf dnf yum;

cd /tmp && tar -xvf cudnn*.tgz;
cd /tmp/cuda && mv include/cudnn.h /usr/local/cuda/include && mv lib64/lib* /usr/local/cuda/lib64;
cd /tmp && /bin/rm -rf /tmp/cud*;

cd /tmp && tar -xvf nccl*.txz;
cd /tmp/nccl*x86_64 &&
mv NCCL-SLA.txt /usr/local/cuda &&
mv include/nccl.h /usr/local/cuda/include &&
mv lib/lib* /usr/local/cuda/lib64;
mkdir -p /usr/local/nccl && cd /usr/local/nccl &&
ln -s /usr/local/cuda/NCCL-SLA.txt NCCL-SLA.txt &&
ln -s /usr/local/cuda/include include &&
ln -s /usr/local/cuda/lib64 lib;
cd /tmp && /bin/rm -rf /tmp/nccl*;

ldconfig;

echo -e '\
\n\
export CUDA_HOME=/usr/local/cuda \n\
export CUDA_PATH=/usr/local/cuda \n\
export PATH=/usr/local/cuda/bin:/usr/local/bin:/usr/bin:${PATH} \n\
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:/usr/local/lib:$LD_LIBRARY_PATH \n\
\n'
>> ~/.bashrc;

\nENV
CUDA_VERSION="9.2"
CUDA_HOME="/usr/local/cuda"
CUDA_PATH="/usr/local/cuda"
CUDNN_VERSION="7.1"
NCCL_VERSION="2.2.12"
PATH="/usr/local/cuda/bin:/usr/local/bin:/usr/bin:${PATH:+:${PATH}}"
LD_LIBRARY_PATH="/usr/local/cuda/lib64:/usr/local/lib:${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"

) },


#if 0
{ 150, 0, "ROCm", "AMD ROCm", BS( # Sorry! AMD ROCm is NYI. ) },
#endif



//
//
// Python Choices (PYTHON_LO_PO_NUM_START == 200 <= po_num < 300 == PYTHON_HI_PO_NUM_LIMIT)
//
// FIXME: Specific versions
// For building, see: "https://danieleriksson.net/2017/02/08/how-to-install-latest-python-on-centos"
//

{ 250, 0, "Python2", "Python2", BS( RUN
cd /usr/local && /bin/rm -rf lib64 && ln -s lib lib64;
if [ -x /usr/bin/python2 ]; then
    yum -y install python2-pip || yum -y install python-pip;
    yum -y install python2-devel || yum -y install python-devel;
    yum -y install python2-setuptools || yum -y install python-setuptools;
else
    /tmp/yum_install.sh bzip2-devel libdb4-devel expat-devel gdbm-devel libpcap-devel
    ncurses-devel openssl-devel readline-devel sqlite-devel tk-devel xz-devel zlib-devel;
    cd /tmp && wget "https://www.python.org/ftp/python/2.7.15/Python-2.7.15.tar.xz" && tar -xf Python*.xz;
    cd /tmp/Python-2.7.15 &&
    ./configure --enable-optimizations --enable-shared --enable-unicode=ucs4 --prefix=/usr/local --with-ensurepip=install LDFLAGS="-Wl,-rpath /usr/local/lib" &&
    make -j`getconf _NPROCESSORS_ONLN` && make install;
    cp -a /usr/local/include/python* /usr/include/;
fi;
cd /var/cache && /bin/rm -rf dnf yum;
pip install --upgrade pip setuptools;
) },

{ 250, 0, "Python3", "Python3", BS( RUN
cd /usr/local && /bin/rm -rf lib64 && ln -s lib lib64;
if [ -x /usr/bin/python3 ]; then
    /tmp/yum_install.sh python3-pip python3-devel python3-setuptools;
    cd /usr/bin && ln -s python3 python && ln -s pip3 pip;
else
    /tmp/yum_install.sh bzip2-devel libdb4-devel expat-devel gdbm-devel libpcap-devel
    ncurses-devel openssl-devel readline-devel sqlite-devel tk-devel xz-devel zlib-devel;
    cd /tmp && wget "https://www.python.org/ftp/python/3.6.5/Python-3.6.5.tar.xz" && tar -xf Python*.xz;
    cd /tmp/Python-3.6.5 &&
    ./configure --enable-optimizations --enable-shared --prefix=/usr/local --with-ensurepip=install LDFLAGS="-Wl,-rpath /usr/local/lib" &&
    make -j`getconf _NPROCESSORS_ONLN` && make install;
    cp -a /usr/local/include/python* /usr/include/;
    cd /usr/local/bin && ln -s python3.6 python && ln -s pip3.6 pip;
fi;
cd /var/cache && /bin/rm -rf dnf yum;
pip install --upgrade pip setuptools;
) },



//
//
// Miscellaneous Packages (MISC_LO_PO_NUM_START == 500 <= po_num)
//
//

//
// See Nvidia nvcc gcc supported versions: "https://docs.nvidia.com/cuda/cuda-installation-guide-linux/index.html"
//


#if INCLUDE_GCC_5_3

{ 600, 0, "GCC-5.3", "Old GCC 5.3", BS( RUN 
mkdir -p /tmp/gcc_tmp_build_dir;
cd /tmp/gcc_tmp_build_dir;
wget -q "https://ftp.gnu.org/gnu/gcc/gcc-5.3.0/gcc-5.3.0.tar.gz";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/gmp-6.1.0.tar.bz2";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/mpfr-3.1.4.tar.bz2";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/mpc-1.0.3.tar.gz";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/isl-0.16.1.tar.bz2";
tar -xf gcc-5.3.0.tar.gz;
tar -xf gmp-6.1.0.tar.bz2;
tar -xf mpfr-3.1.4.tar.bz2;
tar -xf mpc-1.0.3.tar.gz ;
tar -xf isl-0.16.1.tar.bz2;
ln -s /tmp/gcc_tmp_build_dir/gmp-6.1.0 gcc-5.3.0/gmp;
ln -s /tmp/gcc_tmp_build_dir/mpfr-3.1.4 gcc-5.3.0/mpfr;
ln -s /tmp/gcc_tmp_build_dir/mpc-1.0.3 gcc-5.3.0/mpc;
ln -s /tmp/gcc_tmp_build_dir/isl-0.16.1 gcc-5.3.0/isl;
wget -qO gcc_patch.txt 'https://gcc.gnu.org/git/?p=gcc.git;a=patch;h=ec1cc0263f156f70693a62cf17b254a0029f4852';
patch -p1 -d gcc-5.3.0 < gcc_patch.txt;
gcc-5.3.0/configure --disable-multilib --enable-languages=c,c++,fortran --prefix=/usr/local;
make -j`getconf _NPROCESSORS_ONLN`;
make install-strip;
cd /tmp && /bin/rm -rf /tmp/gcc_tmp_build_dir
\nENV
CC="/usr/local/bin/gcc"
CXX="/usr/local/bin/g++"
) },

#endif

{ 600, 0, "GCC-5.5", "Old GCC 5.5", BS( RUN 
mkdir -p /tmp/gcc_tmp_build_dir;
cd /tmp/gcc_tmp_build_dir;
wget -q "https://ftp.gnu.org/gnu/gcc/gcc-5.5.0/gcc-5.5.0.tar.gz";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/gmp-6.1.0.tar.bz2";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/mpfr-3.1.4.tar.bz2";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/mpc-1.0.3.tar.gz";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/isl-0.18.tar.bz2";
tar -xf gcc-5.5.0.tar.gz;
tar -xf gmp-6.1.0.tar.bz2;
tar -xf mpfr-3.1.4.tar.bz2;
tar -xf mpc-1.0.3.tar.gz;
tar -xf isl-0.18.tar.bz2;
ln -s /tmp/gcc_tmp_build_dir/gmp-6.1.0 gcc-5.5.0/gmp;
ln -s /tmp/gcc_tmp_build_dir/mpfr-3.1.4 gcc-5.5.0/mpfr;
ln -s /tmp/gcc_tmp_build_dir/mpc-1.0.3 gcc-5.5.0/mpc;
ln -s /tmp/gcc_tmp_build_dir/isl-0.18 gcc-5.5.0/isl;
gcc-5.5.0/configure --disable-multilib --enable-languages=c,c++,fortran --prefix=/usr/local;
make -j`getconf _NPROCESSORS_ONLN`;
make install-strip;
cd /tmp && /bin/rm -rf /tmp/gcc_tmp_build_dir
\nENV
CC="/usr/local/bin/gcc"
CXX="/usr/local/bin/g++"
) },

#if INCLUDE_GCC_6_3

{ 600, 0, "GCC-6.3", "Old GCC 6.3", BS( RUN 
mkdir -p /tmp/gcc_tmp_build_dir;
cd /tmp/gcc_tmp_build_dir;
wget -q "https://ftp.gnu.org/gnu/gcc/gcc-6.3.0/gcc-6.3.0.tar.bz2";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/gmp-6.1.0.tar.bz2";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/mpfr-3.1.4.tar.bz2";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/mpc-1.0.3.tar.gz";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/isl-0.18.tar.bz2";
tar -xf gcc-6.3.0.tar.bz2;
tar -xf gmp-6.1.0.tar.bz2;
tar -xf mpfr-3.1.4.tar.bz2;
tar -xf mpc-1.0.3.tar.gz;
tar -xf isl-0.18.tar.bz2;
ln -s /tmp/gcc_tmp_build_dir/gmp-6.1.0 gcc-6.3.0/gmp;
ln -s /tmp/gcc_tmp_build_dir/mpfr-3.1.4 gcc-6.3.0/mpfr;
ln -s /tmp/gcc_tmp_build_dir/mpc-1.0.3 gcc-6.3.0/mpc;
ln -s /tmp/gcc_tmp_build_dir/isl-0.18 gcc-6.3.0/isl;
gcc-6.3.0/configure --disable-multilib --enable-languages=c,c++,fortran --prefix=/usr/local;
make -j`getconf _NPROCESSORS_ONLN`;
make install-strip;
cd /tmp && /bin/rm -rf /tmp/gcc_tmp_build_dir
\nENV
CC="/usr/local/bin/gcc"
CXX="/usr/local/bin/g++"
) },

#endif
#if INCLUDE_GCC_6_4

{ 600, 0, "GCC-6.4", "Old GCC 6.4", BS( RUN 
mkdir -p /tmp/gcc_tmp_build_dir;
cd /tmp/gcc_tmp_build_dir;
wget -q "https://ftp.gnu.org/gnu/gcc/gcc-6.4.0/gcc-6.4.0.tar.xz";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/gmp-6.1.0.tar.bz2";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/mpfr-3.1.4.tar.bz2";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/mpc-1.0.3.tar.gz";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/isl-0.18.tar.bz2";
tar -xf gcc-6.4.0.tar.xz;
tar -xf gmp-6.1.0.tar.bz2;
tar -xf mpfr-3.1.4.tar.bz2;
tar -xf mpc-1.0.3.tar.gz;
tar -xf isl-0.18.tar.bz2;
ln -s /tmp/gcc_tmp_build_dir/gmp-6.1.0 gcc-6.4.0/gmp;
ln -s /tmp/gcc_tmp_build_dir/mpfr-3.1.4 gcc-6.4.0/mpfr;
ln -s /tmp/gcc_tmp_build_dir/mpc-1.0.3 gcc-6.4.0/mpc;
ln -s /tmp/gcc_tmp_build_dir/isl-0.18 gcc-6.4.0/isl;
gcc-6.4.0/configure --disable-multilib --enable-languages=c,c++,fortran --prefix=/usr/local;
make -j`getconf _NPROCESSORS_ONLN`;
make install-strip;
cd /tmp && /bin/rm -rf /tmp/gcc_tmp_build_dir
\nENV
CC="/usr/local/bin/gcc"
CXX="/usr/local/bin/g++"
) },

#endif

{ 600, 0, "GCC-7.3", "Old GCC 7.3", BS( RUN 
mkdir -p /tmp/gcc_tmp_build_dir;
cd /tmp/gcc_tmp_build_dir;
wget -q "https://ftp.gnu.org/gnu/gcc/gcc-7.3.0/gcc-7.3.0.tar.xz";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/gmp-6.1.0.tar.bz2";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/mpfr-3.1.4.tar.bz2";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/mpc-1.0.3.tar.gz";
wget -q "https://gcc.gnu.org/pub/gcc/infrastructure/isl-0.18.tar.bz2";
tar -xf gcc-7.3.0.tar.xz;
tar -xf gmp-6.1.0.tar.bz2;
tar -xf mpfr-3.1.4.tar.bz2;
tar -xf mpc-1.0.3.tar.gz;
tar -xf isl-0.18.tar.bz2;
ln -s /tmp/gcc_tmp_build_dir/gmp-6.1.0 gcc-7.3.0/gmp;
ln -s /tmp/gcc_tmp_build_dir/mpfr-3.1.4 gcc-7.3.0/mpfr;
ln -s /tmp/gcc_tmp_build_dir/mpc-1.0.3 gcc-7.3.0/mpc;
ln -s /tmp/gcc_tmp_build_dir/isl-0.18 gcc-7.3.0/isl;
gcc-7.3.0/configure --disable-multilib --enable-languages=c,c++,fortran --prefix=/usr/local;
make -j`getconf _NPROCESSORS_ONLN`;
make install-strip;
cd /tmp && /bin/rm -rf /tmp/gcc_tmp_build_dir
\nENV
CC="/usr/local/bin/gcc"
CXX="/usr/local/bin/g++"
) },


{ 600, 0, "MKL", "MKL", BS( RUN
echo -e '\
[intel-mkl] \n\
name=intel-mkl \n\
baseurl="https://yum.repos.intel.com/mkl" \n\
enabled=1 \n\
gpgcheck=0 \n'
>> /etc/yum.repos.d/intel-mkl.repo;
yum -y install intel-mkl-64bit-2018.2-046; cd /var/cache && /bin/rm -rf dnf yum
) },

// See: "https://github.com/intel/mkl-dnn"
// See: "https://software.intel.com/en-us/articles/intel-mkl-dnn-part-1-library-overview-and-installation"
{ 600, 0, "MKL-DNN", "MKL-DNN", BS( RUN
cd /tmp && git clone "https://github.com/intel/mkl-dnn.git";
cd /tmp/mkl-dnn/scripts && ./prepare_mkl.sh;
mkdir -p /tmp/mkl-dnn/build &&
cd /tmp/mkl-dnn/build &&
cmake .. &&
make -j`getconf _NPROCESSORS_ONLN` &&
make test &&
make install &&
ldconfig;
) },

{ 600, 0, "OpenBLAS", "OpenBLAS", BS( RUN yum -y install openblas; cd /var/cache && /bin/rm -rf dnf yum ) },
{ 600, 0, "Atlas", "Atlas", BS( RUN yum -y install atlas; cd /var/cache && /bin/rm -rf dnf yum ) },
//
// FIXME: should build numpy with MKL, when MKL present?
{ 600, 0, "Numpy", "Numpy", BS( RUN
pip install numpy
\nRUN python -c 'import numpy'
) },


#if 0

// { 600, 0, "TensorFlow", "TensorFlow", BS( RUN pip install tensorflow-gpu==1.6.0 ) },
// { 600, 0, "TensorFlow", "TensorFlow", BS( RUN pip install tensorflow-gpu==1.7.0 ) },
// { 600, 0, "TensorFlow", "TensorFlow", BS( RUN pip install tensorflow-gpu==1.8.0 ) },
// { 600, 0, "TensorFlow", "TensorFlow", BS( RUN pip install tensorflow-gpu ) },

// for built python environment
// PYTHON_BIN_PATH="/usr/local/bin/python"
// PYTHON_LIB_PATH=$(echo /usr/local/lib/python*)
// PYTHON_INCLUDE_PATH=$(echo /usr/local/include/python*)
// for rpm python environment
// PYTHON_BIN_PATH=$(set `whereis python${PYTHON_VERSION}`; echo $2)
// PYTHON_LIB_PATH="$($PYTHON_BIN_PATH -c 'import site; print(site.getsitepackages()[0])')" 
// PYTHON_INCLUDE_PATH="$(ls -d /usr/include/python${PYTHON_VERSION}* | tr '\n' ' ' | cut -d' ' -f1)"

#endif

// FIXME: Need to tune environment variables and options for microarchitecture and desire for MKL, etc.
// -c opt --copt=-mavx --copt=-mavx2 --copt=-mfma --copt=-mfpmath=both --copt=-msse4.1 --copt=-msse4.2 --config=cuda --config=mkl 

// /tmp/yum_install.sh golang java-1.8.0-openjdk java-1.8.0-openjdk-devel java-1.8.0-openjdk-headless;
// echo "#define _BITS_FLOATN_H" >> /usr/local/cuda/include/host_defines.h

{ 600, 0, "TensorFlow", "TensorFlow", BS( RUN 
/tmp/yum_install.sh java-1.8.0-openjdk java-1.8.0-openjdk-devel java-1.8.0-openjdk-headless;
pip install --upgrade pip enum34 mock wheel;
echo -e '\
set -vx \n\
echo $PATH \n\
echo $LD_LIBRARY_PATH \n\
if [ -x /usr/local/bin/python ]; then \n\
    export PYTHON_BIN_PATH="/usr/local/bin/python" \n\
    export PYTHON_LIB_PATH="$(echo /usr/local/lib/python*)" \n\
    export PYTHON_INCLUDE_PATH="$(echo /usr/local/include/python*)" \n\
else \n\
    export PYTHON_BIN_PATH="/usr/bin/python" \n\
    PYTHON_VERSION_OUTPUT=`python --version 2>&1` \n\
    export PYTHON_LIB_PATH="$(echo /usr/lib/python${PYTHON_VERSION_OUTPUT:7:3}/site-packages)" \n\
    export PYTHON_INCLUDE_PATH="$(echo /usr/include/python${PYTHON_VERSION_OUTPUT:7:3}*)" \n\
fi \n\
if [ -x /usr/local/bin/gcc ]; then \n\
    export GCC_HOST_COMPILER_PATH="/usr/local/bin/gcc" \n\
else \n\
    export GCC_HOST_COMPILER_PATH="/usr/bin/gcc" \n\
fi \n\
if [ -d /usr/local/cuda ]; then \n\
    export CUDA_TOOLKIT_PATH="/usr/local/cuda" \n\
    export CUDNN_INSTALL_PATH="/usr/local/cuda" \n\
    export NCCL_INSTALL_PATH="/usr/local/nccl" \n\
    export TF_CUDA_CLANG=0 \n\
    export TF_CUDA_COMPUTE_CAPABILITIES="5.2,6.0,6.1,7.0" \n\
    export TF_CUDA_VERSION=${CUDA_VERSION} \n\
    export TF_CUDNN_VERSION=${CUDNN_VERSION} \n\
    export TF_NCCL_VERSION=${NCCL_VERSION} \n\
    export TF_NEED_CUDA=1 \n\ \n\
    export MLCC_BAZEL_BUILD_OPTIONS="--copt=-mfpmath=both --copt=-mavx2 --copt=-mfma --config=cuda" \n\
else \n\
    export TF_NEED_CUDA=0 \n\
    export MLCC_BAZEL_BUILD_OPTIONS="                     --copt=-mavx2 --copt=-mfma              " \n\
fi \n'
>> /tmp/export_tf_vars.sh;
. /tmp/export_tf_vars.sh;
export
CC_OPT_FLAGS="-march=native" 
TF_DOWNLOAD_MKL=0 
TF_ENABLE_XLA=0 
TF_NEED_GCP=0 
TF_NEED_GDR=0 
TF_NEED_HDFS=0 
TF_NEED_JEMALLOC=1 
TF_NEED_KAFKA=0 
TF_NEED_MKL=0 
TF_NEED_MPI=0 
TF_NEED_OPENCL=0 
TF_NEED_OPENCL_SYCL=0 
TF_NEED_S3=0 
TF_NEED_TENSORRT=0 
TF_NEED_VERBS=0 
TF_SET_ANDROID_WORKSPACE=0
;
mkdir -p /tmp/bazel && cd /tmp/bazel;
wget -q "https://github.com/bazelbuild/bazel/releases/download/0.12.0/bazel-0.12.0-dist.zip";
unzip *.zip;
bash ./compile.sh;
mv -f /tmp/bazel/output/bazel /usr/local/bin;
cd /tmp && /bin/rm -rf /tmp/bazel*;
cd /tmp && git clone "https://github.com/tensorflow/tensorflow.git";
cd /tmp/tensorflow && git checkout master;
df -h;
cd /tmp/tensorflow && bazel clean && ./configure;
df -h;
bazel build 
-c opt 
$MLCC_BAZEL_BUILD_OPTIONS
--jobs=`getconf _NPROCESSORS_ONLN` 
--verbose_failures=1 
"//tensorflow/tools/pip_package:build_pip_package";
df -h;
bazel-bin/tensorflow/tools/pip_package/build_pip_package /tmp/tensorflow/pip/tensorflow_pkg;
pip install /tmp/tensorflow/pip/tensorflow_pkg/tensorflow-*_x86_64.whl;
cd /tmp && /bin/rm -rf /tmp/tensorflow*;
/bin/rm -rf /root/.cache/bazel* /root/.bazel*;
\nEXPOSE 6006
\nRUN python -c 'import tensorflow as tf'
) },


{ 600, 0, "Digits", "Digits", BS( # Sorry! Digits is NYI.  See: "https://developer.nvidia.com/digits" ) },
{ 600, 0, "Neon", "Neon", BS( # Sorry! neon is NYI.  See: "https://github.com/NervanaSystems/neon" ) },
{ 600, 0, "Nnpack", "Nnpack", BS( # Sorry! nnpack is NYI.  See: "https://github.com/Maratyszcza/NNPACK" ) },
{ 600, 0, "Numexpr", "Numexpr", BS( RUN pip install numexpr ) },
{ 600, 0, "Scipy", "Scipy", BS( RUN pip install scipy ) },
{ 600, 0, "Matplotlib", "Matplotlib", BS( RUN pip install matplotlib) },
{ 600, 0, "IPython", "IPython", BS( RUN pip install ipython \nEXPOSE 8888 ) },
{ 600, 0, "Jupyter", "Jupyter", BS( RUN pip install jupyter \nEXPOSE 8888 ) },
{ 600, 0, "Pandas", "Pandas", BS( RUN pip install pandas ) },
{ 600, 0, "Sympy", "Sympy", BS( RUN pip install sympy ) },
{ 600, 0, "Seaborn", "Seaborn", BS( RUN pip install seaborn) },
{ 600, 0, "Statsmodels", "Statsmodels", BS( RUN pip install statsmodels) },
{ 600, 0, "Spyder", "Spyder", BS( RUN pip install spyder ) },
{ 600, 0, "Cython", "Cython", BS( RUN pip install cython ) },
{ 600, 0, "OpenCV", "OpenCV", BS( RUN yum -y install opencv; cd /var/cache && /bin/rm -rf dnf yum ) },

{ 600, 0, "Mxnet", "Mxnet", BS( RUN
echo -e '\
set -vx \n\
if [ -d "/usr/local/cuda-9.2" ]; then \n\
    echo "mxnet-cu92" \n\
elif [ -d "/usr/local/cuda-9.1" ]; then \n\
    echo "mxnet-cu91" \n\
elif [ -d "/usr/local/cuda-9.0" ]; then \n\
    echo "mxnet-cu90" \n\
elif [ -d "/usr/local/cuda-8.0" ]; then \n\
    echo "mxnet-cu80" \n\
else \n\
    echo "mxnet" \n\
fi \n'
>> /tmp/select_mxnet.sh;
pip install `sh /tmp/select_mxnet.sh` 
\nRUN python -c 'import mxnet as mx'
) }, 


// For Cupy / Chainer info see:
// "https://docs-cupy.chainer.org/en/stable/install.html"
// "https://github.com/cupy/cupy/releases"
// Note: Cupy must come after cuDNN and NCCL which are part of the CUDA install and must come before chainer.
// For specific version, use something like: pip install cupy==4.0.0
//
// See bug report for python 3.4: "https://github.com/cupy/cupy/issues/92"
// { 600, 0, "CuPy", "CuPy", BS( RUN pip install cupy ) }, 
// 
// Trying these CUDA version specific cupy wheels, which already include cudnn and nccl?
// (For CUDA 8.0) $ pip install cupy-cuda80 
// (For CUDA 9.0) $ pip install cupy-cuda90 
// (For CUDA 9.1) $ pip install cupy-cuda91
// (For CUDA 9.2) $ pip install cupy-cuda92  <-- Not available as of 5/29/2018

{ 600, 0, "CuPy", "CuPy", BS( RUN 
echo -e '\
set -vx \n\
if [ -d "/usr/local/cuda-9.2" ]; then \n\
    echo "cupy" \n\
elif [ -d "/usr/local/cuda-9.1" ]; then \n\
    echo "cupy-cuda91" \n\
elif [ -d "/usr/local/cuda-9.0" ]; then \n\
    echo "cupy-cuda90" \n\
elif [ -d "/usr/local/cuda-8.0" ]; then \n\
    echo "cupy-cuda80" \n\
else \n\
    echo "cupy" \n\
fi \n'
>> /tmp/select_cupy.sh;
pip install `sh /tmp/select_cupy.sh` 
\nRUN python -c 'import cupy'
) }, 


{ 600, 0, "Chainer", "Chainer", BS( RUN
pip install chainer
\nRUN python -c 'import chainer'
) },

// FIXME: specific version
// Perhaps could use just "torch" for all CUDA8?
{ 600, 0, "PyTorch", "PyTorch", BS( RUN
echo -e '\
set -vx \n\
PYTORCH_VERSION="torch-0.4.0" \n\
if [ -d "/usr/local/cuda-9.2" ]; then \n\
    CUDA_VER="cu92" \n\
elif [ -d "/usr/local/cuda-9.1" ]; then \n\
    CUDA_VER="cu91" \n\
elif [ -d "/usr/local/cuda-9.0" ]; then \n\
    CUDA_VER="cu90" \n\
elif [ -d "/usr/local/cuda-8.0" ]; then \n\
    CUDA_VER="cu80" \n\
else  \n\
    CUDA_VER="cpu" \n\
fi \n\
PYTHON_VERSION_OUTPUT=`python --version 2>&1` \n\
case ${PYTHON_VERSION_OUTPUT:7:3} in \n\
    2.7) PYTHON_VER_SPEC="cp27-cp27mu" ;; \n\
    3.4) PYTHON_VER_SPEC="cp34-cp34m" ;; \n\
    3.5) PYTHON_VER_SPEC="cp35-cp35m" ;; \n\
    3.6) PYTHON_VER_SPEC="cp36-cp36m" ;; \n\
    3.7) PYTHON_VER_SPEC="cp37-cp37m" ;; \n\
      *) PYTHON_VER_SPEC="UNKNOWN" ;; \n\
esac \n\
echo "http://download.pytorch.org/whl/$CUDA_VER/$PYTORCH_VERSION-$PYTHON_VER_SPEC-linux_x86_64.whl" \n'
>> /tmp/select_pytorch.sh;
pip install `sh /tmp/select_pytorch.sh` torchvision
\nRUN python -c 'import torch'
) },

// FIXME: specific version
{ 600, 0, "Julia", "Julia", BS( RUN
cd /tmp &&
wget -q "https://julialang-s3.julialang.org/bin/linux/x64/0.6/julia-0.6.2-linux-x86_64.tar.gz" &&
tar -xf julia*.gz &&
/bin/rm julia*.gz;
cd /tmp/julia* && /bin/rm LICENSE.md && /bin/cp -a . /usr/local;
cd /tmp && /bin/rm -rf /tmp/julia*
) },

{ 600, 0, "Octave", "Octave", BS( RUN yum -y install octave; cd /var/cache && /bin/rm -rf dnf yum ) },

// FIXME: should build with MKL, when MKL present
{ 600, 0, "R", "R", BS( RUN yum -y install R; cd /var/cache && /bin/rm -rf dnf yum ) },

// FIXME: specific version
{ 600, 0, "R-studio", "R-studio", BS( RUN 
cd /tmp && wget -q "https://download1.rstudio.org/rstudio-1.1.447-x86_64.rpm";
cd /tmp && yum -y install --nogpgcheck rstudio*.rpm; cd /var/cache && /bin/rm -rf dnf yum
) },

// { 600, 0, "gpuRcuda", "gpuRcuda", BS( # Sorry! gpuRcuda is NYI. See: "https://github.com/gpuRcore/gpuRcuda" ) },
{ 600, 0, "gpuR", "gpuR", BS( # Sorry! gpuR is NYI.  See: "https://github.com/cdeterman/gpuR" ) },
// gputools: "https://cran.r-project.org/src/contrib/gputools_1.1.tar.gz"
{ 600, 0, "gputools", "gputools", BS( # Sorry! gputools is NYI.  See: "https://cran.r-project.org/web/packages/gputools/index.html" ) },

// FIXME: specific version
// rpud: see: // "http://www.r-tutor.com/content/download"
// "https://github.com/cran/rpud/blob/master/INSTALL"
{ 600, 0, "rpud", "rpud", BS( RUN
cd /tmp && wget -q "http://www.r-tutor.com/sites/default/files/rpud/rpux_0.6.1_linux.tar.gz" && tar -xf rpux*.gz;
echo "install.packages(\"/tmp/rpux_0.6.1_linux/rpud_0.6.1.tar.gz\")" > /tmp/rpud_install.R;
/usr/bin/Rscript /tmp/rpud_install.R;
cd /tmp && /bin/rm -rf /tmp/rpu*
) },

{ 600, 0, "IRkernel", "IRkernel", BS( RUN
/tmp/yum_install.sh openssl-devel libcurl-devel czmq-devel;
R -e "install.packages(c('crayon', 'pbdZMQ', 'devtools'), repos='http://cran.rstudio.com/')";
R -e "devtools::install_github(paste0('IRkernel/', c('repr', 'IRdisplay', 'IRkernel')))";
R -e "IRkernel::installspec(user = FALSE)"
) },

{ 600, 0, "scikit-image", "scikit-image", BS( RUN pip install scikit-image ) },
{ 600, 0, "scikit-learn", "scikit-learn", BS( RUN
pip install scikit-learn
\nRUN python -c 'import sklearn'
) },

{ 600, 0, "spaCy", "spaCy", BS( RUN pip install spacy ) },
{ 600, 0, "Thinc", "Thinc", BS( RUN pip install thinc ) },


{ 600, 0, "Theano", "Theano", BS( RUN 
cd /tmp && git clone --depth 1 "https://github.com/Theano/libgpuarray.git";
mkdir -p /tmp/libgpuarray/build &&
cd /tmp/libgpuarray/build &&
cmake .. -DCMAKE_BUILD_TYPE=Release &&
make -j`getconf _NPROCESSORS_ONLN` &&
make install &&
ldconfig;
cd /tmp/libgpuarray && 
python setup.py build &&
python setup.py install &&
ldconfig;
cd /tmp && /bin/rm -rf /tmp/libgpuarray*;
echo -e '\
[global] \n\
device = cuda0 \n\
floatX = float32 \n\
[nvcc] \n\
fastmath=True \n\
[cuda] \n\
root=/usr/local/cuda \n'
>> ~/.theanorc ;
pip install git+"git://github.com/Theano/Theano.git"
\nRUN python -c 'from theano import *'
) },

// See: "https://docs.microsoft.com/en-us/cognitive-toolkit/setup-linux-python"
// See: "https://github.com/Microsoft/CNTK/tree/master/Tools/docker"
// See: "https://docs.microsoft.com/en-us/cognitive-toolkit/Setup-CNTK-on-Linux"
{ 600, 0, "CNTK", "CNTK", BS( RUN
yum -y install openmpi; cd /var/cache && /bin/rm -rf dnf yum;
echo -e '\
set -vx \n\
if [ -d "/usr/local/cuda" ] \n\
then \n\
    CPU_OR_GPU="GPU" \n\
else \n\
    CPU_OR_GPU="CPU-Only" \n\
fi \n\
PYTHON_VERSION_OUTPUT=`python --version 2>&1` \n\
case ${PYTHON_VERSION_OUTPUT:7:3} in \n\
    2.7) PYTHON_VER_SPEC="cp27-cp27mu" ;; \n\
    3.4) PYTHON_VER_SPEC="cp34-cp34m" ;; \n\
    3.5) PYTHON_VER_SPEC="cp35-cp35m" ;; \n\
    3.6) PYTHON_VER_SPEC="cp36-cp36m" ;; \n\
    3.7) PYTHON_VER_SPEC="cp37-cp37m" ;; \n\
      *) PYTHON_VER_SPEC="UNKNOWN" ;; \n\
esac \n\
echo "https://cntk.ai/PythonWheel/$CPU_OR_GPU/cntk-2.2-$PYTHON_VER_SPEC-linux_x86_64.whl" \n'
>> /tmp/select_cntk.sh;
pip install `sh /tmp/select_cntk.sh`
) },

{ 600, 0, "Lasagne", "Lasagne", BS( RUN
pip install "https://github.com/Lasagne/Lasagne/archive/master.zip"
\nRUN python -c 'import lasagne'
) },

// Keras must come after theano and CNTK
{ 600, 0, "Keras", "Keras", BS( RUN
mkdir -p ~/.keras;
echo -e '\
{ \n\
    "image_data_format": "channels_last", \n\
    "epsilon": 1e-07, \n\
    "floatx": "float32", \n\
    "backend": "KERAS_BACKEND" \n\
} \n'
>> ~/.keras/keras.json;
if [ -f ~/.theanorc ]; then
    sed -i 's/KERAS_BACKEND/theano/g' ~/.keras/keras.json;
elif [ -f /tmp/select_cntk.sh ]; then
    sed -i 's/KERAS_BACKEND/cntk/g' ~/.keras/keras.json;
else
    sed -i 's/KERAS_BACKEND/tensorflow/g' ~/.keras/keras.json;
fi;
pip install keras
) },

// See: "http://doc.paddlepaddle.org/develop/doc/getstarted/build_and_install/build_from_source_en.html"
{ 600, 0, "Paddle", "Paddle", BS( RUN 
yum -y install swig; cd /var/cache && /bin/rm -rf dnf yum;
pip install wheel;
pip install 'protobuf>=3.0.0';
cd /tmp && git clone "https://github.com/PaddlePaddle/Paddle" paddle;
mkdir -p /tmp/paddle/build && cd /tmp/paddle/build &&
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local &&
make -j`getconf _NPROCESSORS_ONLN` && make install &&
ldconfig;
ls -l /usr/local/opt;
cd /usr/local/opt/paddle/share/wheels/ && pip install *.whl;
cd /tmp && /bin/rm -rf /tmp/paddle
\nRUN python -c 'import paddle'
) },

// export BLAS=open # could be BLAS=atlas, or BLAS=mkl
// export BLAS_INCLUDE=/usr/include/openblas #could be path to atlas/mkl
// 'echo "/usr/local/lib" > /etc/ld.so.conf.d/opencv.conf' 
// ldconfig
// make pycaffe
{ 600, 0, "Caffe", "Caffe", BS( RUN
/tmp/yum_install.sh boost-devel gflags-devel glog-devel hdf5-devel leveldb-devel libjpeg-turbo-devel
libtiff lmdb-devel openblas-devel opencv-devel protobuf-devel snappy-devel;
ldconfig;
cd /usr/local && git clone -b 1.0 --depth 1 "https://github.com/BVLC/caffe.git";
cd /usr/local/caffe && cp Makefile.config.example Makefile.config &&
echo -e '36,38c36\n< \
CUDA_ARCH := -gencode arch=compute_20,code=sm_20 \\\n< \t\t-gencode arch=compute_20,code=sm_21 \\\n< \t\t-gencode arch=compute_30,code=sm_30 \\\n---\n> \
CUDA_ARCH :=\t-gencode arch=compute_30,code=sm_30 \
\\\n50c48\n< \
BLAS := atlas\n---\n> # BLAS := atlas\n68,69c66,67\n< \
PYTHON_INCLUDE := /usr/include/python2.7 \\\n< \t\t/usr/lib/python2.7/dist-packages/numpy/core/include\n---\n> # \
PYTHON_INCLUDE := /usr/include/python2.7 \\\n> # \t\t/usr/lib/python2.7/dist-packages/numpy/core/include'
>> diff.out && patch --ignore-whitespace Makefile.config diff.out;
pip install -r /usr/local/caffe/python/requirements.txt;
echo -e '\
set -vx \n\
echo $PATH \n\
echo $LD_LIBRARY_PATH \n\
if [ -x /usr/local/bin/gcc ]; then \n\
    export CUSTOM_CXX="/usr/local/bin/g++" \n\
else \n\
    export CUSTOM_CXX="/usr/bin/g++" \n\
fi \n'
>> /tmp/export_caffe_vars.sh;
. /tmp/export_caffe_vars.sh;
export
USE_CUDNN=1
BLAS=open
BLAS_INCLUDE=/usr/include/openblas;
cd /usr/local/caffe && make all -j`getconf _NPROCESSORS_ONLN` && make test -j`getconf _NPROCESSORS_ONLN`
) },

// See: "https://caffe2.ai/docs/getting-started.html"
// pip install future graphviz hypothesis jupyter matplotlib numpy protobuf pydot python-nvd3 pyyaml requests scikit-image scipy six;
{ 600, 0, "Caffe2", "Caffe2", BS( RUN
/tmp/yum_install.sh automake kernel-devel leveldb-devel libtool lmdb-devel protobuf-devel snappy-devel;
cd /tmp && git clone "https://github.com/gflags/gflags.git" && cd gflags && mkdir build && cd build &&
cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_CXX_FLAGS='-fPIC' .. &&
make -j`getconf _NPROCESSORS_ONLN` && make install;
cd /tmp && git clone "https://github.com/google/glog" && cd glog && mkdir build && cd build &&
cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_CXX_FLAGS='-fPIC' .. &&
make -j`getconf _NPROCESSORS_ONLN` && make install;
pip install future graphviz hypothesis protobuf pydot python-nvd3 pyyaml requests six;
cd /tmp && mkdir caffe2 &&
cd caffe2 && git clone --recursive "https://github.com/pytorch/pytorch.git" &&
cd pytorch && git submodule update --init;
if [ -x /usr/local/bin/python ]; then
    mkdir build && cd build && cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && make -j`getconf _NPROCESSORS_ONLN` install;
else
    mkdir build && cd build && cmake -DCMAKE_INSTALL_PREFIX=/usr .. && make -j`getconf _NPROCESSORS_ONLN` install;
fi;
ldconfig;
cd /tmp && /bin/rm -rf /tmp/gflags* && /bin/rm -rf /tmp/glog*
\nRUN python -c 'from caffe2.python import core'
) },


// FIXME: check new versions
{ 600, 0, "cutorch", "cutorch", BS( # Sorry! cutorch is NYI.  See: "https://github.com/torch/cutorch" ) },

// FIXME: check new versions
// See: "https://github.com/torch/torch7/wiki/Cheatsheet"
{ 600, 0, "Torch", "Torch", BS( RUN
yum -y install sox-plugins-freeworld zeromq3-devel;
/tmp/yum_install.sh fftw-devel gnuplot GraphicsMagick-devel ImageMagick lapack libjpeg-turbo-devel
libpng-devel ncurses-devel qt-devel qtwebkit-devel readline-devel sox sox-devel;
export TORCH_NVCC_FLAGS="-D__CUDA_NO_HALF_OPERATORS__";
cd /usr/local && git clone --depth 1 "https://github.com/torch/distro.git" /usr/local/torch;
cd /usr/local/torch && ./install.sh
) },

{ 600, 0, "VNC", "VNC", BS( RUN 
/tmp/yum_install.sh dejavu-sans-fonts dejavu-serif-fonts tigervnc-server xdotool xorg-x11-twm xterm xulrunner;
mkdir -p /root/.vnc;
echo -e '\
#!/bin/sh \n\
unset SESSION_MANAGER \n\
unset DBUS_SESSION_BUS_ADDRESS \n\
if [ -x /etc/X11/xinit/xinitrc ]; then exec /etc/X11/xinit/xinitrc; fi \n\
if [ -f /etc/X11/xinit/xinitrc ]; then exec sh /etc/X11/xinit/xinitrc; fi \n\
[ -r $HOME/.Xresources ] && xrdb $HOME/.Xresources \n\
xsetroot -solid grey \n\
twm & \n'
>> /root/.vnc/xstartup;
chmod -v +x /root/.vnc/xstartup;
echo 123456 | vncpasswd -f > /root/.vnc/passwd;
chmod -v 600 /root/.vnc/passwd
\nEXPOSE 5901
) }

};
#define NUM_PKGS (sizeof(pkgs) / sizeof(pkgs[0]))


struct pkg_data *selected_set[NUM_PKGS];
struct pkg_data *available_set[NUM_PKGS];
int num_selected = 0;
int num_available = 0;


void list_all_packages() {
    for (int ix = 0;  (ix < NUM_PKGS);  ix++) {
        printf("(%d) %s: %s\n", ix, pkgs[ix].label, pkgs[ix].desc);
    }
}


void display_set(char *title, struct pkg_data **set, int *num, int lo_po_num_start, int hi_po_num_limit) {
    struct winsize win_info;
    ioctl(0, TIOCGWINSZ, &win_info);
    // printf ("lines %d\n", win_info.ws_row);
    // printf ("columns %d\n", win_info.ws_col);
    printf("\n%s:\n", title);
    int line_length = 0;
    for (int ix = 0;  (ix < *num);  ix++) {
        if ((set[ix]->po_num >= lo_po_num_start) && (set[ix]->po_num < hi_po_num_limit)) {
            printf("%2d: %-20s", ix, set[ix]->label);
            line_length += 24;
            if (line_length +24  > win_info.ws_col) {
                line_length = 0;
                printf("\n");
            }
        }
    }
    printf("\n");
}


void sort_set_by_label_and_remove_duplicates(struct pkg_data **set, int *num) {
    int n = *num;
    for (int ix = 0;  (ix < n);  ix++) {
        for (int iy = ix + 1;  (iy < n);  iy++) {
            int different = strcasecmp(set[iy]->label, set[ix]->label);
            if (!different) {
                set[iy] = set[(n - 1)];
                n -= 1;
                iy -= 1;
                continue;
            } else if (different < 0) {
                struct pkg_data *p = set[ix];
                set[ix] = set[iy];
                set[iy] = p;
            }
        }
    }
    *num = n;
}


void sort_set_by_po_num(struct pkg_data **set, int *num) {
    int n = *num;
    for (int ix = 0;  (ix < n);  ix++) {
        for (int iy = ix + 1;  (iy < n);  iy++) {
            if (set[iy]->po_num < set[ix]->po_num) {
                struct pkg_data *p = set[ix];
                set[ix] = set[iy];
                set[iy] = p;
            }
        }
    }
    *num = n;
}


void add_to_set(struct pkg_data *p, struct pkg_data **set, int *n) {
    set[*n] = p;
    *n += 1;
}


void partition_selected() {
    num_selected = 0;
    num_available = 0;
    for (int ix = 0;  (ix < NUM_PKGS);  ix++) {
        if (pkgs[ix].include) {
            add_to_set(&(pkgs[ix]), selected_set, &num_selected);
        } else {
            add_to_set(&(pkgs[ix]), available_set, &num_available);
        }
    }
    sort_set_by_label_and_remove_duplicates(selected_set, &num_selected);
    sort_set_by_po_num(selected_set, &num_selected);
    sort_set_by_label_and_remove_duplicates(available_set, &num_available);
}


void mark_selection(char *s, int yes_or_no) {
    int tokens_found = 0;
    for (int ix = 0;  (ix < NUM_PKGS);  ix++) {
        if (!strcasecmp(s, pkgs[ix].label)) {
            pkgs[ix].include = yes_or_no;
            num_selected += yes_or_no;
            tokens_found += 1;
            if (debug) {
                printf("Marking pkg: %s as %d\n", pkgs[ix].label, yes_or_no);
            }
        }
    }
    if (!tokens_found) {
        fprintf(stderr, "Token %s not found\n", s);
    }
}


void clear_all_selections_in_po_num_group(int po_num) {
    int lo = 100 * (po_num / 100);
    int hi = 100 * ((po_num + 99) / 100);
    for (int ix = 0;  (ix < NUM_PKGS);  ix++) {
        if ((pkgs[ix].po_num >= lo) && (pkgs[ix].po_num < hi)) {
            pkgs[ix].include = 0;
        }
    }
}


int label_to_po_num(char *s) {
    for (int ix = 0;  (ix < NUM_PKGS);  ix++) {
        if (!strcasecmp(s, pkgs[ix].label)) {
            return pkgs[ix].po_num;
        }
    }
    return -1;
}


int selected(char *s) {
    for (int ix = 0;  (ix < NUM_PKGS);  ix++) {
        if (!strcasecmp(s, pkgs[ix].label)) {
            return pkgs[ix].include;
        }
    }
    return 0;
}


int selected_strn(char *s, int n) {
    int result = 0;
    for (int ix = 0;  (ix < NUM_PKGS);  ix++) {
        if (!strncasecmp(s, pkgs[ix].label, n)) {
            result |= pkgs[ix].include;
        }
    }
    return result;
}


void check_compatibility_and_add(char *s) {
    int po_num = label_to_po_num(s);
    if ((po_num >= 0) && (po_num < MISC_LO_PO_NUM_START)) {
        clear_all_selections_in_po_num_group(po_num);
    }
    if (!strncasecmp(s, "spaCy", 5)) {
        mark_selection("Thinc", 2);
    }
    if (!strncasecmp(s, "R-studio", 8)) {
        mark_selection("R", 2);
    }
    if ((!strncasecmp(s, "Caffe", 5)
        || (!strncasecmp(s, "Caffe2", 6)))) {
        mark_selection("Numpy", 2);
        mark_selection("Atlas", 2);
        mark_selection("OpenBLAS", 2);
    }
    if (!strncasecmp(s, "CNTK", 4)) {
        mark_selection("Numpy", 2);
        mark_selection("Scipy", 2);
    }
    if (!strncasecmp(s, "TensorFlow", 10)) {
        mark_selection("Numpy", 2);
    }
    if (!strncasecmp(s, "Paddle", 6)) {
        mark_selection("Numpy", 2);
    }
    if ((!strncasecmp(s, "Scikit", 6)
        || (!strncasecmp(s, "Theano", 6)))) {
        mark_selection("Numpy", 2);
        mark_selection("Scipy", 2);
        mark_selection("Cython", 2);
    }
    if (!strncasecmp(s, "Seaborn", 7)) {
        mark_selection("Numpy", 2);
        mark_selection("Scipy", 2);
        mark_selection("Pandas", 2);
        mark_selection("Matplotlib", 2);
    }
    if ((!strncasecmp(s, "R-studio", 8))
        || (!strncasecmp(s, "Spyder", 6))
        || (!strncasecmp(s, "Octave", 6))
        || (!strncasecmp(s, "Matplotlib", 10))
        || (!strncasecmp(s, "Seaborn", 7))) {
        mark_selection("VNC", 2);
    }
    mark_selection(s, 1);
    if (selected_strn("CUDA", 4)) {
        if (selected("Chainer") || selected("spaCy") || selected("Thinc")) {
            mark_selection("CuPy", 2);
        }
    }
    if ( selected_strn("RHEL", 4) || selected_strn("Centos", 6) || selected_strn("Fedora", 6) ) {
        mark_selection("OS-Utils", 2);
    }
    if (selected_strn("Fedora", 6)) {
        // CUDA needs older GCC compilers:
        // - CUDA8.0 needs GCC 5 or older
        // - CUDA9.0 needs GCC 6 or older
        // - CUDA9.1 needs GCC 6 or older
        // - CUDA9.2 needs GCC 7 or older
        if (selected("CUDA8.0")) {
            mark_selection("GCC-5.5", 2);
        } else if (selected("CUDA9.0") || selected("CUDA9.1")) {
            if (!selected("Fedora25")) {
                // Could use GCC 6 here, if working. Wait for GCC6.5?
                mark_selection("GCC-5.5", 2);
            }
        } else if (selected("CUDA9.2")) {
            if (selected("Fedora28")) {
                mark_selection("GCC-7.3", 2);
            }
        }
    }
    if ( selected("Jupyter") && selected("R") ) {
        mark_selection("IRkernel", 2);
    }
}


#ifdef GUI


#include <gtk/gtk.h>
#include <glib.h>

int num_buttons = 0;
int first_other_button = 0;
GtkWidget *buttons[NUM_PKGS];
GtkWidget *save_rhel72_button = NULL;
GtkWidget *save_cpu_button = NULL;
GtkWidget *save_python2_button = NULL;

void handle_select_event(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    char *button_label = (char *)gtk_button_get_label(GTK_BUTTON(widget));
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        check_compatibility_and_add(button_label);
        for (int ix = first_other_button;  (ix < num_buttons);  ix++) {
            int selected_state = (selected((char *)gtk_button_get_label(GTK_BUTTON(buttons[ix]))) > 0);
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buttons[ix]), selected_state);
        }
    } else {
        mark_selection((char *)button_label, 0);
    }
}

void add_button(GtkWidget *b) {
    buttons[num_buttons++] = b;
    g_signal_connect(b, "clicked", G_CALLBACK(handle_select_event), NULL);
}

void handle_reset_event(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(save_rhel72_button), 1);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(save_cpu_button), 1);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(save_python2_button), 1);
    for (int ix = first_other_button;  (ix < num_buttons);  ix++) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(buttons[ix]), 0);
        mark_selection((char *)gtk_button_get_label(GTK_BUTTON(buttons[ix])), 0);
    }
}

static gboolean handle_create_button(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    for (int ix = 0;  (ix < num_buttons);  ix++) {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(buttons[ix]))) {
            check_compatibility_and_add((char *)gtk_button_get_label(GTK_BUTTON(buttons[ix])));
        }
    }
    gtk_main_quit();
    return(FALSE);
} 

static gboolean handle_destroy_event(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    gtk_main_quit();
    exit(EXIT_SUCCESS);
}


void make_gui_choices() {

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), title_string);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(handle_destroy_event), NULL);
    // gtk_window_set_decorated(GTK_WINDOW(window), show_window_decorations);
    // gtk_window_set_default_size(GTK_WINDOW(window), panel_width, panel_height);
    // gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    GtkWidget *big_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(big_box), FALSE);
    gtk_container_add(GTK_CONTAINER(window), big_box);

    GtkWidget *os_box1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(big_box), os_box1, TRUE, TRUE, 0);
    GtkWidget *button = gtk_radio_button_new_with_label(NULL, "RHEL7.2");
    add_button(button);
    save_rhel72_button = button;
    gtk_box_pack_start(GTK_BOX(os_box1), button, TRUE, FALSE, 2);
    button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button), "RHEL7.3");
    add_button(button);
    gtk_box_pack_start(GTK_BOX(os_box1), button, TRUE, FALSE, 2);
    button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button), "RHEL7.4");
    add_button(button);
    gtk_box_pack_start(GTK_BOX(os_box1), button, TRUE, FALSE, 2);
    button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button), "RHEL7.5");
    add_button(button);
    gtk_box_pack_start(GTK_BOX(os_box1), button, TRUE, FALSE, 2);
    button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button), "Centos7");
    add_button(button);
    gtk_box_pack_start(GTK_BOX(os_box1), button, TRUE, FALSE, 2);

    GtkWidget *os_box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(big_box), os_box2, TRUE, TRUE, 0);
    button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button), "Fedora25");
    add_button(button);
    gtk_box_pack_start(GTK_BOX(os_box2), button, TRUE, FALSE, 2);
    button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button), "Fedora26");
    add_button(button);
    gtk_box_pack_start(GTK_BOX(os_box2), button, TRUE, FALSE, 2);
    button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button), "Fedora27");
    add_button(button);
    gtk_box_pack_start(GTK_BOX(os_box2), button, TRUE, FALSE, 2);
    button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button), "Fedora28");
    add_button(button);
    gtk_box_pack_start(GTK_BOX(os_box2), button, TRUE, FALSE, 2);

    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(big_box), separator, FALSE, TRUE, 0);

    GtkWidget *accel_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(big_box), accel_box, TRUE, TRUE, 0);
    button = gtk_radio_button_new_with_label(NULL, "CPU");
    add_button(button);
    save_cpu_button = button;
    gtk_box_pack_start(GTK_BOX(accel_box), button, TRUE, FALSE, 2);
    button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button), "CUDA8.0");
    add_button(button);
    gtk_box_pack_start(GTK_BOX(accel_box), button, TRUE, FALSE, 2);
    button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button), "CUDA9.0");
    add_button(button);
    gtk_box_pack_start(GTK_BOX(accel_box), button, TRUE, FALSE, 2);
    button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button), "CUDA9.1");
    add_button(button);
    gtk_box_pack_start(GTK_BOX(accel_box), button, TRUE, FALSE, 2);
    button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button), "CUDA9.2");
    add_button(button);
    gtk_box_pack_start(GTK_BOX(accel_box), button, TRUE, FALSE, 2);

    separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(big_box), separator, FALSE, TRUE, 0);

    GtkWidget *python_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(big_box), python_box, TRUE, TRUE, 0);
    button = gtk_radio_button_new_with_label(NULL, "Python2");
    add_button(button);
    save_python2_button = button;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
    gtk_box_pack_start(GTK_BOX(python_box), button, TRUE, FALSE, 2);
    button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(button), "Python3");
    add_button(button);
    gtk_box_pack_start(GTK_BOX(python_box), button, TRUE, FALSE, 2);

    separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(big_box), separator, FALSE, TRUE, 0);

    GtkWidget *other_box = gtk_flow_box_new();
    gtk_flow_box_set_min_children_per_line((GtkFlowBox *)other_box, 3);
    gtk_flow_box_set_max_children_per_line((GtkFlowBox *)other_box, 4);
    gtk_box_pack_start(GTK_BOX(big_box), other_box, TRUE, TRUE, 0);

    num_available = 0;
    for (int ix = 0;  (ix < NUM_PKGS);  ix++) {
        if (pkgs[ix].po_num >= MISC_LO_PO_NUM_START) {
            add_to_set(&(pkgs[ix]), available_set, &num_available);
        }
    }
    sort_set_by_label_and_remove_duplicates(available_set, &num_available);
    first_other_button = num_buttons;
    for (int ix = 0;  (ix < num_available);  ix++) {
        GtkWidget *button = gtk_check_button_new_with_label(available_set[ix]->label);
        add_button(button);
        gtk_flow_box_insert((GtkFlowBox *)other_box, button, -1);
    }

    separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(big_box), separator, FALSE, TRUE, 0);

    GtkWidget *control_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(big_box), control_box, TRUE, TRUE, 0);

    GtkWidget *quit_button = gtk_button_new_with_label("Quit");
    g_signal_connect(quit_button, "clicked", G_CALLBACK(handle_destroy_event), NULL);
    gtk_box_pack_start(GTK_BOX(control_box), quit_button, TRUE, FALSE, 2);

    GtkWidget *reset_button = gtk_button_new_with_label("Reset");
    g_signal_connect(reset_button, "clicked", G_CALLBACK(handle_reset_event), NULL);
    gtk_box_pack_start(GTK_BOX(control_box), reset_button, TRUE, FALSE, 2);

    GtkWidget *create_button = gtk_button_new_with_label("Create Dockerfile");
    g_signal_connect(create_button, "clicked", G_CALLBACK(handle_create_button), NULL);
    gtk_box_pack_start(GTK_BOX(control_box), create_button, TRUE, FALSE, 2);

    gtk_widget_show_all(window);
    gtk_main();

}


#endif


void make_interactive_choices() {
    for (;;) {
        printf("\n");
        partition_selected();
        display_set("Selected", selected_set, &num_selected, 0, MAX_PO_NUM_LIMIT);
        display_set("OS Choices", available_set, &num_available, 0, ACCEL_LO_PO_NUM_START);
        display_set("Accelerator Choices", available_set, &num_available, ACCEL_LO_PO_NUM_START, ACCEL_HI_PO_NUM_LIMIT);
        display_set("Python Choices", available_set, &num_available, PYTHON_LO_PO_NUM_START, PYTHON_HI_PO_NUM_LIMIT);
        display_set("Additional Packages", available_set, &num_available, MISC_LO_PO_NUM_START, MAX_PO_NUM_LIMIT);
        printf("\n(A)dd, (R)emove, (C)reate Dockerfile, (Q)uit: ");
        char buf[255]; 
        char choice = 'A';
        fgets(buf, 255, stdin);
        if (!isdigit(buf[0])) {
            choice = toupper(buf[0]);
            if (choice == 'Q') {
                exit(EXIT_FAILURE);
            } else if (choice == 'C') {
                break;
            } else if (choice == 'A') {
                printf("Enter numbers to add: ");
            } else if (choice == 'R') {
                printf("Enter numbers to remove: ");
            } else {
                continue;
            }
            fgets(buf, 255, stdin);
        }
        char *p = strtok(buf, list_delimiters);
        while (p) {
            unsigned ix = atoi(p);
            if ((choice == 'A') && (ix < num_available)) {
                check_compatibility_and_add(available_set[ix]->label);
            } else if ((choice == 'R') && (ix < num_selected)) {
                mark_selection(selected_set[ix]->label, 0);
            }
            p = strtok(NULL, list_delimiters);
        }
    }
}


void write_docker_file_contents() {
    if (!output_file_name) {
        time_t t = time(NULL);
        struct tm *tmp = localtime(&t);
        if (tmp == NULL) {
            perror("localtime");
            exit(EXIT_FAILURE);
        }
        char buf[127];
        strftime(buf, 127, "%Y%m%d%H%M%S_Dockerfile", tmp);
        output_file_name = buf;
    }
    FILE *f = fopen(output_file_name, "w");
    printf("\nWriting file: %s\n\n", output_file_name);
    // FIXME: just make the mlcc command a comment for now...
    // fprintf(f, "\nLABEL mlcc_command=\"mlcc -i ");
    fprintf(f, "\n# mlcc -i ");
    num_selected = 0;
    for (int ix = 0;  (ix < NUM_PKGS);  ix++) {
        if (pkgs[ix].include == 1) {
            add_to_set(&(pkgs[ix]), selected_set, &num_selected);
        }
    }
    sort_set_by_label_and_remove_duplicates(selected_set, &num_selected);
    sort_set_by_po_num(selected_set, &num_selected);
    int need_comma = 0;
    for (int ix = 0;  (ix < num_selected);  ix++) {
        if (need_comma) {
            fprintf(f, ",");
        }
        fprintf(f, "%s", selected_set[ix]->label);
        need_comma = 1;
    }
    // fprintf(f, "\"\n");
    fprintf(f, "\n# mlcc version: %s: %s\n", version_string, __DATE__);
    for (int ix = 0;  (ix < NUM_PKGS);  ix++) {
        if (pkgs[ix].include) {
            if (debug) {
                printf("Including (%d) %s: %s\n", ix, pkgs[ix].label, pkgs[ix].desc);
            }
            fprintf(f, "\n%s\n", pkgs[ix].frag);
        }
    }
    fprintf(f, "\n");
    fclose(f);
}


int main(int argc, char **argv) {
    int opt;
    prog_name = argv[0];
    if (argc == 1) {
        fprintf(stderr, "Expecting one of { -I | -G | -i <pkg>,<pkg>... }:\n");
        display_usage_and_exit();
    }
    num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    while ((opt = getopt(argc, argv, "dGhi:Ilo:qvV")) != -1) {
        switch (opt) {
            case 'd': debug = 1; break;
            case 'G': {
#ifdef GUI
                gui = 1;
                interactive = 0;
                gtk_init(&argc, &argv);
#endif
                break;
            }
            case 'h': display_usage_and_exit(argv[0]); break;
            case 'i': {
                char *p = strtok(optarg, list_delimiters);
                while (p) {
                    check_compatibility_and_add(p);
                    p = strtok(NULL, list_delimiters);
                }
                break;
            }
            case 'I':
                gui = 0;
                interactive = 1;
                break;
            case 'l': {
                list_all_packages();
                break;
            }
            case 'o': output_file_name = optarg; break;
            case 'q': quiet = 1; break;
            case 't': title_string = optarg; break;
            case 'v': verbose = 1; break;
            case 'V': display_version_and_exit(); break;
            default: display_usage_and_exit(); break;
        }
    }
    if (argc > optind) {
        fprintf(stderr, "Unexpected arg = %s\n", argv[optind]);
        exit(EXIT_FAILURE);
    }
    if (verbose) {
        printf("system has %d cpus\n", num_cpus);
        // . . . .
        fflush(stdout);
    }
    if (interactive) {
        make_interactive_choices();
    }
#ifdef GUI
    if (gui) {
        make_gui_choices();
    }
#endif
    if (num_selected > 0) {
        // Must always select an OS
        if (!selected_strn("RHEL", 4) && !selected_strn("Centos", 6) && !selected_strn("Fedora", 6)) {
            mark_selection("RHEL7.2", 1);
        }
        // Must always select one of the Python versions
        if (!selected_strn("Python", 6)) {
            // FIXME: following conditional is OK until RHEL starts using Python3 as system python
            if (selected_strn("Fedora", 6)) {
                mark_selection("Python3", 1);
            } else {
                mark_selection("Python2", 1);
            }
        }
        // Must always select either CPU or CUDA
        if (!selected_strn("CUDA", 4) && !selected("CPU")) {
            mark_selection("CPU", 1);
        }
        write_docker_file_contents();
    }
    exit(EXIT_SUCCESS);
}

