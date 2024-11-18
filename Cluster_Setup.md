# 1. Startup
This Setup is based on:

https://youtu.be/ytugid1hKRM?si=_rtLtrjdHYCmHzxa

https://www.youtube.com/playlist?list=PLOB8_oGJl40Sxjn9rtgSVgg9tfC4Z4MWe

In this case, the cluster consists of 3 nodes: a master (Raspberry Pi) and two clients (PCs):
- `pipo@master`
- `pipo@client1`
- `pipo@client2`

If needed, you can run the following commands to create the `pipo` user as an administrator:

```bash
sudo useradd -m -s /bin/bash pipo
sudo passwd pipo
sudo usermod -aG sudo pipo
groups pipo
```

To set the hostname:

```bash
hostnamectl set-hostname <hostname>
```

**Note:** It is recommended to turn off and turn on the PC again.

# 2. Install on all machines

```bash
sudo apt update
sudo apt upgrade
sudo apt install gcc gedit net-tools openmpi-bin openmpi-common libopenmpi-dev openssh-client openssh-server nfs-kernel-server
```

# 3. Edit /etc/hosts on all machines

```bash
sudo gedit /etc/hosts
```

Add the following lines:

```bash
<ip_master>    master
<ip_client1>   client1
<ip_client2>   client2
```

## 3.1 Validate communication

Try:

```bash
ping <hostname>
```

# 4. Create .ssh directory on all machines

In /home/pipo, type:

```bash
mkdir ~/.ssh
cd ~/.ssh
```

## 4.1 On the master node

```bash
ssh-keygen -t rsa
cp id_rsa.pub authorized_keys
ssh-copy-id client1
ssh-copy-id client2

ssh client1 'chmod 700 .ssh; chmod 640 .ssh/authorized_keys'
ssh client2 'chmod 700 .ssh; chmod 640 .ssh/authorized_keys'
```

## 4.2 On client1 and client2, edit /etc/ssh/sshd_config

```bash
sudo gedit /etc/ssh/sshd_config
```

Ensure the following lines are set:

```bash
PubkeyAuthentication yes
RSAAuthentication yes
```

Then restart the ssh service:

```bash
sudo service ssh restart
```

## 4.3 Validate ssh communication without password

```bash
ssh client1 uname -a
ssh client2 uname -a
```

# 5. Create sharedcenc directory

## 5.1 On the master node

```bash
sudo mkdir -p /home/pipo/Desktop/sharedcenc
```

Change its permissions:

```bash
sudo chown nobody:nogroup /home/pipo/Desktop/sharedcenc
sudo chmod -R 777 /home/pipo/Desktop/sharedcenc
```

Edit /etc/exports:

```bash
sudo gedit /etc/exports
```

Add:

```bash
/home/pipo/Desktop/sharedcenc client1(rw,sync,no_subtree_check)
/home/pipo/Desktop/sharedcenc client2(rw,sync,no_subtree_check)
#/home/pipo/Desktop/sharedcenc *(rw,sync,no_subtree_check)
```

Then run:

```bash
sudo exportfs -a
sudo systemctl restart nfs-kernel-server
sudo systemctl enable nfs-kernel-server
```

## 5.2 On client1 and client2

```bash
sudo mkdir -p /home/pipo/Desktop/sharedcenc
sudo mount master:/home/pipo/Desktop/sharedcenc /home/pipo/Desktop/sharedcenc
```

## 5.3 Test the sharedcenc directory

```bash
cd sharedcenc
touch test.txt
```

## 5.4 Test mpirun on all machines

```bash
mpirun
```

# 6. Test the cluster

**Note:** Since the master is on a Raspberry Pi with arm64 architecture, it is ideal to leave it out of the processing since the clients have x86_64 architecture.

## 6.1 Add hosts for processing in the cluster

```bash
cd sharedcenc
touch hosts
sudo gedit hosts
```

Add:

```bash
client1
client2
```

## 6.2 Compile the test file from one of the clients (x86_64 architecture)

**Note:** The test file must be on the sharedcenc directory. You can find the code here: 

https://www.dropbox.com/scl/fo/10icx7zax2631jyqfqf7t/AM4H-3IcGXsJ9VzWOGa7bdY?e=1&preview=mpi-prime.c&rlkey=nvggnucm1r0tjjzosx6oct81i&dl=0

```bash
mpicc primes.c -o primes_x86_64
```

## 6.3 Execute the file and validate with top command on Client PCs

```bash
mpirun -hostfile hosts primes_x86_64
```