# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  # The most common configuration options are documented and commented below.
  # For a complete reference, please see the online documentation at
  # https://docs.vagrantup.com.

  config.vm.box = "bento/ubuntu-18.04"
  # config.vm.network "forwarded_port", guest: 80, host: 8080, host_ip: "127.0.0.1"
  # config.vm.provider "virtualbox" do |vb|
  #   vb.memory = "1024"
  # end

  config.vm.provision "shell", privileged: false, inline: <<-SHELL
    sudo apt-get update
    sudo apt-get install -y build-essential
    
    # install latest cmake version
    # sudo apt-get install -y apt-transport-https ca-certificates gnupg software-properties-common wget
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
    sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'
    sudo apt-get update
    sudo apt-get install -y cmake

    cp -r /vagrant ~/dpatch-repo
    cd ~/dpatch-repo
    cmake .
    cmake --build . --target all
    ./dpatch/dpatch ./demo/hello_world
  SHELL
end
